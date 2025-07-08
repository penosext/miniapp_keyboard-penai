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

export type aiSettingsOptions = {};

const component = defineComponent({
    data() {
        return {
            $page: {} as FalconPage<aiSettingsOptions>,
            apiKey: '',
            baseUrl: 'https://api.deepseek.com/v1/',
            modelName: 'deepseek-chat',

            maxTokens: 4096,
            temperature: 0.7,
            topP: 1.0,
            systemPrompt: '你是一个有用的助手。请尽力回答问题。请不要使用任何 Markdown 语法或者表情符号等特殊字符来格式化回答。',

            balanceLoading: false,
            userBalance: 0.0,
            availableModels: [] as string[],
            modelsLoading: false,

            errorMessage: '',
        };
    },

    async mounted() {
        try {
            AI.initialize();
            await this.loadSettings();
            await this.refreshBalance();
            await this.refreshModels();
        } catch (e) { this.errorMessage = e as string || 'AI 初始化失败'; }
    },

    methods: {
        async loadSettings() {
            try {
                const settings = AI.getSettings();
                this.apiKey = settings.apiKey;
                this.baseUrl = settings.baseUrl;
                this.modelName = settings.modelName;
                this.temperature = settings.temperature;
                this.topP = settings.topP;
                this.maxTokens = settings.maxTokens;
                this.systemPrompt = settings.systemPrompt;
            } catch (e) {
                this.errorMessage = e as string || '加载设置失败';
            }
        },

        async refreshBalance() {
            this.userBalance = 0.0;
            this.balanceLoading = true;
            AI.getUserBalance().then((balance) => {
                this.userBalance = balance;
            }).catch((e) => {
                this.errorMessage = `获取余额失败: ${e}`;
            }).finally(() => {
                this.balanceLoading = false;
                this.$forceUpdate();
            });
        },

        async refreshModels() {
            this.availableModels = [];
            this.modelsLoading = true;
            AI.getModels().then((models) => {
                this.availableModels = models;
            }).catch((e) => {
                this.errorMessage = `获取模型列表失败: ${e}`;
            }).finally(() => {
                this.modelsLoading = false;
                this.$forceUpdate();
            });
        },

        selectModel(model: string) {
            this.modelName = model;
            this.$forceUpdate();
        },

        saveSettings() {
            try {
                AI.setSettings(this.apiKey, this.baseUrl,
                    this.modelName, this.maxTokens,
                    this.temperature, this.topP, this.systemPrompt,);
                $falcon.navTo('index', {});
            } catch (e) {
                this.errorMessage = e as string || '保存设置失败';
            }
        },

        editApiKey() {
            $falcon.navTo('softKeyboard', { data: this.apiKey });
            const handler = (e: any) => {
                this.apiKey = e.data.data;
                this.$forceUpdate();
                $falcon.off('softKeyboard', handler);
            };
            $falcon.on<SoftKeyboardEvent>('softKeyboard', handler);
        },

        editBaseUrl() {
            $falcon.navTo('softKeyboard', { data: this.baseUrl });
            const handler = (e: any) => {
                this.baseUrl = e.data.data;
                this.$forceUpdate();
                $falcon.off('softKeyboard', handler);
            };
            $falcon.on<SoftKeyboardEvent>('softKeyboard', handler);
        },

        editMaxTokens() {
            $falcon.navTo('softKeyboard', { data: this.maxTokens.toString() });
            const handler = (e: any) => {
                const value = parseInt(e.data.data);
                if (!isNaN(value) && value > 0) {
                    this.maxTokens = value;
                }
                this.$forceUpdate();
                $falcon.off('softKeyboard', handler);
            };
            $falcon.on<SoftKeyboardEvent>('softKeyboard', handler);
        },

        editTemperature() {
            $falcon.navTo('softKeyboard', { data: this.temperature.toFixed(1) });
            const handler = (e: any) => {
                const value = parseFloat(e.data.data);
                if (!isNaN(value) && value >= 0 && value <= 1) {
                    this.temperature = value;
                }
                this.$forceUpdate();
                $falcon.off('softKeyboard', handler);
            };
            $falcon.on<SoftKeyboardEvent>('softKeyboard', handler);
        },

        editTopP() {
            $falcon.navTo('softKeyboard', { data: this.topP.toFixed(1) });
            const handler = (e: any) => {
                const value = parseFloat(e.data.data);
                if (!isNaN(value) && value >= 0 && value <= 1) {
                    this.topP = value;
                }
                this.$forceUpdate();
                $falcon.off('softKeyboard', handler);
            };
            $falcon.on<SoftKeyboardEvent>('softKeyboard', handler);
        },

        editSystemPrompt() {
            $falcon.navTo('softKeyboard', { data: this.systemPrompt });
            const handler = (e: any) => {
                this.systemPrompt = e.data.data;
                this.$forceUpdate();
                $falcon.off('softKeyboard', handler);
            };
            $falcon.on<SoftKeyboardEvent>('softKeyboard', handler);
        }
    }
});

export default component;
