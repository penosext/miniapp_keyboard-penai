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

import { PinYin } from "./Pinyin";

class TrieNode {
    children: Map<string, TrieNode> = new Map();
    candidates: Map<string, number> = new Map();
}

export type Candidate = {
    pinYin: PinYin;
    hanZi: string;
    freq: number;
}

export default class DictionaryTrie {
    root: TrieNode = new TrieNode();

    private getNode(pinYin: PinYin): TrieNode {
        let node = this.root;
        for (const unit of pinYin) {
            if (!node.children.has(unit)) {
                node.children.set(unit, new TrieNode());
            }
            node = node.children.get(unit)!;
        }
        return node;
    }

    insert(pinYin: PinYin, hanZi: string, freq: number) {
        this.getNode(pinYin).candidates.set(hanZi, freq);
    }

    getFreq(pinYin: PinYin, hanZi: string): number | undefined {
        return this.getNode(pinYin).candidates.get(hanZi);
    }

    getCandidates(pinYin: PinYin): Candidate[] {
        let node = this.root;
        for (const unit of pinYin) {
            if (!node.children.has(unit)) {
                return [];
            }
            node = node.children.get(unit)!;
        }
        const candidates: Candidate[] = [];
        for (const [hanZi, freq] of node.candidates) {
            candidates.push({ hanZi, freq, pinYin });
        }
        return candidates;
    }
}
