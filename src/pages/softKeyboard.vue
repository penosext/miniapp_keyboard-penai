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
    <div>
        <div class="editor-content">
            <text v-for="char in allChars" :key="char.id" :style="char.style" :class="{
                'cursor-overwrite': char.isCursor && editor && !editor.insertMode,
                'selected': char.isSelected
            }" class="editor-char">{{ char.text || ' ' }}</text>
        </div>
        <text v-if="isChineseMode" class="pinyin-display">{{ currentPinyin }}</text>
        <text v-for="candidate in candidateItems" :key="candidate.id" :class="{
            'candidate-selected': candidate.selected,
        }" class="candidate-item" :style="candidate.style">{{ candidate.display }}</text>
        <text v-for="key in keyboardKeys" :key="key.id" :style="key.style" class="key" @click="clicked(key.value)">{{
            key.displayText }}</text>
        <text v-if="keyPopup.visible" :style="keyPopup.style" class="key-popup">{{ keyPopup.displayText }}</text>
        <image-frame v-if="loadingChinese" ref="images" class="loading-image" interval="1" auto-play="true" :src="[
            require('../../images/loading/01.png'),
            require('../../images/loading/02.png'),
            require('../../images/loading/03.png'),
            require('../../images/loading/04.png'),
            require('../../images/loading/05.png'),
            require('../../images/loading/06.png'),
            require('../../images/loading/07.png'),
            require('../../images/loading/08.png'),
            require('../../images/loading/09.png'),
            require('../../images/loading/10.png'),
            require('../../images/loading/11.png'),
            require('../../images/loading/12.png'),
            require('../../images/loading/13.png'),
            require('../../images/loading/14.png'),
            require('../../images/loading/15.png'),
            require('../../images/loading/16.png'),
            require('../../images/loading/17.png'),
            require('../../images/loading/18.png'),
            require('../../images/loading/19.png'),
            require('../../images/loading/20.png'),
            require('../../images/loading/21.png'),
            require('../../images/loading/22.png'),
            require('../../images/loading/23.png'),
            require('../../images/loading/24.png'),
            require('../../images/loading/25.png'),
            require('../../images/loading/26.png'),
            require('../../images/loading/27.png'),
        ]" />
    </div>
</template>

<script>
import component from './softKeyboard';
export default component;
</script>

<style lang="less" scoped>
.editor-content {
    position: absolute;
    left: 0px;
    top: 0px;
    width: 560px;
    height: 170px;
    font-size: 14px;
    z-index: 5;
    background-color: rgb(0, 0, 0);
}

.editor-char {
    position: absolute;
    font-size: 14px;
    line-height: 16px;
    text-align: center;
    color: white;
}

.cursor-overwrite {
    background-color: rgba(255, 255, 255, 0.5);
    color: black;
}

.selected {
    background-color: #0078d4;
    color: white;
}

.key {
    position: absolute;
    color: rgba(255, 255, 255, 0.4);
    border-width: 1px;
    border-style: solid;
    text-align: center;
    z-index: 9;
    border-color: rgba(255, 255, 255, 0.2);
}

.key-popup {
    position: absolute;
    color: white;
    background-color: rgb(0, 0, 0);
    border-color: rgba(255, 255, 255, 0.3);
    text-align: center;
    z-index: 10;
    font-weight: bold;
}

.pinyin-display {
    position: absolute;
    left: 0px;
    top: 130px;
    width: 560px;
    height: 20px;
    align-items: center;
    color: #00ff00;
    font-size: 16px;
    font-weight: bold;
    z-index: 6;
}

.candidate-item {
    position: absolute;
    top: 150px;
    color: rgba(255, 255, 255, 0.8);
    font-size: 14px;
    height: 20px;
    z-index: 6;
}

.candidate-selected {
    background-color: #0078d4;
    color: white;
}

.loading-image {
    position: absolute;
    left: 255px;
    top: 60px;
    width: 50px;
    height: 50px;
    z-index: 11;
}
</style>
