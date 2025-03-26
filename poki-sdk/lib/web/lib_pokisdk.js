// https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html

var LibPokiSdk = {

    $PokiSdk: {

        // must match PokiRewardedBreakResult
        REWARDED_BREAK_ERROR: 0,
        REWARDED_BREAK_SUCCESS: 1,
        REWARDED_BREAK_START: 2,

        _callback: null,
        _urlCallback: null,
        _urlParameters:null,
        _messagesHashTable: {},

        _commercialBreakCallback: function() {
            {{{ makeDynCall("v", "PokiSdk._callback")}}}();
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
        }
    },

    PokiSdkJs_CommercialBreak: function(callback) {
        PokiSdk._callback = callback;
        PokiSDK.commercialBreak().then(PokiSdk._commercialBreakCallback);
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
    }
}

autoAddDeps(LibPokiSdk, '$PokiSdk');
addToLibrary(LibPokiSdk);
