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

#include <JSAI.hpp>
#include <iostream>

JSAI::JSAI() : ai(nullptr) {}

JSAI::~JSAI() {}

void JSAI::initialize(JQFunctionInfo &info)
{
    JSContext *ctx = info.GetContext();

    if (info.Length() < 2)
    {
        info.GetReturnValue().Set(false);
        return;
    }

    std::string apiKey = JQString(ctx, info[0]).getString();
    std::string baseUrl = JQString(ctx, info[1]).getString();

    try
    {
        ai = std::make_unique<AI>(apiKey, baseUrl);
        info.GetReturnValue().Set(true);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::initialize error: %s", e.what());
        info.GetReturnValue().Set(false);
    }
}

void JSAI::sendMessage(JQAsyncInfo &info)
{
    if (!ai)
    {
        info.postError("AI not initialized");
        return;
    }

    std::string userMessage = info[0].string_value();

    try
    {
        ChatCompletionResponse response = ai->sendMessage(userMessage);

        Bson::object result;
        result["success"] = response.success;
        result["statusCode"] = response.statusCode;
        result["content"] = response.content;
        result["id"] = response.id;
        result["model"] = response.model;
        result["created"] = response.created;
        result["systemFingerprint"] = response.systemFingerprint;

        if (!response.errorMessage.empty())
        {
            result["error"] = response.errorMessage;
        }

        if (!response.usage.empty())
        {
            result["usage"] = response.usage.dump();
        }

        info.post(result);
    }
    catch (const std::exception &e)
    {
        info.postError("SendMessage error: %s", e.what());
    }
}

void JSAI::sendMessageStream(JQAsyncInfo &info)
{
    if (!ai)
    {
        info.postError("AI not initialized");
        return;
    }

    std::string userMessage = info[0].string_value();

    try
    {
        AIStreamCallback callback = [this](std::string content)
        {
            Bson::object streamData;
            streamData["type"] = "stream";
            streamData["content"] = content;
            publish("ai_stream", streamData);
        };

        ChatCompletionResponse response = ai->sendMessage(userMessage, callback);

        Bson::object result;
        result["success"] = response.success;
        result["statusCode"] = response.statusCode;
        result["content"] = response.content;
        result["id"] = response.id;
        result["model"] = response.model;
        result["created"] = response.created;
        result["systemFingerprint"] = response.systemFingerprint;

        if (!response.errorMessage.empty())
        {
            result["error"] = response.errorMessage;
        }

        if (!response.usage.empty())
        {
            result["usage"] = response.usage.dump();
        }

        info.post(result);
    }
    catch (const std::exception &e)
    {
        info.postError("SendMessageStream error: %s", e.what());
    }
}

void JSAI::getCurrentPath(JQFunctionInfo &info)
{
    if (!ai)
    {
        info.GetReturnValue().Set(Bson::array());
        return;
    }

    try
    {
        std::vector<AIMessage> path = ai->getCurrentPath();
        Bson::array result;

        for (const auto &msg : path)
        {
            Bson::object msgObj;
            msgObj["role"] = msg.role;
            msgObj["content"] = msg.content;
            result.push_back(msgObj);
        }

        info.GetReturnValue().Set(result);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::getCurrentPath error: %s", e.what());
        info.GetReturnValue().Set(Bson::array());
    }
}

void JSAI::getChildNodes(JQFunctionInfo &info)
{
    if (!ai)
    {
        info.GetReturnValue().Set(Bson::array());
        return;
    }

    JSContext *ctx = info.GetContext();
    std::string nodeId = JQString(ctx, info[0]).getString();

    try
    {
        std::vector<std::string> childIds = ai->getChildNodes(nodeId);
        Bson::array result;

        for (const auto &id : childIds)
        {
            result.push_back(id);
        }

        info.GetReturnValue().Set(result);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::getChildNodes error: %s", e.what());
        info.GetReturnValue().Set(Bson::array());
    }
}

void JSAI::switchToNode(JQFunctionInfo &info)
{
    if (!ai)
    {
        info.GetReturnValue().Set(false);
        return;
    }

    JSContext *ctx = info.GetContext();
    std::string nodeId = JQString(ctx, info[0]).getString();

    try
    {
        bool success = ai->switchToNode(nodeId);
        info.GetReturnValue().Set(success);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::switchToNode error: %s", e.what());
        info.GetReturnValue().Set(false);
    }
}

void JSAI::exportConversationTree(JQFunctionInfo &info)
{
    if (!ai)
    {
        info.GetReturnValue().Set("");
        return;
    }

    try
    {
        nlohmann::json treeJson = ai->exportConversationTree();
        std::string result = treeJson.dump();
        info.GetReturnValue().Set(result);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::exportConversationTree error: %s", e.what());
        info.GetReturnValue().Set("");
    }
}

void JSAI::importConversationTree(JQFunctionInfo &info)
{
    if (!ai)
    {
        info.GetReturnValue().Set(false);
        return;
    }

    JSContext *ctx = info.GetContext();
    std::string treeData = JQString(ctx, info[0]).getString();

    try
    {
        nlohmann::json treeJson = nlohmann::json::parse(treeData);
        bool success = ai->importConversationTree(treeJson);
        info.GetReturnValue().Set(success);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::importConversationTree error: %s", e.what());
        info.GetReturnValue().Set(false);
    }
}

void JSAI::getModels(JQAsyncInfo &info)
{
    if (!ai)
    {
        info.postError("AI not initialized");
        return;
    }

    try
    {
        ModelsResponse response = ai->getModels();

        Bson::object result;
        result["success"] = response.success;
        result["statusCode"] = response.statusCode;

        if (response.success)
        {
            Bson::array modelsArray;
            for (const auto &model : response.models)
            {
                modelsArray.push_back(model.dump());
            }
            result["models"] = modelsArray;
        }

        if (!response.errorMessage.empty())
        {
            result["error"] = response.errorMessage;
        }

        info.post(result);
    }
    catch (const std::exception &e)
    {
        info.postError("GetModels error: %s", e.what());
    }
}

void JSAI::getUserBalance(JQAsyncInfo &info)
{
    if (!ai)
    {
        info.postError("AI not initialized");
        return;
    }

    try
    {
        UserBalanceResponse response = ai->getUserBalance();

        Bson::object result;
        result["success"] = response.success;
        result["statusCode"] = response.statusCode;

        if (response.success)
        {
            result["isAvailable"] = response.isAvailable;
            Bson::array balanceArray;
            for (const auto &balance : response.balanceInfos)
            {
                balanceArray.push_back(balance.dump());
            }
            result["balanceInfos"] = balanceArray;
        }

        if (!response.errorMessage.empty())
        {
            result["error"] = response.errorMessage;
        }

        info.post(result);
    }
    catch (const std::exception &e)
    {
        info.postError("GetUserBalance error: %s", e.what());
    }
}

void JSAI::getCurrentNodeId(JQFunctionInfo &info)
{
    if (!ai)
    {
        info.GetReturnValue().Set("");
        return;
    }

    try
    {
        std::string nodeId = ai->getCurrentNodeId();
        info.GetReturnValue().Set(nodeId);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::getCurrentNodeId error: %s", e.what());
        info.GetReturnValue().Set("");
    }
}

void JSAI::getRootNodeId(JQFunctionInfo &info)
{
    if (!ai)
    {
        info.GetReturnValue().Set("");
        return;
    }

    try
    {
        std::string nodeId = ai->getRootNodeId();
        info.GetReturnValue().Set(nodeId);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::getRootNodeId error: %s", e.what());
        info.GetReturnValue().Set("");
    }
}

extern JSValue createAI(JQModuleEnv *env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "ai");
    tpl->InstanceTemplate()->setObjectCreator([]()
                                              { return new JSAI(); });

    // 同步方法
    tpl->SetProtoMethod("initialize", &JSAI::initialize);
    tpl->SetProtoMethod("getCurrentPath", &JSAI::getCurrentPath);
    tpl->SetProtoMethod("getChildNodes", &JSAI::getChildNodes);
    tpl->SetProtoMethod("switchToNode", &JSAI::switchToNode);
    tpl->SetProtoMethod("exportConversationTree", &JSAI::exportConversationTree);
    tpl->SetProtoMethod("importConversationTree", &JSAI::importConversationTree);
    tpl->SetProtoMethod("getCurrentNodeId", &JSAI::getCurrentNodeId);
    tpl->SetProtoMethod("getRootNodeId", &JSAI::getRootNodeId);

    // 异步方法
    tpl->SetProtoMethodPromise("sendMessage", &JSAI::sendMessage);
    tpl->SetProtoMethodPromise("sendMessageStream", &JSAI::sendMessageStream);
    tpl->SetProtoMethodPromise("getModels", &JSAI::getModels);
    tpl->SetProtoMethodPromise("getUserBalance", &JSAI::getUserBalance);

    // 初始化事件发布
    JSAI::InitTpl(tpl);

    return tpl->CallConstructor();
}
