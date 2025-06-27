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

import { TextData } from "./textBuffer";

export type POS = {
    row: number;
    col: number;
};

export default class Cursor {
    pos: POS = { row: 0, col: 0 };
    preferredCol: number = 0;
    lineLen: number;

    constructor(lineLen: number) {
        this.lineLen = lineLen;
    }

    move(newPos: POS, textData: TextData) {
        this.pos.row = Math.max(0, Math.min(newPos.row, textData.length - 1));
        this.pos.col = this.preferredCol = Math.max(0, Math.min(newPos.col, textData[this.pos.row].length));
    }

    moveLeft(textData: TextData) {
        if (this.pos.col > 0) {
            this.pos.col--;
        } else if (this.pos.row > 0) {
            this.pos.row--;
            this.pos.col = textData[this.pos.row].length;
        }
        this.preferredCol = this.pos.col;
    }

    moveRight(textData: TextData) {
        if (this.pos.col < textData[this.pos.row].length) {
            this.pos.col++;
        } else if (this.pos.row < textData.length - 1) {
            this.pos.row++;
            this.pos.col = 0;
        }
        this.preferredCol = this.pos.col;
    }

    moveUp(textData: TextData) {
        const curLine = textData[this.pos.row];
        if (curLine.length > this.lineLen) {
            const curVisualRow = Math.floor(this.pos.col / this.lineLen);
            if (curVisualRow > 0) {
                const visualColInCurRow = this.preferredCol % this.lineLen;
                const newCol = (curVisualRow - 1) * this.lineLen + visualColInCurRow;
                this.pos.col = Math.max(0, Math.min(newCol, curLine.length));
                return;
            }
        }
        if (this.pos.row > 0) {
            this.pos.row--;
            const prevLine = textData[this.pos.row];
            if (prevLine.length > this.lineLen) {
                const lastVisualRow = Math.floor(prevLine.length / this.lineLen);
                const visualColInCurRow = this.preferredCol % this.lineLen;
                const targetCol = lastVisualRow * this.lineLen + visualColInCurRow;
                this.pos.col = Math.min(targetCol, prevLine.length);
            } else {
                this.pos.col = Math.min(this.preferredCol, prevLine.length);
            }
        } else {
            this.pos.col = this.preferredCol = 0;
        }
    }

    moveDown(textData: TextData) {
        const curLine = textData[this.pos.row];
        if (curLine.length > this.lineLen) {
            const curVisualRow = Math.floor(this.pos.col / this.lineLen);
            const maxVisualRows = Math.floor(curLine.length / this.lineLen);
            if (curVisualRow < maxVisualRows) {
                const visualColInCurRow = this.preferredCol % this.lineLen;
                const newCol = (curVisualRow + 1) * this.lineLen + visualColInCurRow;
                this.pos.col = Math.min(newCol, curLine.length);
                return;
            }
        }
        if (this.pos.row < textData.length - 1) {
            this.pos.row++;
            const nextLine = textData[this.pos.row];
            const visualColInCurRow = this.preferredCol % this.lineLen;
            this.pos.col = Math.min(visualColInCurRow, nextLine.length);
        } else {
            this.pos.col = this.preferredCol = textData[this.pos.row].length;
        }
    }

    moveToHome(textData: TextData, controlPressed: boolean) {
        if (controlPressed) {
            this.pos.row = 0;
            this.pos.col = this.preferredCol = 0;
        } else {
            const curLine = textData[this.pos.row];
            if (curLine.length <= this.lineLen) {
                this.pos.col = this.preferredCol = 0;
            } else {
                const curVisualRow = Math.floor(this.pos.col / this.lineLen);
                const visualLineStart = curVisualRow * this.lineLen;
                if (this.pos.col === visualLineStart && curVisualRow > 0) {
                    this.pos.col = this.preferredCol = 0;
                } else {
                    this.pos.col = this.preferredCol = visualLineStart;
                }
            }
        }
    }

    moveToEnd(textData: TextData, controlPressed: boolean) {
        if (controlPressed) {
            this.pos.row = textData.length - 1;
            this.pos.col = this.preferredCol = textData[this.pos.row].length;
        } else {
            const curLine = textData[this.pos.row];
            if (curLine.length <= this.lineLen) {
                this.pos.col = this.preferredCol = curLine.length;
            } else {
                const curVisualRow = Math.floor(this.pos.col / this.lineLen);
                const visualLineStart = curVisualRow * this.lineLen;
                const visualLineEnd = Math.min(visualLineStart + this.lineLen, curLine.length);
                const visualLineLastChar = Math.max(visualLineStart, visualLineEnd);

                if (this.pos.col === visualLineLastChar && visualLineEnd < curLine.length) {
                    this.pos.col = this.preferredCol = curLine.length;
                } else {
                    this.pos.col = this.preferredCol = visualLineLastChar;
                }
            }
        }
    }

    movePageUp(textData: TextData, linesPerPage: number = 10) {
        this.pos.row = Math.max(0, this.pos.row - linesPerPage);
        this.pos.col = Math.min(this.pos.col, textData[this.pos.row].length);
    }

    movePageDown(textData: TextData, linesPerPage: number = 10) {
        this.pos.row = Math.min(textData.length - 1, this.pos.row + linesPerPage);
        this.pos.col = Math.min(this.pos.col, textData[this.pos.row].length);
    }

    moveWordLeft(textData: TextData) {
        let curRow = this.pos.row;
        let curCol = this.pos.col;
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
        this.pos.col = this.preferredCol = curCol;
        this.pos.row = curRow;
    }

    moveWordRight(textData: TextData) {
        let curRow = this.pos.row;
        let curCol = this.pos.col;
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
        this.pos.col = this.preferredCol = curCol;
        this.pos.row = curRow;
    }
}
