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

#include <jsmodules/JSCModuleExtension.h>
#include <jquick_config.h>
#include <JSFoo.hpp>
#include <JSFooWifi.hpp>
#include <JSAI.hpp>

using namespace JQUTIL_NS;

static std::vector<std::string> exportList = {
    "foo", "fooWifi", "ai"};
static int module_init(JSContext *ctx, JSModuleDef *m)
{
    auto env = JQUTIL_NS::JQModuleEnv::CreateModule(ctx, m, "langningchen");
    env->setModuleExport("foo", createFoo(env.get()));
    env->setModuleExport("fooWifi", createFooWifi(env.get()));
    env->setModuleExport("ai", createAI(env.get()));
    env->setModuleExportDone(JS_UNDEFINED, exportList);
    return 0;
}
DEF_MODULE_LOAD_FUNC_EXPORT(langningchen, module_init, exportList)

extern "C" JQUICK_EXPORT void custom_init_jsapis()
{
    registerCModuleLoader("langningchen", &langningchen_module_load);
}
