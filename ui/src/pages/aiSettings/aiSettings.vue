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
@import url('aiSettings.less');
</style>

<script>
import aiSettings from './aiSettings';
import ToastMessage from '../../components/ToastMessage.vue';
export default {
    ...aiSettings,
    components: {
        ToastMessage
    }
};
</script>
