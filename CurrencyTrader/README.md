```
$ cat CurrencyTrader.cfg
ib_instance   = 11

              # pair    start    stop
pair_setting  = eur.usd,01:00:00,11:00:00
pair_setting  = gbp.usd,01:00:00,11:00:00
pair_setting  = usd.cad,06:00:00,11:00:00
pair_setting  = usd.jpy,19:00:00,23:00:00
pair_setting  = aud.usd,19:00:00,23:00:00
pair_setting  = nzd.usd,19:00:00,23:00:00

exchange      = IDEAL   # IDEAL (paper trading), IDEALPRO (live)

max_life_time = 03:00:00
pip_profit    = 30
pip_stop_loss = 30

# 0700 to 2000 GMT are acceptable hours for day trading the EUR/USD
# ideally want to day trade the EUR/USD between 1300 and 1600 GMT to maximize efficiency
# use indicators like the average true range (ATR) that gauge volatility based on recent periods.
```
