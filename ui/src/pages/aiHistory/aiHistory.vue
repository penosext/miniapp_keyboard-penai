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
    <scroller class="history-container" scroll-direction="vertical" :show-scrollbar="true">
        <div class="history-section">
            <text class="section-title">搜索和管理</text>

            <div class="history-item">
                <text class="history-text">搜索对话&emsp;</text>
                <text class="history-input" @click="editSearchKeyword">{{ searchKeyword || '点击搜索对话...' }}</text>
                <text v-if="searchKeyword" @click="clearSearch" class="clear-btn">清除</text>
            </div>

            <div class="history-item">
                <text class="history-text">总计&emsp;</text>
                <text class="count-text">{{ conversationList.length }} 个对话</text>
                <text @click="createConversation" class="create-btn">新建对话</text>
            </div>
        </div>

        <div class="history-section" v-if="filteredConversations.length > 0">
            <text class="section-title">对话列表</text>

            <div v-for="conversation in filteredConversations" :key="conversation.id" class="conversation-card">
                <div class="conversation-main" @click="loadConversation(conversation.id)">
                    <text class="conversation-title">{{ conversation.title }}</text>
                    <text v-if="conversation.id === currentConversationId" class="current-indicator">当前</text>
                    <text class="conversation-time">{{ formatTime(conversation.updatedAt) }}</text>
                </div>

                <div class="conversation-actions">
                    <text @click="editConversationTitle(conversation.id, conversation.title)"
                        class="action-btn edit-btn">编辑</text>
                    <text @click="deleteConversation(conversation.id)" class="action-btn delete-btn">删除</text>
                </div>
            </div>
        </div>

        <div v-else class="empty-section">
            <text class="empty-title">{{ searchKeyword ? '没有找到匹配的对话' : '暂无对话历史' }}</text>
        </div>

        <ToastMessage />
        <Loading />
    </scroller>
</template>

<style lang="less" scoped>
@import url('aiHistory.less');
</style>

<script>
import Loading from '../../components/Loading.vue';
import aiHistory from './aiHistory';
import ToastMessage from '../../components/ToastMessage.vue';
export default {
    ...aiHistory,
    components: {
        Loading,
        ToastMessage,
    }
};
</script>
