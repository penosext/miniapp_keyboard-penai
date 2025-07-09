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
@import url('index.less');
</style>

<script>
import ToastMessage from '../../components/ToastMessage.vue';
import index from './index';
export default {
    ...index,
    components: {
        ToastMessage
    }
}
</script>
