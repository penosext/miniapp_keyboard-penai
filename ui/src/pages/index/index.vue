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
    <div class="container" style="display: flex; flex-direction: column;">
        <div style="flex: 1; display: flex; flex-direction: row;">
            <scroller class="messages-scroller" scroll-direction="vertical" :show-scrollbar="true">
                <div v-for="message in displayMessages" :key="message.id">
                    <text :class="'message message-' + message.role">{{ message.content }}</text>
                    <div v-if="message.role === 1" class="message-actions">
                        <text @click="regenerateMessage(message.id)" class="square-btn">重</text>
                        <text @click="previousVariant(message.id)"
                            :class="'square-btn' + (canGoPrevious(message.id) ? '' : ' square-btn-disabled')">左</text>
                        <text class="action-text">{{ getCurrentVariantInfo(message.id) }}</text>
                        <text @click="nextVariant(message.id)"
                            :class="'square-btn' + (canGoNext(message.id) ? '' : ' square-btn-disabled')">右</text>
                    </div>
                </div>
            </scroller>

            <div class="side-buttons">
                <text @click="openHistory" class="square-btn">历</text>
                <text @click="openSettings" class="square-btn">设</text>
            </div>
        </div>

        <div class="item">
            <text class="item-input" @click="loadSoftKeyboard">{{ currentInput || '点击输入...' }}</text>
            <text @click="sendMessage(this.currentInput)"
                :class="'square-btn square-btn-' + (this.canSendMessage && this.currentInput.trim().length > 0 ? 'primary' : 'disabled')">{{
                    isStreaming ? '...' : '发'
                }}</text>
        </div>
    </div>
    <ToastMessage />
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
