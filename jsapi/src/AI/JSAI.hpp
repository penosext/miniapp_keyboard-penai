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

#include "AI.hpp"
#include <jqutil_v2/jqutil.h>
#include <memory>

using namespace JQUTIL_NS;

class JSAI : public JQPublishObject
{
private:
    std::unique_ptr<AI> AIObject;

public:
    JSAI();
    ~JSAI();

    void initialize(JQFunctionInfo &info);
    void getCurrentPath(JQFunctionInfo &info);
    void getChildNodes(JQFunctionInfo &info);
    void switchToNode(JQFunctionInfo &info);
    void getCurrentNodeId(JQFunctionInfo &info);
    void getRootNodeId(JQFunctionInfo &info);
    void getCurrentConversationId(JQFunctionInfo &info);

    void addUserMessage(JQAsyncInfo &info);
    void generateResponse(JQAsyncInfo &info);
    void getModels(JQAsyncInfo &info);
    void getUserBalance(JQAsyncInfo &info);

    void getConversationList(JQAsyncInfo &info);
    void createConversation(JQAsyncInfo &info);
    void loadConversation(JQAsyncInfo &info);
    void deleteConversation(JQAsyncInfo &info);
    void updateConversationTitle(JQAsyncInfo &info);

    void setSettings(JQFunctionInfo &info);
    void getSettings(JQFunctionInfo &info);
};

extern JSValue createAI(JQModuleEnv *env);
