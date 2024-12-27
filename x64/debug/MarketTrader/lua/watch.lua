-- file:    test1.lua
-- author:  raymond@burkholder.net
-- created: 2024/12/22 16:28:31

description = 'watch a symbol and react'

print( "watch.lua" )

local watch = nil
local symbol = '@ES#'
--local symbol = 'SPY'

handle_trade = function( price, volume )
  print( symbol .. ': ' .. volume .. '@' .. price )
end

attach = function()
  watch = tie.new();
  watch:watch( symbol )
end

detach = function()
end
