# Depth of Market (Level II Ladder)

* requires dom.cfg file with a symbol and dynamic indicator settings:


```
symbol = <symbol name>
depth = <mm|order>
block_size=1
levels=10
period_width=7
ma1_periods=5
ma2_periods=8
ma3_periods=12
stochastic1_periods=15
stochastic2_periods=48
stochastic3_periods=125
```
* Depth: mm (market maker for stocks), or order (orders based for futures)
* Block Size: typically 100 for stocks, 1 for futures
* Levels:  number of levels in the Feature Vector Set
* Period Width: seconds

