// https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/Interacting-with-code.html

var LibPokiSdk = {

    $PokiSdk: {

        _callback: null,

        _commercialBreakCallback: function() {
            dynCall("v", PokiSdk._callback);
        },

        _rewardedBreakCallback: function(success) {
            var msg = success ? 1 : 0; 
            dynCall("vi", PokiSdk._callback, [msg]);
        }
    },

    PokiSdkJs_CommercialBreak: function(callback) {
        PokiSdk._callback = callback;
        PokiSDK.commercialBreak().then(PokiSdk._commercialBreakCallback);
    },

    PokiSdkJs_RewardedBreak: function(callback) {
        PokiSdk._callback = callback;
        PokiSDK.rewardedBreak().then(PokiSdk._rewardedBreakCallback);
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

    PokiSdkJs_HappyTime: function(value) {
        PokiSDK.happyTime(value);
    },

    PokiSdkJs_IsAdBlock: function() {
        return Module.PokiSDK_isAdBlock;
    },
}

autoAddDeps(LibPokiSdk, '$PokiSdk');
mergeInto(LibraryManager.library, LibPokiSdk);
