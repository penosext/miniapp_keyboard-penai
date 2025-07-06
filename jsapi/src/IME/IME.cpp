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
#include "strUtils.hpp"
#include <algorithm>
#include <sstream>
#include "rawdict_data.hpp"

IME::IME() : database("/userdisk/database/ime.db")
{
    database.table("ime_dict")
        .column("pinyin", TABLE::TEXT, TABLE::NOT_NULL)
        .column("hanZi", TABLE::TEXT, TABLE::NOT_NULL | TABLE::UNIQUE)
        .column("freq", TABLE::REAL, TABLE::NOT_NULL)
        .execute();

    pinyinDict.reserve(100000);
    pinyinUnits.reserve(500);
}

void IME::insert(const Pinyin &pinyin, const std::string &hanZi, double freq)
{
    std::string pinyinStr = strUtils::join(pinyin, " ");
    auto &entries = pinyinDict[pinyinStr];
    auto it = std::find_if(entries.begin(), entries.end(),
                           [&hanZi](const DictEntry &entry)
                           { return entry.hanZi == hanZi; });
    if (it != entries.end())
        it->freq = freq;
    else
        entries.push_back({hanZi, freq, pinyin.size()});
    std::sort(entries.begin(), entries.end(),
              [](const DictEntry &a, const DictEntry &b)
              { return a.freq > b.freq; });
}
double IME::getFreq(const Pinyin &pinyin, const std::string &hanZi)
{
    std::string pinyinStr = strUtils::join(pinyin, " ");
    auto it = pinyinDict.find(pinyinStr);
    if (it == pinyinDict.end())
        return 0;

    for (const auto &entry : it->second)
    {
        if (entry.hanZi == hanZi)
            return entry.freq;
    }
    return 0;
}

void IME::initialize()
{
    if (initialized)
        return;

    std::istringstream iss(RAWDICT_DATA);
    std::string line;
    size_t lineCount = 0;
    while (std::getline(iss, line))
    {
        if (!line.empty() && line[0] != '#')
            lineCount++;
    }

    iss.clear();
    iss.seekg(0);
    pinyinDict.reserve(lineCount);

    while (std::getline(iss, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
        auto parts = strUtils::split(line, " ");
        if (parts.size() < 4)
            continue;
        std::string hanZi = parts[0];
        double freq = std::strtod(parts[1].c_str(), nullptr);
        int flag = std::strtol(parts[2].c_str(), nullptr, 10);
        if (flag != 0)
            continue;
        Pinyin pinyin(parts.begin() + 3, parts.end());
        for (const auto &unit : pinyin)
            pinyinUnits.insert(unit);
        insert(pinyin, hanZi, freq);
    }

    auto cb = [this](std::vector<std::unordered_map<std::string, std::string>> rows)
    {
        for (auto &row : rows)
            if (row.count("pinyin") && row.count("hanZi") && row.count("freq"))
            {
                Pinyin pinyin = strUtils::split(row["pinyin"], " ");
                for (const auto &pinyinUnit : pinyin)
                    pinyinUnits.insert(pinyinUnit);
                std::string hanZi = row["hanZi"];
                double freq = std::stod(row["freq"]);
                insert(pinyin, hanZi, freq);
            }
    };
    database.select("ime_dict").select("pinyin").select("hanZi").select("freq").execute(cb);

    initialized = true;
}
std::vector<Candidate> IME::getCandidates(const std::string &rawPinyin)
{
    Pinyin pinyin = splitPinyin(rawPinyin);
    std::vector<Candidate> candidates;
    for (int endIndex = pinyin.size(); endIndex > 0; --endIndex)
    {
        Pinyin currentPinyin(pinyin.begin(), pinyin.begin() + endIndex);
        std::string pinyinStr = strUtils::join(currentPinyin, " ");

        auto dictIt = pinyinDict.find(pinyinStr);
        if (dictIt != pinyinDict.end())
            for (const auto &entry : dictIt->second)
                candidates.push_back({currentPinyin, entry.hanZi, entry.freq});
    }
    std::sort(candidates.begin(), candidates.end(),
              [](const Candidate &a, const Candidate &b)
              {
                  if (b.pinyin.size() != a.pinyin.size())
                      return b.pinyin.size() < a.pinyin.size();
                  return b.freq < a.freq;
              });
    return candidates;
}
void IME::updateWordFrequency(const Pinyin &pinyin, const std::string &hanZi)
{
    double freq = getFreq(pinyin, hanZi);
    double newFreq = freq ? freq + 100 : 500;
    insert(pinyin, hanZi, newFreq);

    std::string pinyinStr = strUtils::join(pinyin, " ");
    auto cb = [this, &pinyinStr, &hanZi, &newFreq](auto data)
    {
        if (data.empty())
        {
            database.insert("ime_dict")
                .insert("pinyin", pinyinStr)
                .insert("hanZi", hanZi)
                .insert("freq", newFreq)
                .execute();
        }
        else
        {
            database.update("ime_dict")
                .set("freq", std::to_string(newFreq))
                .where("pinyin", pinyinStr)
                .where("hanZi", hanZi)
                .execute();
        }
    };
    database.select("ime_dict").where("pinyin", pinyinStr).where("hanZi", hanZi).execute(cb);
}
Pinyin IME::splitPinyin(const std::string &rawPinyin)
{
    Pinyin pinyin;
    size_t i = 0;
    while (i < rawPinyin.size())
    {
        bool matched = false;
        for (int len = std::min(MAX_PINYIN_UNIT_LENGTH, rawPinyin.size() - i); len >= 1; --len)
        {
            std::string segment = rawPinyin.substr(i, len);
            if (pinyinUnits.count(segment))
            {
                pinyin.push_back(segment);
                i += len;
                matched = true;
                break;
            }
        }
        if (!matched)
        {
            pinyin.push_back(std::string(1, rawPinyin[i]));
            ++i;
        }
    }
    return pinyin;
}
