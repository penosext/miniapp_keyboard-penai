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
      <text 
        v-for="char in allChars" 
        :key="char.id"
        :style="char.style"
        :class="{
          'cursor-overwrite': char.isCursor && editor && !editor.insertMode,
          'selected': char.isSelected
        }"
        class="editor-char"
      >{{ char.text || ' ' }}</text>
    </div>
    <text
      v-for="key in keyboardKeys"
      :key="key.id"
      :style="key.style"
      class="key"
      @click="clicked(key.value)"
    >{{ key.displayText }}</text>
    <text
      v-if="keyPopup.visible"
      :style="keyPopup.style"
      class="key-popup"
    >{{ keyPopup.displayText }}</text>
  </div>
</template>

<script>
import Editor from '../../editor/editor.js';

const maxLineLength = 50;

export default {
    name: 'KeyboardIndex',
    data() { 
        return { 
            editor: null, 
            keyPopup: {
                visible: false,
                displayText: '',
                style: {}
            },
            popupTimer: null
        }; 
    },
    mounted() {
        this.editor = new Editor(maxLineLength);
        this.editor.handleInput('Hello World!\nThis is a text editor.');
    },
    computed: {
        allChars() {
            if (!this.editor) return [];

            const chars = [];
            const textData = this.editor.textBuffer.data;
            const cursorRow = this.editor.cursor.row;
            const cursorCol = this.editor.cursor.col;
            const charWidth = 8;
            const lineHeight = 16;

            const selectionRange = this.editor.selection.active ? this.editor.selection.getNormalizedRange() : null;

            let visualRow = 0; // 视觉行号

            textData.forEach((line, logicalRow) => {
                // 确保行长度至少包含光标位置
                const lineLength = Math.max(line.length, logicalRow === cursorRow ? cursorCol + 1 : line.length);

                // 将长行按 maxLineLength 拆分成多个视觉行
                const numVisualLines = Math.max(1, Math.ceil(lineLength / maxLineLength));

                for (let visualLineIndex = 0; visualLineIndex < numVisualLines; visualLineIndex++) {
                    const startCharIndex = visualLineIndex * maxLineLength;
                    const endCharIndex = Math.min(startCharIndex + maxLineLength, lineLength);

                    for (let charIndex = startCharIndex; charIndex < endCharIndex; charIndex++) {
                        const isCursor = (logicalRow === cursorRow && charIndex === cursorCol);
                        let isSelected = false;

                        // 检查是否在选择范围内
                        if (selectionRange) {
                            const { startRow, startCol, endRow, endCol } = selectionRange;
                            if (logicalRow > startRow && logicalRow < endRow) {
                                isSelected = true;
                            } else if (logicalRow === startRow && logicalRow === endRow) {
                                isSelected = charIndex >= startCol && charIndex < endCol;
                            } else if (logicalRow === startRow) {
                                isSelected = charIndex >= startCol;
                            } else if (logicalRow === endRow) {
                                isSelected = charIndex < endCol;
                            }
                        }

                        const visualCol = charIndex - startCharIndex;

                        chars.push({
                            id: `char-${logicalRow}-${charIndex}`,
                            text: charIndex < line.length ? line[charIndex] : ' ',
                            isCursor,
                            isSelected,
                            style: {
                                position: 'absolute',
                                left: `${visualCol * charWidth}px`,
                                top: `${visualRow * lineHeight}px`,
                                width: `${charWidth}px`,
                                height: `${lineHeight}px`
                            }
                        });

                        // 插入模式时，在光标位置添加竖线
                        if (isCursor && this.editor && this.editor.insertMode) {
                            chars.push({
                                id: `cursor-line-${logicalRow}-${charIndex}`,
                                text: '',
                                isCursor: false,
                                isSelected: false,
                                style: {
                                    position: 'absolute',
                                    left: `${visualCol * charWidth}px`,
                                    top: `${visualRow * lineHeight}px`,
                                    width: '1px',
                                    height: `${lineHeight}px`,
                                    backgroundColor: 'white',
                                    zIndex: 7
                                }
                            });
                        }
                    }

                    // 特殊处理：如果光标在当前视觉行的末尾位置，需要额外添加
                    const cursorAtLineEnd = (logicalRow === cursorRow && cursorCol >= startCharIndex && cursorCol < startCharIndex + maxLineLength && cursorCol >= endCharIndex);
                    if (cursorAtLineEnd) {
                        const visualCol = cursorCol - startCharIndex;
                        chars.push({
                            id: `char-${logicalRow}-${cursorCol}`,
                            text: ' ',
                            isCursor: true,
                            isSelected: false,
                            style: {
                                position: 'absolute',
                                left: `${visualCol * charWidth}px`,
                                top: `${visualRow * lineHeight}px`,
                                width: `${charWidth}px`,
                                height: `${lineHeight}px`
                            }
                        });

                        // 插入模式时，在光标位置添加竖线
                        if (this.editor && this.editor.insertMode) {
                            chars.push({
                                id: `cursor-line-${logicalRow}-${cursorCol}`,
                                text: '',
                                isCursor: false,
                                isSelected: false,
                                style: {
                                    position: 'absolute',
                                    left: `${visualCol * charWidth}px`,
                                    top: `${visualRow * lineHeight}px`,
                                    width: '1px',
                                    height: `${lineHeight}px`,
                                    backgroundColor: 'white',
                                    zIndex: 7
                                }
                            });
                        }
                    }

                    visualRow++;
                }
            });

            return chars;
        },

        keyboardKeys() {
            const keyHeight = 34;
            const standardKeyWidth = 31;
            const fontSize = 20;

            const keyboardLayout = [
                [
                    { value: '`', displayText: '`' },
                    { value: '1', displayText: '1' },
                    { value: '2', displayText: '2' },
                    { value: '3', displayText: '3' },
                    { value: '4', displayText: '4' },
                    { value: '5', displayText: '5' },
                    { value: '6', displayText: '6' },
                    { value: '7', displayText: '7' },
                    { value: '8', displayText: '8' },
                    { value: '9', displayText: '9' },
                    { value: '0', displayText: '0' },
                    { value: '-', displayText: '-' },
                    { value: '=', displayText: '=' },
                    { value: 'Backspace', displayText: 'back', width: 2 },
                    { value: 'Insert', displayText: 'ins' },
                    { value: 'Home', displayText: 'hm' },
                    { value: 'PageUp', displayText: 'pu' },
                ],
                [
                    { value: 'Tab', displayText: 'Tab', width: 1.5 },
                    { value: 'q', displayText: 'Q' },
                    { value: 'w', displayText: 'W' },
                    { value: 'e', displayText: 'E' },
                    { value: 'r', displayText: 'R' },
                    { value: 't', displayText: 'T' },
                    { value: 'y', displayText: 'Y' },
                    { value: 'u', displayText: 'U' },
                    { value: 'i', displayText: 'I' },
                    { value: 'o', displayText: 'O' },
                    { value: 'p', displayText: 'P' },
                    { value: '[', displayText: '[' },
                    { value: ']', displayText: ']' },
                    { value: '\\', displayText: '\\', width: 1.5 },
                    { value: 'Delete', displayText: 'del' },
                    { value: 'End', displayText: 'ed' },
                    { value: 'PageDown', displayText: 'pd' },
                ],
                [
                    { value: 'CapsLock', displayText: 'Caps', width: 2 },
                    { value: 'a', displayText: 'A' },
                    { value: 's', displayText: 'S' },
                    { value: 'd', displayText: 'D' },
                    { value: 'f', displayText: 'F' },
                    { value: 'g', displayText: 'G' },
                    { value: 'h', displayText: 'H' },
                    { value: 'j', displayText: 'J' },
                    { value: 'k', displayText: 'K' },
                    { value: 'l', displayText: 'L' },
                    { value: ';', displayText: ';' },
                    { value: "'", displayText: "'" },
                    { value: 'Enter', displayText: 'Enter', width: 2 },
                ],
                [
                    { value: 'Shift', displayText: 'Shift', width: 2.5 },
                    { value: 'z', displayText: 'Z' },
                    { value: 'x', displayText: 'X' },
                    { value: 'c', displayText: 'C' },
                    { value: 'v', displayText: 'V' },
                    { value: 'b', displayText: 'B' },
                    { value: 'n', displayText: 'N' },
                    { value: 'm', displayText: 'M' },
                    { value: ',', displayText: ',' },
                    { value: '.', displayText: '.' },
                    { value: '/', displayText: '/' },
                    { value: 'Shift', displayText: 'Shift', width: 2.5 },
                    { value: 'ArrowUp', displayText: '↑', leftOffset: 1 },
                ],
                [
                    { value: 'Control', displayText: 'Ctrl', width: 1.5 },
                    { value: 'Zh', displayText: 'Zh', width: 1.5 },
                    { value: ' ', displayText: '', width: 8.5 },
                    { value: 'Dictation', displayText: 'Dc' },
                    { value: 'Close', displayText: 'cl' },
                    { value: 'Control', displayText: 'Ctrl', width: 1.5 },
                    { value: 'ArrowLeft', displayText: '←' },
                    { value: 'ArrowDown', displayText: '↓' },
                    { value: 'ArrowRight', displayText: '→' },
                ],
            ];

            const generatedKeys = [];
            let keyId = 0;

            keyboardLayout.forEach((row, rowIndex) => {
                let currentX = 0;
                const currentY = rowIndex * keyHeight;
                row.forEach((keyConfig) => {
                    const keyWidth = Math.round((keyConfig.width || 1) * standardKeyWidth);
                    const isActive = this.isKeyActive(keyConfig.value);
                    if (keyConfig.leftOffset) { currentX += keyConfig.leftOffset * standardKeyWidth; }
                    let displayText = keyConfig.displayText;
                    if (this.editor && this.editor.shiftPressed && 
                        keyConfig.value.trim().length == 1 && 
                        !/[a-zA-Z]/.test(keyConfig.value)) {
                        displayText = this.editor.getShiftedChar(keyConfig.value);
                    }
                    generatedKeys.push({
                        id: `key-${keyId++}`,
                        value: keyConfig.value,
                        displayText,
                        isActive,
                        style: {
                            left: `${currentX}px`,
                            top: `${currentY}px`,
                            width: `${keyWidth}px`,
                            height: `${keyHeight}px`,
                            backgroundColor: isActive ? 'rgba(255, 255, 255, 0.3)' : 'rgba(255, 255, 255, 0.1)',
                            lineHeight: `${keyHeight}px`,
                            fontSize: `${fontSize}px`,
                        },
                    });
                    currentX += keyWidth;
                });
            });
            return generatedKeys;
        },
    },

    methods: {
        clicked(key) {
            if (this.editor) {
                this.editor.pressKey(key);
                this.showKeyPopup(key);
                this.$forceUpdate();
            }
        },
        showKeyPopup(keyValue) {
            if (keyValue.trim().length !== 1) { return; }

            const keyElement = this.keyboardKeys.find(k => k.value === keyValue);
            if (!keyElement) return;

            let displayText = keyElement.displayText;
            if (this.editor && this.editor.shiftPressed) {
                displayText = this.editor.getShiftedChar(keyValue);
            }

            const keyLeft = parseInt(keyElement.style.left);
            const keyTop = parseInt(keyElement.style.top);
            const keyWidth = parseInt(keyElement.style.width);
            const popupWidth = 40;
            const popupHeight = 40;
            const popupTop = keyTop - popupHeight;
            this.keyPopup = {
                visible: true,
                displayText: displayText,
                style: {
                    left: `${keyLeft + keyWidth / 2 - popupWidth / 2}px`,
                    top: `${popupTop}px`,
                    width: `${popupWidth}px`,
                    height: `${popupHeight}px`,
                    lineHeight: `${popupHeight}px`,
                    fontSize: `${popupHeight * 0.8}px`,
                }
            };
            if (this.popupTimer) { clearTimeout(this.popupTimer); }
            this.popupTimer = setTimeout(() => {
                this.keyPopup.visible = false;
                this.popupTimer = null;
            }, 1000);
        },
        isKeyActive(key) {
            if (!this.editor) return false;
            switch (key) {
                case 'CapsLock':
                    return this.editor.capsLock;
                case 'Shift':
                    return this.editor.shiftPressed;
                case 'Control':
                    return this.editor.controlPressed;
                case 'Insert':
                    return this.editor.insertMode;
                default:
                    return false;
            }
        }
    },
    beforeDestroy() {
        if (this.popupTimer) { clearTimeout(this.popupTimer); }
    }
};
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
</style>
