// Copyright (C) 2025 Langning Chen
//
// This file is part of miniapp.
//
// miniapp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// miniapp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with miniapp.  If not, see <https://www.gnu.org/licenses/>.

#include "AI.hpp"
#include "strUtils.hpp"
#include <iostream>
#include <sstream>
#include <regex>
#include <chrono>

AI::AI()
{
    conversationManager.loadApiSettings(apiKey, baseUrl, model, maxTokens, temperature, topP, systemPrompt);

    auto conversationsResponse = conversationManager.getConversationList();
    if (conversationsResponse.conversations.empty())
    {
        conversationManager.createConversation("默认对话", conversationId);

        currentNodeId = rootNodeId = strUtils::randomId();
        nodeMap[currentNodeId] = std::make_unique<ConversationNode>(
            currentNodeId, ConversationNode::ROLE_SYSTEM, systemPrompt, "");
        saveConversation();
    }
    else
    {
        conversationId = conversationsResponse.conversations[0].id;
        conversationManager.loadConversation(conversationId, nodeMap, rootNodeId);
        currentNodeId = rootNodeId;
        while (!nodeMap[currentNodeId]->childIds.empty())
            currentNodeId = nodeMap[currentNodeId]->childIds.back();
    }
}

ConversationNode *AI::findNode(const std::string &nodeId)
{
    auto it = nodeMap.find(nodeId);
    return (it != nodeMap.end()) ? it->second.get() : nullptr;
}

std::vector<ConversationNode> AI::getPathFromRoot(const std::string &nodeId)
{
    std::vector<ConversationNode> path;
    std::string currentId = nodeId;
    while (!currentId.empty())
    {
        ConversationNode *node = findNode(currentId);
        if (!node)
            break;
        path.push_back(*node);
        currentId = node->parentId;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

void AI::addNode(ConversationNode::ROLE role, std::string content)
{
    std::string nodeId = strUtils::randomId();
    ConversationNode *parent = findNode(currentNodeId);
    if (parent)
        parent->childIds.push_back(nodeId);
    nodeMap[nodeId] = std::make_unique<ConversationNode>(nodeId, role, content, currentNodeId);
    currentNodeId = nodeId;
    saveConversation();
}

bool AI::deleteNode(const std::string &nodeId)
{
    ConversationNode *node = findNode(nodeId);
    if (!node)
        return false;
    ConversationNode *parent = findNode(node->parentId);
    if (parent)
    {
        auto it = std::find(parent->childIds.begin(), parent->childIds.end(), nodeId);
        if (it != parent->childIds.end())
            parent->childIds.erase(it);
    }
    nodeMap.erase(nodeId);
    if (currentNodeId == nodeId)
        currentNodeId = node->parentId;
    saveConversation();
    return true;
}

bool AI::switchNode(const std::string &nodeId)
{
    ConversationNode *node = findNode(nodeId);
    if (node)
    {
        currentNodeId = nodeId;
        return true;
    }
    return false;
}

std::vector<std::string> AI::getChildren(const std::string &nodeId)
{
    ConversationNode *node = findNode(nodeId);
    if (node)
        return node->childIds;
    return {};
}

std::vector<ConversationNode> AI::getCurrentPath() { return getPathFromRoot(currentNodeId); }
std::string AI::getCurrentNodeId() const { return currentNodeId; }
std::string AI::getRootNodeId() const { return rootNodeId; }
std::string AI::getConversationId() const { return conversationId; }

void AI::saveConversation()
{
    if (!conversationId.empty())
    {
        conversationManager.saveConversation(conversationId, nodeMap);
    }
}

ConversationListResponse AI::getConversationList()
{
    return conversationManager.getConversationList();
}

void AI::createConversation(const std::string &title)
{
    std::string newConversationId;
    conversationManager.createConversation(title, newConversationId);

    conversationId = newConversationId;
    nodeMap.clear();
    currentNodeId = rootNodeId = strUtils::randomId();
    nodeMap[currentNodeId] = std::make_unique<ConversationNode>(
        currentNodeId, ConversationNode::ROLE_SYSTEM, systemPrompt, "");
    saveConversation();
}

void AI::loadConversation(const std::string &conversationId)
{
    conversationManager.loadConversation(conversationId, nodeMap, rootNodeId);
    this->conversationId = conversationId;
    currentNodeId = rootNodeId;
    while (!nodeMap[currentNodeId]->childIds.empty())
        currentNodeId = nodeMap[currentNodeId]->childIds.back();
}

void AI::deleteConversation(const std::string &conversationId)
{
    conversationManager.deleteConversation(conversationId);
    if (this->conversationId == conversationId)
        createConversation("默认对话");
}

void AI::updateConversationTitle(const std::string &conversationId, const std::string &title)
{
    conversationManager.updateConversationTitle(conversationId, title);
}

void AI::setSettings(const std::string &apiKey, const std::string &baseUrl,
                     const std::string &model, int maxTokens,
                     double temperature, double topP, std::string systemPrompt)
{
    this->apiKey = apiKey, this->baseUrl = baseUrl;
    this->model = model, this->maxTokens = maxTokens;
    this->temperature = temperature, this->topP = topP, this->systemPrompt = systemPrompt;
    conversationManager.saveApiSettings(apiKey, baseUrl, model, maxTokens, temperature, topP, systemPrompt);
}
SettingsResponse AI::getSettings() const
{
    return SettingsResponse(true, 0, "",
                            apiKey, baseUrl,
                            model, maxTokens,
                            temperature, topP, systemPrompt);
}

ChatCompletionResponse AI::generateResponse(AIStreamCallback streamCallback)
{
    ChatCompletionResponse chatResponse(false, 0, "Unknown error");
    nlohmann::json requestJson;
    requestJson["model"] = model;
    requestJson["max_tokens"] = maxTokens;
    requestJson["temperature"] = temperature;
    requestJson["top_p"] = topP;
    requestJson["stream"] = true;

    const std::string_view roleString[3] = {"user", "assistant", "system"};
    nlohmann::json messagesArray = nlohmann::json::array();
    for (const auto &msg : getPathFromRoot(currentNodeId))
        messagesArray.push_back({{"role", roleString[msg.role]},
                                 {"content", msg.content}});
    requestJson["messages"] = messagesArray;

    std::string fullAssistantResponse;

    StreamCallback packedStreamCallback = [&fullAssistantResponse, streamCallback](std::string chunk)
    {
        if (chunk.empty() || chunk == "[DONE]")
            return;
        AIStreamResult result;
        try
        {
            nlohmann::json chunkJson = nlohmann::json::parse(chunk);
            auto choice = chunkJson["choices"][0];
            nlohmann::json content = choice["delta"]["content"];
            if (choice["finish_reason"].is_string())
            {
                std::string finishReason = choice["finish_reason"];
                if (finishReason == "stop")
                    result.type = AIStreamResult::DONE;
                else if (finishReason == "length")
                    result.type = AIStreamResult::LENGTH;
                else if (finishReason == "content_filter")
                {
                    result.type = AIStreamResult::ERROR;
                    result.errorMessage = "Content filter triggered.";
                }
                else if (finishReason == "tool_calls")
                {
                    result.type = AIStreamResult::ERROR;
                    result.errorMessage = "Tool calls not supported in this context.";
                }
                else if (finishReason == "insufficient_system_resource")
                {
                    result.type = AIStreamResult::ERROR;
                    result.errorMessage = "Insufficient system resources.";
                }
                else
                {
                    result.type = AIStreamResult::ERROR;
                    result.errorMessage = "Unknown finish reason: " + finishReason;
                }
            }
            else if (content.is_string())
            {
                fullAssistantResponse += content;
                result.type = AIStreamResult::MESSAGE;
                result.messageDelta = content;
            }
        }
        catch (const nlohmann::json::parse_error &e)
        {
            result.type = AIStreamResult::ERROR;
            result.errorMessage = "JSON parse error: " + std::string(e.what());
        }
        streamCallback(result);
    };

    Response response = Fetch::fetch(baseUrl + "chat/completions",
                                     FetchOptions("POST",
                                                  {{"Content-Type", "application/json"},
                                                   {"Authorization", "Bearer " + apiKey},
                                                   {"Accept", "text/event-stream"}},
                                                  requestJson.dump(),
                                                  true,
                                                  packedStreamCallback));
    chatResponse.statusCode = response.status;
    if (response.isOk())
    {
        addNode(ConversationNode::ROLE_ASSISTANT, fullAssistantResponse);
        chatResponse.success = true;
        chatResponse.content = fullAssistantResponse;
    }
    else
    {
        deleteNode(currentNodeId);
        chatResponse.errorMessage = "Send message failed with status: " + std::to_string(response.status);
    }
    return chatResponse;
}

ModelsResponse AI::getModels()
{
    ModelsResponse modelsResponse(false, 0, "Unknown error");
    Response response = Fetch::fetch(baseUrl + "models",
                                     FetchOptions("GET",
                                                  {{"Authorization", "Bearer " + apiKey}}));
    modelsResponse.statusCode = response.status;
    if (response.isOk())
    {
        nlohmann::json responseJson = response.json();
        modelsResponse.success = true;
        modelsResponse.models.clear();
        for (const auto &model : responseJson["data"])
            modelsResponse.models.push_back(model["id"].get<std::string>());
    }
    else
        modelsResponse.errorMessage = "Get models failed with status: " + std::to_string(response.status);
    return modelsResponse;
}

UserBalanceResponse AI::getUserBalance()
{
    UserBalanceResponse balanceResponse(false, 0, "Unknown error");
    Response response = Fetch::fetch(baseUrl + "user/balance",
                                     FetchOptions("GET",
                                                  {{"Authorization", "Bearer " + apiKey}}));
    balanceResponse.statusCode = response.status;
    if (response.isOk())
    {
        nlohmann::json responseJson = response.json();
        balanceResponse.isAvailable = responseJson.value("is_available", false);
        for (const auto &balanceInfo : responseJson["balance_infos"])
            if (balanceInfo["currency"].get<std::string>() == "CNY")
            {
                balanceResponse.success = balanceResponse.isAvailable = true;
                balanceResponse.balance = std::atof(std::string(balanceInfo["total_balance"]).c_str());
                break;
            }
    }
    else
        balanceResponse.errorMessage = "Get user balance failed with status: " + std::to_string(response.status);
    return balanceResponse;
}
