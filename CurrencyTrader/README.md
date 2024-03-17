$ cat CurrencyTrader.cfg
ib_instance   = 11
symbol_name   = eur.usd
exchange      = IDEAL   # IDEAL (paper trading), IDEALPRO (live)
start_time    = 08:00:00
stop_time     = 20:00:00
max_life_time = 03:00:00
pip_profit    = 30
pip_stop_loss = 30
# 0700 to 2000 GMT are acceptable hours for day trading the EUR/USD
# ideally want to day trade the EUR/USD between 1300 and 1600 GMT to maximize efficiency
# use indicators like the average true range (ATR) that gauge volatility based on recent periods.
