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

#include <jqutil_v2/jqutil.h>
#include <AI.hpp>
#include <memory>

using namespace JQUTIL_NS;

class JSAI : public JQPublishObject
{
private:
    std::unique_ptr<AI> ai;

public:
    JSAI();
    ~JSAI();

    // 初始化AI
    void initialize(JQFunctionInfo &info);

    // 发送消息
    void sendMessage(JQAsyncInfo &info);

    // 发送流式消息
    void sendMessageStream(JQAsyncInfo &info);

    // 获取当前对话路径
    void getCurrentPath(JQFunctionInfo &info);

    // 获取子节点
    void getChildNodes(JQFunctionInfo &info);

    // 切换到指定节点
    void switchToNode(JQFunctionInfo &info);

    // 导出对话树
    void exportConversationTree(JQFunctionInfo &info);

    // 导入对话树
    void importConversationTree(JQFunctionInfo &info);

    // 获取模型列表
    void getModels(JQAsyncInfo &info);

    // 获取用户余额
    void getUserBalance(JQAsyncInfo &info);

    // 获取当前状态
    void getCurrentNodeId(JQFunctionInfo &info);
    void getRootNodeId(JQFunctionInfo &info);
};

extern JSValue createAI(JQModuleEnv *env);
