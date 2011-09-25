using System;
using System.Text;
using System.Collections;
using System.Windows.Forms;
using System.Collections.Generic;

//using QDCustom;

//using SmartQuant;
//using SmartQuant.FIX;
//using SmartQuant.Data;
//using SmartQuant.Series;
using SmartQuant.Trading;
//using SmartQuant.Charting;
//using SmartQuant.Execution;
//using SmartQuant.Providers;
//using SmartQuant.Indicators;
//using SmartQuant.Instruments;
using SmartQuant.Optimization;

namespace OneUnified.GeneticProgramming {

  public class GPOptimizer : Optimizer {
    double fOldObjective;
    double fNewObjective;
    double[] fOptParam;

    //ATSMetaStrategy meta;
    //StrategyBase sb;

    public Individual curIndividual;

    public GPOptimizer( IOptimizable Optimizable )
      : base(Optimizable) {
      // Coordinate descent normal constructor

      fType = EOptimizerType.GeneticAlgorithm;

      //GPStrategy.SignalInit += OnSignalInit;
      //GPStrategy.SignalBar += OnSignalBar;
      //GPStrategy.SignalTrade += OnSignalTrade;
      //GPStrategy.SignalQuote += OnSignalQuote;

      //Console.WriteLine("GPOptimizer Construct");
     
      //meta = Optimizable as ATSMetaStrategy;
      //sb = meta.Strategies["ATS"];
      //sb.Global.Add("Individual", null);
      

    }
/*
    void OnSignalInit( object source, GPStrategy gpis ) {
    }

    void OnSignalBar( object source, GPStrategy gpis ) {
    }

    void OnSignalTrade( object source, GPStrategy gpis ) {
    }

    void OnSignalQuote( object source, GPStrategy gpis ) {
      try {
        gpis.BuySignal = curIndividual.LongSignal.EvaluateBool(gpis);
        gpis.SellSignal = curIndividual.ShortSignal.EvaluateBool(gpis);
      }
      catch {
        Console.WriteLine("problems");
      }
    }
*/
    public override void Optimize() {

      // Brute Force
      base.Optimize();
      fOptParam = new double[fNParam];
      fOldObjective = double.MaxValue - 1;
      //OptimizeParam(0);
      FindBestFormula(400);
      //ProcessIndividuals(20);

      EmitBestObjectiveReceived();

      EmitCompleted();
    }

    void FindBestFormula( int cnt ) {

      if (stopped)
        return;

      Population pop = new Population(cnt);

      Console.WriteLine("Started {0}", DateTime.Now);

      do {
        for (int i = 0; i < cnt; i++) {
          curIndividual = pop.curGeneration[i];
          if (!curIndividual.Computed) {

            //sb.Global["Individual"] = curIndividual;

            Update();

            Console.WriteLine("--------");
            Console.Write("{0} {1} ", i, curIndividual.id);

            fNewObjective = -Objective();
            curIndividual.RawFitness = fNewObjective;
            curIndividual.Computed = true;

            Console.WriteLine("  LEnter {0}", curIndividual.LongEnter.TreeToString());
            Console.WriteLine("  LExit  {0}", curIndividual.LongExit.TreeToString());
            Console.WriteLine("  SEnter {0}", curIndividual.ShortEnter.TreeToString());
            Console.WriteLine("  SExit  {0}", curIndividual.ShortExit.TreeToString());

            OnStep();
            Application.DoEvents(); 

          }
        }

        //Console.Beep();

        pop.CalcFitness();

        Console.WriteLine("********");
        Console.WriteLine( "Population {0} at {1}", pop.cntGenerations, DateTime.Now );
        for (int i = 0; i < 15; i++) {
          Console.WriteLine("========");
          Individual ind = pop.curGeneration[i];
          Console.WriteLine("id {0} raw {1:#.00} adj {2:#.00000} norm {3:#.00000}",
            ind.id, ind.RawFitness, ind.AdjustedFitness, ind.NormalizedFitness);
          Console.WriteLine("  LEnter Depth {0} {1}", ind.LongEnter.TreeDepth, ind.LongEnter.TreeToString());
          Console.WriteLine("  LExit  Depth {0} {1}", ind.LongExit.TreeDepth, ind.LongExit.TreeToString());
          Console.WriteLine("  SEnter Depth {0} {1}", ind.ShortEnter.TreeDepth, ind.ShortEnter.TreeToString());
          Console.WriteLine("  SExit  Depth {0} {1}", ind.ShortExit.TreeDepth, ind.ShortExit.TreeToString());
        }

          /*
        foreach (Individual i in pop.curGeneration) {
          Console.WriteLine("=======");
          Console.WriteLine("id {0} raw {1:#.00} adj {2:#.00000} norm {3:#.00000}",
            i.id, i.RawFitness, i.AdjustedFitness, i.NormalizedFitness);
          Console.WriteLine("  Long {0} {1}", i.LongSignal.TreeDepth, i.LongSignal.Print());
          Console.WriteLine("  Shrt {0} {1}", i.ShortSignal.TreeDepth, i.ShortSignal.Print());
        }
           */
      } while (pop.MakeNewGeneration(true) && !stopped);

    }

    // recursively calling Optimize
    void OptimizeParam( int i ) {
      //Console.WriteLine("GPOptimizer Optimize Param i {0}", i);
      if (stopped)
        return;
      // Return true if better parameter value is found
      if (i < fNParam) {
        if (fIsParamFixed[i]) // skip fixed parameters
          OptimizeParam(i++);
        else {
          for (double pv = fLowerBound[i]; pv <= fUpperBound[i]; pv += fSteps[i]) {
            fParam[i] = pv;
            OptimizeParam((i + 1));
          }
        }
      }
      else {
        Step();
      }
    }

    /// <summary>
    ///	Perform one optimizer step
    /// <br></br>
    /// <br></br>
    ///	One iteration step is done
    /// </summary>
    public void Step() {
      //Console.WriteLine("GPOptimizer Step");
      Update();
      fNewObjective = Objective();
      if (fNewObjective < fOldObjective) {
        fOldObjective = fNewObjective;
        for (int j = 0; j < fNParam; j++) fOptParam[j] = fParam[j];
      }
      OnStep();
      //      Application.DoEvents();
    }

    public override double Objective() {
      //Console.WriteLine("GPOptimizer Objective");
      return base.Objective();
    }

    public override void OnCircle() {
      //Console.WriteLine("GPOptimizer OnCircle");
      base.OnCircle();
    }

    public override void OnStep() {
      //Console.WriteLine("GPOptimizer OnStep");
      base.OnStep();
    }

    public override void Update() {
      //Console.WriteLine("GPOptimizer Update");
      base.Update();
    }



    public void OptimizeOld() {
      int i;

      // Brute Force
      base.Optimize();
      fOptParam = new double[fNParam];
      fOldObjective = double.MaxValue - 1;
      OptimizeParam(0);
      for (i = 0; i < fNParam; i++) fParam[i] = fOptParam[i];
      Update();

      EmitBestObjectiveReceived();

      fNewObjective = Objective();

      // ???
      base.Update();

      if (fVerboseMode == EVerboseMode.Debug) {
        for (i = 0; i < fNParam; i++)
          Console.WriteLine("Param[{0}] = {1}", i, fParam[i]);

        base.Print();
      }

      EmitCompleted();
    }

    /// <summary>
    /// Print
    /// </summary>
    public override void Print() {
      base.Print();
    }
  }
}
