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

#include <AI.hpp>
#include <iostream>
#include <sstream>
#include <regex>
#include <chrono>
#include <random>

AI::AI(std::string apiKey, std::string baseUrl) : apiKey(apiKey), baseUrl(baseUrl) 
{
    // 初始化根节点
    rootNodeId = generateNodeId();
    ConversationNode rootNode(rootNodeId, "system", "conversation_start", "");
    rootNode.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    conversationTree.push_back(rootNode);
    currentNodeId = rootNodeId;
}

std::string AI::generateNodeId()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 16; i++) {
        ss << dis(gen);
    }
    return ss.str();
}

ConversationNode* AI::findNode(const std::string& nodeId)
{
    for (auto& node : conversationTree) {
        if (node.id == nodeId) {
            return &node;
        }
    }
    return nullptr;
}

std::vector<AIMessage> AI::getPathFromRoot(const std::string& nodeId)
{
    std::vector<AIMessage> path;
    std::string currentId = nodeId;
    
    // 从目标节点向上追溯到根节点
    std::vector<ConversationNode*> nodes;
    while (!currentId.empty()) {
        ConversationNode* node = findNode(currentId);
        if (!node) break;
        
        if (node->role != "system" || node->content != "conversation_start") {
            nodes.push_back(node);
        }
        currentId = node->parentId;
    }
    
    // 反转得到从根到目标的路径
    for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
        path.emplace_back((*it)->role, (*it)->content);
    }
    
    return path;
}

void AI::addMessage(std::string role, std::string content) 
{ 
    addMessageToNode(role, content, currentNodeId);
}

std::string AI::addMessageToNode(std::string role, std::string content, const std::string& parentId)
{
    std::string nodeId = generateNodeId();
    std::string actualParentId = parentId.empty() ? currentNodeId : parentId;
    
    ConversationNode newNode(nodeId, role, content, actualParentId);
    newNode.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // 添加到父节点的子节点列表
    ConversationNode* parent = findNode(actualParentId);
    if (parent) {
        parent->childIds.push_back(nodeId);
    }
    
    conversationTree.push_back(newNode);
    currentNodeId = nodeId;  // 更新当前位置
    
    return nodeId;
}

bool AI::switchToNode(const std::string& nodeId)
{
    ConversationNode* node = findNode(nodeId);
    if (node) {
        currentNodeId = nodeId;
        return true;
    }
    return false;
}

std::vector<std::string> AI::getChildNodes(const std::string& nodeId)
{
    ConversationNode* node = findNode(nodeId);
    if (node) {
        return node->childIds;
    }
    return {};
}

std::vector<AIMessage> AI::getCurrentPath()
{
    return getPathFromRoot(currentNodeId);
}

std::vector<AIMessage> AI::getPathToNode(const std::string& nodeId)
{
    return getPathFromRoot(nodeId);
}

ChatCompletionResponse AI::sendMessage(std::string userMessage, AIStreamCallback callback)
{
    try
    {
        addMessage("user", userMessage);

        // 构建请求体
        nlohmann::json requestJson;
        requestJson["model"] = model;
        requestJson["max_tokens"] = maxTokens;
        requestJson["temperature"] = temperature;
        requestJson["top_p"] = topP;
        requestJson["stream"] = (callback != nullptr);

        // 添加消息历史
        nlohmann::json messagesArray = nlohmann::json::array();
        for (const auto &msg : getCurrentPath())
            messagesArray.push_back(msg.toJson());
        requestJson["messages"] = messagesArray;

        if (callback)
        {
            // 流式处理
            std::string fullAssistantResponse;

            StreamCallback streamCallback = [&fullAssistantResponse, callback](std::string chunk)
            {
                // 内联 parseStreamChunk 功能
                if (chunk.empty() || chunk == "[DONE]")
                    return;

                try
                {
                    nlohmann::json chunkJson = nlohmann::json::parse(chunk);
                    if (chunkJson.contains("choices") && !chunkJson["choices"].empty())
                    {
                        auto choice = chunkJson["choices"][0];
                        if (choice.contains("delta") && choice["delta"].contains("content"))
                        {
                            std::string content = choice["delta"]["content"];
                            if (!content.empty())
                            {
                                fullAssistantResponse += content;
                                callback(content);
                            }
                        }
                    }
                }
                catch (...)
                {
                    // 忽略解析错误
                }
            };

            // 发送流式请求
            FetchOptions options("POST",
                                 {{"Content-Type", "application/json"},
                                  {"Authorization", "Bearer " + apiKey},
                                  {"Accept", "text/event-stream"}},
                                 requestJson.dump(),
                                 true,
                                 streamCallback);

            Response response = Fetch::fetch(baseUrl + "chat/completions", options);

            if (response.isOk())
            {
                if (!fullAssistantResponse.empty())
                    addMessage("assistant", fullAssistantResponse);

                ChatCompletionResponse chatResponse(true, response.status);
                chatResponse.content = fullAssistantResponse;
                chatResponse.model = model;
                return chatResponse;
            }
            else
            {
                // 流式请求失败，移除刚添加的用户消息
                if (!conversationTree.empty() && conversationTree.back().role == "user")
                {
                    // 从父节点的子节点列表中移除
                    ConversationNode* parent = findNode(conversationTree.back().parentId);
                    if (parent && !parent->childIds.empty())
                        parent->childIds.pop_back();
                    
                    conversationTree.pop_back();
                    currentNodeId = conversationTree.empty() ? rootNodeId : conversationTree.back().parentId;
                }

                std::string errorMsg = "Stream request failed with status: " + std::to_string(response.status);
                return ChatCompletionResponse(false, response.status, errorMsg);
            }
        }
        else
        {
            // 非流式处理
            Response response = Fetch::fetch(baseUrl + "chat/completions",
                                             FetchOptions("POST",
                                                          {{"Content-Type", "application/json"},
                                                           {"Authorization", "Bearer " + apiKey}},
                                                          requestJson.dump()));

            if (response.isOk())
            {
                nlohmann::json responseJson = response.json();

                // 提取响应信息
                if (responseJson.contains("choices") && !responseJson["choices"].empty())
                {
                    auto choice = responseJson["choices"][0];
                    if (choice.contains("message") && choice["message"].contains("content"))
                    {
                        std::string assistantContent = choice["message"]["content"];
                        addMessage("assistant", assistantContent);

                        ChatCompletionResponse chatResponse(true, response.status);
                        chatResponse.id = responseJson.value("id", "");
                        chatResponse.model = responseJson.value("model", "");
                        chatResponse.content = assistantContent;
                        chatResponse.created = responseJson.value("created", 0);
                        chatResponse.systemFingerprint = responseJson.value("system_fingerprint", "");
                        if (responseJson.contains("usage"))
                            chatResponse.usage = responseJson["usage"];

                        return chatResponse;
                    }
                }

                return ChatCompletionResponse(false, response.status, "Invalid response format");
            }
            else
            {
                // 请求失败，移除用户消息
                if (!conversationTree.empty() && conversationTree.back().role == "user")
                {
                    // 从父节点的子节点列表中移除
                    ConversationNode* parent = findNode(conversationTree.back().parentId);
                    if (parent && !parent->childIds.empty())
                        parent->childIds.pop_back();
                    
                    conversationTree.pop_back();
                    currentNodeId = conversationTree.empty() ? rootNodeId : conversationTree.back().parentId;
                }

                std::string errorMsg = "Request failed with status: " + std::to_string(response.status);
                try
                {
                    nlohmann::json errorJson = response.json();
                    if (errorJson.contains("error") && errorJson["error"].contains("message"))
                    {
                        errorMsg = errorJson["error"]["message"];
                    }
                }
                catch (...)
                {
                    errorMsg += ". Response: " + response.text();
                }

                return ChatCompletionResponse(false, response.status, errorMsg);
            }
        }
    }
    catch (const std::exception &e)
    {
        // 异常时移除用户消息
        if (!conversationTree.empty() && conversationTree.back().role == "user")
        {
            // 从父节点的子节点列表中移除
            ConversationNode* parent = findNode(conversationTree.back().parentId);
            if (parent && !parent->childIds.empty())
                parent->childIds.pop_back();
            
            conversationTree.pop_back();
            currentNodeId = conversationTree.empty() ? rootNodeId : conversationTree.back().parentId;
        }

        return ChatCompletionResponse(false, 0, "Exception: " + std::string(e.what()));
    }
}

std::vector<AIMessage> AI::getMessageHistory() 
{ 
    return getCurrentPath(); 
}

std::vector<ConversationNode> AI::getFullConversationTree()
{
    return conversationTree;
}

nlohmann::json AI::exportConversationTree()
{
    nlohmann::json treeJson;
    treeJson["currentNodeId"] = currentNodeId;
    treeJson["rootNodeId"] = rootNodeId;
    treeJson["nodes"] = nlohmann::json::array();
    
    for (const auto& node : conversationTree) {
        treeJson["nodes"].push_back(node.toJson());
    }
    
    return treeJson;
}

bool AI::importConversationTree(const nlohmann::json& treeData)
{
    try {
        if (!treeData.contains("nodes") || !treeData.contains("rootNodeId")) {
            return false;
        }
        
        conversationTree.clear();
        
        for (const auto& nodeJson : treeData["nodes"]) {
            ConversationNode node(
                nodeJson.value("id", ""),
                nodeJson.value("role", ""),
                nodeJson.value("content", ""),
                nodeJson.value("parentId", "")
            );
            
            if (nodeJson.contains("childIds")) {
                for (const auto& childId : nodeJson["childIds"]) {
                    node.childIds.push_back(childId);
                }
            }
            
            node.timestamp = nodeJson.value("timestamp", 0);
            conversationTree.push_back(node);
        }
        
        rootNodeId = treeData.value("rootNodeId", "");
        currentNodeId = treeData.value("currentNodeId", rootNodeId);
        
        return true;
    } catch (...) {
        return false;
    }
}

ModelsResponse AI::getModels()
{
    try
    {
        FetchOptions options("GET",
                             {{"Authorization", "Bearer " + apiKey}});

        Response response = Fetch::fetch(baseUrl + "models", options);

        if (response.isOk())
        {
            nlohmann::json responseJson = response.json();
            ModelsResponse modelsResponse(true, response.status);

            if (responseJson.contains("data") && responseJson["data"].is_array())
            {
                for (const auto &model : responseJson["data"])
                {
                    modelsResponse.models.push_back(model);
                }
            }

            return modelsResponse;
        }
        else
        {
            std::string errorMsg = "Get models failed with status: " + std::to_string(response.status);
            return ModelsResponse(false, response.status, errorMsg);
        }
    }
    catch (const std::exception &e)
    {
        return ModelsResponse(false, 0, "Get models exception: " + std::string(e.what()));
    }
}

UserBalanceResponse AI::getUserBalance()
{
    try
    {
        FetchOptions options("GET",
                             {{"Authorization", "Bearer " + apiKey}});

        Response response = Fetch::fetch(baseUrl + "user/balance", options);

        if (response.isOk())
        {
            nlohmann::json responseJson = response.json();
            UserBalanceResponse balanceResponse(true, response.status);

            balanceResponse.isAvailable = responseJson.value("is_available", false);

            if (responseJson.contains("balance_infos") && responseJson["balance_infos"].is_array())
            {
                for (const auto &balanceInfo : responseJson["balance_infos"])
                {
                    balanceResponse.balanceInfos.push_back(balanceInfo);
                }
            }

            return balanceResponse;
        }
        else
        {
            std::string errorMsg = "Get user balance failed with status: " + std::to_string(response.status);
            return UserBalanceResponse(false, response.status, errorMsg);
        }
    }
    catch (const std::exception &e)
    {
        return UserBalanceResponse(false, 0, "Get user balance exception: " + std::string(e.what()));
    }
}
