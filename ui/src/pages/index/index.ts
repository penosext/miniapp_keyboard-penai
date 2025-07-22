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
import { AI } from 'langningchen';
import { ROLE, AIStreamResult, ConversationNode } from '../../@types/langningchen';
import { showError } from '../../components/ToastMessage';
import { openSoftKeyboard } from '../../utils/softKeyboardUtils';

export type indexOptions = {};

const index = defineComponent({
    data() {
        return {
            $page: {} as FalconPage<indexOptions>,
            aiInitialized: false,
            currentInput: '',
            streamingContent: '',
            isStreaming: false,
            messages: [] as ConversationNode[],

            currentConversationId: '',

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
            showError(e as string || 'AI 初始化失败');
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
                showError(e as string || '获取消息失败');
            }
        },
        getMessage(messageId: string): ConversationNode | undefined { return this.displayMessages.find(m => m.id === messageId); },

        async sendMessage(userMessage: string) {
            if (!this.canSendMessage) return;
            userMessage = userMessage.trim();

            this.streamingContent = '';

            AI.addUserMessage(userMessage).then(() => {
                this.refreshMessages();
                this.$forceUpdate();
                this.generateResponse();
            }).catch((e) => {
                showError(e as string || '添加用户消息失败');
            });
            this.currentInput = '';
        },

        async generateResponse() {
            this.isStreaming = true;
            AI.generateResponse().then(() => {
                this.refreshMessages();
                this.$forceUpdate();
            }).catch((e) => {
                showError(e as string || '生成响应失败');
            }).finally(() => {
                this.isStreaming = false;
                this.streamingContent = '';
            });
        },

        stopGeneration() {
            if (this.isStreaming) {
                AI.stopGeneration();
                // Give a small delay to allow backend to save the partial response
                setTimeout(() => {
                    this.isStreaming = false;
                    this.streamingContent = '';
                    this.refreshMessages();
                    this.$forceUpdate();
                }, 100);
            }
        },

        loadSoftKeyboard() {
            if (this.isStreaming) return;
            openSoftKeyboard(
                () => this.currentInput,
                (value) => { this.currentInput = value; this.$forceUpdate(); }
            );
        },

        openSettings() {
            if (this.isStreaming) return;
            $falcon.navTo('aiSettings', {});
        },

        openHistory() {
            if (this.isStreaming) return;
            $falcon.navTo('aiHistory', {});
        },

        async regenerateMessage(messageId: string) {
            if (this.isStreaming) return;
            try {
                AI.switchToNode(this.getMessage(messageId)!.parentId);
                this.generateResponse();
            } catch (e) {
                showError(e as string || '切换消息失败');
            }
        },

        switchVariant(messageId: string, direction: number) {
            if (this.isStreaming) return;
            const message = this.getMessage(messageId)!;
            const parentMessage = this.getMessage(message.parentId);
            if (!parentMessage) return;
            const currentIndex = parentMessage.childIds.indexOf(messageId);
            const newIndex = currentIndex + direction;
            if (newIndex >= 0 && newIndex < parentMessage.childIds.length) {
                try {
                    let newId = parentMessage.childIds[newIndex];
                    while (AI.getChildNodes(newId).length > 0) {
                        newId = AI.getChildNodes(newId)[0];
                    }
                    AI.switchToNode(newId);
                    this.refreshMessages();
                    this.$forceUpdate();
                } catch (e) {
                    showError(e as string || '切换消息失败');
                }
            }
        },

        getCurrentVariantInfo(messageId: string): string {
            return this.getVariantInfo(messageId);
        },

        canGoVariant(messageId: string, direction: number): boolean {
            if (this.isStreaming) return false;
            const message = this.getMessage(messageId)!;
            const parentMessage = this.getMessage(message.parentId);
            if (!parentMessage) return false;

            const currentIndex = parentMessage.childIds.indexOf(messageId);
            if (direction < 0) {
                return currentIndex > 0;
            } else {
                return currentIndex < parentMessage.childIds.length - 1;
            }
        },

        editUserMessage(messageId: string) {
            if (this.isStreaming) return;
            const message = this.getMessage(messageId)!;
            openSoftKeyboard(
                () => message.content,
                (newContent) => {
                    if (newContent.trim() !== message.content.trim()) {
                        try {
                            AI.switchToNode(message.parentId);
                            this.sendMessage(newContent);
                        } catch (e) {
                            showError(e as string || '编辑消息失败');
                        }
                    }
                }
            );
        },

        getVariantInfo(messageId: string): string {
            const message = this.getMessage(messageId)!;
            const parentMessage = this.getMessage(message.parentId);
            if (!parentMessage) return "1/1";

            const currentIndex = parentMessage.childIds.indexOf(messageId);
            return `${currentIndex + 1}/${parentMessage.childIds.length}`;
        }
    }
});

export default index;
