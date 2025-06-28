import baseDictionary from './baseDictionary';
import DictionaryTrie, { Candidate } from './DictionaryTrie';
import { PinYin } from './Pinyin';

export default class IME {
    pinyinUnits: Set<string>;
    MAX_PINYIN_UNIT_LENGTH = 5;
    dictionary: DictionaryTrie;
    initialized = false;

    constructor() {
        this.pinyinUnits = new Set();
        this.dictionary = new DictionaryTrie();
    }

    private yieldControl(): Promise<void> {
        return new Promise(resolve => { setTimeout(resolve, 0); });
    }

    async initializeDictionary() {
        if (this.initialized) { return; }
        this.initialized = true;
        for (const pinyinUnit of baseDictionary.pinyinUnits) {
            this.pinyinUnits.add(pinyinUnit);
        }

        const BATCH_SIZE = 200;
        const data = baseDictionary.dictionaryData;

        for (let i = 0; i < data.length; i += BATCH_SIZE) {
            const batch = data.slice(i, i + BATCH_SIZE);
            for (const item of batch) {
                const { pinYin, hanZi, freq } = item;
                this.dictionary.insert(pinYin, hanZi, freq);
            }
            if (i + BATCH_SIZE < data.length) { await this.yieldControl(); }
        }
        const storageInfo = await $falcon.jsapi.storage.getStorageInfo({});
        const keys = storageInfo.keys;
        if (keys && keys.length > 0) {
            const STORAGE_BATCH_SIZE = 20;
            for (let i = 0; i < keys.length; i += STORAGE_BATCH_SIZE) {
                const batch = keys.slice(i, i + STORAGE_BATCH_SIZE);
                for (const key of batch) {
                    const [pinYinString, hanZi] = key.split('-');
                    if (pinYinString && hanZi) {
                        const freqResult = await $falcon.jsapi.storage.getStorage({ key });
                        if (freqResult?.data) {
                            const freq = parseFloat(freqResult.data);
                            if (!isNaN(freq)) {
                                const pinYin = pinYinString.split(',');
                                this.dictionary.insert(pinYin, hanZi, freq);
                            }
                        }
                    }
                }
                if (i + STORAGE_BATCH_SIZE < keys.length) { await this.yieldControl(); }
            }
        }
    }

    getCandidates(rawPinyin: string): Candidate[] {
        const pinYin = this.splitPinyin(rawPinyin.toLowerCase().trim());
        const candidates: Candidate[] = [];
        for (let endIndex = pinYin.length; endIndex > 0; endIndex--) {
            candidates.push(...this.dictionary.getCandidates(pinYin.slice(0, endIndex)));
        }
        candidates.sort((a, b) => {
            if (b.pinYin.length !== a.pinYin.length) {
                return b.pinYin.length - a.pinYin.length;
            }
            return b.freq - a.freq;
        });
        return candidates;
    }

    async updateWordFrequency(pinYin: PinYin, hanZi: string) {
        const key = `${pinYin.join()}-${hanZi}`;
        const freq = this.dictionary.getFreq(pinYin, hanZi);
        const newFreq: number = freq ? freq + 100 : 500;
        this.dictionary.insert(pinYin, hanZi, newFreq);
        await $falcon.jsapi.storage.setStorage({ key, data: newFreq.toString() });
    }

    splitPinyin(rawPinyin: string): PinYin {
        const pinYin: PinYin = [];
        let i = 0;
        while (i < rawPinyin.length) {
            let matched = false;
            for (let len = Math.min(this.MAX_PINYIN_UNIT_LENGTH, rawPinyin.length - i); len >= 1; len--) {
                const segment = rawPinyin.substring(i, i + len);
                if (this.pinyinUnits.has(segment)) {
                    pinYin.push(segment);
                    i += len;
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                pinYin.push(rawPinyin.charAt(i));
                i++;
            }
        }
        return pinYin;
    }
}
