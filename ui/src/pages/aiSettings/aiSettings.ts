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
import { SoftKeyboardEvent } from '../softKeyboard/softKeyboard';
import { AI } from 'langningchen';
import { showError, showSuccess, showWarning } from '../../components/ToastMessage';
import { hideLoading, showLoading } from '../../components/Loading';

export type aiSettingsOptions = {};

const aiSettings = defineComponent({
    data() {
        return {
            $page: {} as FalconPage<aiSettingsOptions>,
            apiKey: '',
            baseUrl: '',
            modelName: '',

            maxTokens: 0,
            temperature: 0,
            topP: 0,
            systemPrompt: '',

            userBalance: 0.0,
            availableModels: [] as string[],
        };
    },

    mounted() {
        try {
            AI.initialize();
            this.loadSettings();
            this.refreshBalance();
            this.refreshModels();
        } catch (e) {
            showError(e as string || 'AI 初始化失败');
        }
    },

    methods: {
        loadSettings() {
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
                showError(e as string || '加载设置失败');
            }
        },

        refreshBalance() {
            this.userBalance = 0.0;
            showLoading();
            AI.getUserBalance().then((balance) => {
                this.userBalance = balance;
            }).catch((e) => {
                showError(`获取余额失败: ${e}`);
            }).finally(() => {
                hideLoading();
            });
        },

        refreshModels() {
            this.availableModels = [];
            showLoading();
            AI.getModels().then((models) => {
                this.availableModels = models;
            }).catch((e) => {
                showError(`获取模型列表失败: ${e}`);
            }).finally(() => {
                hideLoading();
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
                showSuccess('设置已保存');
            } catch (e) {
                showError(e as string || '保存设置失败');
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
                if (!this.baseUrl.startsWith("http")) {
                    showWarning('基础 URL 需要以 http 或 https 开头')
                }
                else if (!this.baseUrl.endsWith('/')) {
                    showWarning('基础 URL 需要以 / 结尾')
                }
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

export default aiSettings;
