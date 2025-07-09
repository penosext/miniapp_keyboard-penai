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
    <scroller class="settings-container" scroll-direction="vertical" :show-scrollbar="true">
        <div class="setting-section">
            <text class="section-title">API配置</text>

            <div class="setting-item">
                <text class="setting-text">API密钥&emsp;</text>
                <text class="setting-input" @click="editApiKey">{{apiKey.split('').map(_ => '*').join('') ||
                    '点击输入API密钥'}}</text>
            </div>

            <div class="setting-item">
                <text class="setting-text">基础URL&emsp;</text>
                <text class="setting-input" @click="editBaseUrl">{{ baseUrl || '点击输入基础URL' }}</text>
            </div>

            <div class="setting-item">
                <text class="setting-text">账户余额&emsp;</text>
                <text :class="'balance-text balance-' + (userBalance ? '' : 'un') + 'available'">{{
                    userBalance ? `¥${userBalance.toFixed(2)}` : '余额不可用'
                }}</text>
                <text @click="refreshBalance" class="refresh-btn">刷新</text>
            </div>
        </div>

        <div class="setting-section">
            <text class="section-title">模型参数</text>

            <div class="setting-item">
                <text class="setting-text">可用模型&emsp;</text>
                <div class="models-grid">
                    <text v-for="model in availableModels" :key="model" @click="selectModel(model)"
                        :class="'model-item ' + (modelName === model ? 'model-selected' : '')">{{ model }}</text>
                </div>
                <text @click="refreshModels" class="refresh-btn">刷新模型</text>
            </div>

            <div class="setting-item">
                <text class="setting-text">温度&emsp;</text>
                <text class="setting-input" @click="editTemperature">{{ temperature.toFixed(1) }}</text>
            </div>

            <div class="setting-item">
                <text class="setting-text">TopP&emsp;</text>
                <text class="setting-input" @click="editTopP">{{ topP.toFixed(1) }}</text>
            </div>

            <div class="setting-item">
                <text class="setting-text">最大长度&emsp;</text>
                <text class="setting-input" @click="editMaxTokens">{{ maxTokens }}</text>
            </div>
        </div>

        <div class="setting-section">
            <text class="section-title">系统设置</text>

            <div class="setting-item">
                <text class="setting-text">系统提示词&emsp;</text>
                <text class="setting-textarea" @click="editSystemPrompt">{{ systemPrompt }}</text>
            </div>
        </div>

        <div class="btn-area">
            <text @click="saveSettings" class="save-btn">保存</text>
        </div>

        <ToastMessage />
    </scroller>
</template>

<style lang="less" scoped>
.settings-container {
    position: absolute;
    left: 0;
    top: 0;
    width: 560px;
    height: 170px;
    color: #ffffff;
    font-family: monospace;
    padding: 5px;
}

.setting-section {
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

.setting-item {
    min-height: 24px;
    margin-bottom: 8px;
    display: flex;
    flex-direction: row;
    align-items: center;
}

.setting-toggle {
    flex-direction: row;
    align-items: center;
}

.setting-text {
    height: 24px;
    line-height: 24px;
    font-size: 18px;
    color: #ffffff;
}

.setting-input,
.setting-textarea {
    display: flex;
    flex-direction: row;
    flex: 1;

    height: 24px;
    background-color: #111111;
    color: #ffffff;
    border-width: 1px;
    border-color: #444444;
    font-size: 18px;
    line-height: 20px;
    padding: 2px;
    border-radius: 2px;
}

.setting-textarea {
    min-height: 80px;
}

.balance-text {
    flex: 1;
    font-weight: bold;
    height: 24px;
    line-height: 24px;
    font-size: 18px;
    padding: 0 8px;
    border-radius: 2px;
}

.balance-available {
    color: #28a745;
    background-color: #1a4a2e;
}

.balance-unavailable {
    color: #dc3545;
    background-color: #4a1e1e;
}

.refresh-btn {
    height: 24px;
    background-color: #17a2b8;
    color: #ffffff;
    text-align: center;
    font-size: 18px;
    line-height: 24px;
    border-radius: 2px;
    margin-left: 8px;
    padding: 0 8px;
}

.models-grid {
    flex: 1;
    flex-direction: row;
    flex-wrap: wrap;
    margin-bottom: 5px;
}

.model-item {
    height: 24px;
    line-height: 24px;
    font-size: 18px;

    background-color: #333333;
    color: #ffffff;
    padding: 0px 8px;
}

.model-selected {
    background-color: #007acc;
    border-color: #007acc;
    font-weight: bold;
}

.btn-area {
    height: 24px;
    width: 100%;
    display: flex;
    justify-content: center;
    align-items: center;
}

.save-btn {
    border-radius: 2px;
    line-height: 24px;
    font-size: 18px;
    padding: 0 8px;

    background-color: #007acc;
    color: #ffffff;
}
</style>

<script>
import aiSettings from './aiSettings';
import ToastMessage from '../components/ToastMessage.vue';
export default {
    ...aiSettings,
    components: {
        ToastMessage
    }
};
</script>
