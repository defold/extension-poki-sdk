# Poki SDK Native extension for Defold engine

## Installation

You can use the DefVideoAds extension in your own project by adding this project as a [Defold library dependency](http://www.defold.com/manuals/libraries/).
Open your game.project file and in the dependencies field under project add:

>https://github.com/AGulev/defold-poki-sdk/archive/master.zip

or point to the ZIP file of a [specific release](https://github.com/AGulev/defold-poki-sdk/releases).

Lua API corresponds to original JS API SDK:

```lua
poki_sdk.gameplay_start() -- in JS it's PokiSDK.gameplayStart()
poki_sdk.gameplay_stop() -- in JS it's PokiSDK.gameplayStop()
poki_sdk.commercial_break(function(self)end) -- in JS it's PokiSDK.commercialBreak()
poki_sdk.rewarded_break(function(self, success)end) -- in JS it's PokiSDK.rewardedBreak()
poki_sdk.happy_time(value) -- in JS it's PokiSDK.happyTime(value), where value is between 0 and 1

-- Also, it's possible to check if AdBlock is active.
-- Updates ones when game init.
poki_sdk.is_adblock_active()
```


---

If you have any issues, questions or suggestions please [create an issue](https://github.com/AGulev/defold-poki-sdk/issues) or contact me: me@agulev.com
