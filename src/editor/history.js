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

export default class History {
    constructor(initialData, initialCursorRow, initialCursorCol) {
        this.undoStack = [{
            data: initialData,
            cursorRow: initialCursorRow,
            cursorCol: initialCursorCol
        }];
        this.redoStack = [];
        this.maxHistory = 100;
    }

    saveState(textData, cursorRow, cursorCol) {
        this.redoStack = [];
        const state = {
            data: textData.map(line => line),
            cursorRow: cursorRow,
            cursorCol: cursorCol
        };
        if (this.undoStack.length > 0) {
            const lastState = this.undoStack[this.undoStack.length - 1];
            if (lastState.data.length === state.data.length &&
                lastState.data.every((line, i) => line === state.data[i]) &&
                lastState.cursorRow === state.cursorRow &&
                lastState.cursorCol === state.cursorCol) {
                return;
            }
        }
        this.undoStack.push(state);
        if (this.undoStack.length > this.maxHistory) { this.undoStack.shift(); }
    }

    undo() {
        if (this.undoStack.length <= 1) { return null; }
        this.redoStack.push(this.undoStack.pop());
        return this.undoStack[this.undoStack.length - 1];
    }

    redo() {
        if (this.redoStack.length === 0) { return null; }
        this.undoStack.push(this.redoStack.pop());
        return this.undoStack[this.undoStack.length - 1];
    }
}
