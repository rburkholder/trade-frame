-- file:    watch.lua
-- author:  raymond@burkholder.net
-- created: 2024/12/22 16:28:31

description = 'watch a symbol and react'

-- commands:
--   start watch on symbol (create a new instance here)
--   stop watch

--   show triggers
--   kill trigger hi
--   kill trigger lo
--   set trigger hi
--   set trigger lo
--   current quote

-- current news items

--local symbol = 'SPY'
--local symbol = '@ES#'
local symbol = 'QGCZ25'
local instrument = nil

local hi_trigger = 4150
local hi_triggered = false

local lo_trigger = 4100
local lo_triggered = false

handle_trade = function( price, volume ) -- called from c++
  if hi < price then
    --telegram_message( 'symbol ' .. symbol .. 'hi(' .. hi .. ') @ ' .. price )
  end
  if min > price then
    --telegram_message( 'symbol ' .. symbol .. 'lo(' .. lo .. ') @ ' .. price )
  end
  -- print( symbol .. ': ' .. volume .. '@' .. price )
end

attach = function()
  instrument = tie.new();
  instrument:watch( symbol )
  telegram_message( symbol .. ' - watch start' )
end

detach = function()
  telegram_message( symbol .. ' - watch stop'  )
  instrument = nil;
end
