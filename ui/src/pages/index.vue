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
                    <div v-if="message.role === 1" class="message-actions">
                        <text @click="regenerateMessage(message.id)" class="action action-btn">重</text>
                        <text @click="previousVariant(message.id)"
                            :class="'action action-btn' + (canGoPrevious(message.id) ? '' : ' action-btn-disabled')">左</text>
                        <text class="action">{{ getCurrentVariantInfo(message.id) }}</text>
                        <text @click="nextVariant(message.id)"
                            :class="'action action-btn' + (canGoNext(message.id) ? '' : ' action-btn-disabled')">右</text>
                    </div>
                </div>

            </div>
        </scroller>

        <div class="side-buttons">
            <text @click="openHistory" class="side-btn">历</text>
            <text @click="openSettings" class="side-btn">设</text>
        </div>

        <div class="input-area">
            <text class="input-display" @click="loadSoftKeyboard">{{ currentInput || '点击输入...' }}</text>
            <text @click="sendMessage(this.currentInput)" :disabled="!canSendMessage"
                :class="this.canSendMessage && this.currentInput.trim().length > 0 ? 'send-btn' : 'send-btn-disabled'"
                class="send-btn">{{ isStreaming ? '...' : '发' }}</text>
        </div>

        <ToastMessage />
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
    width: 526px;
    height: 134px;
    background-color: #000000;
}

.messages-content {
    padding-top: 5px;
    padding-bottom: 5px;
    padding-left: 5px;
    padding-right: 5px;
    min-height: 134px;
}

.side-buttons {
    position: absolute;
    right: 0;
    top: 0;
    width: 34px;
    height: 134px;
    background-color: #111111;
    flex-direction: column;
    align-items: center;
    padding-top: 5px;
    padding-bottom: 5px;
}

.side-btn {
    width: 28px;
    height: 28px;
    background-color: #333333;
    color: #ffffff;
    text-align: center;
    font-size: 16px;
    line-height: 28px;
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
    height: 36px;
    background-color: #222222;
    flex-direction: row;
    align-items: center;
    padding-left: 5px;
    padding-right: 5px;
}

.input-display {
    flex: 1;
    height: 28px;
    background-color: #111111;
    border-width: 1px;
    border-color: #444444;
    color: #ffffff;
    padding-top: 4px;
    padding-bottom: 4px;
    padding-left: 8px;
    padding-right: 8px;
    font-size: 16px;
    line-height: 20px;
    margin-right: 5px;
}

.send-btn {
    width: 28px;
    height: 28px;
    background-color: #007acc;
    color: #ffffff;
    text-align: center;
    font-size: 16px;
    line-height: 28px;
    border-radius: 3px;
}

.send-btn-disabled {
    background-color: #444444;
    color: #888888;
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
    padding-top: 4px;
    padding-bottom: 4px;
    padding-left: 8px;
    padding-right: 8px;
    border-radius: 3px;
    line-height: 20px;
    margin-bottom: 2px;
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

.message-content {
    font-size: 18px;
    color: #ffffff;
}

.message-actions {
    flex-direction: row;
    align-items: center;
    margin-top: 5px;
}

.action {
    height: 24px;
    font-size: 16px;
    color: #ffffff;
    margin-right: 5px;
    text-align: center;
    line-height: 24px;
    border-radius: 3px;
    min-width: 40px;
    padding: 0 4px;
}

.action-btn {
    width: 24px;
    background-color: #555555;
}

.action-btn-disabled {
    background-color: #333333;
    color: #666666;
}
</style>

<script>
import component from './index';
import ToastMessage from '../components/ToastMessage.vue';
export default {
    ...component,
    components: {
        ToastMessage
    }
};
</script>
