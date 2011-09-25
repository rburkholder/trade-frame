using System;
using System.Collections.Generic;
using System.Text;

//using QDCustom;
//using OneUnified.GeneticProgramming;
//using OneUnified.SmartQuant;

//using SmartQuant.FIX;
using SmartQuant.Data;
//using SmartQuant.Series;
using SmartQuant.Trading;
//using SmartQuant.Charting;
//using SmartQuant.Execution;
//using SmartQuant.Providers;
//using SmartQuant.Indicators;
//using SmartQuant.Instruments;
//using SmartQuant.Optimization;

namespace OneUnified.GeneticProgramming {

  public class GPStrategy {

    /*
    public delegate void NewDataEventHandler( object source, GPStrategy gpis );

    // GP Optimizer assigns itself to these events
    public static NewDataEventHandler SignalInit;
    public static NewDataEventHandler SignalBar;
    public static NewDataEventHandler SignalTrade;
    public static NewDataEventHandler SignalQuote;
    public static NewDataEventHandler SignalStage1;
    public static NewDataEventHandler SignalStage2;
    */
    public ATSComponent atsc;

    // returned by optimzer to ATSComponent
    public bool BuySignal;
    public bool SellSignal;
    public bool ExitSignal;

    // calculated by ATS Strategy
    public TimeSpan TradingTimeBegin;
    public TimeSpan TradingTimeEnd;

    //public bool HasPosition = false;

    public GPStrategy() {
    }

    /*
    public void EmitSignalInit( object o ) {
      Init();
      if (null != SignalInit) SignalInit(o, this);
    }

    public void EmitSignalBar( object o, Bar bar ) {
      OnBar(bar);
      if (null != SignalBar) SignalBar(o, this);
    }

    public void EmitSignalTrade( object o, Trade trade ) {
      OnTrade(trade);
      if (null != SignalTrade) SignalTrade(o, this);
    }

    public void EmitSignalQuote( object o, Quote quote ) {
      OnQuote(quote);
      if (null != SignalQuote) SignalQuote(o, this);
    }

    public void EmitSignalStage1( object o ) {
      BuySignal = false;
      SellSignal = false;
      ExitSignal = false;
      if (null != SignalStage1) SignalStage1(o, this); 
    }

    public void EmitSignalStage2( object o ) {
      if (null!=SignalStage2) SignalStage2(o, this);

    }

    public virtual void OnQuote( Quote quote ) {
    }

    public virtual void Init() {
    }

    public virtual void OnBar( Bar bar ) {
    }

    public virtual void OnTrade( Trade trade ) {
    }
    */

  }

}

