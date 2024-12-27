-- file:    watch.lua
-- author:  raymond@burkholder.net
-- created: 2024/12/22 16:28:31

description = 'watch a symbol and react'

--local symbol = 'SPY'
local symbol = '@ES#'
local instrument = nil

handle_trade = function( price, volume ) -- called from c++
  print( symbol .. ': ' .. volume .. '@' .. price )
end

attach = function()
  instrument = tie.new();
  instrument:watch( symbol )
  --telegram_message( 'watching ' .. symbol )
end

detach = function()
  instrument = nil;
end
