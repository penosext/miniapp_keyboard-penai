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

        <div v-else-if="!loading" class="empty-section">
            <text class="empty-title">{{ searchKeyword ? '没有找到匹配的对话' : '暂无对话历史' }}</text>
        </div>

        <div v-if="loading" class="loading-section">
            <text class="loading-text">加载中...</text>
        </div>

        <div v-if="errorMessage" class="error-section">
            <text class="error-text">{{ errorMessage }}</text>
        </div>
    </scroller>
</template>

<style lang="less" scoped>
.history-container {
    position: absolute;
    left: 0;
    top: 0;
    width: 560px;
    height: 170px;
    color: #ffffff;
    font-family: monospace;
    padding: 5px;
}

.back-btn {
    border-radius: 2px;
    line-height: 24px;
    font-size: 18px;
    padding: 0 8px;
    background-color: #6c757d;
    color: #ffffff;
}

.history-section {
    margin-bottom: 15px;
}

.section-title {
    font-size: 20px;
    color: #ffc107;
    font-weight: bold;
    margin-bottom: 8px;
    border-bottom-width: 1px;
    border-bottom-color: #333333;
    padding-bottom: 2px;
}

.history-item {
    min-height: 24px;
    margin-bottom: 8px;
    display: flex;
    flex-direction: row;
    align-items: center;
}

.history-text {
    height: 24px;
    line-height: 24px;
    font-size: 18px;
    color: #ffffff;
}

.history-input {
    display: flex;
    flex-direction: row;
    flex: 1;
    height: 24px;
    background-color: #111111;
    color: #ffffff;
    border-width: 1px;
    border-color: #444444;
    font-size: 16px;
    line-height: 20px;
    padding: 2px;
    border-radius: 2px;
    margin-right: 8px;
}

.clear-btn {
    height: 24px;
    background-color: #dc3545;
    color: #ffffff;
    text-align: center;
    font-size: 16px;
    line-height: 24px;
    border-radius: 2px;
    padding: 0 8px;
}

.count-text {
    flex: 1;
    font-size: 18px;
    color: #28a745;
    background-color: #1a4a2e;
    height: 24px;
    line-height: 24px;
    padding: 0 8px;
    border-radius: 2px;
    margin-right: 8px;
}

.create-btn {
    height: 24px;
    background-color: #28a745;
    color: #ffffff;
    text-align: center;
    font-size: 16px;
    line-height: 24px;
    border-radius: 2px;
    padding: 0 8px;
}

.conversation-card {
    flex-direction: row;
    margin-bottom: 8px;
    background-color: #111111;
    border-radius: 3px;
    border-width: 1px;
    border-color: #333333;
    padding: 6px 8px;
    align-items: center;
}

.conversation-main {
    flex: 1;
    flex-direction: row;
    align-items: center;
}

.conversation-title {
    font-size: 16px;
    font-weight: bold;
    color: #ffffff;
    flex: 1;
    margin-right: 10px;
}

.current-indicator {
    font-size: 14px;
    color: #28a745;
    margin-right: 5px;
}

.conversation-time {
    font-size: 14px;
    color: #888888;
    margin-right: 10px;
}

.conversation-actions {
    flex-direction: row;
    align-items: center;
}

.action-btn {
    height: 24px;
    text-align: center;
    font-size: 16px;
    line-height: 24px;
    border-radius: 2px;
    margin-left: 8px;
    padding: 0 8px;
}

.edit-btn {
    background-color: #ffc107;
    color: #000000;
}

.delete-btn {
    background-color: #dc3545;
    color: #ffffff;
}

.empty-section {
    flex-direction: column;
    align-items: center;
    margin-top: 30px;
}

.empty-title {
    font-size: 18px;
    color: #888888;
    margin-bottom: 15px;
}

.loading-section {
    flex-direction: column;
    align-items: center;
    margin-top: 30px;
}

.loading-text {
    font-size: 18px;
    color: #17a2b8;
}

.error-section {
    background-color: #dc3545;
    padding: 8px;
    border-radius: 3px;
    margin-top: 10px;
}

.error-text {
    font-size: 16px;
    color: #ffffff;
    text-align: center;
}
</style>

<script>
import component from './aiHistory';
export default component;
</script>
