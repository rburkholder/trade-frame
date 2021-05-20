## 2021/05/20

https://www.investopedia.com/trading/options-trading-volume-and-open-interest/
* a big increase in price accompanied by low trading volume does not necessarily signify strength. In fact, that combination may well indicate that a price reversal is coming soon. 
* Open interest indicates the total number of option contracts that are currently out there. These are contracts that have been traded but not yet liquidated by an offsetting trade or an exercise or assignment. 
* open interest is not updated during the trading day.
* Open interest also gives you key information regarding the liquidity of an option. If there is no open interest in an option, there is no secondary market for that option. When options have a significant open interest, it means there are a large number of buyers and sellers out there. An active secondary market increases the odds of getting option orders filled at good prices. 
* All other things being equal, the bigger the open interest, the easier it will be to trade that option at a reasonable spread between the bid and ask.

https://www.optionsplaybook.com/options-introduction/open-interest/
* As opposed to stocks, which have a fixed number of shares outstanding, there’s no minimum or maximum number of option contracts that can exist for any given underlying stock. There will simply be as many option contracts as trader demand dictates.
* whenever you enter an option order, it’s not good enough to simply say “buy” or “sell” as you would with a stock. You need to specify whether you are buying or selling “to open” or “to close” your position.
* open interest is a lagging number: it's not updated during the course of a trading day. Instead, it is officially posted by The OCC the morning after any given trading session, once the figures have been calculated. For the rest of the trading day the figure remains static.

## 2021/05/18

* Tracking Market Gamma - more details are following and using option trades https://faac6eed-793d-4bff-8146-33b8614b8705.filesusr.com/ugd/5fe006_08f114fd7a8c48629a080aed2cc5c321.pdf

## 2021/05/17

* additional second order indicatorsa like vanna and charm: [Options Definitions](https://www.rcmalternatives.com/2020/11/10-options-definitions-you-need-to-know/)

* Very nice white paper on an ES inventory book based upon options: [Implied Order Book](https://squeezemetrics.com/download/The_Implied_Order_Book.pdf)

* "Because publicly reported open interest (OI) only tells us the number of contracts in existence on any given day, we have to delve into transaction-level data to assess the direction (buy/sell) of every SPX option trade and to bin it according to how it ought to affect open interest, and then finally, to verify trade direction by tracking the subsequent actual change in OI. Every option contract must be tracked through time in this manner to maintain an accurate picture of dealers' option exposures (DDOI). "

* we need is a function that returns the Black-Scholes delta.With this handy function, we can not only compute the current delta of every existing SPX option, but we can also compute exactly how these deltas will change as the implied vol and index price change.

* all you have to do is plug in new values to S and V and you've found how much the option's delta changes with underlying moves and implied volatility changes. And if you know how much the delta changes with respect to these sensitivities, all you have to do is convert these deltas to shares (or dollars) and you already have a rudimentary "implied order book."

* gamma exposure (GEX).Remember, gamma is an option's sensitivity to changes in underlying price, so in the function above, we'll be modifying the S variable, and to bring the numbers into the problem domain of the S&P 500

* To find the GEX of the whole SPX option universe, we do what we did above for every option contract in dealer directional open interest (DDOI), and then simply add them all together. The result can be a positive number or a negative number. A positive number means that customers (funds and the like) sold enough options to cause dealers to hedge in a way that provides stabilizing liquidity to the market. A negative number means the opposite—that customers are long more options, causing dealers to be "short gamma," and thus to always be takingliquidity, which is destabilizing.

* proof of liquidity is in the volatility.If there's merit to the idea that dealers' gamma exposure can be measured and that SPX options comprise a large portion of index liquidity, it should be very clear: There should be some statistical relationship between GEX and S&P 500 volatility.

* When implied volatilities go up, gammas (and thus GEX) move toward zero. Since we already know that high implied volatility is associated with high realized volatility, this seems like it could point us to the reason that sometimes zero GEX leads to volatility and sometimes it doesn't. I

* In the same way that there is a provable, logical, and clearly causal relationship between dealer gamma exposure and S&P 500 liquidity, there is also a causal relationship between vanna exposure (VEX) and S&P 500 liquidity. Recall that VEX measures dealers' delta sensitivity to implied volatility. In the same way that option dealers must provide, or take, liquidity based on changes in underlying price, they must provide or take liquidity based on changes in implied volatility.

* implied volatility is a measurement of market liquidity. IVs rise when liquidity is inadequate and fall when liquidity is abundant. In the context of gamma exposure, this is interesting, because when an option is sold, it increases GEX and lowers IV (IVs go down when people sell options), and when IVs go down, GEX goes up even more! So the process of selling an option features a two-pronged effect to increase GEX (add liquidity). 

* post-2008, are customers buying OTM puts and selling OTM calls. This creates and sustains a momentum effect, whereby higher prices lead to higher GEX, and higher GEX improves top-of-book (nearby bid-ask) liquidity, and better liquidity means that dips always get bought. This works to keep volatility in check... until it doesn't. Eventually, the effect of vanna overwhelms the effect of gamma, and that's when things get crazy.

* One is customer short OTM calls, and the other is customer long OTM puts (the biggest SPX flows). 

* And this is why VEX is GEX's evil twin. GEX is capable of providing so much liquidity to the index that daily ranges tighten to an average of 0.20%. VEX, the other side of the same coin, is able to take so much liquidity from the index that average daily ranges rise to 6.00%! And by quantifying both of these dealer delta sensitivities in concert, we get the whole picture—the whole "implied order book"—that drives S&P 500 liquidity.

* Over the last 16 years, it's clear that the impact of gamma and vanna on index liquidity has been pretty dramatic—an artifact of how options have obliquely become the "order type" of choice, i.e., the preferred method of offering and taking liquidity in the S&P 500.

* we're interested in being able to know when and where the over leveraged masses are going to start taking so much liquidity that they take the market down, because crashes always look the same: A crash happens when the conditional demand for liquidity below the market is larger than people think it is, and larger than prices imply. In terms of the order book, it's when there are tons of stop-losses below the market. One spark, one catalyst, and it's an unstoppable, illiquid chain-reaction.

* In terms of gamma and vanna, crash risk is a function of how many investors have sold puts, plain and simple. Sold puts are, quite literally, a bunch of huge buy limit orders below the market, and then a bunch of liquidity-taking stop-losses further down. To understand why, think of gamma and vanna separately:

* It's never enough to know the current liquidity situation in the market. To make good decisions, we need to know about conditional liquidity. Will there be buyers or sellers if the S&P 500 falls 5%? How about 10%? What will happen to VEX liquidity if VIX goes up 20 points? What if it goes down 10? To know all of this, we need to draw a "map" of the order book, and by extension, a map of future liquidity and volatility. We want to know where, exactly, option-originated liquidity is scarce or abundant. 

* This means that, when things get really bad, long puts actually end up adding a bit of liquidity. This means that the effect of customers buying put protection tends to be short, sharp corrections—not crashes

## 2013/09/05

https://www.interactivebrokers.com/en/software/tws/usersguidebook/technicalanalytics/market_scanner_types.htm

### 30-day (V30) Implied Volatilities:

* Implied volatility is calculated using a 100-step binary tree for American style options, and a Black-Scholes model for European style options. Interest rates are calculated using the settlement prices from the days Eurodollar futures contracts, and dividends are based on historical payouts.

* The 30-day volatility is the at-market volatility estimated for a maturity thirty calendar days forward of the current trading day. It is based on option prices from two consecutive expiration months. The first expiration month is that which has at least eight calendar days to run. The implied volatility is estimated for the eight options on the four closest to market strikes in each expiry. The implied volatilities are fit to a parabola as a function of the strike price for each expiry. The at-the-market implied volatility for an expiry is then taken to be the value of the fit parabola at the expected future price for the expiry. A linear interpolation (or extrapolation, as required) of the 30-day variance based on the squares of the at-market volatilities is performed. V30 is then the square root of the estimated variance. If there is no first expiration month with less than sixty calendar days to run, we do not calculate a V30.

## 2012/12/29

### Black Scholes And Beyond
```
pg 16 Dividends:  ex-dividend date The date on which buyers of a stock are no longer elibigble to recieve the next dividend payment.
pg 23 Riskless Zero Coupon Bond interest rate calculation.  
  interest rate goes up, bond value goes down; 
  interest rates go down, bond value goes up
  http://iris.nyit.edu/appack/pdf/FINC201_5.pdf calculating values of zero coupon bond

pg 47 The only dates on which it will ever be 
       optimal to exercise an American option are those dates immediately prior to ex-dividend dates

pg 163 raises the question of how to forecast the volatility, forecast volatility direction?

pg 180 formula summaries

pg 224 has a lead into on how some option price forecasting.  Volatility can change based upon trending price patterns.  
  Elsewhere there is a statement that high volatility leads to low volatility, and low volatility leads to high volatility
  With transition probabilites, is it possible to forecast stock movements?

pg 227 flexible trees all changes in volatility to be modelled

pg 228 discusses probabilistic model of stock returns, where as discussions of BSM indicated returns are not part of the model
  * start with this idea:  next price is probability of up move plus 1-p probability of down move

pg 231 local volatility

pg 247 review forward interest rates, something isn't making sense

pg 265 Arrow-Debreu price of a node

pg 271 Arrow-Debreu prices and butterfly spreads

pg 403 direction forecasting?
```
