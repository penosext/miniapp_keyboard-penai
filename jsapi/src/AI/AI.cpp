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
#include <Exceptions/NetworkError.hpp>
#include <iostream>
#include <sstream>
#include <regex>
#include <chrono>

AI::AI()
{
    conversationManager.loadApiSettings(apiKey, baseUrl, model, maxTokens, temperature, topP, systemPrompt);

    auto conversationsResponse = conversationManager.getConversationList();
    if (conversationsResponse.empty())
    {
        conversationManager.createConversation("默认对话", conversationId);

        currentNodeId = rootNodeId = strUtils::randomId();
        nodeMap[currentNodeId] = std::make_unique<ConversationNode>(
            currentNodeId, ConversationNode::ROLE_SYSTEM, systemPrompt, "");
        saveConversation();
    }
    else
    {
        conversationId = conversationsResponse[0].id;
        conversationManager.loadConversation(conversationId, nodeMap, rootNodeId, currentNodeId);
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

std::vector<ConversationInfo> AI::getConversationList()
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
    this->conversationId = conversationId;
    conversationManager.loadConversation(conversationId, nodeMap, rootNodeId, currentNodeId);
}

void AI::deleteConversation(const std::string &conversationId)
{
    conversationManager.deleteConversation(conversationId);
    if (this->conversationId == conversationId)
    {
        auto conversations = conversationManager.getConversationList();
        if (!conversations.empty())
        {
            this->conversationId = conversations[0].id;
            conversationManager.loadConversation(this->conversationId, nodeMap, rootNodeId, currentNodeId);
        }
        else
            createConversation("默认对话");
    }
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
    return SettingsResponse(apiKey, baseUrl,
                            model, maxTokens,
                            temperature, topP, systemPrompt);
}

std::string AI::generateResponse(AIStreamCallback streamCallback)
{
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
                ASSERT(false);
        }
        else if (content.is_string())
        {
            fullAssistantResponse += content;
            result.type = AIStreamResult::MESSAGE;
            result.messageDelta = content;
        }
        else
            ASSERT(false);
        streamCallback(result);
    };

    Response response = Fetch::fetch(baseUrl + "chat/completions",
                                     FetchOptions("POST",
                                                  {{"Content-Type", "application/json"},
                                                   {"Authorization", "Bearer " + apiKey},
                                                   {"Accept", "text/event-stream"}},
                                                  requestJson.dump(),
                                                  true,
                                                  packedStreamCallback,
                                                  0));
    if (!response.isOk())
        THROW_NETWORK_ERROR(response.status);
    addNode(ConversationNode::ROLE_ASSISTANT, fullAssistantResponse);
    return fullAssistantResponse;
}

std::vector<std::string> AI::getModels()
{
    std::vector<std::string> modelIds;
    Response response = Fetch::fetch(baseUrl + "models",
                                     FetchOptions("GET",
                                                  {{"Authorization", "Bearer " + apiKey}}));
    if (!response.isOk())
        THROW_NETWORK_ERROR(response.status);
    nlohmann::json responseJson = response.json();
    for (const auto &model : responseJson.at("data"))
        modelIds.push_back(model.at("id"));
    return modelIds;
}

float AI::getUserBalance()
{
    Response response = Fetch::fetch(baseUrl + "user/balance",
                                     FetchOptions("GET",
                                                  {{"Authorization", "Bearer " + apiKey}}));
    if (!response.isOk())
        THROW_NETWORK_ERROR(response.status);
    nlohmann::json responseJson = response.json();
    for (const auto &balanceInfo : responseJson.at("balance_infos"))
        if (balanceInfo.at("currency") == "CNY")
            return std::atof(std::string(balanceInfo.at("total_balance")).c_str());
    return 0.0f;
}
