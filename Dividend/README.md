Dividend looks at iqfeed fundamental data to look for dividend 
based equities.

Example output:

```
exchange,symbol,yield,rate,amount,vol,exdiv,payed,trade,option
AFCG,NASDAQ,11.98,2.24,0.56,218000,2022-Jun-29,2022-Jul-15,18.85,AFCG
ALTY,NASDAQ,8.21,0.9961,0.0672,14000,2022-Aug-03,2022-Aug-11,12.13,ALTY
BKCH,NASDAQ,13.55,0.9685,0.0345,193000,2022-Jun-29,2022-Jul-08,7.3,BKCH
BKEPP,NASDAQ,8.19,0.715,0.1787,28000,2022-Aug-04,2022-Aug-12,8.73,
CCAP,NASDAQ,9.27,1.64,0.05,49000,2022-Sep-01,2022-Sep-15,17.76,CCAP
COMT,NASDAQ,14.11,5.4941,5.4941,466000,2021-Dec-13,2021-Dec-17,38.93,COMT
....
```

The configuration file:

```
$ cat x64/debug/dividend.cfg
listed_market=NYSE
listed_market=NYSE_ARCA
listed_market=NYSE_AMERICAN
listed_market=NASDAQ
listed_market=NCM
listed_market=NGM
listed_market=NGSM
ignore_name=LYPHF
security_type=EQUITY
security_state=XFLT,portfolio
minimum_yield=8.0
minimum_volume=5000
max_in_transit=40
```
Data is placed into a sqlite3 database.  Sample query for dividends paid:

```
sqlite> .headers on
sqlite> .mode column
sqlite> .param set :symbol "GDXY"
sqlite> select
  a.symbol_name, a.date_run, b.last_trade, b.yield, a.date_exdividend, a.amount_payed, a.date_payed
        from
  dividend as a
inner join
  daily as b
on
  a.symbol_name=b.symbol_name and a.date_run=b.date_run
where
  a.symbol_name = :symbol
order by
  a.date_run desc
;
symbol_name  date_run    last_trade  yield  date_exdividend  amount_payed  date_payed
-----------  ----------  ----------  -----  ---------------  ------------  ----------
GDXY         2025-08-15  15.15       50.36  2025-08-14       0.6098        2025-08-15
GDXY         2025-07-19  14.48       53.53  2025-07-17       0.3321        2025-07-18
GDXY         2025-06-24  15.02       49.82  2025-06-20       0.8449        2025-06-23
GDXY         2025-05-24  15.21       44.0   2025-05-22       0.3739        2025-05-23
GDXY         2025-04-26  15.47       39.41  2025-04-24       0.7284        2025-04-25
GDXY         2025-04-12  16.67       32.82  2025-03-27       0.6394        2025-03-28
```

Sample query for highest yielding instruments (trade at your own risk & assessment):

```
sqlite> .mode column
sqlite> .headers on
sqlite> select daily.symbol_name, daily.date_run, daily.last_trade, daily.yield, dividend.date_exdividend, dividend.amount_payed, dividend.date_payed
from daily
inner join dividend
on daily.symbol_name = dividend.symbol_name and daily.date_run = dividend.date_run
where daily.yield>50 and dividend.date_exdividend > "2025-07-01" and daily.date_run > "2025-07-01"
group by daily.symbol_name
order by daily.yield desc
;
symbol_name  date_run    last_trade  yield   date_exdividend  amount_payed  date_payed
-----------  ----------  ----------  ------  ---------------  ------------  ----------
FIAT         2025-07-26  2.94        363.91  2025-07-24       0.1381        2025-07-25
MRNY         2025-07-19  2.38        165.68  2025-07-17       0.2004        2025-07-18
CRSH         2025-07-12  4.51        164.46  2025-07-10       0.2156        2025-07-11
TSLY         2025-07-12  7.71        143.44  2025-07-10       0.3873        2025-07-11
MSTY         2025-07-05  20.89       143.25  2025-07-03       1.2382        2025-07-07
CONY         2025-07-26  8.99        134.32  2025-07-24       0.7951        2025-07-25
...
```

For additional research, use the ![OptionTrader](../OptionTrader) project to look at historical daily bars and current session 1 minute bars for historical context.


Something to consider for calculations:
* highest-yielding monthly dividend stocks with market capitalizations of at least $1 billion and payout ratios below 100%, meaning they are paying out less in dividends per share than they are bringing in in earnings per share (EPS)
* order by forward dividend yield, which is calculated by dividing the sum of a company’s projected dividend payouts over the next year by its current share price.

Source of ideas:
* [Best Monthly Dividend Stocks and ETFs](https://www.marketbeat.com/dividends/monthly-dividend-stocks/)
* [Stocks That Pay Monthly Dividends`](https://stockanalysis.com/list/monthly-dividend-stocks/)

Key Calculations:
* Market Capitalization = Outstanding Shares x Stock Price
* Earnings per Share = (Net Income - Preferred Dividends) ÷ Average Oustanding Common Shares
* Cash Flow per Share = (Operating Cash Flow – Preferred Dividends) ÷ Common Shares Outstanding

Concepts:
* Announcement date: The date a company declares a dividend.
* Ex-dividend date: The first trading day on which buyers of a stock no longer qualify for a previously declared dividend is called the ex-dividend date, or simply the ex-date. For instance, if a dividend has an ex-date of Tuesday, June 5, only the shareholders who purchased the stock prior to June 5 are eligible for the payment.
* Record date: The record date is the cut-off date set by the company to determine the roster of shareholders eligible to receive a dividend. Since it currently takes two days to settle a share purchase, the record date is typically the next business day after the ex-date (and two trading days after the last day on which share buyers qualify for a previously declared dividend).
* Payment date: The company pays the dividend on the payment date, also known as the payable date, which is when the money gets credited to investors' accounts.
* Short Stock: The borrower of the stock is responsible for paying any dividends to the lenders.
* https://www.investopedia.com/ask/answers/042215/if-investor-short-dividendpaying-stock-record-date-are-they-entitled-dividend.asp
