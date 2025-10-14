-- file:    watch.lua
-- author:  raymond@burkholder.net
-- created: 2024/12/22 16:28:31

description = 'watch a symbol and react'

--local symbol = 'SPY'
--local symbol = '@ES#'
local symbol = 'QGCZ25'
local instrument = nil

handle_trade = function( price, volume ) -- called from c++
  local hi = 4189
  local lo = 4187
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
  telegram_message( 'watching ' .. symbol )
end

detach = function()
  telegram_message( 'un-watch ' .. symbol )
  instrument = nil;
end
