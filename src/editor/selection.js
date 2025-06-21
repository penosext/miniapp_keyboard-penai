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

export default class Selection {
    constructor(cursor) {
        this.cursor = cursor;
        this.active = false;
        this.startRow = 0;
        this.startCol = 0;
        this.endRow = 0;
        this.endCol = 0;
    }

    start() {
        this.active = true;
        this.startRow = this.cursor.row;
        this.startCol = this.cursor.col;
        this.endRow = this.cursor.row;
        this.endCol = this.cursor.col;
    }

    update() {
        if (this.active) {
            this.endRow = this.cursor.row;
            this.endCol = this.cursor.col;
        }
    }

    clear() {
        this.active = false;
        this.startRow = this.startCol = this.endRow = this.endCol = 0;
    }

    getNormalizedRange() {
        if (!this.active) return null;
        let startRow = this.startRow;
        let startCol = this.startCol;
        let endRow = this.endRow;
        let endCol = this.endCol;
        if (startRow > endRow || (startRow === endRow && startCol > endCol)) {
            [startRow, endRow] = [endRow, startRow];
            [startCol, endCol] = [endCol, startCol];
        }
        return { startRow, startCol, endRow, endCol };
    }
}
