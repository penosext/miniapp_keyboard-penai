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
#include <unordered_set>
#include <vector>
#include <string>

typedef std::vector<std::string> Pinyin;

struct Candidate
{
    Pinyin pinyin;
    std::string hanZi;
    double freq;
};

// 更高效的词典条目结构
struct DictEntry
{
    std::string hanZi;
    double freq;
};

class IME
{
private:
    DATABASE database;

    std::unordered_map<std::string, std::vector<DictEntry>> pinyinDict;
    std::unordered_set<std::string> pinyinUnits;
    const size_t MAX_PINYIN_UNIT_LENGTH = 5;

    void insert(const Pinyin &pinyin, const std::string &hanZi, double freq);
    double getFreq(const Pinyin &pinyin, const std::string &hanZi);

public:
    bool initialized = false;

    IME();
    void initialize();
    std::vector<Candidate> getCandidates(const std::string &rawPinyin);
    void updateWordFrequency(const Pinyin &pinyin, const std::string &hanZi);
    Pinyin splitPinyin(const std::string &rawPinyin);
};
