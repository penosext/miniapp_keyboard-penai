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

#include "JSIME.hpp"
#include <nlohmann/json.hpp>

JSIME::JSIME() : IMEObject(nullptr) {}
JSIME::~JSIME() {}

void JSIME::initialize(JQAsyncInfo &info)
{
    try
    {
        IMEObject = std::make_unique<IME>();
        IMEObject->initialize();
        info.post({});
    }
    catch (const std::exception &e)
    {
        info.postError("IME initialization failed: %s", e.what());
    }
}

void JSIME::getCandidates(JQFunctionInfo &info)
{
    try
    {
        JSContext *ctx = info.GetContext();
        if (info.Length() != 1)
        {
            info.GetReturnValue().Set(false);
            return;
        }
        std::string rawPinyin = JQString(ctx, info[0]).getString();

        auto candidates = IMEObject->getCandidates(rawPinyin);
        Bson::array arr;
        for (const auto &c : candidates)
        {
            Bson::object candidateObj = {
                {"hanZi", c.hanZi},
                {"freq", c.freq}};
            Bson::array pinyin;
            for (const auto &py : c.pinyin)
                pinyin.push_back(py);
            candidateObj["pinyin"] = pinyin;
            arr.push_back(candidateObj);
        }
        info.GetReturnValue().Set(arr);
    }
    catch (const std::exception &e)
    {
        info.GetReturnValue().ThrowInternalError("IME getCandidates failed: %s", e.what());
    }
}

void JSIME::updateWordFrequency(JQFunctionInfo &info)
{
    try
    {
        JSContext *ctx = info.GetContext();
        if (info.Length() != 2)
        {
            info.GetReturnValue().Set(false);
            return;
        }
        std::vector<std::string> pinyin;
        JQArray(ctx, info[0]).toStringVector(pinyin);
        std::string hanZi = JQString(ctx, info[1]).getString();

        IMEObject->updateWordFrequency(pinyin, hanZi);
        info.GetReturnValue().Set(true);
    }
    catch (const std::exception &e)
    {
        info.GetReturnValue().ThrowInternalError("IME updateWordFrequency failed: %s", e.what());
    }
}

void JSIME::splitPinyin(JQFunctionInfo &info)
{
    try
    {
        JSContext *ctx = info.GetContext();
        if (info.Length() != 1)
        {
            info.GetReturnValue().Set(Bson::array());
            return;
        }
        std::string rawPinyin = JQString(ctx, info[0]).getString();

        auto result = IMEObject->splitPinyin(rawPinyin);
        Bson::array arr;
        for (const auto &pinyin : result)
            arr.push_back(pinyin);
        info.GetReturnValue().Set(arr);
    }
    catch (const std::exception &e)
    {
        info.GetReturnValue().ThrowInternalError("IME splitPinyin failed: %s", e.what());
    }
}

JSValue createIME(JQModuleEnv *env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "IME");
    tpl->InstanceTemplate()->setObjectCreator([]()
                                              { return new JSIME(); });

    tpl->SetProtoMethod("getCandidates", &JSIME::getCandidates);
    tpl->SetProtoMethod("updateWordFrequency", &JSIME::updateWordFrequency);
    tpl->SetProtoMethod("splitPinyin", &JSIME::splitPinyin);

    tpl->SetProtoMethodPromise("initialize", &JSIME::initialize);

    JSIME::InitTpl(tpl);
    return tpl->CallConstructor();
}
