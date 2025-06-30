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

#include <JSFooWifi.hpp>

// wifi 扫描&通知机制
void JSFooWifi::scanWifi(JQAsyncInfo &info)
{
    // 模拟通知 JS 空间扫描结果
    Bson::array result;
    result.push_back("ssid0");
    result.push_back("ssid1");
    result.push_back("ssid2");
    publish("scan_result", result);
    // 异步接口必须回调
    info.post(0);
}

extern JSValue createFooWifi(JQModuleEnv *env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "fooWifi");
    tpl->InstanceTemplate()->setObjectCreator([]()
                                              { return new JSFooWifi(); });
    tpl->SetProtoMethodPromise("scanWifi", &JSFooWifi::scanWifi);
    JSFooWifi::InitTpl(tpl);
    return tpl->CallConstructor();
}
