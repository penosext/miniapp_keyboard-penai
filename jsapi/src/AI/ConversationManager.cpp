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

#include "ConversationManager.hpp"
#include "strUtils.hpp"
#include <chrono>
#include <algorithm>
#include <stdexcept>

ConversationManager::ConversationManager() : database("/userdisk/database/langningchen-ai.db")
{
    database.table("conversations")
        .column("id", TABLE::TEXT, TABLE::PRIMARY_KEY)
        .column("title", TABLE::TEXT, TABLE::NOT_NULL)
        .column("created_at", TABLE::INTEGER, TABLE::NOT_NULL)
        .column("updated_at", TABLE::INTEGER, TABLE::NOT_NULL)
        .execute();
    database.table("conversation_nodes")
        .column("id", TABLE::TEXT, TABLE::PRIMARY_KEY)
        .column("conversation_id", TABLE::TEXT, TABLE::NOT_NULL)
        .column("parent_id", TABLE::TEXT)
        .column("role", TABLE::INTEGER, TABLE::NOT_NULL)
        .column("content", TABLE::TEXT, TABLE::NOT_NULL)
        .column("stop_reason", TABLE::INTEGER, TABLE::NOT_NULL)
        .column("created_at", TABLE::INTEGER, TABLE::NOT_NULL)
        .execute();
    database.table("api_settings")
        .column("id", TABLE::TEXT, TABLE::PRIMARY_KEY)
        .column("api_key", TABLE::TEXT, TABLE::NOT_NULL)
        .column("base_url", TABLE::TEXT, TABLE::NOT_NULL)
        .column("model", TABLE::TEXT)
        .column("max_tokens", TABLE::INTEGER, TABLE::NOT_NULL)
        .column("temperature", TABLE::REAL, TABLE::NOT_NULL)
        .column("top_p", TABLE::REAL, TABLE::NOT_NULL)
        .column("system_prompt", TABLE::TEXT, TABLE::NOT_NULL)
        .execute();
}

std::vector<ConversationInfo> ConversationManager::getConversationList()
{
    std::lock_guard<std::mutex> lock(dbMutex);
    std::vector<ConversationInfo> conversations;
    auto results = database.select("conversations")
                       .select("id")
                       .select("title")
                       .select("created_at")
                       .select("updated_at")
                       .order("updated_at", false)
                       .execute();
    for (const auto &row : results)
        conversations.push_back(ConversationInfo(
            row.at("id"),
            row.at("title"),
            std::stoll(row.at("created_at")),
            std::stoll(row.at("updated_at"))));
    return conversations;
}

void ConversationManager::createConversation(const std::string &title, std::string &outConversationId)
{
    std::lock_guard<std::mutex> lock(dbMutex);
    outConversationId = strUtils::randomId();
    auto currentTime = std::chrono::duration_cast<std::chrono::seconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();
    database.insert("conversations")
        .value("id", outConversationId)
        .value("title", title)
        .value("created_at", currentTime)
        .value("updated_at", currentTime)
        .execute();
}
void ConversationManager::deleteConversation(const std::string &conversationId)
{
    std::lock_guard<std::mutex> lock(dbMutex);
    database.remove("conversation_nodes")
        .where("conversation_id", conversationId)
        .execute();
    database.remove("conversations")
        .where("id", conversationId)
        .execute();
}
void ConversationManager::updateConversationTitle(const std::string &conversationId, const std::string &title)
{
    std::lock_guard<std::mutex> lock(dbMutex);
    database.update("conversations")
        .set("title", title)
        .where("id", conversationId)
        .execute();
}

void ConversationManager::saveConversation(const std::string &conversationId,
                                           const std::unordered_map<std::string, std::unique_ptr<ConversationNode>> &nodeMap)
{
    std::lock_guard<std::mutex> lock(dbMutex);
    auto currentTime = std::chrono::duration_cast<std::chrono::seconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();

    database.update("conversations")
        .set("updated_at", currentTime)
        .where("id", conversationId)
        .execute();

    database.remove("conversation_nodes")
        .where("conversation_id", conversationId)
        .execute();

    for (const auto &pair : nodeMap)
    {
        const auto &node = pair.second;
        if (!node)
            continue;

        database.insert("conversation_nodes")
            .value("id", node->id)
            .value("conversation_id", conversationId)
            .value("parent_id", node->parentId)
            .value("role", (int)node->role)
            .value("content", node->content)
            .value("stop_reason", (int)node->stopReason)
            .value("created_at", currentTime)
            .execute();
    }
}
void ConversationManager::loadConversation(const std::string &conversationId,
                                           std::unordered_map<std::string, std::unique_ptr<ConversationNode>> &nodeMap,
                                           std::string &rootNodeId, std::string &leafNodeId)
{
    std::lock_guard<std::mutex> lock(dbMutex);
    nodeMap.clear();
    rootNodeId.clear();

    auto nodeResults = database.select("conversation_nodes")
                           .where("conversation_id", conversationId)
                           .execute();

    std::unordered_map<std::string, std::vector<std::string>> parentToChildren;

    for (const auto &row : nodeResults)
    {
        std::string nodeId = row.at("id");
        std::string parentId = row.at("parent_id");
        int role = std::stoi(row.at("role"));
        std::string content = row.at("content");
        int stopReason = row.count("stop_reason") ? std::stoi(row.at("stop_reason")) : 6; // Default to STOP_REASON_NONE

        nodeMap[nodeId] = std::make_unique<ConversationNode>(
            nodeId, static_cast<ConversationNode::ROLE>(role), content, parentId, static_cast<ConversationNode::STOP_REASON>(stopReason));

        if (!parentId.empty())
            parentToChildren[parentId].push_back(nodeId);
        else
            rootNodeId = nodeId;
    }

    for (const auto &pair : parentToChildren)
        if (nodeMap.find(pair.first) != nodeMap.end())
            nodeMap[pair.first]->childIds = pair.second;

    leafNodeId = rootNodeId;
    while (!nodeMap[leafNodeId]->childIds.empty())
        leafNodeId = nodeMap[leafNodeId]->childIds.back();
}

void ConversationManager::saveApiSettings(const std::string &apiKey, const std::string &baseUrl,
                                          const std::string &model, int maxTokens,
                                          double temperature, double topP, const std::string &systemPrompt)
{
    std::lock_guard<std::mutex> lock(dbMutex);
    database.remove("api_settings").execute();
    database.insert("api_settings")
        .value("id", "default")
        .value("api_key", apiKey)
        .value("base_url", baseUrl)
        .value("model", model)
        .value("max_tokens", maxTokens)
        .value("temperature", temperature)
        .value("top_p", topP)
        .value("system_prompt", systemPrompt)
        .execute();
}

void ConversationManager::loadApiSettings(std::string &apiKey, std::string &baseUrl,
                                          std::string &model, int &maxTokens,
                                          double &temperature, double &topP, std::string &systemPrompt)
{
    std::lock_guard<std::mutex> lock(dbMutex);
    auto results = database.select("api_settings")
                       .where("id", "default")
                       .execute();

    if (!results.empty())
    {
        const auto &row = results[0];
        apiKey = row.at("api_key");
        baseUrl = row.at("base_url");
        model = row.at("model");
        maxTokens = std::stoi(row.at("max_tokens"));
        temperature = std::stod(row.at("temperature"));
        topP = std::stod(row.at("top_p"));
        systemPrompt = row.at("system_prompt");
    }
}
