using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;

namespace OneUnified.GeneticProgramming {

  public class IndividualComparison : IComparer {
    int IComparer.Compare( object x1, object y1 ) {
      int val = 0;
      Individual x = x1 as Individual;
      Individual y = y1 as Individual;
      if ( x.NormalizedFitness > y.NormalizedFitness ) val = -1;
      else {
        if ( x.NormalizedFitness < y.NormalizedFitness ) val = 1;
      }
      return val;
    }
  }

  public class Individual {

    public int id;

    public RootNode LongEnter;
    public RootNode LongExit;
    public RootNode ShortEnter;
    public RootNode ShortExit;
    //public Node Signal;
    //public RootNode LongSignal;
    //public RootNode ShortSignal;
    //public Node StopSignal;
    //public Node LongExitSignal;
    //public Node ShortExitSignal;
    //public Node ReverseSignal;
    //public Node GearUpSignal;
    //public Node GearDownSignal;

    public bool Computed = false;
    
    public double RawFitness = 0; // absolute dollars value
    public double RelativeFitness = 0;  // maxfitness - rawfitness, larger numbers therefore worse (aka standardized fitness)
    public double AdjustedFitness = 0;  // 1 / ( 1 + rf ), range 0 to 1, with 1 being best
    public double NormalizedFitness = 0;  // af / sum(af), range 0 to 1, with 1 being best, sum is 1

    private static int Count = 1;

    public Individual() {
      id = Count++;

      //LongSignal = new RootNode();
      //ShortSignal = new RootNode();
      LongEnter = new RootNode();
      LongExit = new RootNode();
      ShortEnter = new RootNode();
      ShortExit = new RootNode();
    }

    public override string ToString() {
      //return "Long=" + LongSignal.TreeToString() + " Short=" + ShortSignal.TreeToString();
      return "LEnter=" + LongEnter.TreeToString()
          + " LExit=" + LongExit.TreeToString()
          + " SEnter=" + ShortEnter.TreeToString()
          + " SExit=" + ShortExit.TreeToString();
    }
  }
}
