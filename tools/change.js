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

import rawDict from './rawdict_utf16_65105_freq.js';

const pinyinUnits = new Set();
const dictionaryData = new Map();

rawDict.split('\n').forEach(line => {
    const parts = line.trim().split(' ');
    if (parts.length >= 4 && parts[2] == '0') {
        const hanZi = parts[0];
        const freq = parseFloat(parts[1]);
        const pinYin = parts.slice(3);
        pinYin.forEach(unit => pinyinUnits.add(unit));
        if (pinYin.length > 0 && !isNaN(freq)) {
            const key = `${pinYin.join()}-${hanZi}`;
            dictionaryData.set(key, { pinYin, hanZi, freq });
        }
    }
});

console.log(`export interface BaseDictionary {
    pinyinUnits: string[];
    dictionaryData: {
        pinYin: string[];
        hanZi: string;
        freq: number;
    }[];
}`);
console.log("const baseDictionary: BaseDictionary = " + JSON.stringify({
    pinyinUnits: Array.from(pinyinUnits),
    dictionaryData: Array.from(dictionaryData.values()),
}) + ";");
console.log("")
console.log("export default baseDictionary;")
