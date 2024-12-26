-- file:    test1.lua
-- author:  raymond@burkholder.net
-- created: 2024/12/22 16:28:31

description = 'watch a symbol and react'

print( "watch.lua" )

watch = tie.new();
watch:watch( "SPY" )

attach = function()
end

detach = function()
end
