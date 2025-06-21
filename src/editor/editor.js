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

import Cursor from './cursor.js';
import Selection from './selection.js';
import TextBuffer from './textBuffer.js';
import History from './history.js';

export default class Editor {
    constructor(maxLineLength, maxLines) {
        this.insertMode = true;
        this.controlPressed = false;
        this.shiftPressed = false;
        this.capsLock = false;
        this.clipboard = '';
        this.maxLines = maxLines;
        this.scrollOffset = 0;

        this.cursor = new Cursor(maxLineLength);
        this.selection = new Selection(this.cursor);
        this.textBuffer = new TextBuffer();
        this.history = new History(this.textBuffer.data, this.cursor.row, this.cursor.col);

        this.shiftedChars = {
            '`': '~', '1': '!', '2': '@', '3': '#', '4': '$', '5': '%', '6': '^', '7': '&', '8': '*', '9': '(', '0': ')',
            '-': '_', '=': '+', '[': '{', ']': '}', '\\': '|', ';': ':', "'": '"', ',': '<', '.': '>', '/': '?'
        };
        this.keyMap = {
            default: (key) => {
                if (/[a-zA-Z]/.test(key)) {
                    if (this.controlPressed) { return; }
                    if (this.shiftPressed && this.capsLock) {
                        key = key.toLowerCase();
                    } else if (this.shiftPressed || this.capsLock) {
                        key = key.toUpperCase();
                    } else {
                        key = key.toLowerCase();
                    }
                } else if (this.shiftPressed) {
                    key = this.shiftedChars[key] || key;
                }
                this.handleInput(key);
                this.controlPressed = false;
            },
            'Enter': () => {
                if (this.controlPressed) {
                    const currentRow = this.cursor.row;
                    const newPos = this.textBuffer.insertText(currentRow, this.textBuffer.data[currentRow].length, '\n');
                    this.cursor.move(newPos.newRow, 0, this.textBuffer.data);
                    this.history.saveState(this.textBuffer.data, this.cursor.row, this.cursor.col);
                } else {
                    this.handleInput('\n');
                }
                this.controlPressed = this.shiftPressed = false;
            },
            'Tab': () => { this.handleInput('    '); this.controlPressed = this.shiftPressed = false; },
            'Backspace': () => {
                if (this.controlPressed && !this.selection.active) {
                    const originalRow = this.cursor.row;
                    const originalCol = this.cursor.col;
                    this.cursor.moveWordLeft(this.textBuffer.data);
                    const wordStart = { row: this.cursor.row, col: this.cursor.col };
                    const newPos = this.textBuffer.deleteText(wordStart.row, wordStart.col, originalRow, originalCol);
                    this.cursor.move(newPos.newRow, newPos.newCol, this.textBuffer.data);
                } else if (this.selection.active) {
                    const range = this.selection.getNormalizedRange();
                    const newPos = this.textBuffer.deleteText(range.startRow, range.startCol, range.endRow, range.endCol);
                    this.cursor.move(newPos.newRow, newPos.newCol, this.textBuffer.data);
                    this.selection.clear();
                } else {
                    const newPos = this.textBuffer.backspace(this.cursor.row, this.cursor.col);
                    this.cursor.move(newPos.newRow, newPos.newCol, this.textBuffer.data);
                }
                this.history.saveState(this.textBuffer.data, this.cursor.row, this.cursor.col);
                this.controlPressed = this.shiftPressed = false;
            },
            'Delete': () => {
                if (this.selection.active) {
                    const range = this.selection.getNormalizedRange();
                    const newPos = this.textBuffer.deleteText(range.startRow, range.startCol, range.endRow, range.endCol);
                    this.cursor.move(newPos.newRow, newPos.newCol, this.textBuffer.data);
                    this.selection.clear();
                } else {
                    const newPos = this.textBuffer.deleteForward(this.cursor.row, this.cursor.col);
                    this.cursor.move(newPos.newRow, newPos.newCol, this.textBuffer.data);
                }
                this.history.saveState(this.textBuffer.data, this.cursor.row, this.cursor.col);
                this.controlPressed = this.shiftPressed = false;
            },
            'Insert': () => { this.insertMode = !this.insertMode; },
            'Home': () => {
                if (this.shiftPressed) {
                    if (!this.selection.active) { this.selection.start(); }
                    this.cursor.moveToHome(this.textBuffer.data, this.controlPressed);
                    this.selection.update();
                } else {
                    this.cursor.moveToHome(this.textBuffer.data, this.controlPressed);
                    this.selection.clear();
                }
                this.ensureCursorVisible();
                this.controlPressed = this.shiftPressed = false;
            },
            'End': () => {
                if (this.shiftPressed) {
                    if (!this.selection.active) { this.selection.start(); }
                    this.cursor.moveToEnd(this.textBuffer.data, this.controlPressed);
                    this.selection.update();
                } else {
                    this.cursor.moveToEnd(this.textBuffer.data, this.controlPressed);
                    this.selection.clear();
                }
                this.ensureCursorVisible();
                this.controlPressed = this.shiftPressed = false;
            },
            'ArrowLeft': () => {
                if (this.shiftPressed) {
                    if (!this.selection.active) { this.selection.start(); }
                    if (this.controlPressed) {
                        this.cursor.moveWordLeft(this.textBuffer.data);
                    } else {
                        this.cursor.moveLeft(this.textBuffer.data);
                    }
                    this.selection.update();
                } else {
                    if (this.selection.active) {
                        const range = this.selection.getNormalizedRange();
                        this.cursor.move(range.startRow, range.startCol, this.textBuffer.data);
                        this.selection.clear();
                    } else {
                        if (this.controlPressed) {
                            this.cursor.moveWordLeft(this.textBuffer.data);
                        } else {
                            this.cursor.moveLeft(this.textBuffer.data);
                        }
                    }
                }
                this.ensureCursorVisible();
            },
            'ArrowRight': () => {
                if (this.shiftPressed) {
                    if (!this.selection.active) { this.selection.start(); }
                    if (this.controlPressed) {
                        this.cursor.moveWordRight(this.textBuffer.data);
                    } else {
                        this.cursor.moveRight(this.textBuffer.data);
                    }
                    this.selection.update();
                } else {
                    if (this.selection.active) {
                        const range = this.selection.getNormalizedRange();
                        this.cursor.move(range.endRow, range.endCol, this.textBuffer.data);
                        this.selection.clear();
                    } else {
                        if (this.controlPressed) {
                            this.cursor.moveWordRight(this.textBuffer.data);
                        } else {
                            this.cursor.moveRight(this.textBuffer.data);
                        }
                    }
                }
                this.ensureCursorVisible();
            },
            'ArrowUp': () => {
                if (this.controlPressed) {
                    this.scrollOffset = Math.max(0, this.scrollOffset - 1);
                } else if (this.shiftPressed) {
                    if (!this.selection.active) { this.selection.start(); }
                    this.cursor.moveUp(this.textBuffer.data);
                    this.selection.update();
                    this.ensureCursorVisible();
                } else {
                    this.cursor.moveUp(this.textBuffer.data);
                    this.selection.clear();
                    this.ensureCursorVisible();
                }
                this.controlPressed = false;
            },
            'ArrowDown': () => {
                if (this.controlPressed) {
                    let totalVisualLines = 0;
                    for (let logicalRow = 0; logicalRow < this.textBuffer.data.length; logicalRow++) {
                        const line = this.textBuffer.data[logicalRow];
                        const lineLength = Math.max(line.length, 1);
                        const numVisualLines = Math.max(1, Math.ceil(lineLength / this.cursor.lineLen));
                        totalVisualLines += numVisualLines;
                    }
                    const maxScrollOffset = Math.max(0, totalVisualLines - this.maxLines);
                    this.scrollOffset = Math.min(maxScrollOffset, this.scrollOffset + 1);
                } else if (this.shiftPressed) {
                    if (!this.selection.active) { this.selection.start(); }
                    this.cursor.moveDown(this.textBuffer.data);
                    this.selection.update();
                    this.ensureCursorVisible();
                } else {
                    this.cursor.moveDown(this.textBuffer.data);
                    this.selection.clear();
                    this.ensureCursorVisible();
                }
                this.controlPressed = false;
            },
            'PageUp': () => {
                if (this.shiftPressed) {
                    if (!this.selection.active) { this.selection.start(); }
                    this.cursor.movePageUp(this.textBuffer.data);
                    this.selection.update();
                } else {
                    this.cursor.movePageUp(this.textBuffer.data);
                    this.selection.clear();
                }
                this.ensureCursorVisible();
                this.controlPressed = this.shiftPressed = false;
            },
            'PageDown': () => {
                if (this.shiftPressed) {
                    if (!this.selection.active) { this.selection.start(); }
                    this.cursor.movePageDown(this.textBuffer.data);
                    this.selection.update();
                } else {
                    this.cursor.movePageDown(this.textBuffer.data);
                    this.selection.clear();
                }
                this.ensureCursorVisible();
                this.controlPressed = this.shiftPressed = false;
            },
            'Control': () => { this.controlPressed = !this.controlPressed; },
            'Shift': () => {
                this.shiftPressed = !this.shiftPressed;
                if (!this.shiftPressed && this.selection.active) {
                    const range = this.selection.getNormalizedRange();
                    if (range && range.startRow === range.endRow && range.startCol === range.endCol) {
                        this.selection.clear();
                    }
                }
            },
            'CapsLock': () => { this.capsLock = !this.capsLock; },
            'a': () => {
                if (this.controlPressed) {
                    this.selection.clear();
                    this.cursor.move(0, 0, this.textBuffer.data);
                    this.selection.start();
                    const lastRow = this.textBuffer.data.length - 1;
                    const lastCol = this.textBuffer.data[lastRow].length;
                    this.cursor.move(lastRow, lastCol, this.textBuffer.data);
                    this.selection.update();
                } else {
                    this.keyMap.default('a');
                }
            },
            'c': () => {
                if (this.controlPressed) {
                    if (this.selection.active) {
                        const range = this.selection.getNormalizedRange();
                        this.clipboard = this.textBuffer.getText(range.startRow, range.startCol, range.endRow, range.endCol);
                    } else {
                        this.clipboard = this.textBuffer.data[this.cursor.row];
                    }
                } else {
                    this.keyMap.default('c');
                }
            },
            'x': () => {
                if (this.controlPressed) {
                    if (this.selection.active) {
                        const range = this.selection.getNormalizedRange();
                        this.clipboard = this.textBuffer.getText(range.startRow, range.startCol, range.endRow, range.endCol);
                        const newPos = this.textBuffer.deleteText(range.startRow, range.startCol, range.endRow, range.endCol);
                        this.cursor.move(newPos.newRow, newPos.newCol, this.textBuffer.data);
                        this.selection.clear();
                        this.history.saveState(this.textBuffer.data, this.cursor.row, this.cursor.col);
                    } else {
                        this.clipboard = this.textBuffer.data[this.cursor.row];
                        if (this.textBuffer.data.length > 1) {
                            this.textBuffer.data.splice(this.cursor.row, 1);
                            if (this.cursor.row >= this.textBuffer.data.length) {
                                this.cursor.row = this.textBuffer.data.length - 1;
                            }
                            this.cursor.col = 0;
                        } else {
                            this.textBuffer.data[0] = '';
                            this.cursor.col = 0;
                        }
                        this.cursor.preferredCol = this.cursor.col;
                        this.history.saveState(this.textBuffer.data, this.cursor.row, this.cursor.col);
                    }
                } else {
                    this.keyMap.default('x');
                }
            },
            'v': () => {
                if (this.controlPressed) {
                    this.handleInput(this.clipboard);
                } else {
                    this.keyMap.default('v');
                }
            },
            'z': () => {
                if (this.controlPressed) {
                    const prevState = this.history.undo();
                    if (prevState) {
                        this.textBuffer.data = prevState.data.map(line => line);
                        this.cursor.move(prevState.cursorRow, prevState.cursorCol, this.textBuffer.data);
                        this.selection.clear();
                    }
                } else {
                    this.keyMap.default('z');
                }
            },
            'y': () => {
                if (this.controlPressed) {
                    const nextState = this.history.redo();
                    if (nextState) {
                        this.textBuffer.data = nextState.data.map(line => line);
                        this.cursor.move(nextState.cursorRow, nextState.cursorCol, this.textBuffer.data);
                        this.selection.clear();
                    }
                } else {
                    this.keyMap.default('y');
                }
            },
        };
    }

    handleInput(char) {
        let newCursorPos;
        if (this.selection.active) {
            const range = this.selection.getNormalizedRange();
            newCursorPos = this.textBuffer.deleteText(range.startRow, range.startCol, range.endRow, range.endCol);
            this.cursor.move(newCursorPos.newRow, newCursorPos.newCol, this.textBuffer.data);
            this.selection.clear();
        }
        if (this.insertMode) {
            newCursorPos = this.textBuffer.insertText(this.cursor.row, this.cursor.col, char);
        } else {
            const charLength = char.length;
            const currentLineLength = this.textBuffer.data[this.cursor.row].length;
            const charsToOverwrite = Math.min(charLength, currentLineLength - this.cursor.col);
            if (charsToOverwrite > 0) {
                this.textBuffer.deleteText(this.cursor.row, this.cursor.col, this.cursor.row, this.cursor.col + charsToOverwrite);
            }
            newCursorPos = this.textBuffer.insertText(this.cursor.row, this.cursor.col, char);
        }
        this.cursor.move(newCursorPos.newRow, newCursorPos.newCol, this.textBuffer.data);
        this.history.saveState(this.textBuffer.data, this.cursor.row, this.cursor.col);
        this.ensureCursorVisible();
        this.shiftPressed = false;
    }

    pressKey(key) {
        const handler = this.keyMap[key];
        if (typeof handler === 'function') {
            handler();
        } else if (key.length === 1) {
            this.keyMap.default(key);
        }
        if (key.length === 1) { this.controlPressed = false; }
    }

    ensureCursorVisible() {
        let cursorVisualRow = 0;
        const cursorRow = this.cursor.row;
        const cursorCol = this.cursor.col;
        for (let logicalRow = 0; logicalRow < cursorRow; logicalRow++) {
            const line = this.textBuffer.data[logicalRow];
            const lineLength = Math.max(line.length, 1);
            const numVisualLines = Math.max(1, Math.ceil(lineLength / this.cursor.lineLen));
            cursorVisualRow += numVisualLines;
        }
        const currentLineVisualRow = Math.floor(cursorCol / this.cursor.lineLen);
        cursorVisualRow += currentLineVisualRow;
        if (cursorVisualRow < this.scrollOffset) { this.scrollOffset = cursorVisualRow; }
        if (cursorVisualRow >= this.scrollOffset + this.maxLines) { this.scrollOffset = cursorVisualRow - this.maxLines + 1; }
    }

    getVisibleLines() {
        const visibleLines = [];
        let visualRow = 0;
        for (let logicalRow = 0; logicalRow < this.textBuffer.data.length; logicalRow++) {
            const line = this.textBuffer.data[logicalRow];
            const lineLength = Math.max(line.length, 1);
            const numVisualLines = Math.max(1, Math.ceil(lineLength / this.cursor.lineLen));
            for (let visualLineIndex = 0; visualLineIndex < numVisualLines; visualLineIndex++) {
                if (visualRow >= this.scrollOffset && visualRow < this.scrollOffset + this.maxLines) {
                    const startCharIndex = visualLineIndex * this.cursor.lineLen;
                    const endCharIndex = Math.min(startCharIndex + this.cursor.lineLen, lineLength);
                    visibleLines.push({
                        logicalRow,
                        visualLineIndex,
                        visualRow,
                        startCharIndex,
                        endCharIndex,
                        displayRow: visualRow - this.scrollOffset
                    });
                }
                visualRow++;
            }
        }
        return visibleLines;
    }

    getShiftedChar(keyValue) { return this.shiftedChars[keyValue] || keyValue; }
}
