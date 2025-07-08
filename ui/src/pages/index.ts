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
import { ROLE, AIStreamResult, ConversationNode } from '../@types/langningchen';

export type indexOptions = {};

const component = defineComponent({
    data() {
        return {
            $page: {} as FalconPage<indexOptions>,
            aiInitialized: false,
            currentInput: '',
            streamingContent: '',
            isStreaming: false,
            messages: [] as ConversationNode[],

            currentConversationId: '',

            errorMessage: '',

            inputResult: [''],
        };
    },

    created() {
        this.$page.on("show", this.onPageShow);
    },
    destroyed() {
        this.$page.off("show", this.onPageShow);
    },

    async mounted() {
        try {
            AI.initialize();
            this.aiInitialized = true;
            this.refreshMessages();
            AI.on('ai_stream', (data: AIStreamResult) => {
                this.streamingContent += data.messageDelta;
                this.$forceUpdate();
            });
        } catch (e) {
            this.errorMessage = e as string || 'AI 初始化失败';
        }
    },

    computed: {
        displayMessages(): ConversationNode[] {
            if (this.isStreaming && this.streamingContent) {
                const lastMessage = this.messages[this.messages.length - 1];
                if (lastMessage && lastMessage.role === ROLE.ROLE_ASSISTANT) {
                    lastMessage.content = this.streamingContent;
                }
                else if (lastMessage) {
                    const tempId = `streaming_${Date.now()}`;
                    lastMessage.childIds.push(tempId);
                    const streamingMessage: ConversationNode = {
                        role: ROLE.ROLE_ASSISTANT,
                        content: this.streamingContent,
                        timestamp: Date.now().toString(),
                        id: tempId,
                        parentId: lastMessage.id,
                        childIds: [],
                    };
                    this.messages.push(streamingMessage);
                }
            }
            return this.messages;
        },
        canSendMessage(): boolean {
            return this.aiInitialized && !this.isStreaming;
        }
    },

    methods: {
        onPageShow() {
            this.refreshMessages();
        },

        refreshMessages() {
            try {
                this.messages = AI.getCurrentPath().map((node: ConversationNode) => ({ ...node, childIds: [...node.childIds] }));
            } catch (e) {
                this.errorMessage = e as string || '获取消息失败';
            }
        },
        getMessage(messageId: string): ConversationNode | undefined { return this.displayMessages.find(m => m.id === messageId); },

        async sendMessage(userMessage: string) {
            if (!this.canSendMessage) return;
            userMessage = userMessage.trim();

            this.streamingContent = '';
            this.errorMessage = '';

            AI.addUserMessage(userMessage).then(() => {
                this.refreshMessages();
                this.$forceUpdate();
                this.generateResponse();
            }).catch((e) => {
                this.errorMessage = e as string || '添加用户消息失败';
            });
            this.currentInput = '';
        },

        async generateResponse() {
            this.isStreaming = true;
            AI.generateResponse().then(() => {
                this.refreshMessages();
                this.$forceUpdate();
            }).catch((e) => {
                this.errorMessage = e as string || '生成响应失败';
            }).finally(() => {
                this.isStreaming = false;
                this.streamingContent = '';
            });
        },

        loadSoftKeyboard() {
            $falcon.navTo('softKeyboard', { data: this.currentInput });
            const handler = (e: any) => {
                this.currentInput = e.data.data;
                this.$forceUpdate();
                $falcon.off('softKeyboard', handler);
            };
            $falcon.on<SoftKeyboardEvent>('softKeyboard', handler);
        },

        openSettings() {
            $falcon.navTo('aiSettings', {});
        },

        openHistory() {
            $falcon.navTo('aiHistory', {});
        },

        async regenerateMessage(messageId: string) {
            try {
                AI.switchToNode(this.getMessage(messageId)!.parentId);
                this.generateResponse();
            } catch (e) {
                this.errorMessage = e as string || '切换消息失败';
            }
        },

        previousVariant(messageId: string) {
            const message = this.getMessage(messageId)!;
            const userMessage = this.getMessage(message.parentId)!;
            const currentIndex = userMessage.childIds.indexOf(messageId);
            try {
                let newId = userMessage.childIds[currentIndex - 1];
                while (AI.getChildNodes(newId).length > 0) {
                    newId = AI.getChildNodes(newId)[0];
                }
                AI.switchToNode(newId);
                this.refreshMessages();
                this.$forceUpdate();
            } catch (e) {
                this.errorMessage = e as string || '切换消息失败';
            }
        },

        nextVariant(messageId: string) {
            const message = this.getMessage(messageId)!;
            const userMessage = this.getMessage(message.parentId)!;
            const currentIndex = userMessage.childIds.indexOf(messageId);
            try {
                let newId = userMessage.childIds[currentIndex + 1];
                while (AI.getChildNodes(newId).length > 0) {
                    newId = AI.getChildNodes(newId)[0];
                }
                AI.switchToNode(newId);
                this.refreshMessages();
                this.$forceUpdate();
            } catch (e) {
                this.errorMessage = e as string || '切换消息失败';
            }
        },

        getCurrentVariantInfo(messageId: string): string {
            const message = this.getMessage(messageId)!;
            const userMessage = this.getMessage(message.parentId)!;
            const currentIndex = userMessage.childIds.indexOf(messageId);
            return `${currentIndex + 1}/${userMessage.childIds.length}`;
        },

        canGoPrevious(messageId: string): boolean {
            const message = this.getMessage(messageId)!;
            const userMessage = this.getMessage(message.parentId)!;
            const currentIndex = userMessage.childIds.indexOf(messageId);
            return currentIndex > 0;
        },

        canGoNext(messageId: string): boolean {
            const message = this.getMessage(messageId)!;
            const userMessage = this.getMessage(message.parentId)!;
            const currentIndex = userMessage.childIds.indexOf(messageId);
            return currentIndex < userMessage.childIds.length - 1;
        }
    }
});

export default component;
