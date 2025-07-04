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
        <scroller class="messages-scroller" scroll-direction="vertical" :show-scrollbar="true">
            <div class="messages-content">
                <div v-for="message in displayMessages" :key="message.id" class="message-container">
                    <div :class="'message message-' + message.role">
                        <text class="message-content">{{ message.content }}</text>
                    </div>
                    <div v-if="message.role === ROLE.ROLE_ASSISTANT" class="message-actions">
                        <text @click="regenerateMessage(message.id)" class="action action-btn">重</text>
                        <text @click="previousVariant(message.id)"
                            :class="'action action-btn' + (canGoPrevious(message.id) ? '' : ' action-btn-disabled')">左</text>
                        <text class="action">{{ getCurrentVariantInfo(message.id) }}</text>
                        <text @click="nextVariant(message.id)"
                            :class="'action action-btn' + (canGoNext(message.id) ? '' : ' action-btn-disabled')">右</text>
                    </div>
                </div>
                <div v-if="errorMessage" class="message message-error">
                    <text class="message-content">{{ errorMessage }}</text>
                </div>
            </div>
        </scroller>

        <div class="side-buttons">
            <text @click="showSettings = !showSettings" class="side-btn">设</text>
            <text @click="showBranches = !showBranches" :disabled="!aiInitialized" class="side-btn">历</text>
            <text @click="newConversation" :disabled="!aiInitialized" class="side-btn">新</text>
        </div>

        <div class="input-area">
            <text class="input-display" @click="loadSoftKeyboard">{{ currentInput || '点击输入...' }}</text>
            <text @click="sendMessage(this.currentInput)" :disabled="!canSendMessage"
                :class="this.canSendMessage && this.currentInput.trim().length > 0 ? 'btn-primary' : 'btn-disabled'"
                class="send-btn">{{ isLoading ? '...' : '发' }}</text>
        </div>

        <div v-if="showSettings" class="settings-overlay">
            <text class="overlay-title">AI设置</text>
            <div class="setting-item">
                <text class="setting-label">API密钥:</text>
                <text class="setting-input" @click="editApiKey">{{ apiKey || '点击输入API密钥' }}</text>
            </div>
            <div class="setting-item">
                <text class="setting-label">URL:</text>
                <text class="setting-input" @click="editBaseUrl">{{ baseUrl || '点击输入URL' }}</text>
            </div>
            <div class="setting-buttons">
                <text @click="saveAndApplySettings" class="setting-btn setting-btn-save">保存</text>
                <text @click="showSettings = false" class="setting-btn setting-btn-cancel">取消</text>
            </div>
        </div>
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
    font-family: monospace;
}

.messages-scroller {
    position: absolute;
    left: 0;
    top: 0;
    width: 530px;
    height: 140px;
    background-color: #000000;
}

.messages-content {
    padding-top: 5px;
    padding-bottom: 5px;
    padding-left: 5px;
    padding-right: 5px;
    min-height: 140px;
}

.side-buttons {
    position: absolute;
    right: 0;
    top: 0;
    width: 30px;
    height: 140px;
    background-color: #111111;
    flex-direction: column;
    align-items: center;
    padding-top: 5px;
    padding-bottom: 5px;
}

.side-btn {
    width: 24px;
    height: 24px;
    background-color: #333333;
    color: #ffffff;
    text-align: center;
    font-size: 12px;
    line-height: 24px;
    margin-bottom: 5px;
    border-radius: 3px;
}

.side-btn:disabled {
    background-color: #222222;
    color: #666666;
}

.input-area {
    position: absolute;
    left: 0;
    bottom: 0;
    width: 560px;
    height: 30px;
    background-color: #222222;
    flex-direction: row;
    align-items: center;
    padding-left: 5px;
    padding-right: 5px;
}

.input-display {
    flex: 1;
    height: 22px;
    background-color: #111111;
    border-width: 1px;
    border-color: #444444;
    color: #ffffff;
    padding-top: 3px;
    padding-bottom: 3px;
    padding-left: 5px;
    padding-right: 5px;
    font-size: 12px;
    line-height: 16px;
    margin-right: 5px;
}

.send-btn {
    width: 22px;
    height: 22px;
    background-color: #007acc;
    color: #ffffff;
    text-align: center;
    font-size: 15px;
    line-height: 22px;
    border-radius: 3px;
}

.send-btn:disabled {
    background-color: #444444;
    color: #ffffff;
}

.send-btn-loading {
    background-color: #666666;
    color: #ffffff;
}

.message-container {
    margin-bottom: 10px;
}

.message {
    max-width: 400px;
    padding-top: 2px;
    padding-bottom: 2px;
    padding-left: 5px;
    padding-right: 5px;
    border-radius: 3px;
    line-height: 14px;
    margin-bottom: 1px;
}

// user
.message-0 {
    background-color: #007acc;
    align-self: flex-end;
}

// assistant
.message-1 {
    background-color: #333333;
    align-self: flex-start;
}

// system
.message-2 {
    background-color: #28a745;
    align-self: center;
}

.message-error {
    background-color: #dc3545;
    color: #ffffff;
    align-self: center;
}

.message-content {
    font-size: 15px;
    color: #ffffff;
}

.message-actions {
    flex-direction: row;
    align-items: center;
    margin-top: 5px;
}

.action {
    height: 20px;
    font-size: 12px;
    color: #ffffff;
    margin-right: 3px;
    text-align: center;
    line-height: 20px;
    border-radius: 5px;
}

.action-btn {
    width: 20px;
    background-color: #555555;
}

.action-btn-disabled {
    background-color: #333333;
    color: #666666;
}

.settings-overlay {
    position: absolute;
    left: 0;
    top: 0;
    width: 560px;
    height: 170px;
    background-color: rgb(0, 0, 0);
    z-index: 100;
    padding: 5px;
}

.overlay-title {
    height: 25px;
    line-height: 25px;
    font-size: 17px;
    font-weight: bold;
    color: #ffc107;
}

.setting-item {
    margin-bottom: 5px;
}

.setting-label {
    font-size: 15px;
    color: #ffffff;
    margin-bottom: 2px;
}

.setting-input {
    width: 540px;
    height: 25px;
    background-color: #111111;
    color: #ffffff;
    border-width: 1px;
    border-color: #444444;
    font-size: 15px;
    line-height: 21px;
    padding: 2px;
}

.setting-buttons {
    flex-direction: row;
    margin-top: 5px;
}

.setting-btn {
    width: 60px;
    height: 25px;
    text-align: center;
    font-size: 15px;
    line-height: 25px;
    border-radius: 3px;
    margin-right: 10px;
}

.setting-btn-save {
    background-color: #28a745;
    color: #ffffff;
}

.setting-btn-cancel {
    background-color: #6c757d;
    color: #ffffff;
}
</style>

<script>
import { ROLE } from '../ime/Role';
import component from './index';
export default component;
</script>
