<!--
    Copyright (C) 2025 Langning Chen

    This file is part of miniapp.

    miniapp is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    miniapp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with miniapp.  If not, see <https://www.gnu.org/licenses/>.
-->

<template>
    <div class="ai-chat-container">
        <!-- 顶部工具栏 -->
        <div class="toolbar">
            <text @click="newConversation" :disabled="!aiInitialized" class="btn btn-primary">新对话</text>
            <text @click="showBranches = !showBranches" :disabled="!aiInitialized || conversationBranches.length === 0"
                class="btn btn-success">分支({{ conversationBranches.length }})</text>
            <text @click="showSettings = !showSettings" class="btn btn-warning">设置</text>
        </div>

        <!-- 分支选择器 (覆盖层) -->
        <div v-if="showBranches" class="branches-overlay">
            <text class="branch-title">选择分支:</text>
            <div class="branch-list">
                <text v-for="branch in conversationBranches" :key="branch.nodeId" @click="switchToBranch(branch.nodeId)"
                    :class="getBranchClass(branch)">{{ branch.preview.substring(0, 8) }}...</text>
            </div>
            <text @click="showBranches = false" class="btn btn-small">关闭</text>
        </div>

        <!-- 设置面板 (覆盖层) -->
        <div v-if="showSettings" class="settings-overlay">
            <text class="settings-title">AI设置</text>
            <div class="setting-item">
                <text class="setting-label">API密钥:</text>
                <text class="setting-input" @click="editApiKey">{{ apiKey || '点击输入API密钥' }}</text>
            </div>
            <div class="setting-item">
                <text class="setting-label">URL:</text>
                <text class="setting-input" @click="editBaseUrl">{{ baseUrl || '点击输入URL' }}</text>
            </div>
            <div class="setting-buttons">
                <text @click="saveAndApplySettings" class="btn btn-success">保存</text>
                <text @click="showSettings = false" class="btn btn-secondary">取消</text>
            </div>
        </div>

        <!-- 消息区域 -->
        <div class="messages-area">
            <div v-for="(message, index) in displayMessages" :key="index" :class="getMessageClass(message)">
                <text class="message-content">{{ message.content }}</text>
            </div>

            <!-- 加载指示器 -->
            <div v-if="isLoading" class="message-loading">
                <text class="message-content">AI思考中...</text>
            </div>
        </div>

        <!-- 输入区域 -->
        <div class="input-area">
            <text class="input-display" @click="loadSoftKeyboard">{{ currentInput || '点击输入...' }}</text>
            <text @click="sendMessage" :disabled="!canSendMessage" :class="getSendButtonClass()">{{ isLoading ? '...' :
                '发'}}</text>
        </div>

        <!-- 错误提示 -->
        <text v-if="errorMessage" class="error-message">{{ errorMessage }}</text>

        <!-- 初始化提示 -->
        <text v-if="!aiInitialized" class="init-message">请先配置API密钥</text>
    </div>
</template>

<style lang="less" scoped>
.ai-chat-container {
    position: absolute;
    left: 0;
    top: 0;
    width: 560px;
    height: 170px;
    background-color: #000000;
    color: #ffffff;
    font-size: 11px;
    font-family: monospace;
}

.toolbar {
    position: absolute;
    left: 0;
    top: 0;
    width: 560px;
    height: 20px;
    background-color: #222222;
    flex-direction: row;
    align-items: center;
    padding-left: 2px;
    padding-right: 2px;
    z-index: 10;
}

.btn {
    padding-top: 2px;
    padding-bottom: 2px;
    padding-left: 6px;
    padding-right: 6px;
    font-size: 9px;
    text-align: center;
    border-radius: 2px;
    margin-left: 2px;
    margin-right: 2px;
}

.btn-primary {
    background-color: #007acc;
    color: #ffffff;
}

.btn-success {
    background-color: #28a745;
    color: #ffffff;
}

.btn-warning {
    background-color: #ffc107;
    color: #000000;
}

.btn-secondary {
    background-color: #6c757d;
    color: #ffffff;
}

.btn-disabled {
    background-color: #444444;
    color: #999999;
}

.btn-small {
    padding-top: 1px;
    padding-bottom: 1px;
    padding-left: 4px;
    padding-right: 4px;
    font-size: 8px;
}

.branches-overlay,
.settings-overlay {
    position: absolute;
    left: 0;
    top: 0;
    width: 560px;
    height: 170px;
    background-color: rgba(0, 0, 0, 0.95);
    z-index: 20;
    padding-top: 5px;
    padding-bottom: 5px;
    padding-left: 5px;
    padding-right: 5px;
}

.branch-title,
.settings-title {
    font-size: 10px;
    font-weight: bold;
    margin-bottom: 5px;
    color: #ffc107;
}

.branch-list {
    height: 100px;
    margin-bottom: 5px;
    flex-direction: row;
    flex-wrap: wrap;
}

.branch-item {
    padding-top: 2px;
    padding-bottom: 2px;
    padding-left: 4px;
    padding-right: 4px;
    margin-top: 1px;
    margin-bottom: 1px;
    margin-left: 1px;
    margin-right: 1px;
    background-color: #444444;
    font-size: 8px;
    border-radius: 2px;
}

.branch-item-active {
    background-color: #007acc;
}

.setting-item {
    margin-bottom: 5px;
}

.setting-label {
    font-size: 9px;
    margin-bottom: 2px;
}

.setting-input {
    width: 550px;
    height: 16px;
    background-color: #111111;
    color: #ffffff;
    border-width: 1px;
    border-color: #444444;
    font-size: 9px;
    padding-top: 1px;
    padding-bottom: 1px;
    padding-left: 3px;
    padding-right: 3px;
}

.setting-buttons {
    flex-direction: row;
    margin-top: 5px;
}

.messages-area {
    position: absolute;
    left: 0;
    top: 22px;
    width: 560px;
    height: 120px;
    padding-top: 2px;
    padding-bottom: 2px;
    padding-left: 2px;
    padding-right: 2px;
    z-index: 5;
}

.message {
    margin-bottom: 3px;
    width: 400px;
    padding-top: 3px;
    padding-bottom: 3px;
    padding-left: 5px;
    padding-right: 5px;
    border-radius: 3px;
    font-size: 9px;
    line-height: 12px;
}

.message-user {
    background-color: #007acc;
    align-self: flex-end;
    text-align: right;
}

.message-assistant {
    background-color: #333333;
    align-self: flex-start;
}

.message-loading {
    background-color: #444444;
    color: #999999;
}

.message-content {
    font-size: 9px;
}

.input-area {
    position: absolute;
    left: 0;
    bottom: 0;
    width: 560px;
    height: 26px;
    background-color: #222222;
    flex-direction: row;
    align-items: center;
    padding-left: 2px;
    padding-right: 2px;
    z-index: 10;
}

.input-display {
    flex: 1;
    height: 20px;
    background-color: #111111;
    border-width: 1px;
    border-color: #444444;
    padding-top: 2px;
    padding-bottom: 2px;
    padding-left: 4px;
    padding-right: 4px;
    font-size: 9px;
    line-height: 16px;
    margin-right: 2px;
}

.error-message {
    position: absolute;
    left: 0;
    top: 85px;
    width: 560px;
    text-align: center;
    background-color: #dc3545;
    color: #ffffff;
    padding-top: 5px;
    padding-bottom: 5px;
    padding-left: 5px;
    padding-right: 5px;
    font-size: 9px;
    z-index: 30;
}

.init-message {
    position: absolute;
    left: 0;
    top: 85px;
    width: 560px;
    text-align: center;
    color: #ffc107;
    font-size: 10px;
    z-index: 15;
}
</style>

<script>
import component from './index';
export default component;
</script>
