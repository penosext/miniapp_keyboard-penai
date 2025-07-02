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
#include <nlohmann/json.hpp>
#include <Fetch.hpp>

// 对话历史树节点结构
struct ConversationNode
{
    std::string id;           // 节点唯一ID
    std::string role;         // "system", "user", "assistant"
    std::string content;      // 消息内容
    std::string parentId;     // 父节点ID，根节点为空
    std::vector<std::string> childIds; // 子节点ID列表
    int64_t timestamp;        // 创建时间戳
    
    ConversationNode(std::string id, std::string role, std::string content, std::string parentId = "")
        : id(id), role(role), content(content), parentId(parentId), timestamp(0) {}

    nlohmann::json toJson() const
    {
        return nlohmann::json{
            {"id", id},
            {"role", role}, 
            {"content", content},
            {"parentId", parentId},
            {"childIds", childIds},
            {"timestamp", timestamp}
        };
    }
};

// AI 消息结构（为了兼容性保留）
struct AIMessage
{
    std::string role;    // "system", "user", "assistant"
    std::string content; // 消息内容

    AIMessage(std::string role, std::string content)
        : role(role), content(content) {}

    nlohmann::json toJson() const
    {
        return nlohmann::json{{"role", role}, {"content", content}};
    }
};

// 基础响应结构
struct BaseResponse
{
    bool success = false;
    int statusCode = 0;
    std::string errorMessage;

    BaseResponse(bool success = false, int statusCode = 0, std::string errorMessage = "")
        : success(success), statusCode(statusCode), errorMessage(errorMessage) {}

    nlohmann::json toJson() const
    {
        nlohmann::json json;
        json["success"] = success;
        json["statusCode"] = statusCode;
        if (!errorMessage.empty())
            json["error"] = errorMessage;
        return json;
    }
};

// 聊天补全响应结构
struct ChatCompletionResponse : BaseResponse
{
    std::string id;
    std::string model;
    std::string content;
    int created = 0;
    std::string systemFingerprint;
    nlohmann::json usage;

    ChatCompletionResponse(bool success = false, int statusCode = 0, std::string errorMessage = "")
        : BaseResponse(success, statusCode, errorMessage) {}

    nlohmann::json toJson() const
    {
        nlohmann::json json = BaseResponse::toJson();
        if (success)
        {
            json["id"] = id;
            json["model"] = model;
            json["content"] = content;
            json["created"] = created;
            json["system_fingerprint"] = systemFingerprint;
            json["usage"] = usage;
        }
        return json;
    }
};

// 模型列表响应结构
struct ModelsResponse : BaseResponse
{
    std::vector<nlohmann::json> models;

    ModelsResponse(bool success = false, int statusCode = 0, std::string errorMessage = "")
        : BaseResponse(success, statusCode, errorMessage) {}

    nlohmann::json toJson() const
    {
        nlohmann::json json = BaseResponse::toJson();
        if (success)
        {
            json["object"] = "list";
            json["data"] = models;
        }
        return json;
    }
};

// 用户余额响应结构
struct UserBalanceResponse : BaseResponse
{
    bool isAvailable = false;
    std::vector<nlohmann::json> balanceInfos;

    UserBalanceResponse(bool success = false, int statusCode = 0, std::string errorMessage = "")
        : BaseResponse(success, statusCode, errorMessage) {}

    nlohmann::json toJson() const
    {
        nlohmann::json json = BaseResponse::toJson();
        if (success)
        {
            json["is_available"] = isAvailable;
            json["balance_infos"] = balanceInfos;
        }
        return json;
    }
};

// 流式回调函数类型
using AIStreamCallback = std::function<void(std::string content)>;

// AI 聊天类
class AI
{
private:
    std::string apiKey, baseUrl;
    std::string model = "deepseek-chat";
    int maxTokens = 1000;
    double temperature = 0.7;
    double topP = 1.0;
    
    // 对话历史树存储
    std::vector<ConversationNode> conversationTree;
    std::string currentNodeId;  // 当前对话位置的节点ID
    std::string rootNodeId;     // 根节点ID
    
    // 私有辅助方法
    std::string generateNodeId();
    ConversationNode* findNode(const std::string& nodeId);
    std::vector<AIMessage> getPathFromRoot(const std::string& nodeId);

public:
    AI(std::string apiKey, std::string baseUrl);
    
    // 基本消息操作
    void addMessage(std::string role, std::string content);
    ChatCompletionResponse sendMessage(std::string userMessage, AIStreamCallback callback = nullptr);
    
    // 树形对话历史管理
    std::string addMessageToNode(std::string role, std::string content, const std::string& parentId = "");
    bool switchToNode(const std::string& nodeId);
    std::vector<std::string> getChildNodes(const std::string& nodeId);
    std::vector<AIMessage> getCurrentPath();
    std::vector<AIMessage> getPathToNode(const std::string& nodeId);
    
    // 历史管理
    std::vector<AIMessage> getMessageHistory(); // 兼容性方法
    std::vector<ConversationNode> getFullConversationTree();
    nlohmann::json exportConversationTree();
    bool importConversationTree(const nlohmann::json& treeData);
    
    // API 调用
    ModelsResponse getModels();
    UserBalanceResponse getUserBalance();
    
    // 获取当前状态
    std::string getCurrentNodeId() const { return currentNodeId; }
    std::string getRootNodeId() const { return rootNodeId; }
};
