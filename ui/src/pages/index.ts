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
import { ai } from 'langningchen';

// const ai = {
//     initialize: (apiKey: string, baseUrl: string) => {
//         console.log(`AI initialized with API Key: ${apiKey} and Base URL: ${baseUrl}`);
//         return true;
//     },
//     getCurrentNodeId: () => 'root-node-id',
//     getRootNodeId: () => 'root-node-id',
//     getCurrentPath: () => [
//         { role: 'system', content: 'Welcome to the AI conversation!' },
//         { role: 'user', content: 'Hello, AI!' },
//         { role: 'assistant', content: 'Hello! How can I assist you today?' }
//     ],
//     getChildNodes: (nodeId: string) => ['child-node-1', 'child-node-2'],
//     sendMessageStream: async (message: string) => {
//         console.log(`Sending message: ${message}`);
//         // Simulate a successful response
//         return {
//             success: true,
//             content: `AI response to: ${message}`
//         };
//     },
//     switchToNode: (nodeId: string) => {
//         console.log(`Switched to node: ${nodeId}`);
//         return true;
//     },
//     exportConversationTree: () => {
//         console.log('Exporting conversation tree...');
//         return JSON.stringify({
//             rootNodeId: 'root-node-id',
//             nodes: [
//                 { id: 'root-node-id', messages: [] },
//                 { id: 'child-node-1', messages: [] },
//                 { id: 'child-node-2', messages: [] }
//             ]
//         });
//     },
//     importConversationTree: (treeData: string) => {
//         console.log('Importing conversation tree:', treeData);
//         // Simulate successful import
//         return true;
//     },
//     on: (event: string, callback: (data: any) => void) => {
//         console.log(`Listening for event: ${event}`);
//         // Simulate an event
//         setTimeout(() => {
//             if (event === 'ai_stream') {
//                 callback({ type: 'stream', content: 'Streaming content...' });
//             }
//         }, 1000);
//     }
// };

interface Message {
    role: 'user' | 'assistant' | 'system';
    content: string;
    nodeId?: string;
    timestamp?: number;
}

interface ConversationBranch {
    nodeId: string;
    preview: string;
    isActive: boolean;
}

const component = defineComponent({
    data() {
        return {
            $page: {} as FalconPage,
            // AI相关
            aiInitialized: false,
            isLoading: false,
            currentInput: '',
            messages: [] as Message[],
            streamingContent: '',
            isStreaming: false,

            // 对话树相关
            currentNodeId: '',
            rootNodeId: '',
            conversationBranches: [] as ConversationBranch[],
            showBranches: false,

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
        await this.loadSettings();
        if (this.apiKey) {
            await this.initializeAI();
        }

        // 监听AI流式事件
        ai.on('ai_stream', (data: any) => {
            if (data.type === 'stream') {
                this.streamingContent += data.content;
                this.$forceUpdate();
            }
        });
    },

    computed: {
        displayMessages(): Message[] {
            const messages = [...this.messages];
            if (this.isStreaming && this.streamingContent) {
                messages.push({
                    role: 'assistant',
                    content: this.streamingContent,
                    timestamp: Date.now()
                });
            }
            return messages;
        },

        canSendMessage(): boolean {
            return this.aiInitialized && !this.isLoading && !this.isStreaming && this.currentInput.trim().length > 0;
        }
    },

    methods: {
        async loadSettings() {
            // 从jsapi storage加载设置
            try {
                const result = await $falcon.jsapi.storage.getStorage({ key: 'ai_settings' });
                if (result && result.data) {
                    const settings = JSON.parse(result.data);
                    this.apiKey = settings.apiKey || '';
                    this.baseUrl = settings.baseUrl || 'https://api.deepseek.com/';
                }
            } catch (e) {
                console.error('Failed to load settings:', e);
                // 如果没有设置或加载失败，使用默认值
                this.apiKey = '';
                this.baseUrl = 'https://api.deepseek.com/';
            }
        },

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

            try {
                const success = ai.initialize(this.apiKey, this.baseUrl);
                if (success) {
                    this.aiInitialized = true;
                    this.currentNodeId = ai.getCurrentNodeId();
                    this.rootNodeId = ai.getRootNodeId();
                    await this.refreshConversation();
                    this.errorMessage = '';
                } else {
                    this.errorMessage = 'AI初始化失败';
                }
            } catch (error) {
                this.errorMessage = `初始化错误: ${error}`;
                console.error('AI initialization failed:', error);
            }
        },

        async refreshConversation() {
            if (!this.aiInitialized) return;

            try {
                // 获取当前对话路径
                const path = ai.getCurrentPath();
                this.messages = path.filter((msg: any) => msg.role !== 'system');

                // 获取当前节点的分支
                await this.updateBranches();

                this.$forceUpdate();
            } catch (error) {
                console.error('Failed to refresh conversation:', error);
            }
        },

        async updateBranches() {
            if (!this.aiInitialized || !this.currentNodeId) return;

            try {
                // 找到最近的用户消息节点来获取分支
                const path = ai.getCurrentPath();
                let lastUserNodeId = this.currentNodeId;

                // 简化：直接获取当前节点的子节点作为分支
                const childNodes = ai.getChildNodes(this.currentNodeId);
                this.conversationBranches = childNodes.map((nodeId: string) => ({
                    nodeId,
                    preview: `分支 ${nodeId.substring(0, 8)}...`,
                    isActive: nodeId === this.currentNodeId
                }));
            } catch (error) {
                console.error('Failed to update branches:', error);
            }
        },

        async sendMessage() {
            if (!this.canSendMessage) return;

            const userMessage = this.currentInput.trim();
            this.currentInput = '';
            this.isLoading = true;
            this.isStreaming = true;
            this.streamingContent = '';
            this.errorMessage = '';

            // 立即显示用户消息
            this.messages.push({
                role: 'user',
                content: userMessage,
                timestamp: Date.now()
            });

            try {
                // 发送流式消息
                const response = await ai.sendMessageStream(userMessage);

                if (response.success) {
                    // 添加助手回复到消息列表
                    this.messages.push({
                        role: 'assistant',
                        content: response.content,
                        timestamp: Date.now()
                    });

                    // 更新当前节点ID
                    this.currentNodeId = ai.getCurrentNodeId();
                    await this.updateBranches();
                } else {
                    this.errorMessage = response.error || '发送消息失败';
                    // 移除用户消息
                    this.messages.pop();
                }
            } catch (error) {
                this.errorMessage = `发送消息错误: ${error}`;
                this.messages.pop();
                console.error('Send message failed:', error);
            } finally {
                this.isLoading = false;
                this.isStreaming = false;
                this.streamingContent = '';
                this.$forceUpdate();
            }
        },

        async switchToBranch(nodeId: string) {
            if (!this.aiInitialized) return;

            try {
                const success = ai.switchToNode(nodeId);
                if (success) {
                    this.currentNodeId = nodeId;
                    await this.refreshConversation();
                    this.showBranches = false;
                }
            } catch (error) {
                console.error('Failed to switch branch:', error);
            }
        },

        async newConversation() {
            if (!this.aiInitialized) return;

            try {
                const success = ai.switchToNode(this.rootNodeId);
                if (success) {
                    this.currentNodeId = this.rootNodeId;
                    await this.refreshConversation();
                }
            } catch (error) {
                console.error('Failed to start new conversation:', error);
            }
        },

        // 软键盘相关
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

        // 设置相关
        async saveAndApplySettings() {
            await this.saveSettings();
            await this.initializeAI();
            this.showSettings = false;
        },

        formatTime(timestamp?: number): string {
            if (!timestamp) return '';
            return new Date(timestamp).toLocaleTimeString();
        },

        // CSS class helpers for Weex compatibility
        getMessageClass(message: any): string {
            if (message.role === 'user') {
                return 'message-user';
            } else if (message.role === 'assistant') {
                return 'message-assistant';
            }
            return 'message';
        },

        getBranchClass(branch: any): string {
            return branch.isActive ? 'branch-item-active' : 'branch-item';
        },

        getSendButtonClass(): string {
            return this.canSendMessage ? 'btn-primary' : 'btn-disabled';
        }
    }
});

export default component;
