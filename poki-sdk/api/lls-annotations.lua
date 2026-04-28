---@meta poki_sdk
 
---Functions and constants for interacting with Poki SDK APIs
poki_sdk = {}
 
---@type number
poki_sdk.COMMERCIAL_BREAK_SUCCESS = nil
 
---@type number
poki_sdk.COMMERCIAL_BREAK_START = nil
 
---@type number
poki_sdk.REWARDED_BREAK_ERROR = nil
 
---@type number
poki_sdk.REWARDED_BREAK_SUCCESS = nil
 
---@type number
poki_sdk.REWARDED_BREAK_START = nil
 
---Signals that gameplay has started.
function poki_sdk.gameplay_start() end
 
---Signals that gameplay has stopped.
function poki_sdk.gameplay_stop() end
 
---Requests a commercial break (ad). The callback is called when the ad finishes.
---@param callback fun(self: userdata, status: number) Called with one of the statuses: `poki_sdk.COMMERCIAL_BREAK_START`, `poki_sdk.COMMERCIAL_BREAK_SUCCESS`
function poki_sdk.commercial_break(callback) end
 
---Requests a rewarded break (rewarded ad). The callback is called when the ad finishes.
---@param size string The size of the reward. Accepted values are `"small"`, `"medium"` and `"large"`. Optional, default is `"small"`.
---@param callback fun(self: userdata, status: number) Called with one of the statuses: `poki_sdk.REWARDED_BREAK_ERROR`, `poki_sdk.REWARDED_BREAK_START`, `poki_sdk.REWARDED_BREAK_SUCCESS`
function poki_sdk.rewarded_break(size, callback) end
 
---Enables or disables debug mode.
---@param is_debug boolean
function poki_sdk.set_debug(is_debug) end
 
---Captures and reports an error to Poki.
---@param error string
function poki_sdk.capture_error(error) end
 
---Generates a shareable URL with the given parameters.
---@param params table
---@param callback function
function poki_sdk.shareable_url(params, callback) end
 
---Returns the value of a URL query parameter by key.
---@param key string
---@return string value
function poki_sdk.get_url_param(key) end
 
---Sends a custom analytics event to Poki.
---@param category string
---@param what string
---@param action string
function poki_sdk.measure(category, what, action) end
 
---Moves the Poki pill (branding element) to a different position.
---@param topPercent number Position from the top in percent.
---@param topPx number Position from the top in pixels.
function poki_sdk.move_pill(topPercent, topPx) end
 
---Retrieves the currently logged-in Poki user.
---@param callback fun(self: userdata, user: {username: string, avatar_url: string}|nil, error: string|nil) `user` is a table with `username` and `avatar_url`, or `nil` if no user is logged in. `error` is the rejection message if user accounts are unavailable, or `nil` otherwise.
function poki_sdk.get_user(callback) end
 
---Retrieves the Poki auth token for the currently logged-in user.
---@param callback fun(self: userdata, token: string|nil, error: string|nil) `token` is the Poki auth token, or `nil` if no user is logged in. `error` is the rejection message if user accounts are unavailable, or `nil` otherwise.
function poki_sdk.get_token(callback) end
 
---Prompts the user to log in to their Poki account.
---@param callback fun(self: userdata, success: boolean, error: string|nil) `success` is `true` if the login promise resolved, otherwise `false`. `error` is the rejection message on failure, or `nil` on success.
function poki_sdk.login(callback) end
 
---Opens an external link in a new browser tab.
---@param url string
function poki_sdk.open_external_link(url) end
 
return poki_sdk