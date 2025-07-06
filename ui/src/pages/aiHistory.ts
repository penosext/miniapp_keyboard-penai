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

import { defineComponent } from 'vue';
import { SoftKeyboardEvent } from './softKeyboard';
import { AI } from 'langningchen';

export type aiHistoryOptions = {};

const component = defineComponent({
    data() {
        return {
            $page: {} as FalconPage<aiHistoryOptions>,
            // 对话数据
            conversationList: [] as any[],
            currentConversationId: '',

            // 搜索功能
            searchKeyword: '',

            // UI状态
            errorMessage: '',
            loading: false,
        };
    },

    async mounted() {
        if (AI.initialize()) {
            await this.loadConversationList();
        }
        else { this.errorMessage = 'AI 初始化失败'; }
    },

    computed: {
        filteredConversations(): any[] {
            let filtered = [...this.conversationList];

            // 搜索过滤
            if (this.searchKeyword) {
                const keyword = this.searchKeyword.toLowerCase();
                filtered = filtered.filter(conv => conv.title.toLowerCase().includes(keyword));
            }

            // 按更新时间降序排序
            filtered.sort((a, b) => {
                return b.updatedAt - a.updatedAt;
            });

            return filtered;
        }
    },

    methods: {
        async loadConversationList() {
            this.loading = true;
            try {
                const response = await AI.getConversationList();
                if (response.success) {
                    this.conversationList = response.conversations;
                    this.currentConversationId = AI.getCurrentConversationId();
                } else {
                    this.errorMessage = response.errorMessage || '加载对话列表失败';
                }
            } catch (e) {
                this.errorMessage = '加载对话列表时发生错误';
            } finally {
                this.loading = false;
                this.$forceUpdate();
            }
        },

        getMessageCount(conversationId: string): number {
            return Math.floor(Math.random() * 20) + 1;
        },

        async createConversation() {
            try {
                const success = await AI.createConversation(`新对话 ${Date.now()}`);
                if (success) {
                    await this.loadConversationList();
                    this.goBack();
                } else {
                    this.errorMessage = '创建新对话失败';
                }
            } catch (e) {
                this.errorMessage = '创建新对话时发生错误';
            }
        },

        async loadConversation(conversationId: string) {
            try {
                const success = await AI.loadConversation(conversationId);
                if (success) {
                    this.currentConversationId = conversationId;
                    this.goBack(); // 加载后返回主界面
                } else {
                    this.errorMessage = '加载对话失败';
                }
            } catch (e) {
                this.errorMessage = '加载对话时发生错误';
                console.error('Failed to load conversation:', e);
            }
        },

        async deleteConversation(conversationId: string) {
            try {
                const success = await AI.deleteConversation(conversationId);
                if (success) {
                    await this.loadConversationList();
                    if (conversationId === this.currentConversationId) {
                        this.currentConversationId = AI.getCurrentConversationId();
                    }
                } else {
                    this.errorMessage = '删除对话失败';
                }
            } catch (e) {
                this.errorMessage = '删除对话时发生错误';
                console.error('Failed to delete conversation:', e);
            }
        },

        editConversationTitle(conversationId: string, currentTitle: string) {
            $falcon.navTo('softKeyboard', { data: currentTitle });
            const handler = async (e: any) => {
                const newTitle = e.data.data.trim();
                if (newTitle && newTitle !== currentTitle) {
                    try {
                        const success = await AI.updateConversationTitle(conversationId, newTitle);
                        if (success) {
                            await this.loadConversationList();
                        } else {
                            this.errorMessage = '修改对话标题失败';
                        }
                    } catch (error) {
                        this.errorMessage = '修改对话标题时发生错误';
                        console.error('Failed to update conversation title:', error);
                    }
                }
                $falcon.off('softKeyboard', handler);
            };
            $falcon.on<SoftKeyboardEvent>('softKeyboard', handler);
        },

        editSearchKeyword() {
            $falcon.navTo('softKeyboard', { data: this.searchKeyword });
            const handler = (e: any) => {
                this.searchKeyword = e.data.data;
                this.$forceUpdate();
                $falcon.off('softKeyboard', handler);
            };
            $falcon.on<SoftKeyboardEvent>('softKeyboard', handler);
        },

        clearSearch() {
            this.searchKeyword = '';
            this.$forceUpdate();
        },

        formatTime(timestamp: number): string {
            const date = new Date(timestamp * 1000);
            const now = new Date();
            const diffMs = now.getTime() - date.getTime();
            const diffMins = Math.floor(diffMs / (1000 * 60));
            const diffHours = Math.floor(diffMs / (1000 * 60 * 60));
            const diffDays = Math.floor(diffMs / (1000 * 60 * 60 * 24));

            if (diffMins < 1) return '刚刚';
            if (diffMins < 60) return `${diffMins}分钟前`;
            if (diffHours < 24) return `${diffHours}小时前`;
            if (diffDays < 7) return `${diffDays}天前`;
            return date.toLocaleDateString();
        },

        goBack() {
            $falcon.navTo('index', {});
        }
    }
});

export default component;
