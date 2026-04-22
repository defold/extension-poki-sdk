#define EXTENSION_NAME pokisdk
#define LIB_NAME "PokiSdk"
#define MODULE_NAME "poki_sdk"

#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_HTML5)

static const dmhash_t MASTER_SOUND_GROUP = dmHashString64("master");

enum PokiCallbackSlot
{
    CALLBACK_SLOT_COMMERCIAL_BREAK = 0,
    CALLBACK_SLOT_REWARDED_BREAK,
    CALLBACK_SLOT_SHAREABLE_URL,
    CALLBACK_SLOT_GET_USER,
    CALLBACK_SLOT_GET_TOKEN,
    CALLBACK_SLOT_LOGIN,
    CALLBACK_SLOT_COUNT
};

// make sure these match with the constants in lib_pokisdk.js
enum PokiCommercialBreakResult
{
    COMMERCIAL_BREAK_SUCCESS = 1,
    COMMERCIAL_BREAK_START = 2,
};

// make sure these match with the constants in lib_pokisdk.js
enum PokiRewardedBreakResult
{
    REWARDED_BREAK_ERROR = 0,
    REWARDED_BREAK_SUCCESS = 1,
    REWARDED_BREAK_START = 2,
};

typedef void (*CommercialBreakCallback)(PokiCommercialBreakResult result);
typedef void (*RewardedBreakCallback)(PokiRewardedBreakResult result);
typedef void (*ShareableURLCallback)(const char* url, int url_length);
typedef void (*GetUserCallback)(int has_user, const char* username, int username_length, const char* avatar_url, int avatar_url_length);
typedef void (*GetTokenCallback)(int has_token, const char* token, int token_length);
typedef void (*LoginSuccessCallback)();
typedef void (*AsyncErrorCallback)(const char* error, int error_length);

extern "C" {
    void PokiSdkJs_GameplayStart();
    void PokiSdkJs_GameplayStop();
    void PokiSdkJs_SetDebug(bool value);
    void PokiSdkJs_CaptureError(const char* error);
    void PokiSdkJs_InternalCaptureError(const char* formatted_string);

    void PokiSdkJs_CommercialBreak(CommercialBreakCallback callback);
    void PokiSdkJs_RewardedBreak(const char* size, RewardedBreakCallback callback);

    int PokiSdkJs_IsAdBlocked();

    void PokiSdkJs_AddParameterForURL(const char* key, const char* value);
    void PokiSdkJs_ShareableURL(ShareableURLCallback callback);
    const char* PokiSdkJs_GetURLParam(const char* key);

    void PokiSdkJs_Measure(const char* category, const char* what, const char* action);

    void PokiSdkJs_MovePill(double topPercent, double topPx);

    void PokiSdkJs_GetUser(GetUserCallback callback, AsyncErrorCallback error_callback);
    void PokiSdkJs_GetToken(GetTokenCallback callback, AsyncErrorCallback error_callback);
    void PokiSdkJs_Login(LoginSuccessCallback callback, AsyncErrorCallback error_callback);
}

static dmScript::LuaCallbackInfo* pokiSdk_Callbacks[CALLBACK_SLOT_COUNT] = {0x0};

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

static const char* PokiSdk_GetCallbackName(PokiCallbackSlot slot)
{
    switch (slot)
    {
        case CALLBACK_SLOT_COMMERCIAL_BREAK:
            return "commercial_break";
        case CALLBACK_SLOT_REWARDED_BREAK:
            return "rewarded_break";
        case CALLBACK_SLOT_SHAREABLE_URL:
            return "shareable_url";
        case CALLBACK_SLOT_GET_USER:
            return "get_user";
        case CALLBACK_SLOT_GET_TOKEN:
            return "get_token";
        case CALLBACK_SLOT_LOGIN:
            return "login";
        case CALLBACK_SLOT_COUNT:
        default:
            return "unknown";
    }
}

static void PokiSdk_DestroyCallback(PokiCallbackSlot slot)
{
    if (pokiSdk_Callbacks[slot] != 0x0)
    {
        dmScript::DestroyCallback(pokiSdk_Callbacks[slot]);
        pokiSdk_Callbacks[slot] = 0x0;
    }
}

static void PokiSdk_SetCallback(lua_State* L, int index, PokiCallbackSlot slot, const char* invalid_message)
{
    if (lua_type(L, index) != LUA_TFUNCTION)
    {
        luaL_error(L, "%s", invalid_message);
        return;
    }

    if (pokiSdk_Callbacks[slot] != 0x0)
    {
        dmLogError("PokiSDK %s callback already exists. Replacing pending callback.", PokiSdk_GetCallbackName(slot));
        PokiSdk_DestroyCallback(slot);
    }

    pokiSdk_Callbacks[slot] = dmScript::CreateCallback(L, index);
}

static dmScript::LuaCallbackInfo* PokiSdk_GetValidCallback(PokiCallbackSlot slot)
{
    dmScript::LuaCallbackInfo* callback = pokiSdk_Callbacks[slot];
    if (!dmScript::IsCallbackValid(callback))
    {
        dmLogError("PokiSDK %s callback is invalid.", PokiSdk_GetCallbackName(slot));
        PokiSdk_DestroyCallback(slot);
        return 0x0;
    }
    return callback;
}

static void PokiSdk_CommercialBreakCallback(PokiCommercialBreakResult result)
{
    dmSound::SetGroupMute(MASTER_SOUND_GROUP, true);

    dmScript::LuaCallbackInfo* callback = PokiSdk_GetValidCallback(CALLBACK_SLOT_COMMERCIAL_BREAK);
    if (callback == 0x0)
    {
        return;
    }

    lua_State* L = dmScript::GetCallbackLuaContext(callback);
    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(callback))
    {
        return;
    }

    bool destroy_callback = (result != COMMERCIAL_BREAK_START);
    lua_pushnumber(L, result);

    if (destroy_callback)
    {
        dmSound::SetGroupMute(MASTER_SOUND_GROUP, false);
    }

    int ret = dmScript::PCall(L, 2, 0);
    (void)ret;

    dmScript::TeardownCallback(callback);
    if (destroy_callback)
    {
        PokiSdk_DestroyCallback(CALLBACK_SLOT_COMMERCIAL_BREAK);
    }
}

static void PokiSdk_RewardedBreakCallback(PokiRewardedBreakResult result)
{
    dmSound::SetGroupMute(MASTER_SOUND_GROUP, true);

    dmScript::LuaCallbackInfo* callback = PokiSdk_GetValidCallback(CALLBACK_SLOT_REWARDED_BREAK);
    if (callback == 0x0)
    {
        return;
    }

    lua_State* L = dmScript::GetCallbackLuaContext(callback);
    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(callback))
    {
        return;
    }

    bool destroy_callback = (result != REWARDED_BREAK_START);
    lua_pushnumber(L, result);

    if (destroy_callback)
    {
        dmSound::SetGroupMute(MASTER_SOUND_GROUP, false);
    }

    int ret = dmScript::PCall(L, 2, 0);
    (void)ret;

    dmScript::TeardownCallback(callback);
    if (destroy_callback)
    {
        PokiSdk_DestroyCallback(CALLBACK_SLOT_REWARDED_BREAK);
    }
}

static void PokiSdk_ShareableURLCallback(const char* url, int url_length)
{
    dmScript::LuaCallbackInfo* callback = PokiSdk_GetValidCallback(CALLBACK_SLOT_SHAREABLE_URL);
    if (callback == 0x0)
    {
        return;
    }

    lua_State* L = dmScript::GetCallbackLuaContext(callback);
    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(callback))
    {
        return;
    }

    lua_pushlstring(L, url, url_length);

    int ret = dmScript::PCall(L, 2, 0);
    (void)ret;

    dmScript::TeardownCallback(callback);
    PokiSdk_DestroyCallback(CALLBACK_SLOT_SHAREABLE_URL);
}

static void PokiSdk_PushUser(lua_State* L, const char* username, int username_length, const char* avatar_url, int avatar_url_length)
{
    lua_createtable(L, 0, 2);

    lua_pushlstring(L, username, username_length);
    lua_setfield(L, -2, "username");

    lua_pushlstring(L, avatar_url, avatar_url_length);
    lua_setfield(L, -2, "avatar_url");
}

static void PokiSdk_GetUserCallback(int has_user, const char* username, int username_length, const char* avatar_url, int avatar_url_length)
{
    dmScript::LuaCallbackInfo* callback = PokiSdk_GetValidCallback(CALLBACK_SLOT_GET_USER);
    if (callback == 0x0)
    {
        return;
    }

    lua_State* L = dmScript::GetCallbackLuaContext(callback);
    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(callback))
    {
        return;
    }

    if (has_user)
    {
        PokiSdk_PushUser(L, username, username_length, avatar_url, avatar_url_length);
    }
    else
    {
        lua_pushnil(L);
    }
    lua_pushnil(L);

    int ret = dmScript::PCall(L, 3, 0);
    (void)ret;

    dmScript::TeardownCallback(callback);
    PokiSdk_DestroyCallback(CALLBACK_SLOT_GET_USER);
}

static void PokiSdk_GetUserErrorCallback(const char* error, int error_length)
{
    dmScript::LuaCallbackInfo* callback = PokiSdk_GetValidCallback(CALLBACK_SLOT_GET_USER);
    if (callback == 0x0)
    {
        return;
    }

    lua_State* L = dmScript::GetCallbackLuaContext(callback);
    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(callback))
    {
        return;
    }

    lua_pushnil(L);
    lua_pushlstring(L, error, error_length);

    int ret = dmScript::PCall(L, 3, 0);
    (void)ret;

    dmScript::TeardownCallback(callback);
    PokiSdk_DestroyCallback(CALLBACK_SLOT_GET_USER);
}

static void PokiSdk_GetTokenCallback(int has_token, const char* token, int token_length)
{
    dmScript::LuaCallbackInfo* callback = PokiSdk_GetValidCallback(CALLBACK_SLOT_GET_TOKEN);
    if (callback == 0x0)
    {
        return;
    }

    lua_State* L = dmScript::GetCallbackLuaContext(callback);
    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(callback))
    {
        return;
    }

    if (has_token)
    {
        lua_pushlstring(L, token, token_length);
    }
    else
    {
        lua_pushnil(L);
    }
    lua_pushnil(L);

    int ret = dmScript::PCall(L, 3, 0);
    (void)ret;

    dmScript::TeardownCallback(callback);
    PokiSdk_DestroyCallback(CALLBACK_SLOT_GET_TOKEN);
}

static void PokiSdk_GetTokenErrorCallback(const char* error, int error_length)
{
    dmScript::LuaCallbackInfo* callback = PokiSdk_GetValidCallback(CALLBACK_SLOT_GET_TOKEN);
    if (callback == 0x0)
    {
        return;
    }

    lua_State* L = dmScript::GetCallbackLuaContext(callback);
    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(callback))
    {
        return;
    }

    lua_pushnil(L);
    lua_pushlstring(L, error, error_length);

    int ret = dmScript::PCall(L, 3, 0);
    (void)ret;

    dmScript::TeardownCallback(callback);
    PokiSdk_DestroyCallback(CALLBACK_SLOT_GET_TOKEN);
}

static void PokiSdk_LoginCallback()
{
    dmScript::LuaCallbackInfo* callback = PokiSdk_GetValidCallback(CALLBACK_SLOT_LOGIN);
    if (callback == 0x0)
    {
        return;
    }

    lua_State* L = dmScript::GetCallbackLuaContext(callback);
    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(callback))
    {
        return;
    }

    lua_pushboolean(L, true);
    lua_pushnil(L);

    int ret = dmScript::PCall(L, 3, 0);
    (void)ret;

    dmScript::TeardownCallback(callback);
    PokiSdk_DestroyCallback(CALLBACK_SLOT_LOGIN);
}

static void PokiSdk_LoginErrorCallback(const char* error, int error_length)
{
    dmScript::LuaCallbackInfo* callback = PokiSdk_GetValidCallback(CALLBACK_SLOT_LOGIN);
    if (callback == 0x0)
    {
        return;
    }

    lua_State* L = dmScript::GetCallbackLuaContext(callback);
    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(callback))
    {
        return;
    }

    lua_pushboolean(L, false);
    lua_pushlstring(L, error, error_length);

    int ret = dmScript::PCall(L, 3, 0);
    (void)ret;

    dmScript::TeardownCallback(callback);
    PokiSdk_DestroyCallback(CALLBACK_SLOT_LOGIN);
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
    DM_LUA_STACK_CHECK(L, 0);
    PokiSdk_SetCallback(L, 1, CALLBACK_SLOT_COMMERCIAL_BREAK, "PokiSDK CommercialBreak callback is invalid. Use callback function as an argument when showing ads.");
    PokiSdkJs_CommercialBreak((CommercialBreakCallback)PokiSdk_CommercialBreakCallback);
    return 0;
}

static void PokiSdk_SetRewardedBreakCallback(lua_State* L, int index)
{
    PokiSdk_SetCallback(L, index, CALLBACK_SLOT_REWARDED_BREAK, "PokiSDK RewardedBreak callback is invalid. Use callback function as an argument when showing ads.");
}

static int PokiSdk_RewardedBreak(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    const char* size;
    if (lua_type(L, 1) == LUA_TSTRING)
    {
        size = luaL_checkstring(L, 1);
        PokiSdk_SetRewardedBreakCallback(L, 2);
    }
    else
    {
        size = "small";
        PokiSdk_SetRewardedBreakCallback(L, 1);
    }

    PokiSdkJs_RewardedBreak(size, (RewardedBreakCallback)PokiSdk_RewardedBreakCallback);

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
    
    DM_LUA_STACK_CHECK(L, 0);
    PokiSdk_SetCallback(L, 2, CALLBACK_SLOT_SHAREABLE_URL, "PokiSDK callback is invalid. The second argument should be a callback function.");
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

static int PokiSdk_Measure(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    const char* category = luaL_checkstring(L, 1);
    // what is optional and defaults to an empty string
    const char* what = luaL_optstring(L, 2, "");
    // action is optional and defaults to an empty string
    const char* action = luaL_optstring(L, 3, "");
    PokiSdkJs_Measure(category, what, action);
    return 0;
}

static int PokiSdk_MovePill(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    lua_Number topPercent = luaL_checknumber(L, 1);
    lua_Number topPx = luaL_checknumber(L, 2);
    PokiSdkJs_MovePill(topPercent, topPx);
    return 0;
}

static int PokiSdk_GetUser(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    PokiSdk_SetCallback(L, 1, CALLBACK_SLOT_GET_USER, "PokiSDK get_user callback is invalid. The first argument should be a callback function.");
    PokiSdkJs_GetUser((GetUserCallback)PokiSdk_GetUserCallback, (AsyncErrorCallback)PokiSdk_GetUserErrorCallback);
    return 0;
}

static int PokiSdk_GetToken(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    PokiSdk_SetCallback(L, 1, CALLBACK_SLOT_GET_TOKEN, "PokiSDK get_token callback is invalid. The first argument should be a callback function.");
    PokiSdkJs_GetToken((GetTokenCallback)PokiSdk_GetTokenCallback, (AsyncErrorCallback)PokiSdk_GetTokenErrorCallback);
    return 0;
}

static int PokiSdk_Login(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    PokiSdk_SetCallback(L, 1, CALLBACK_SLOT_LOGIN, "PokiSDK login callback is invalid. The first argument should be a callback function.");
    PokiSdkJs_Login((LoginSuccessCallback)PokiSdk_LoginCallback, (AsyncErrorCallback)PokiSdk_LoginErrorCallback);
    return 0;
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
    {"measure", PokiSdk_Measure},
    {"move_pill", PokiSdk_MovePill},
    {"get_user", PokiSdk_GetUser},
    {"get_token", PokiSdk_GetToken},
    {"login", PokiSdk_Login},
    {0, 0}
};

#define SETCONSTANT(name, value) \
    lua_pushnumber(L, (lua_Number) (value)); \
    lua_setfield(L, -2, #name);\

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    luaL_register(L, MODULE_NAME, Module_methods);

    SETCONSTANT(COMMERCIAL_BREAK_SUCCESS, COMMERCIAL_BREAK_SUCCESS);
    SETCONSTANT(COMMERCIAL_BREAK_START, COMMERCIAL_BREAK_START);

    SETCONSTANT(REWARDED_BREAK_ERROR, REWARDED_BREAK_ERROR);
    SETCONSTANT(REWARDED_BREAK_SUCCESS, REWARDED_BREAK_SUCCESS);
    SETCONSTANT(REWARDED_BREAK_START, REWARDED_BREAK_START);

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
