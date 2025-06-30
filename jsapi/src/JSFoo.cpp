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

#include <JSFoo.hpp>

// 简单同步接口
void JSFoo::joinPath(JQFunctionInfo &info)
{
    JSContext *ctx = info.GetContext();

    std::string path;
    for (unsigned idx = 0; idx < info.Length(); idx++)
    {
        path += "/";
        path += JQString(ctx, info[idx]).getString();
    }

    info.GetReturnValue().Set(path);
}

// 网络异步接口
void JSFoo::requestHttp(JQAsyncInfo &info)
{
    static bool mockError = false;

    std::string url = info[0].string_value();
    LOGD("JSFoo::requestHttp url: %s", url.c_str());

    if (mockError)
        info.postError("got mockError(%d) from requestHttp", mockError);
    else
        info.post("<html><body>here is a http result</body></html>");
}

extern JSValue createFoo(JQModuleEnv *env)
{
    JQFunctionTemplateRef tpl = JQFunctionTemplate::New(env, "foo");
    tpl->InstanceTemplate()->setObjectCreator([]()
                                              { return new JSFoo(); });
    tpl->SetProtoMethod("joinPath", &JSFoo::joinPath);
    tpl->SetProtoMethodPromise("requestHttp", &JSFoo::requestHttp);
    return tpl->CallConstructor();
}
