# Poki SDK native extension for Defold game engine
![pokisdk-hero](https://user-images.githubusercontent.com/2209596/102117637-db1df480-3e3e-11eb-9822-db237f36c0f6.jpg)
## Installation

You can use the Poki SDK extension in your own project by adding this project as a [Defold library dependency](http://www.defold.com/manuals/libraries/).
Open your game.project file and in the dependencies field under project add:

>https://github.com/AGulev/defold-poki-sdk/archive/main.zip

or point to the ZIP file of a [specific release](https://github.com/AGulev/defold-poki-sdk/releases).

Lua API corresponds to original JS API SDK:

```lua
poki_sdk.gameplay_start() -- in JS it's PokiSDK.gameplayStart()
poki_sdk.gameplay_stop() -- in JS it's PokiSDK.gameplayStop()
poki_sdk.commercial_break(function(self)end) -- in JS it's PokiSDK.commercialBreak()
poki_sdk.rewarded_break(function(self, success)end) -- in JS it's PokiSDK.rewardedBreak()
poki_sdk.set_debug(value) -- in JS it's PokiSDK.setDebug(value)
poki_sdk.happy_time(value) -- in JS it's PokiSDK.happyTime(value), where value is between 0 and 1
poki_sdk.shareable_url(params, callback) -- in JS it's PokiSDK.shareableURL({}).then(url => {})
local value = poki_sdk.get_url_param(key) -- in JS it's PokiSDK.getURLParam('id')

-- Also, it's possible to check if AdBlock is active.
-- The value updates ones when game init.
poki_sdk.is_adblock_active()
```
It's possible to use the [original HTML5 integration manual](https://sdk.poki.com/html5/), but this Defold SDK implements the following points from the original manual, so you can ignore them:

>~~1. Include our Javascript SDK~~

>~~2. Initialize the SDK~~

>~~3. Implement gameLoadingStart and gameLoadingFinished~~

>~~7. Prevent page jump~~

Make sure you pause sounds and music when show ADS.

When you get your PokiSDK Sitelock code, just add it to your game as:
```lua
html5.run("Sitelock JS code from Poki")
```
For security reasons, this piece of code is not public, so please request it from your Poki contact.

---

If you have any issues, questions or suggestions please [create an issue](https://github.com/AGulev/defold-poki-sdk/issues) or contact me: me@agulev.com
