Project MarketTrader

* designed to monitor symbols and to send telegram messages upon detection a defined event
* uses lua scripts as a flexible mechanism for defining events

config file (x64/debug/MarketTrader/MarketTrader.cfg) requires two entries:

* telegram_token   = .... # supplied by BotFather
* telegram_chat_id = ....

lua scripts are maintained in x64/debug/MarketTrader/MarketTrader/lua

* both iqfeed and interactive brokers are required to be available for startup

Next Steps:

* generate trades based upon event detection
* incorporate indicators
* add machine learning capabilities

