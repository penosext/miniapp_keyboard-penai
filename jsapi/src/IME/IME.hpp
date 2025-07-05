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

#include "Database/Database.hpp"
#include <unordered_map>

typedef std::vector<std::string> Pinyin;

struct Candidate
{
    Pinyin pinyin;
    std::string hanZi;
    double freq;
};

class IME
{
private:
    DATABASE database;
    bool initialized = false;

    struct TrieNode
    {
        std::unordered_map<std::string, TrieNode *> children;
        std::unordered_map<std::string, double> candidates;
        ~TrieNode();
    };
    TrieNode *root = new TrieNode();
    std::unordered_map<std::string, bool> pinyinUnits;
    const size_t MAX_PINYIN_UNIT_LENGTH = 5;

    void insert(const Pinyin &pinyin, const std::string &hanZi, double freq);
    double getFreq(const Pinyin &pinyin, const std::string &hanZi);

public:
    IME();
    void initialize();
    std::vector<Candidate> getCandidates(const std::string &rawPinyin);
    void updateWordFrequency(const Pinyin &pinyin, const std::string &hanZi);
    Pinyin splitPinyin(const std::string &rawPinyin);
};
