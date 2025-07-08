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

#include "JSAI.hpp"
#include <iostream>

JSAI::JSAI() : AIObject(nullptr) {}

JSAI::~JSAI() {}

void JSAI::initialize(JQFunctionInfo &info)
{
    if (info.Length() != 0)
    {
        info.GetReturnValue().Set(false);
        return;
    }

    try
    {
        AIObject = std::make_unique<AI>();
        info.GetReturnValue().Set(true);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::initialize error: %s", e.what());
        info.GetReturnValue().Set(false);
    }
}
void JSAI::getCurrentPath(JQFunctionInfo &info)
{
    if (!AIObject)
    {
        info.GetReturnValue().Set(Bson::array());
        return;
    }
    if (info.Length() != 0)
    {
        info.GetReturnValue().Set(Bson::array());
        return;
    }

    try
    {
        std::vector<ConversationNode> path = AIObject->getCurrentPath();
        Bson::array result;
        for (const auto &msg : path)
        {
            Bson::object msgObj = {
                {"id", msg.id},
                {"role", msg.role},
                {"content", msg.content},
                {"parentId", msg.parentId},
                {"timestamp", std::to_string(msg.timestamp)}};
            Bson::array childIds;
            for (const auto &childId : msg.childIds)
                childIds.push_back(childId);
            msgObj["childIds"] = childIds;
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
    if (!AIObject)
    {
        info.GetReturnValue().Set(Bson::array());
        return;
    }
    if (info.Length() != 1)
    {
        info.GetReturnValue().Set(Bson::array());
        return;
    }
    JSContext *ctx = info.GetContext();
    std::string nodeId = JQString(ctx, info[0]).getString();

    try
    {
        std::vector<std::string> childIds = AIObject->getChildren(nodeId);
        Bson::array result;
        for (const auto &id : childIds)
            result.push_back(id);
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
    if (!AIObject)
    {
        info.GetReturnValue().Set(false);
        return;
    }
    if (info.Length() != 1)
    {
        info.GetReturnValue().Set(false);
        return;
    }
    JSContext *ctx = info.GetContext();
    std::string nodeId = JQString(ctx, info[0]).getString();

    try
    {
        info.GetReturnValue().Set(AIObject->switchNode(nodeId));
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::switchToNode error: %s", e.what());
        info.GetReturnValue().Set(false);
    }
}
void JSAI::getCurrentNodeId(JQFunctionInfo &info)
{
    if (!AIObject)
    {
        info.GetReturnValue().Set("");
        return;
    }
    if (info.Length() != 0)
    {
        info.GetReturnValue().Set("");
        return;
    }

    try
    {
        info.GetReturnValue().Set(AIObject->getCurrentNodeId());
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::getCurrentNodeId error: %s", e.what());
        info.GetReturnValue().Set("");
    }
}
void JSAI::getRootNodeId(JQFunctionInfo &info)
{
    if (!AIObject)
    {
        info.GetReturnValue().Set("");
        return;
    }
    if (info.Length() != 0)
    {
        info.GetReturnValue().Set("");
        return;
    }

    try
    {
        info.GetReturnValue().Set(AIObject->getRootNodeId());
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::getRootNodeId error: %s", e.what());
        info.GetReturnValue().Set("");
    }
}

void JSAI::getCurrentConversationId(JQFunctionInfo &info)
{
    if (!AIObject)
    {
        info.GetReturnValue().Set("");
        return;
    }
    if (info.Length() != 0)
    {
        info.GetReturnValue().Set("");
        return;
    }

    try
    {
        info.GetReturnValue().Set(AIObject->getConversationId());
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::getCurrentConversationId error: %s", e.what());
        info.GetReturnValue().Set("");
    }
}

void JSAI::addUserMessage(JQAsyncInfo &info)
{
    if (!AIObject)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "AI not initialized"}});
        return;
    }
    if (info.Length() != 1)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "addUserMessage requires exactly one argument"}});
        return;
    }
    std::string userMessage = info[0].string_value();

    try
    {
        AIObject->addNode(ConversationNode::ROLE_USER, userMessage);
        info.post(Bson::object{
            {"success", true},
            {"statusCode", 0},
            {"message", "User message added successfully"}});
    }
    catch (const std::exception &e)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", e.what()}});
    }
}
void JSAI::generateResponse(JQAsyncInfo &info)
{
    if (!AIObject)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "AI not initialized"}});
        return;
    }
    if (info.Length() != 0)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "generateResponse does not require any arguments"}});
        return;
    }

    try
    {
        AIStreamCallback callback = [this](AIStreamResult result)
        {
            publish("ai_stream", Bson::object{
                                     {"type", result.type},
                                     {"messageDelta", result.messageDelta},
                                     {"errorMessage", result.errorMessage}});
        };
        ChatCompletionResponse response = AIObject->generateResponse(callback);
        info.post(Bson::object{
            {"success", response.success},
            {"statusCode", response.statusCode},
            {"content", response.content},
            {"errorMessage", response.errorMessage}});
    }
    catch (const std::exception &e)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", e.what()}});
    }
}
void JSAI::getModels(JQAsyncInfo &info)
{
    if (!AIObject)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "AI not initialized"}});
        return;
    }
    if (info.Length() != 0)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "getModels does not require any arguments"}});
        return;
    }

    try
    {
        ModelsResponse response = AIObject->getModels();
        Bson::array modelsArray;
        for (const auto &model : response.models)
            modelsArray.push_back(model);
        info.post(Bson::object{
            {"success", response.success},
            {"statusCode", response.statusCode},
            {"models", modelsArray},
            {"errorMessage", response.errorMessage}});
    }
    catch (const std::exception &e)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", e.what()}});
    }
}
void JSAI::getUserBalance(JQAsyncInfo &info)
{
    if (!AIObject)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "AI not initialized"}});
        return;
    }
    if (info.Length() != 0)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "getUserBalance does not require any arguments"}});
        return;
    }

    try
    {
        UserBalanceResponse response = AIObject->getUserBalance();
        info.post(Bson::object{
            {"success", response.success},
            {"statusCode", response.statusCode},
            {"balance", response.balance},
            {"errorMessage", response.errorMessage}});
    }
    catch (const std::exception &e)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", e.what()}});
    }
}

void JSAI::getConversationList(JQAsyncInfo &info)
{
    if (!AIObject)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "AI not initialized"}});
        return;
    }
    if (info.Length() != 0)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "getConversationList does not require any arguments"}});
        return;
    }

    try
    {
        ConversationListResponse response = AIObject->getConversationList();
        Bson::array conversationsArray;
        for (const auto &conv : response.conversations)
        {
            conversationsArray.push_back(Bson::object{
                {"id", conv.id},
                {"title", conv.title},
                {"createdAt", std::to_string(conv.createdAt)},
                {"updatedAt", std::to_string(conv.updatedAt)}});
        }
        info.post(Bson::object{
            {"success", response.success},
            {"statusCode", response.statusCode},
            {"conversations", conversationsArray},
            {"errorMessage", response.errorMessage}});
    }
    catch (const std::exception &e)
    {
        info.post(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", e.what()}});
    }
}

void JSAI::createConversation(JQAsyncInfo &info)
{
    if (!AIObject)
    {
        info.post(false);
        return;
    }
    std::string title = "新对话";
    if (info.Length() >= 1 && !info[0].string_value().empty())
    {
        title = info[0].string_value();
    }

    try
    {
        AIObject->createConversation(title);
        info.post(true);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::createConversation error: %s", e.what());
        info.post(false);
    }
}

void JSAI::loadConversation(JQAsyncInfo &info)
{
    if (!AIObject)
    {
        info.post(false);
        return;
    }
    if (info.Length() != 1)
    {
        info.post(false);
        return;
    }
    std::string conversationId = info[0].string_value();

    try
    {
        AIObject->loadConversation(conversationId);
        info.post(true);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::loadConversation error: %s", e.what());
        info.post(false);
    }
}

void JSAI::deleteConversation(JQAsyncInfo &info)
{
    if (!AIObject)
    {
        info.post(false);
        return;
    }
    if (info.Length() != 1)
    {
        info.post(false);
        return;
    }
    std::string conversationId = info[0].string_value();

    try
    {
        AIObject->deleteConversation(conversationId);
        info.post(true);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::deleteConversation error: %s", e.what());
        info.post(false);
    }
}

void JSAI::updateConversationTitle(JQAsyncInfo &info)
{
    if (!AIObject)
    {
        info.post(false);
        return;
    }
    if (info.Length() != 2)
    {
        info.post(false);
        return;
    }
    std::string conversationId = info[0].string_value();
    std::string title = info[1].string_value();

    try
    {
        AIObject->updateConversationTitle(conversationId, title);
        info.post(true);
    }
    catch (const std::exception &e)
    {
        LOGD("JSAI::updateConversationTitle error: %s", e.what());
        info.post(false);
    }
}

void JSAI::setSettings(JQFunctionInfo &info)
{
    if (!AIObject)
    {
        info.GetReturnValue().Set(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "AI not initialized"}});
        return;
    }
    if (info.Length() != 7)
    {
        info.GetReturnValue().Set(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "setSettings requires exactly 6 arguments"}});
        return;
    }
    JSContext *ctx = info.GetContext();

    try
    {
        std::string apiKey = JQString(ctx, info[0]).getString();
        std::string baseUrl = JQString(ctx, info[1]).getString();
        std::string modelName = JQString(ctx, info[2]).getString();
        int maxTokens = JQNumber(ctx, info[3]).getInt32();
        double temperature = JQNumber(ctx, info[4]).getDouble();
        double topP = JQNumber(ctx, info[5]).getDouble();
        std::string systemPrompt = JQString(ctx, info[6]).getString();

        AIObject->setSettings(apiKey, baseUrl, modelName, maxTokens, temperature, topP, systemPrompt);
        info.GetReturnValue().Set(Bson::object{
            {"success", true},
            {"statusCode", 0}});
    }
    catch (const std::exception &e)
    {
        info.GetReturnValue().Set(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", e.what()}});
    }
}
void JSAI::getSettings(JQFunctionInfo &info)
{
    if (!AIObject)
    {
        info.GetReturnValue().Set(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "AI not initialized"}});
        return;
    }
    if (info.Length() != 0)
    {
        info.GetReturnValue().Set(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", "getSettings does not require any arguments"}});
        return;
    }

    try
    {
        SettingsResponse settings = AIObject->getSettings();
        info.GetReturnValue().Set(Bson::object{
            {"success", settings.success},
            {"statusCode", settings.statusCode},
            {"apiKey", settings.apiKey},
            {"baseUrl", settings.baseUrl},
            {"modelName", settings.modelName},
            {"maxTokens", settings.maxTokens},
            {"temperature", settings.temperature},
            {"topP", settings.topP},
            {"systemPrompt", settings.systemPrompt},
            {"errorMessage", settings.errorMessage}});
    }
    catch (const std::exception &e)
    {
        info.GetReturnValue().Set(Bson::object{
            {"success", false},
            {"statusCode", 0},
            {"errorMessage", e.what()}});
    }
}

extern JSValue createAI(JQModuleEnv *env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "AI");
    tpl->InstanceTemplate()->setObjectCreator([]()
                                              { return new JSAI(); });

    tpl->SetProtoMethod("initialize", &JSAI::initialize);
    tpl->SetProtoMethod("getCurrentPath", &JSAI::getCurrentPath);
    tpl->SetProtoMethod("getChildNodes", &JSAI::getChildNodes);
    tpl->SetProtoMethod("switchToNode", &JSAI::switchToNode);
    tpl->SetProtoMethod("getCurrentNodeId", &JSAI::getCurrentNodeId);
    tpl->SetProtoMethod("getRootNodeId", &JSAI::getRootNodeId);
    tpl->SetProtoMethod("getCurrentConversationId", &JSAI::getCurrentConversationId);

    tpl->SetProtoMethodPromise("addUserMessage", &JSAI::addUserMessage);
    tpl->SetProtoMethodPromise("generateResponse", &JSAI::generateResponse);
    tpl->SetProtoMethodPromise("getModels", &JSAI::getModels);
    tpl->SetProtoMethodPromise("getUserBalance", &JSAI::getUserBalance);

    tpl->SetProtoMethodPromise("getConversationList", &JSAI::getConversationList);
    tpl->SetProtoMethodPromise("createConversation", &JSAI::createConversation);
    tpl->SetProtoMethodPromise("loadConversation", &JSAI::loadConversation);
    tpl->SetProtoMethodPromise("deleteConversation", &JSAI::deleteConversation);
    tpl->SetProtoMethodPromise("updateConversationTitle", &JSAI::updateConversationTitle);

    tpl->SetProtoMethod("setSettings", &JSAI::setSettings);
    tpl->SetProtoMethod("getSettings", &JSAI::getSettings);

    JSAI::InitTpl(tpl);
    return tpl->CallConstructor();
}
