// https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html

var LibPokiSdk = {

    $PokiSdk: {
        // must match PokiCommercialBreakResult
        COMMERCIAL_BREAK_SUCCESS: 1,
        COMMERCIAL_BREAK_START: 2,

        // must match PokiRewardedBreakResult
        REWARDED_BREAK_ERROR: 0,
        REWARDED_BREAK_SUCCESS: 1,
        REWARDED_BREAK_START: 2,

        _callback: null,
        _urlCallback: null,
        _urlParameters:null,
        _messagesHashTable: {},
        _userCallback: null,
        _userErrorCallback: null,
        _tokenCallback: null,
        _tokenErrorCallback: null,
        _loginCallback: null,
        _loginErrorCallback: null,

        _commercialBreakCallback: function() {
            var msg = PokiSdk.COMMERCIAL_BREAK_SUCCESS;
            {{{ makeDynCall("vi", "PokiSdk._callback")}}}(msg);
        },

        _commercialBreakStartedCallback: function() {
            var msg = PokiSdk.COMMERCIAL_BREAK_START;
            {{{ makeDynCall("vi", "PokiSdk._callback")}}}(msg);
        },

        _rewardedBreakCallback: function(success) {
            var msg = success ? PokiSdk.REWARDED_BREAK_SUCCESS : PokiSdk.REWARDED_BREAK_ERROR;
            {{{ makeDynCall("vi", "PokiSdk._callback")}}}(msg);
        },

        _rewardedBreakStartedCallback: function() {
            var msg = PokiSdk.REWARDED_BREAK_START;
            {{{ makeDynCall("vi", "PokiSdk._callback")}}}(msg);
        },

        _shareableURLCallback: function(url) {
            var _url = stringToNewUTF8(url);
            {{{ makeDynCall("vii", "PokiSdk._urlCallback")}}}(_url, lengthBytesUTF8(url));
            Module._free(_url);
        },

        _getUserCallback: function(user) {
            if (user == null) {
                {{{ makeDynCall("viiiii", "PokiSdk._userCallback")}}}(0, 0, 0, 0, 0);
                return;
            }

            var username = user.username != null ? user.username : "";
            var avatarUrl = user.avatarUrl != null ? user.avatarUrl : "";
            var _username = stringToNewUTF8(username);
            var _avatarUrl = stringToNewUTF8(avatarUrl);
            {{{ makeDynCall("viiiii", "PokiSdk._userCallback")}}}(1, _username, lengthBytesUTF8(username), _avatarUrl, lengthBytesUTF8(avatarUrl));
            Module._free(_username);
            Module._free(_avatarUrl);
        },

        _getUserErrorCallback: function(error) {
            var jsError = error && typeof error.message === "string" ? error.message : String(error);
            var _error = stringToNewUTF8(jsError);
            {{{ makeDynCall("vii", "PokiSdk._userErrorCallback")}}}(_error, lengthBytesUTF8(jsError));
            Module._free(_error);
        },

        _getTokenCallback: function(token) {
            if (token == null) {
                {{{ makeDynCall("viii", "PokiSdk._tokenCallback")}}}(0, 0, 0);
                return;
            }

            var _token = stringToNewUTF8(token);
            {{{ makeDynCall("viii", "PokiSdk._tokenCallback")}}}(1, _token, lengthBytesUTF8(token));
            Module._free(_token);
        },

        _getTokenErrorCallback: function(error) {
            var jsError = error && typeof error.message === "string" ? error.message : String(error);
            var _error = stringToNewUTF8(jsError);
            {{{ makeDynCall("vii", "PokiSdk._tokenErrorCallback")}}}(_error, lengthBytesUTF8(jsError));
            Module._free(_error);
        },

        _loginSuccessCallback: function() {
            {{{ makeDynCall("v", "PokiSdk._loginCallback")}}}();
        },

        _loginFailureCallback: function(error) {
            var jsError = error && typeof error.message === "string" ? error.message : String(error);
            var _error = stringToNewUTF8(jsError);
            {{{ makeDynCall("vii", "PokiSdk._loginErrorCallback")}}}(_error, lengthBytesUTF8(jsError));
            Module._free(_error);
        },
    },

    PokiSdkJs_CommercialBreak: function(callback) {
        PokiSdk._callback = callback;
        PokiSDK.commercialBreak(PokiSdk._commercialBreakStartedCallback).then(PokiSdk._commercialBreakCallback);
    },

    PokiSdkJs_RewardedBreak: function(size, callback) {
        PokiSdk._callback = callback;
        let options = {
            size: UTF8ToString(size),
            onStart: PokiSdk._rewardedBreakStartedCallback,
        }
        PokiSDK.rewardedBreak(options).then(PokiSdk._rewardedBreakCallback);
    },

    PokiSdkJs_GameplayStart: function() {
        PokiSDK.gameplayStart();
    },

    PokiSdkJs_GameplayStop: function() {
        PokiSDK.gameplayStop();
    },

    PokiSdkJs_SetDebug: function(value) {
        PokiSDK.setDebug(value); 
    },

    PokiSdkJs_CaptureError: function(error) {
        PokiSDK.captureError(UTF8ToString(error));
    },

    PokiSdkJs_InternalCaptureError: function(formatted_string) {
        var js_formatted_string = UTF8ToString(formatted_string);
        if (PokiSdk._messagesHashTable[js_formatted_string]) {
            return;
        }
        PokiSdk._messagesHashTable[js_formatted_string] = true;
        var final_message = "DEFOLD:" + js_formatted_string;
        PokiSDK.captureError(final_message);
    },

    PokiSdkJs_IsAdBlocked: function() {
        console.log("poki_sdk.is_ad_blocked() is deprecated.");
        return false;
    },

    PokiSdkJs_AddParameterForURL: function(key, value) {
        if (PokiSdk._urlParameters == null) {
            PokiSdk._urlParameters = {};
        }
        PokiSdk._urlParameters[UTF8ToString(key)] = UTF8ToString(value);
    },

    PokiSdkJs_ShareableURL: function(callback) {
        PokiSdk._urlCallback = callback;
        PokiSDK.shareableURL(PokiSdk._urlParameters).then(PokiSdk._shareableURLCallback);
        PokiSdk._urlParameters = null;
    },

    PokiSdkJs_GetURLParam: function(key) {
        var key = UTF8ToString(key);
        var value = PokiSDK.getURLParam(key);
        return stringToUTF8OnStack(value);
    },

    PokiSdkJs_Measure: function(category, what, action) {
        PokiSDK.measure(UTF8ToString(category), UTF8ToString(what), UTF8ToString(action));
    },

    PokiSdkJs_MovePill: function(topPercent, topPx) {
        PokiSDK.movePill(topPercent, topPx);
    },

    PokiSdkJs_GetUser: function(callback, error_callback) {
        PokiSdk._userCallback = callback;
        PokiSdk._userErrorCallback = error_callback;
        PokiSDK.getUser().then(PokiSdk._getUserCallback).catch(PokiSdk._getUserErrorCallback);
    },

    PokiSdkJs_GetToken: function(callback, error_callback) {
        PokiSdk._tokenCallback = callback;
        PokiSdk._tokenErrorCallback = error_callback;
        PokiSDK.getToken().then(PokiSdk._getTokenCallback).catch(PokiSdk._getTokenErrorCallback);
    },

    PokiSdkJs_Login: function(callback, error_callback) {
        PokiSdk._loginCallback = callback;
        PokiSdk._loginErrorCallback = error_callback;
        PokiSDK.login().then(PokiSdk._loginSuccessCallback).catch(PokiSdk._loginFailureCallback);
    },

    PokiSdkJs_OpenExternalLink: function(url) {
        PokiSDK.openExternalLink(UTF8ToString(url));
    }
}

autoAddDeps(LibPokiSdk, '$PokiSdk');
addToLibrary(LibPokiSdk);
