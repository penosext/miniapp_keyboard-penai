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
import { showError, showSuccess } from '../components/ToastMessage';

export type aiHistoryOptions = {};

const component = defineComponent({
    data() {
        return {
            $page: {} as FalconPage<aiHistoryOptions>,
            conversationList: [] as any[],
            currentConversationId: '',

            searchKeyword: '',

            loading: false,
        };
    },

    async mounted() {
        try {
            AI.initialize()
            await this.loadConversationList();
        } catch (e) {
            showError(e as string || 'AI 初始化失败');
        }
    },

    computed: {
        filteredConversations(): any[] {
            let filtered = [...this.conversationList];
            if (this.searchKeyword) {
                const keyword = this.searchKeyword.toLowerCase();
                filtered = filtered.filter(conv => conv.title.toLowerCase().includes(keyword));
            }
            filtered.sort((a, b) => {
                return b.updatedAt - a.updatedAt;
            });
            return filtered;
        }
    },

    methods: {
        async loadConversationList() {
            this.loading = true;
            AI.getConversationList().then((list) => {
                this.conversationList = list;
                this.currentConversationId = AI.getCurrentConversationId();
            }).catch((e) => {
                showError(e as string || '加载对话列表失败');
            }).finally(() => {
                this.loading = false;
                this.$forceUpdate();
            });
        },

        async createConversation() {
            AI.createConversation(`新对话 ${Date.now()}`).then(() => {
                return this.loadConversationList();
            }).then(() => {
                this.$page.finish();
            }).catch((e) => {
                showError(e as string || '创建对话失败');
            });
        },

        async loadConversation(conversationId: string) {
            AI.loadConversation(conversationId).then(() => {
                this.currentConversationId = conversationId;
                this.$page.finish();
            }).catch((e) => {
                showError(e as string || '加载对话失败');
            });
        },

        async deleteConversation(conversationId: string) {
            AI.deleteConversation(conversationId).then(() => {
                return this.loadConversationList();
            }).then(() => {
                showSuccess('对话删除成功');
                if (conversationId === this.currentConversationId) {
                    this.currentConversationId = AI.getCurrentConversationId();
                }
            }).catch((e) => {
                showError(e as string || '删除对话失败');
            });
        },

        editConversationTitle(conversationId: string, currentTitle: string) {
            $falcon.navTo('softKeyboard', { data: currentTitle });
            const handler = async (e: any) => {
                const newTitle = e.data.data.trim();
                if (newTitle && newTitle !== currentTitle) {
                    AI.updateConversationTitle(conversationId, newTitle).then(() => {
                        showSuccess('标题修改成功');
                        return this.loadConversationList();
                    }).catch((e) => {
                        showError(e as string || '修改对话标题失败');
                    });
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
        }
    }
});

export default component;
