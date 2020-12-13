#define EXTENSION_NAME pokisdk
#define LIB_NAME "PokiSdk"
#define MODULE_NAME "poki_sdk"

#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_HTML5)

typedef void (*CommercialBreakCallback)();
typedef void (*RewardedBreakCallback)(int success);

extern "C" {
    void PokiSdkJs_GameplayStart();
    void PokiSdkJs_GameplayStop();
    void PokiSdkJs_SetDebug(bool value);

    void PokiSdkJs_CommercialBreak(CommercialBreakCallback callback);
    void PokiSdkJs_RewardedBreak(RewardedBreakCallback callback);
    void PokiSdkJs_HappyTime(float value);

    int PokiSdkJs_IsAdBlock();
}

static dmScript::LuaCallbackInfo* pokiSdk_AdsCallback = 0x0;

static bool PokiSdk_luaL_checkbool(lua_State *L, int numArg)
{
    bool b = false;
    if (lua_isboolean(L, numArg))
    {
        b = lua_toboolean(L, numArg);
    }
    else
    {
        luaL_typerror(L, numArg, lua_typename(L, LUA_TBOOLEAN));
    }
    return b;
}

static void PokiSdk_InvokeCallback(int numOfArgs, int success)
{
    if (!dmScript::IsCallbackValid(pokiSdk_AdsCallback))
    {
        dmLogError("PokiSDK callback is invalid. Use callback function as an argument when show ADS.");
        return;
    }

    lua_State* L = dmScript::GetCallbackLuaContext(pokiSdk_AdsCallback);

    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(pokiSdk_AdsCallback))
    {
        return;
    }

    if (numOfArgs == 1)
    {
        lua_pushboolean(L, success);
    }
    numOfArgs += 1;

    int ret = dmScript::PCall(L, numOfArgs, 0);
    (void)ret;

    dmScript::TeardownCallback(pokiSdk_AdsCallback);

    if (pokiSdk_AdsCallback != 0x0)
    {
        dmScript::DestroyCallback(pokiSdk_AdsCallback);
        pokiSdk_AdsCallback = 0x0;
    }
}

static void PokiSdk_CommercialBreakCallback()
{
    PokiSdk_InvokeCallback(0, 0);
}

static void PokiSdk_RewardedBreakCallback(int success)
{
    PokiSdk_InvokeCallback(1, success);
}

static int PokiSdk_GameplayStart(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    PokiSdkJs_GameplayStart();
    return 0;
}

static int PokiSdk_GameplayStop(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    PokiSdkJs_GameplayStop();
    return 0;
}

static int PokiSdk_CommercialBreak(lua_State* L)
{
    int type = lua_type(L, 1);
    if (type != LUA_TFUNCTION) {
        dmLogError("PokiSDK callback is invalid. Use callback function as an argument when show ADS.");
        return 0;
    }
    DM_LUA_STACK_CHECK(L, 0);
    pokiSdk_AdsCallback = dmScript::CreateCallback(L, 1);
    PokiSdkJs_CommercialBreak((CommercialBreakCallback)PokiSdk_CommercialBreakCallback);
    return 0;
}

static int PokiSdk_RewardedBreak(lua_State* L)
{
    int type = lua_type(L, 1);
    if (type != LUA_TFUNCTION) {
        dmLogError("PokiSDK callback is invalid. Use callback function as an argument when show ADS.");
        return 0;
    }
    DM_LUA_STACK_CHECK(L, 0);
    pokiSdk_AdsCallback = dmScript::CreateCallback(L, 1);
    PokiSdkJs_RewardedBreak((RewardedBreakCallback)PokiSdk_RewardedBreakCallback);
    return 0;
}

static int PokiSdk_HappyTime(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    float value = luaL_checknumber(L, 1);
    PokiSdkJs_HappyTime(value);
    return 0;
}

static int PokiSdk_SetDebug(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    bool debugValue = PokiSdk_luaL_checkbool(L, 1);
    PokiSdkJs_SetDebug(debugValue);
    return 0;
}

static int PokiSdk_IsAdBlock(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);
    int isAdBlock = PokiSdkJs_IsAdBlock();
    lua_pushboolean(L, isAdBlock);
    return 1;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"gameplay_start", PokiSdk_GameplayStart},
    {"gameplay_stop", PokiSdk_GameplayStop},
    {"commercial_break", PokiSdk_CommercialBreak},
    {"rewarded_break", PokiSdk_RewardedBreak},
    {"happy_time", PokiSdk_HappyTime},
    {"set_debug", PokiSdk_SetDebug},
    {"is_adblock_active", PokiSdk_IsAdBlock},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

dmExtension::Result InitializePokiSdk(dmExtension::Params* params)
{
    LuaInit(params->m_L);
    dmLogInfo("Registered %s Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizePokiSdk(dmExtension::Params* params)
{
    dmLogInfo("Finalize %s Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

#else // unsupported platforms

dmExtension::Result InitializePokiSdk(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizePokiSdk(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

#endif

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, 0, 0, InitializePokiSdk, 0, 0, FinalizePokiSdk)
