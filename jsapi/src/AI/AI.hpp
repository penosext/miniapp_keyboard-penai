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
#include <functional>
#include <memory>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "Fetch.hpp"
#include "ConversationNode.hpp"
#include "ChatCompletionResponse.hpp"
#include "ModelsResponse.hpp"
#include "UserBalanceResponse.hpp"
#include "AICallback.hpp"

class AI
{
private:
    std::string apiKey, baseUrl;
    std::string model = "deepseek-chat";
    int maxTokens = 1000;
    double temperature = 0.7;
    double topP = 1.0;

    std::unordered_map<std::string, std::unique_ptr<ConversationNode>> nodeMap;
    std::string currentNodeId, rootNodeId;

    ConversationNode *findNode(const std::string &nodeId);
    std::vector<ConversationNode> getPathFromRoot(const std::string &nodeId);

public:
    AI(std::string apiKey, std::string baseUrl, std::string systemMessage = "You are a helpful assistant.");

    void addNode(ConversationNode::ROLE role, std::string content);
    bool deleteNode(const std::string &nodeId);
    bool switchNode(const std::string &nodeId);

    std::vector<std::string> getChildren(const std::string &nodeId);
    std::vector<ConversationNode> getCurrentPath();
    std::string getCurrentNodeId() const;
    std::string getRootNodeId() const;

    ChatCompletionResponse generateResponse(AIStreamCallback streamCallback);
    ModelsResponse getModels();
    UserBalanceResponse getUserBalance();
};
