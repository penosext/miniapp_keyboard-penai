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
        if (this.pos.row > 0) {
            this.pos.row--;
            const prevLine = textData[this.pos.row];
            this.pos.col = Math.min(this.preferredCol, prevLine.length);
        }
    }

    moveDown(textData: TextData) {
        if (this.pos.row < textData.length - 1) {
            this.pos.row++;
            const nextLine = textData[this.pos.row];
            this.pos.col = Math.min(this.preferredCol, nextLine.length);
        }
    }

    moveToHome(controlPressed: boolean) {
        if (controlPressed) {
            this.pos.row = 0;
            this.pos.col = this.preferredCol = 0;
        } else {
            this.pos.col = this.preferredCol = 0;
        }
    }

    moveToEnd(textData: TextData, controlPressed: boolean) {
        if (controlPressed) {
            this.pos.row = textData.length - 1;
            this.pos.col = this.preferredCol = textData[this.pos.row].length;
        } else {
            this.pos.col = this.preferredCol = textData[this.pos.row].length;
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
