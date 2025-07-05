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

#include "IME.hpp"
#include <algorithm>
#include <sstream>

IME::IME()
{
    root = new TrieNode();
}

IME::TrieNode::~TrieNode()
{
    for (auto &kv : children)
        delete kv.second;
}

void IME::initialize(const std::string &dictText)
{
    size_t start = 0, end = 0, len = dictText.size();
    while (start < len)
    {
        end = dictText.find('\n', start);
        if (end == std::string::npos)
            end = len;
        const std::string &line = dictText.substr(start, end - start);
        start = end + 1;
        if (line.empty() || line[0] == '#')
            continue;
        size_t pos = 0, next = 0;
        next = line.find(' ', pos);
        if (next == std::string::npos)
            continue;
        std::string hanZi = line.substr(pos, next - pos);
        pos = next + 1;
        next = line.find(' ', pos);
        if (next == std::string::npos)
            continue;
        double freq = std::strtod(line.c_str() + pos, nullptr);
        pos = next + 1;
        next = line.find(' ', pos);
        int flag = 0;
        if (next == std::string::npos)
        {
            flag = std::strtol(line.c_str() + pos, nullptr, 10);
            pos = line.size();
        }
        else
        {
            flag = std::strtol(line.c_str() + pos, nullptr, 10);
            pos = next + 1;
        }
        if (flag != 0)
            continue;
        PinYin pinYin;
        while (pos < line.size())
        {
            while (pos < line.size() && line[pos] == ' ')
                ++pos;
            if (pos >= line.size())
                break;
            next = line.find(' ', pos);
            std::string unit = (next == std::string::npos) ? line.substr(pos) : line.substr(pos, next - pos);
            pinYin.push_back(unit);
            pinyinUnits[unit] = true;
            if (next == std::string::npos)
                break;
            pos = next + 1;
        }
        if (!pinYin.empty())
        {
            insert(pinYin, hanZi, freq);
        }
    }
}

void IME::insert(const PinYin &pinYin, const std::string &hanZi, double freq)
{
    TrieNode *node = root;
    for (const auto &unit : pinYin)
    {
        if (!node->children.count(unit))
            node->children[unit] = new TrieNode();
        node = node->children[unit];
    }
    node->candidates[hanZi] = freq;
}

double IME::getFreq(const PinYin &pinYin, const std::string &hanZi)
{
    TrieNode *node = root;
    for (const auto &unit : pinYin)
    {
        if (!node->children.count(unit))
            return 0;
        node = node->children[unit];
    }
    auto it = node->candidates.find(hanZi);
    return it != node->candidates.end() ? it->second : 0;
}

std::vector<Candidate> IME::getCandidates(const std::string &rawPinyin)
{
    PinYin pinYin = splitPinyin(rawPinyin);
    std::vector<Candidate> candidates;
    for (int endIndex = pinYin.size(); endIndex > 0; --endIndex)
    {
        TrieNode *node = root;
        bool found = true;
        for (int i = 0; i < endIndex; ++i)
        {
            if (!node->children.count(pinYin[i]))
            {
                found = false;
                break;
            }
            node = node->children[pinYin[i]];
        }
        if (found)
            for (const auto &kv : node->candidates)
                candidates.push_back({PinYin(pinYin.begin(), pinYin.begin() + endIndex), kv.first, kv.second});
    }
    std::sort(candidates.begin(), candidates.end(),
              [](const Candidate &a, const Candidate &b)
              {
                  if (b.pinYin.size() != a.pinYin.size())
                      return b.pinYin.size() < a.pinYin.size();
                  return b.freq < a.freq;
              });
    return candidates;
}

void IME::updateWordFrequency(const PinYin &pinYin, const std::string &hanZi)
{
    double freq = getFreq(pinYin, hanZi);
    double newFreq = freq ? freq + 100 : 500;
    insert(pinYin, hanZi, newFreq);
}

PinYin IME::splitPinyin(const std::string &rawPinyin)
{
    PinYin pinYin;
    size_t i = 0;
    while (i < rawPinyin.size())
    {
        bool matched = false;
        for (int len = std::min(MAX_PINYIN_UNIT_LENGTH, rawPinyin.size() - i); len >= 1; --len)
        {
            std::string segment = rawPinyin.substr(i, len);
            if (pinyinUnits.count(segment))
            {
                pinYin.push_back(segment);
                i += len;
                matched = true;
                break;
            }
        }
        if (!matched)
        {
            pinYin.push_back(std::string(1, rawPinyin[i]));
            ++i;
        }
    }
    return pinYin;
}
