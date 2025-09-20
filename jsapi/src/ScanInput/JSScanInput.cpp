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

#include "JSScanInput.hpp"
#include <nlohmann/json.hpp>
#include <unistd.h>

JSSCAN_INPUT::JSSCAN_INPUT() : ScanInputObject(std::make_unique<ScanInput>()) {}
JSSCAN_INPUT::~JSSCAN_INPUT() {}

void JSSCAN_INPUT::initialize(JQAsyncInfo &info)
{
    try
    {
        ASSERT(info.Length() == 0);
        ScanInputCallback callback = [this](const std::string &data)
        {
            publish("scan_input", data);
        };
        ScanInputObject->initialize(callback);
        info.post({});
    }
    catch (const std::exception &e)
    {
        info.postError(e.what());
    }
}

void JSSCAN_INPUT::deinitialize(JQAsyncInfo &info)
{
    try
    {
        ASSERT(info.Length() == 0);
        ScanInputObject->deinitialize();
        info.post({});
    }
    catch (const std::exception &e)
    {
        info.postError(e.what());
    }
}

JSValue createScanInput(JQModuleEnv *env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "ScanInput");
    tpl->InstanceTemplate()->setObjectCreator([]()
                                              { return new JSSCAN_INPUT(); });

    tpl->SetProtoMethodPromise("initialize", &JSSCAN_INPUT::initialize);
    tpl->SetProtoMethodPromise("deinitialize", &JSSCAN_INPUT::deinitialize);

    JSSCAN_INPUT::InitTpl(tpl);
    return tpl->CallConstructor();
}
