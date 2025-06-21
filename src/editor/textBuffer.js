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

export default class TextBuffer {
    constructor(initialData = ['']) {
        this.data = initialData;
    }

    insertText(row, col, text) {
        const linesToInsert = text.split('\n');
        const currentLine = this.data[row];
        if (linesToInsert.length === 1) {
            this.data[row] = currentLine.slice(0, col) + text + currentLine.slice(col);
            return { newRow: row, newCol: col + text.length };
        } else {
            const firstPart = currentLine.slice(0, col);
            const lastPart = currentLine.slice(col);
            this.data[row] = firstPart + linesToInsert[0];
            for (let i = 1; i < linesToInsert.length - 1; i++) {
                this.data.splice(row + i, 0, linesToInsert[i]);
            }
            this.data.splice(row + linesToInsert.length - 1, 0, linesToInsert[linesToInsert.length - 1] + lastPart);
            return {
                newRow: row + linesToInsert.length - 1,
                newCol: linesToInsert[linesToInsert.length - 1].length
            };
        }
    }

    deleteText(startRow, startCol, endRow, endCol) {
        if (startRow === endRow) {
            this.data[startRow] = this.data[startRow].slice(0, startCol) + this.data[startRow].slice(endCol);
        } else {
            this.data[startRow] = this.data[startRow].slice(0, startCol) + this.data[endRow].slice(endCol);
            this.data.splice(startRow + 1, endRow - startRow);
        }
        if (this.data.length === 0) {
            this.data.push('');
        }
        return { newRow: startRow, newCol: startCol };
    }

    backspace(row, col) {
        if (col > 0) {
            this.data[row] = this.data[row].slice(0, col - 1) + this.data[row].slice(col);
            return { newRow: row, newCol: col - 1 };
        } else if (row > 0) {
            const newCol = this.data[row - 1].length;
            this.data[row - 1] += this.data[row];
            this.data.splice(row, 1);
            return { newRow: row - 1, newCol: newCol };
        }
        return { newRow: row, newCol: col };
    }

    deleteForward(row, col) {
        if (col < this.data[row].length) {
            this.data[row] = this.data[row].slice(0, col) + this.data[row].slice(col + 1);
            return { newRow: row, newCol: col };
        } else if (row < this.data.length - 1) {
            this.data[row] += this.data[row + 1];
            this.data.splice(row + 1, 1);
            return { newRow: row, newCol: col };
        }
        return { newRow: row, newCol: col };
    }

    getText(startRow, startCol, endRow, endCol) {
        let selectedText = [];
        if (startRow === endRow) {
            selectedText.push(this.data[startRow].slice(startCol, endCol));
        } else {
            selectedText.push(this.data[startRow].slice(startCol));
            for (let i = startRow + 1; i < endRow; i++) {
                selectedText.push(this.data[i]);
            }
            selectedText.push(this.data[endRow].slice(0, endCol));
        }
        return selectedText.join('\n');
    }
}
