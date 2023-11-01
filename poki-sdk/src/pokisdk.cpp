#define EXTENSION_NAME pokisdk
#define LIB_NAME "PokiSdk"
#define MODULE_NAME "poki_sdk"

#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_HTML5)

enum PokiCallbackType
{
    TYPE_INTERSTITIAL,
    TYPE_REWARDED,
    TYPE_SHARABLE_URL
};

typedef void (*CommercialBreakCallback)();
typedef void (*RewardedBreakCallback)(int success);
typedef void (*ShareableURLCallback)(const char* url, int url_length);

extern "C" {
    void PokiSdkJs_GameplayStart();
    void PokiSdkJs_GameplayStop();
    void PokiSdkJs_SetDebug(bool value);
    void PokiSdkJs_CaptureError(const char* error);
    void PokiSdkJs_InternalCaptureError(const char* formatted_string);

    void PokiSdkJs_CommercialBreak(CommercialBreakCallback callback);
    void PokiSdkJs_RewardedBreak(RewardedBreakCallback callback);

    int PokiSdkJs_IsAdBlocked();

    void PokiSdkJs_AddParameterForURL(const char* key, const char* value);
    void PokiSdkJs_ShareableURL(ShareableURLCallback callback);
    const char* PokiSdkJs_GetURLParam(const char* key);
}

static dmScript::LuaCallbackInfo* pokiSdk_Callback = 0x0;

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

static void PokiSdk_InvokeCallback(PokiCallbackType callbackType, int intArg, const char* charArg)
{
    if (!dmScript::IsCallbackValid(pokiSdk_Callback))
    {
        dmLogError("PokiSDK callback is invalid. Use callback function as an argument when show ADS.");
        return;
    }

    lua_State* L = dmScript::GetCallbackLuaContext(pokiSdk_Callback);

    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(pokiSdk_Callback))
    {
        return;
    }

    int numOfArgs = 0;

    if (callbackType == TYPE_REWARDED)
    {
        lua_pushboolean(L, intArg);
        numOfArgs = 1;
    }
    else if (callbackType == TYPE_SHARABLE_URL)
    {
        lua_pushlstring(L, charArg, intArg);
        numOfArgs = 1;
    }
    numOfArgs += 1;

    int ret = dmScript::PCall(L, numOfArgs, 0);
    (void)ret;

    dmScript::TeardownCallback(pokiSdk_Callback);

    if (pokiSdk_Callback != 0x0)
    {
        dmScript::DestroyCallback(pokiSdk_Callback);
        pokiSdk_Callback = 0x0;
    }
}

static void PokiSdk_CommercialBreakCallback()
{
    PokiSdk_InvokeCallback(TYPE_INTERSTITIAL, 0, 0);
}

static void PokiSdk_RewardedBreakCallback(int success)
{
    PokiSdk_InvokeCallback(TYPE_REWARDED, success, 0);
}

static void PokiSdk_ShareableURLCallback(const char* url, int url_length)
{
    PokiSdk_InvokeCallback(TYPE_SHARABLE_URL, url_length, url);
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
        luaL_error(L, "PokiSDK callback is invalid. Use callback function as an argument when show ADS.");
        return 0;
    }
    DM_LUA_STACK_CHECK(L, 0);
    pokiSdk_Callback = dmScript::CreateCallback(L, 1);
    PokiSdkJs_CommercialBreak((CommercialBreakCallback)PokiSdk_CommercialBreakCallback);
    return 0;
}

static int PokiSdk_RewardedBreak(lua_State* L)
{
    int type = lua_type(L, 1);
    if (type != LUA_TFUNCTION) {
        luaL_error(L, "PokiSDK callback is invalid. Use callback function as an argument when show ADS.");
        return 0;
    }
    DM_LUA_STACK_CHECK(L, 0);
    pokiSdk_Callback = dmScript::CreateCallback(L, 1);
    PokiSdkJs_RewardedBreak((RewardedBreakCallback)PokiSdk_RewardedBreakCallback);
    return 0;
}

static int PokiSdk_SetDebug(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    bool debugValue = PokiSdk_luaL_checkbool(L, 1);
    PokiSdkJs_SetDebug(debugValue);
    return 0;
}

static int PokiSdk_CaptureError(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    const char* error = luaL_checkstring(L, 1);
    PokiSdkJs_CaptureError(error);
    return 0;
}

static int PokiSdk_IsAdBlocked(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);
    int isAdBlocked = PokiSdkJs_IsAdBlocked();
    lua_pushboolean(L, isAdBlocked);
    return 1;
}

static int PokiSdk_ShareableURL(lua_State* L)
{
    int type = lua_type(L, 1);
    if (lua_type(L, 1) != LUA_TTABLE)
    {
        luaL_error(L, "Invalid parameters table. Use table with key:value pairs.");
        return 0;
    }
    lua_pushvalue(L, 1);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0)
    {
        const char* key = luaL_checkstring(L, -2);
        const char* value = luaL_checkstring(L, -1);
        PokiSdkJs_AddParameterForURL(key, value);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    
    type = lua_type(L, 2);
    if (type != LUA_TFUNCTION)
    {
        luaL_error(L, "PokiSDK callback is invalid. The second argument should be a callback function.");
        return 0;
    }
    DM_LUA_STACK_CHECK(L, 0);
    pokiSdk_Callback = dmScript::CreateCallback(L, 2);
    PokiSdkJs_ShareableURL((ShareableURLCallback)PokiSdk_ShareableURLCallback);
    return 0;
}

static int PokiSdk_GetURLParam(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 1);
    const char* key = luaL_checkstring(L, 1);
    const char* value = PokiSdkJs_GetURLParam(key);
    if (value)
    {
        lua_pushstring(L, value);
    }
    else
    {
        lua_pushnil(L);
    }
    return 1;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"gameplay_start", PokiSdk_GameplayStart},
    {"gameplay_stop", PokiSdk_GameplayStop},
    {"commercial_break", PokiSdk_CommercialBreak},
    {"rewarded_break", PokiSdk_RewardedBreak},
    {"set_debug", PokiSdk_SetDebug},
    {"capture_error", PokiSdk_CaptureError},
    {"is_ad_blocked", PokiSdk_IsAdBlocked},
    {"shareable_url", PokiSdk_ShareableURL},
    {"get_url_param", PokiSdk_GetURLParam},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

static dmExtension::Result InitializePokiSdk(dmExtension::Params* params)
{
    LuaInit(params->m_L);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizePokiSdk(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

static void LogListener(LogSeverity severity, const char* domain, const char* formatted_string)
{
    switch (severity)
    {
        case LOG_SEVERITY_ERROR:
        case LOG_SEVERITY_FATAL:
            PokiSdkJs_InternalCaptureError(formatted_string);
        break;
        case LOG_SEVERITY_WARNING:
        case LOG_SEVERITY_DEBUG:
        case LOG_SEVERITY_USER_DEBUG:
        case LOG_SEVERITY_INFO:
        // do nothing
        break;
        default:
        // do nothing
        break;
    }
}

static dmExtension::Result AppInitializePokiSdk(dmExtension::AppParams *params)
{
    dmLogRegisterListener(&LogListener);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizePokiSdk(dmExtension::AppParams *params)
{
    dmLogUnregisterListener(&LogListener);
    return dmExtension::RESULT_OK;
}

#else // unsupported platforms

static dmExtension::Result InitializePokiSdk(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizePokiSdk(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppInitializePokiSdk(dmExtension::AppParams *params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizePokiSdk(dmExtension::AppParams *params)
{
    return dmExtension::RESULT_OK;
}

#endif

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializePokiSdk, AppFinalizePokiSdk, InitializePokiSdk, 0, 0, FinalizePokiSdk)
