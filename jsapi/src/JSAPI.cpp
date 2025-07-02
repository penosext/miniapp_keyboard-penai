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
