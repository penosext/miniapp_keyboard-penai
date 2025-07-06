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

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Database/Database.hpp"
#include "ConversationNode.hpp"
#include "ConversationInfo.hpp"

class ConversationManager
{
private:
    DATABASE database;

public:
    ConversationManager();
    ~ConversationManager() = default;

    ConversationListResponse getConversationList();
    void createConversation(const std::string &title, std::string &outConversationId);
    void deleteConversation(const std::string &conversationId);
    void updateConversationTitle(const std::string &conversationId, const std::string &title);

    void saveConversation(const std::string &conversationId,
                          const std::unordered_map<std::string, std::unique_ptr<ConversationNode>> &nodeMap);
    void loadConversation(const std::string &conversationId,
                          std::unordered_map<std::string, std::unique_ptr<ConversationNode>> &nodeMap,
                          std::string &rootNodeId);

    void saveApiSettings(const std::string &apiKey, const std::string &baseUrl,
                         const std::string &model, int maxTokens,
                         double temperature, double topP, const std::string &systemPrompt);
    void loadApiSettings(std::string &apiKey, std::string &baseUrl,
                         std::string &model, int &maxTokens,
                         double &temperature, double &topP, std::string &systemPrompt);
};
