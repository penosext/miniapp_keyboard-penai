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
    std::lock_guard<std::mutex> settingsLock(settingsMutex);
    std::lock_guard<std::mutex> conversationLock(conversationMutex);

    conversationManager.loadApiSettings(apiKey, baseUrl, model, maxTokens, temperature, topP, systemPrompt);

    auto conversationsResponse = conversationManager.getConversationList();
    if (conversationsResponse.empty())
    {
        conversationManager.createConversation("默认对话", conversationId);

        std::unique_lock<std::shared_mutex> stateLock(stateMutex);
        currentNodeId = rootNodeId = strUtils::randomId();
        nodeMap[currentNodeId] = std::make_unique<ConversationNode>(
            currentNodeId, ConversationNode::ROLE_SYSTEM, systemPrompt, "");
        stateLock.unlock();
        saveConversation();
    }
    else
    {
        conversationId = conversationsResponse[0].id;
        std::unique_lock<std::shared_mutex> stateLock(stateMutex);
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
    std::unique_lock<std::shared_mutex> stateLock(stateMutex);
    std::string nodeId = strUtils::randomId();
    ConversationNode *parent = findNode(currentNodeId);
    if (parent)
        parent->childIds.push_back(nodeId);
    nodeMap[nodeId] = std::make_unique<ConversationNode>(nodeId, role, content, currentNodeId);
    currentNodeId = nodeId;
    stateLock.unlock();
    saveConversation();
}

bool AI::deleteNode(const std::string &nodeId)
{
    std::unique_lock<std::shared_mutex> stateLock(stateMutex);
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
    stateLock.unlock();
    saveConversation();
    return true;
}

bool AI::switchNode(const std::string &nodeId)
{
    std::unique_lock<std::shared_mutex> stateLock(stateMutex);
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
    std::shared_lock<std::shared_mutex> stateLock(stateMutex);
    ConversationNode *node = findNode(nodeId);
    if (node)
        return node->childIds;
    return {};
}

std::vector<ConversationNode> AI::getCurrentPath()
{
    std::shared_lock<std::shared_mutex> stateLock(stateMutex);
    return getPathFromRoot(currentNodeId);
}
std::string AI::getCurrentNodeId() const
{
    std::shared_lock<std::shared_mutex> stateLock(stateMutex);
    return currentNodeId;
}
std::string AI::getRootNodeId() const
{
    std::shared_lock<std::shared_mutex> stateLock(stateMutex);
    return rootNodeId;
}
std::string AI::getConversationId() const
{
    std::shared_lock<std::shared_mutex> stateLock(stateMutex);
    return conversationId;
}

void AI::saveConversation()
{
    std::shared_lock<std::shared_mutex> stateLock(stateMutex);
    if (!conversationId.empty())
    {
        conversationManager.saveConversation(conversationId, nodeMap);
    }
}

std::vector<ConversationInfo> AI::getConversationList()
{
    std::lock_guard<std::mutex> conversationLock(conversationMutex);
    return conversationManager.getConversationList();
}

void AI::createConversation(const std::string &title)
{
    std::lock_guard<std::mutex> conversationLock(conversationMutex);
    std::string newConversationId;
    conversationManager.createConversation(title, newConversationId);

    {
        std::unique_lock<std::shared_mutex> stateLock(stateMutex);
        conversationId = newConversationId;
        nodeMap.clear();

        std::lock_guard<std::mutex> settingsLock(settingsMutex);
        currentNodeId = rootNodeId = strUtils::randomId();
        nodeMap[currentNodeId] = std::make_unique<ConversationNode>(
            currentNodeId, ConversationNode::ROLE_SYSTEM, systemPrompt, "");
    }
    saveConversation();
}

void AI::loadConversation(const std::string &conversationId)
{
    std::lock_guard<std::mutex> conversationLock(conversationMutex);
    std::unique_lock<std::shared_mutex> stateLock(stateMutex);
    this->conversationId = conversationId;
    conversationManager.loadConversation(conversationId, nodeMap, rootNodeId, currentNodeId);
}

void AI::deleteConversation(const std::string &conversationId)
{
    std::lock_guard<std::mutex> conversationLock(conversationMutex);
    conversationManager.deleteConversation(conversationId);

    std::unique_lock<std::shared_mutex> stateLock(stateMutex);
    if (this->conversationId == conversationId)
    {
        auto conversations = conversationManager.getConversationList();
        if (!conversations.empty())
        {
            this->conversationId = conversations[0].id;
            conversationManager.loadConversation(this->conversationId, nodeMap, rootNodeId, currentNodeId);
        }
        else
        {
            stateLock.unlock();
            createConversation("默认对话");
        }
    }
}

void AI::updateConversationTitle(const std::string &conversationId, const std::string &title)
{
    std::lock_guard<std::mutex> conversationLock(conversationMutex);
    conversationManager.updateConversationTitle(conversationId, title);
}

void AI::setSettings(const std::string &apiKey, const std::string &baseUrl,
                     const std::string &model, int maxTokens,
                     double temperature, double topP, std::string systemPrompt)
{
    std::lock_guard<std::mutex> settingsLock(settingsMutex);
    this->apiKey = apiKey, this->baseUrl = baseUrl;
    this->model = model, this->maxTokens = maxTokens;
    this->temperature = temperature, this->topP = topP, this->systemPrompt = systemPrompt;
    conversationManager.saveApiSettings(apiKey, baseUrl, model, maxTokens, temperature, topP, systemPrompt);
}
SettingsResponse AI::getSettings() const
{
    std::lock_guard<std::mutex> settingsLock(settingsMutex);
    return SettingsResponse(apiKey, baseUrl,
                            model, maxTokens,
                            temperature, topP, systemPrompt);
}

std::string AI::generateResponse(AIStreamCallback streamCallback)
{
    nlohmann::json requestJson;
    {
        std::lock_guard<std::mutex> settingsLock(settingsMutex);
        requestJson["model"] = model;
        requestJson["max_tokens"] = maxTokens;
        requestJson["temperature"] = temperature;
        requestJson["top_p"] = topP;
    }

    requestJson["stream"] = true;

    const std::string_view roleString[3] = {"user", "assistant", "system"};
    nlohmann::json messagesArray = nlohmann::json::array();

    {
        std::shared_lock<std::shared_mutex> stateLock(stateMutex);
        for (const auto &msg : getPathFromRoot(currentNodeId))
            messagesArray.push_back({{"role", roleString[msg.role]},
                                     {"content", msg.content}});
    }

    requestJson["messages"] = messagesArray;

    std::string fullAssistantResponse;
    std::mutex responseMutex; // Protect fullAssistantResponse from concurrent access

    StreamCallback packedStreamCallback = [&fullAssistantResponse, &responseMutex, streamCallback](std::string chunk)
    {
        if (chunk.empty() || chunk == "[DONE]")
            return;
        AIStreamResult result;
        nlohmann::json chunkJson = nlohmann::json::parse(chunk);
        auto choice = chunkJson["choices"][0];
        std::string content = "";
        if (choice["delta"]["reasoning_content"].is_string())
            content += choice["delta"]["reasoning_content"];
        if (choice["delta"]["content"].is_string())
            content += choice["delta"]["content"];

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
        else
        {
            {
                std::lock_guard<std::mutex> lock(responseMutex);
                fullAssistantResponse += content;
            }
            result.type = AIStreamResult::MESSAGE;
            result.messageDelta = content;
        }
        streamCallback(result);
    };

    std::string currentApiKey, currentBaseUrl;
    {
        std::lock_guard<std::mutex> settingsLock(settingsMutex);
        currentApiKey = apiKey;
        currentBaseUrl = baseUrl;
    }

    Response response = Fetch::fetch(currentBaseUrl + "chat/completions",
                                     FetchOptions("POST",
                                                  {{"Content-Type", "application/json"},
                                                   {"Authorization", "Bearer " + currentApiKey},
                                                   {"Accept", "text/event-stream"}},
                                                  requestJson.dump(),
                                                  true,
                                                  packedStreamCallback,
                                                  0));
    if (!response.isOk())
        THROW_NETWORK_ERROR(response.status);

    {
        std::lock_guard<std::mutex> lock(responseMutex);
        addNode(ConversationNode::ROLE_ASSISTANT, fullAssistantResponse);
        return fullAssistantResponse;
    }
}

std::vector<std::string> AI::getModels()
{
    std::string currentApiKey, currentBaseUrl;
    {
        std::lock_guard<std::mutex> settingsLock(settingsMutex);
        currentApiKey = apiKey;
        currentBaseUrl = baseUrl;
    }

    std::vector<std::string> modelIds;
    Response response = Fetch::fetch(currentBaseUrl + "models",
                                     FetchOptions("GET",
                                                  {{"Authorization", "Bearer " + currentApiKey}}));
    if (!response.isOk())
        THROW_NETWORK_ERROR(response.status);
    nlohmann::json responseJson = response.json();
    for (const auto &model : responseJson.at("data"))
        modelIds.push_back(model.at("id"));
    return modelIds;
}

float AI::getUserBalance()
{
    std::string currentApiKey, currentBaseUrl;
    {
        std::lock_guard<std::mutex> settingsLock(settingsMutex);
        currentApiKey = apiKey;
        currentBaseUrl = baseUrl;
    }

    Response response = Fetch::fetch(currentBaseUrl + "user/balance",
                                     FetchOptions("GET",
                                                  {{"Authorization", "Bearer " + currentApiKey}}));
    if (!response.isOk())
        THROW_NETWORK_ERROR(response.status);
    nlohmann::json responseJson = response.json();
    for (const auto &balanceInfo : responseJson.at("balance_infos"))
        if (balanceInfo.at("currency") == "CNY")
            return std::atof(std::string(balanceInfo.at("total_balance")).c_str());
    return 0.0f;
}
