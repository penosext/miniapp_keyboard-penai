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

export default class Cursor {
    constructor(lineLen, row = 0, col = 0) {
        this.row = row;
        this.col = this.preferredCol = col;
        this.lineLen = lineLen;
    }

    move(newRow, newCol, textData) {
        this.row = Math.max(0, Math.min(newRow, textData.length - 1));
        this.col = this.preferredCol = Math.max(0, Math.min(newCol, textData[this.row].length));
    }

    moveLeft(textData) {
        if (this.col > 0) {
            this.col--;
        } else if (this.row > 0) {
            this.row--;
            this.col = textData[this.row].length;
        }
        this.preferredCol = this.col;
    }

    moveRight(textData) {
        if (this.col < textData[this.row].length) {
            this.col++;
        } else if (this.row < textData.length - 1) {
            this.row++;
            this.col = 0;
        }
        this.preferredCol = this.col;
    }

    moveUp(textData) {
        const curLine = textData[this.row];
        if (curLine.length > this.lineLen) {
            const curVisualRow = Math.floor(this.col / this.lineLen);
            if (curVisualRow > 0) {
                const visualColInCurRow = this.preferredCol % this.lineLen;
                const newCol = (curVisualRow - 1) * this.lineLen + visualColInCurRow;
                this.col = Math.max(0, Math.min(newCol, curLine.length));
                return;
            }
        }
        if (this.row > 0) {
            this.row--;
            const prevLine = textData[this.row];
            if (prevLine.length > this.lineLen) {
                const lastVisualRow = Math.floor(prevLine.length / this.lineLen);
                const visualColInCurRow = this.preferredCol % this.lineLen;
                const targetCol = lastVisualRow * this.lineLen + visualColInCurRow;
                this.col = Math.min(targetCol, prevLine.length);
            } else {
                this.col = Math.min(this.preferredCol, prevLine.length);
            }
        } else {
            this.col = this.preferredCol = 0;
        }
    }

    moveDown(textData) {
        const curLine = textData[this.row];
        if (curLine.length > this.lineLen) {
            const curVisualRow = Math.floor(this.col / this.lineLen);
            const maxVisualRows = Math.floor(curLine.length / this.lineLen);
            if (curVisualRow < maxVisualRows) {
                const visualColInCurRow = this.preferredCol % this.lineLen;
                const newCol = (curVisualRow + 1) * this.lineLen + visualColInCurRow;
                this.col = Math.min(newCol, curLine.length);
                return;
            }
        }
        if (this.row < textData.length - 1) {
            this.row++;
            const nextLine = textData[this.row];
            const visualColInCurRow = this.preferredCol % this.lineLen;
            this.col = Math.min(visualColInCurRow, nextLine.length);
        } else {
            this.col = this.preferredCol = textData[this.row].length;
        }
    }

    moveToHome(textData, controlPressed) {
        if (controlPressed) {
            this.row = 0;
            this.col = this.preferredCol = 0;
        } else {
            const curLine = textData[this.row];
            if (curLine.length <= this.lineLen) {
                this.col = this.preferredCol = 0;
            } else {
                const curVisualRow = Math.floor(this.col / this.lineLen);
                const visualLineStart = curVisualRow * this.lineLen;
                if (this.col === visualLineStart && curVisualRow > 0) {
                    this.col = this.preferredCol = 0;
                } else {
                    this.col = this.preferredCol = visualLineStart;
                }
            }
        }
    }

    moveToEnd(textData, controlPressed) {
        if (controlPressed) {
            this.row = textData.length - 1;
            this.col = this.preferredCol = textData[this.row].length;
        } else {
            const curLine = textData[this.row];
            if (curLine.length <= this.lineLen) {
                this.col = this.preferredCol = curLine.length;
            } else {
                const curVisualRow = Math.floor(this.col / this.lineLen);
                const visualLineStart = curVisualRow * this.lineLen;
                const visualLineEnd = Math.min(visualLineStart + this.lineLen, curLine.length);
                const visualLineLastChar = Math.max(visualLineStart, visualLineEnd);

                if (this.col === visualLineLastChar && visualLineEnd < curLine.length) {
                    this.col = this.preferredCol = curLine.length;
                } else {
                    this.col = this.preferredCol = visualLineLastChar;
                }
            }
        }
    }

    movePageUp(textData, linesPerPage = 10) {
        this.row = Math.max(0, this.row - linesPerPage);
        this.col = Math.min(this.col, textData[this.row].length);
    }

    movePageDown(textData, linesPerPage = 10) {
        this.row = Math.min(textData.length - 1, this.row + linesPerPage);
        this.col = Math.min(this.col, textData[this.row].length);
    }

    moveWordLeft(textData) {
        let curRow = this.row;
        let curCol = this.col;
        let curLine = textData[curRow];
        if (curCol === 0) {
            if (curRow > 0) {
                curRow--;
                curLine = textData[curRow];
                curCol = curLine.length;
            } else {
                return;
            }
        }
        while (curCol > 0 && !/\w/.test(curLine[curCol - 1])) { curCol--; }
        while (curCol > 0 && /\w/.test(curLine[curCol - 1])) { curCol--; }
        this.col = this.preferredCol = curCol;
        this.row = curRow;
    }

    moveWordRight(textData) {
        let curRow = this.row;
        let curCol = this.col;
        let curLine = textData[curRow];
        if (curCol === curLine.length) {
            if (curRow < textData.length - 1) {
                curRow++;
                curLine = textData[curRow];
                curCol = 0;
            } else {
                return;
            }
        }
        while (curCol < curLine.length && !/\w/.test(curLine[curCol])) { curCol++; }
        while (curCol < curLine.length && /\w/.test(curLine[curCol])) { curCol++; }
        this.col = this.preferredCol = curCol;
        this.row = curRow;
    }
}
