import baseDictionary from './baseDictionary';
import DictionaryTrie, { Candidate } from './DictionaryTrie';
import { PinYin } from './Pinyin';

declare const $falcon: {
    jsapi: {
        storage: {
            setStorage(params: { key: string; data: string }): Promise<any>;
            getStorage(params: { key: string }): Promise<{ data: string }>;
            getStorageInfo(params: {}): Promise<{ keys: string[]; currentSize: number; limitSize: number; }>;
        };
    };
};

export default class IME {
    pinyinUnits: Set<string>;
    MAX_PINYIN_UNIT_LENGTH = 5;
    dictionary: DictionaryTrie;

    constructor() {
        this.pinyinUnits = new Set();
        this.dictionary = new DictionaryTrie();
        this.initializeDictionary();
    }

    private async initializeDictionary() {
        for (const pinyinUnit of baseDictionary.pinyinUnits) {
            this.pinyinUnits.add(pinyinUnit);
        }
        for (const item of baseDictionary.dictionaryData) {
            const { pinYin, hanZi, freq } = item;
            this.dictionary.insert(pinYin, hanZi, freq);
        }
        const storageInfo = await $falcon.jsapi.storage.getStorageInfo({});
        const keys = storageInfo.keys;
        if (keys && keys.length > 0) {
            for (const key of keys) {
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
