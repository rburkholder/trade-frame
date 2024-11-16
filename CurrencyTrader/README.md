```
$ cat CurrencyTrader.cfg
ib_instance   = 11
base_currency = usd
base_currency_top_up = 1000000

# currencies start at 17:15 eastern

              # pair    start    stop     timezone
currency_pair  = eur.usd,01:00:00,11:00:00,America/Edmonton,100000
currency_pair  = usd.chf,01:00:00,11:00:00,America/Edmonton,100000
currency_pair  = gbp.usd,01:00:00,11:00:00,America/Edmonton,100000
currency_pair  = usd.cad,18:15:00,16:45:00,America/New_York,100000
currency_pair  = usd.jpy,19:00:00,23:00:00,America/Edmonton,100000
currency_pair  = aud.usd,19:00:00,23:00:00,America/Edmonton,100000
currency_pair  = nzd.usd,19:00:00,23:00:00,America/Edmonton,100000

exchange      = IDEAL   # IDEAL (paper trading), IDEALPRO (live)

smoother_seconds = 180  #  3 minutes
smoother_seconds = 420  #  7 minutes
smoother_seconds = 840  # 14 minutes

max_life_time = 03:00:00
pip_profit    = 30
pip_stop_loss = 30

# 0700 to 2000 GMT are acceptable hours for day trading the EUR/USD
# ideally want to day trade the EUR/USD between 1300 and 1600 GMT to maximize efficiency
# use indicators like the average true range (ATR) that gauge volatility based on recent periods.
```

