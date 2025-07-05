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

#pragma once
#include <string>
#include <vector>
#include <unordered_map>

typedef std::vector<std::string> PinYin;

struct Candidate
{
    PinYin pinYin;
    std::string hanZi;
    double freq;
};

class IME
{
private:
    struct TrieNode
    {
        std::unordered_map<std::string, TrieNode *> children;
        std::unordered_map<std::string, double> candidates;
        ~TrieNode();
    };
    TrieNode *root;
    std::unordered_map<std::string, bool> pinyinUnits;
    size_t MAX_PINYIN_UNIT_LENGTH = 5;
    void insert(const PinYin &pinYin, const std::string &hanZi, double freq);
    double getFreq(const PinYin &pinYin, const std::string &hanZi);

public:
    IME();
    void initialize(const std::string &dictText);
    std::vector<Candidate> getCandidates(const std::string &rawPinyin);
    void updateWordFrequency(const PinYin &pinYin, const std::string &hanZi);
    PinYin splitPinyin(const std::string &rawPinyin);
};
