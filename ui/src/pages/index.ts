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
import { AI, AIStreamResult, ConversationNode } from 'langningchen';
import { ROLE } from '../ime/Role';

export type indexOptions = {};

const component = defineComponent({
    data() {
        return {
            $page: {} as FalconPage<indexOptions>,
            // AI相关
            aiInitialized: false,
            currentInput: '',
            streamingContent: '',
            isStreaming: false,
            messages: [] as ConversationNode[],

            // UI状态
            showSettings: false,
            apiKey: '',
            baseUrl: 'https://api.deepseek.com/',

            // 错误处理
            errorMessage: '',

            // 软键盘
            inputResult: [''],
        };
    },

    async mounted() {
        this.apiKey = '';
        this.baseUrl = 'https://api.deepseek.com/';
        const result = await $falcon.jsapi.storage.getStorage({ key: 'ai_settings' });
        if (result?.data) {
            const settings = JSON.parse(result.data);
            this.apiKey = settings.apiKey;
            this.baseUrl = settings.baseUrl;
        }

        if (this.apiKey) {
            await this.initializeAI();
        }

        AI.on('ai_stream', (data: AIStreamResult) => {
            this.streamingContent += data.messageDelta;
            this.$forceUpdate();
        });
    },

    computed: {
        displayMessages(): ConversationNode[] {
            if (this.isStreaming && this.streamingContent) {
                const lastMessage = this.messages[this.messages.length - 1];
                if (lastMessage && lastMessage.role === ROLE.ROLE_ASSISTANT) {
                    lastMessage.content = this.streamingContent;
                }
                else {
                    const tempId = `streaming_${Date.now()}`;
                    this.messages[this.messages.length - 1].childIds.push(tempId);
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
        refreshMessages() {
            this.messages = AI.getCurrentPath().map((node: ConversationNode) => ({ ...node, childIds: [...node.childIds] }));
        },
        getMessage(messageId: string): ConversationNode | undefined { return this.displayMessages.find(m => m.id === messageId); },
        async saveSettings() {
            const settings = {
                apiKey: this.apiKey,
                baseUrl: this.baseUrl
            };
            try {
                await $falcon.jsapi.storage.setStorage({
                    key: 'ai_settings',
                    data: JSON.stringify(settings)
                });
            } catch (e) {
                console.error('Failed to save settings:', e);
            }
        },

        async initializeAI() {
            if (!this.apiKey || !this.baseUrl) {
                this.errorMessage = '请先配置API密钥和基础URL';
                return;
            }

            const success = AI.initialize(this.apiKey, this.baseUrl);
            if (success) {
                this.aiInitialized = true;
                this.refreshMessages();
                this.errorMessage = '';
            } else {
                this.errorMessage = 'AI初始化失败';
            }
        },

        async sendMessage(userMessage: string) {
            if (!this.canSendMessage) return;
            userMessage = userMessage.trim();

            this.streamingContent = '';
            this.errorMessage = '';

            const addMessageResponse = await AI.addUserMessage(userMessage);
            if (!addMessageResponse.success) {
                this.errorMessage = addMessageResponse.errorMessage || '添加用户消息失败';
                return;
            }
            this.refreshMessages();
            this.$forceUpdate();
            this.generateResponse();
        },

        async generateResponse() {
            this.currentInput = '';
            this.isStreaming = true;
            const response = await AI.generateResponse();
            if (!response.success) {
                this.errorMessage = response.errorMessage || '发送消息失败';
            }
            this.isStreaming = false;
            this.streamingContent = '';
            this.refreshMessages();
            this.$forceUpdate();
        },

        async newConversation() { console.log('to be implemented'); },

        loadSoftKeyboard() {
            $falcon.navTo('softKeyboard', { data: this.currentInput });
            $falcon.on<SoftKeyboardEvent>('softKeyboard', (e) => {
                this.currentInput = e.data.data;
                this.$forceUpdate();
            });
        },

        editApiKey() {
            $falcon.navTo('softKeyboard', { data: this.apiKey });
            $falcon.on<SoftKeyboardEvent>('softKeyboard', (e) => {
                this.apiKey = e.data.data;
                this.$forceUpdate();
            });
        },

        editBaseUrl() {
            $falcon.navTo('softKeyboard', { data: this.baseUrl });
            $falcon.on<SoftKeyboardEvent>('softKeyboard', (e) => {
                this.baseUrl = e.data.data;
                this.$forceUpdate();
            });
        },

        async saveAndApplySettings() {
            await this.saveSettings();
            await this.initializeAI();
            this.showSettings = false;
        },

        async regenerateMessage(messageId: string) {
            AI.switchToNode(this.getMessage(messageId)!.parentId);
            this.generateResponse();
        },

        previousVariant(messageId: string) {
            const message = this.getMessage(messageId)!;
            const userMessage = this.getMessage(message.parentId)!;
            const currentIndex = userMessage.childIds.indexOf(messageId);
            let newId = userMessage.childIds[currentIndex - 1];
            while (AI.getChildNodes(newId).length > 0) {
                newId = AI.getChildNodes(newId)[0];
            }
            AI.switchToNode(newId);
            this.refreshMessages();
            this.$forceUpdate();
        },

        nextVariant(messageId: string) {
            const message = this.getMessage(messageId)!;
            const userMessage = this.getMessage(message.parentId)!;
            const currentIndex = userMessage.childIds.indexOf(messageId);
            let newId = userMessage.childIds[currentIndex + 1];
            while (AI.getChildNodes(newId).length > 0) {
                newId = AI.getChildNodes(newId)[0];
            }
            AI.switchToNode(newId);
            this.refreshMessages();
            this.$forceUpdate();
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
