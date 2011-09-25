using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace OneUnified.GeneticProgramming {
  class Population {

    // page 114 Koza 1992
    public int PopulationSize = 20; // should be some multiple of ( 2 * ( size / maxdepth - 1 ) )
    public int MaxGenerations = 25;

    public double probCrossover = 0.95;
    public double probReproduction = 0.10;

    public double probFunctionPointCrossover = 0.90;
    public double probTerminalPointCrossover = 0.10;

    public int MaxDepthOnCreation = 4;
    public int MaxDepthOnCrossover = 17;

    public double probMutation = 0.0;
    public double probPermutation = 0.0;

    public int TournamentSize = 2;

    public double probDecimation = 0.58;
    public double ratioElitism = 0.012;

    public int cntSignals = 4; // LongEnter, LongExit, ShortEnter, ShortExit

    //public Individual[] individuals;
    public Individual[] curGeneration;
    private Individual[] nxtGeneration;
    private ArrayList Generations;
    public int cntGenerations = 0;

    public int cntAboveAvg = 0;

    private Random random;

    public Population( int PopulationSize ) {

      // initialize sizes and random number generator
      random = new Random();
      this.PopulationSize = PopulationSize;
      Generations = new ArrayList(MaxGenerations);

      // allocate a generation of individuals
      curGeneration = new Individual[PopulationSize];
      cntGenerations = 1;
      Generations.Add(curGeneration);

      CreateIndividuals(PopulationSize, curGeneration);

    }

    private void CreateIndividuals( int Count, Individual[] Generation ) {

      // can only populate starting at beginning

      // create individuals with RootNode signals
      // keep rootnodes in a list so they can be populated with random expressions
      ArrayList rn = new ArrayList(cntSignals * Count);
      for (int i = 0; i < Count; i++) {
        Individual individual = new Individual();
        Generation[i] = individual;
        rn.Add(individual.LongEnter);
        rn.Add(individual.LongExit);
        rn.Add(individual.ShortEnter);
        rn.Add(individual.ShortExit);
      }

      // implement ramped half and half, koza, 1992, page 93
      RootNode node;
      int nodesperstep = rn.Count / (MaxDepthOnCreation - 1);
      int nodesperhalf = nodesperstep / 2;
      for (int step = 2; step <= MaxDepthOnCreation; step++) {
        for (int i = 1; i <= nodesperhalf; i++) {
          int r = random.Next(rn.Count - 1);
          node = rn[r] as RootNode;
          rn[r] = null;
          rn.RemoveAt(r);
          node.BuildTree(true, true, step); // random tree

          r = random.Next(rn.Count - 1);
          node = rn[r] as RootNode;
          rn[r] = null;
          rn.RemoveAt(r);
          node.BuildTree(false, true, step); // full tree
        }
      }

      // due to rounding above, finish up any unused rootnodes
      while (0 != rn.Count) {
        node = rn[0] as RootNode;
        rn[0] = null;
        rn.RemoveAt(0);
        node.BuildTree(true, true, MaxDepthOnCreation);  // random tree
      }
    }

    private int TournamentSelection() {
      double prob1; // use above average individuals % of the time
      int prob2;  // used for selecting individual
      int result = PopulationSize - 1; // default to worst individual (array is sorted)
      for (int i = TournamentSize; i >= 1; i--) {
        prob1 = random.NextDouble();
        if (prob1 < 0.35) {
          prob2 = random.Next(cntAboveAvg - 1);
          result = Math.Min(result, prob2);
        }
        else {
          prob2 = random.Next(cntAboveAvg, PopulationSize - 1);
          result = Math.Min(result, prob2);
        }
      }
      return result;
    }

    private bool CheckForMatchInGeneration( Individual individual, Individual[] Generation, int MaxIndex ) {
      bool b = false;
      for (int i = 0; i <= MaxIndex; i++) {
        if (individual.ToString() == Generation[i].ToString()) {
          b = true;
          break;
        }
      }
      return b;
    }

    public bool MakeNewGeneration( bool CopyValues ) {
      cntGenerations++;
      bool bMore = false;
      if (MaxGenerations >= cntGenerations) {

        bMore = true;
        nxtGeneration = new Individual[PopulationSize];
        Generations.Add(nxtGeneration);

        int ix = 0;

        // decimate and repopulate everything at average and below
        int decimation = (int)Math.Min(0.40 * PopulationSize, PopulationSize - cntAboveAvg);
        // always introduce a certain minimum set of new individuals
        decimation = Math.Max(decimation, (int)(0.04 * PopulationSize));
        Console.WriteLine("Decimating {0} individuals", decimation);
        CreateIndividuals(decimation, nxtGeneration);
        ix += decimation;

        // elitism
        int j = 0;
        int cntElites = (int)(ratioElitism * PopulationSize);
        int cntAcceptedElites = 0;
        if (0 == ix && cntElites>0) {
          nxtGeneration[ix] = curGeneration[j];
          ix++;
          j++;
          cntAcceptedElites++;
        }
        while ( cntAcceptedElites < cntElites ) {
          if (!CheckForMatchInGeneration(curGeneration[j],nxtGeneration,ix-1)) {
            nxtGeneration[ix] = curGeneration[j];
            ix++;
            cntAcceptedElites++;
          }
          j++;
        }

        // reproduction & crossover
        int prob;
        while (ix < PopulationSize) {
        //for (int i = PopulationSize - ix; ix >= 1; i--) {
          double prob1 = random.NextDouble();
          if (prob1 < probReproduction || ( PopulationSize - ix ) == 1 ) {
            // reproduction
            //prob = random.Next(curGeneration.Length - 1);
            nxtGeneration[ix++] = curGeneration[TournamentSelection()];
          }
          else {
            // crossover
            bool bOk = true;
            Individual i1;
            Individual i2;
            //prob = random.Next(curGeneration.Length - 1);
            i1 = curGeneration[TournamentSelection()];
            //prob = random.Next(curGeneration.Length - 1);
            i2 = curGeneration[TournamentSelection()];

            RootNode newxvr1; // seed and grow crossovers here
            RootNode newxvr2;
            RootNode oldxvr1; // obtain seed from here
            RootNode oldxvr2;
            RootNode node;  // holds node

            // load array with references to old nodes
            ArrayList nodesSrc = new ArrayList(2 * cntSignals);

            nodesSrc.Add(i1.LongEnter);
            nodesSrc.Add(i1.LongExit);
            nodesSrc.Add(i1.ShortEnter);
            nodesSrc.Add(i1.ShortExit);
            nodesSrc.Add(i2.LongEnter);
            nodesSrc.Add(i2.LongExit);
            nodesSrc.Add(i2.ShortEnter);
            nodesSrc.Add(i2.ShortExit);

            // create new individuals and provide references to new nodes
            i1 = new Individual();
            i2 = new Individual();

            ArrayList nodesDst = new ArrayList(2 * cntSignals);

            nodesDst.Add(i1.LongEnter);
            nodesDst.Add(i1.LongExit);
            nodesDst.Add(i1.ShortEnter);
            nodesDst.Add(i1.ShortExit);
            nodesDst.Add(i2.LongEnter);
            nodesDst.Add(i2.LongExit);
            nodesDst.Add(i2.ShortEnter);
            nodesDst.Add(i2.ShortExit);

            // crossover all but two random pairs
            for (int k = cntSignals - 1; k >= 1; k-- ) {

              prob = random.Next(nodesSrc.Count - 1);
              oldxvr1 = nodesSrc[prob] as RootNode;
              nodesSrc.RemoveAt(prob);

              prob = random.Next(nodesSrc.Count - 1);
              oldxvr2 = nodesSrc[prob] as RootNode;
              nodesSrc.RemoveAt(prob);

              newxvr1 = oldxvr1.Replicate(CopyValues) as RootNode;
              newxvr2 = oldxvr2.Replicate(CopyValues) as RootNode;

              newxvr1.RebuildCandidateLists();
              newxvr2.RebuildCandidateLists();

              bOk = bOk && CrossOver(newxvr1, newxvr2);

              node = nodesDst[0] as RootNode;
              nodesDst.RemoveAt(0);
              node = newxvr1;
              node = nodesDst[0] as RootNode;
              nodesDst.RemoveAt(0);
              node = newxvr2;

            }

            // crossover last two pairs
            oldxvr1 = nodesSrc[0] as RootNode;
            nodesSrc.RemoveAt(0);
            oldxvr2 = nodesSrc[0] as RootNode;
            nodesSrc.RemoveAt(0);

            newxvr1 = oldxvr1.Replicate(CopyValues) as RootNode;
            newxvr2 = oldxvr2.Replicate(CopyValues) as RootNode;

            newxvr1.RebuildCandidateLists();
            newxvr2.RebuildCandidateLists();

            bOk = bOk && CrossOver(newxvr1, newxvr2);

            node = nodesDst[0] as RootNode;
            nodesDst.RemoveAt(0);
            node = newxvr1;
            node = nodesDst[0] as RootNode;
            nodesDst.RemoveAt(0);
            node = newxvr2;

            if (bOk) {
              nxtGeneration[ix++] = i1;
              nxtGeneration[ix++] = i2;
            }

          }
        }
        curGeneration = nxtGeneration;
      }
      return bMore;
    }

    private bool CrossOver( RootNode rn1, RootNode rn2 ) {

      bool bSuccessful = false;
      int prob;
      Node node1 = null;
      bool b = false;
      bool d = false;

      if ( rn1.alBoolCandidateNodes.Count > 0
        && rn2.alBoolCandidateNodes.Count > 0
        && rn1.alDblCandidateNodes.Count > 0
        && rn2.alDblCandidateNodes.Count > 0 
        ) {
        prob = random.Next(rn1.alAllCandidateNodes.Count - 1);
        node1 = rn1.alAllCandidateNodes[prob] as Node;
        b = node1 is BoolNode;
        d = node1 is DoubleNode;
      }
      else {
        if ( rn1.alBoolCandidateNodes.Count > 0
          && rn2.alBoolCandidateNodes.Count > 0 
          ) {
          prob = random.Next(rn1.alBoolCandidateNodes.Count - 1);
          node1 = rn1.alBoolCandidateNodes[prob] as Node;
          b = node1 is BoolNode;
        }
        else {
          if ( rn1.alDblCandidateNodes.Count > 0
            && rn2.alDblCandidateNodes.Count > 0 
            ) {
            prob = random.Next(rn1.alDblCandidateNodes.Count - 1);
            node1 = rn1.alDblCandidateNodes[prob] as Node;
            d = node1 is DoubleNode;
          }
        }
      }

      if (b || d) {
        Node node2 = null;
        if (b) {
          prob = random.Next(rn2.alBoolCandidateNodes.Count - 1);
          node2 = rn2.alBoolCandidateNodes[prob] as Node;
        }
        if (d) {
          prob = random.Next(rn2.alDblCandidateNodes.Count - 1);
          node2 = rn2.alDblCandidateNodes[prob] as Node;
        }

        Node Parent1 = node1.Parent;
        Node Parent2 = node2.Parent;

        switch (node1.ParentSide) {
          case Node.EParentLink.Left:
            Parent1.AddLeft(node2);
            break;
          case Node.EParentLink.Center:
            Parent1.AddCenter(node2);
            break;
          case Node.EParentLink.Right:
            Parent1.AddRight(node2);
            break;
        }

        switch (node2.ParentSide) {
          case Node.EParentLink.Left:
            Parent2.AddLeft(node1);
            break;
          case Node.EParentLink.Center:
            Parent2.AddCenter(node1);
            break;
          case Node.EParentLink.Right:
            Parent2.AddRight(node1);
            break;
        }
        bSuccessful = true;
      }
      return bSuccessful;
    }

    public void CalcFitness() {

      double max = 0;
      double min = 0;
      int cntIndividuals = 0;
      foreach (Individual i in curGeneration) {
        if (0 == cntIndividuals) {
          min = i.RawFitness;
          max = i.RawFitness;
        }
        else {
          max = Math.Max(max, i.RawFitness);
          min = Math.Min(min, i.RawFitness);
        }
        cntIndividuals++;
      }
      double mid = (max - min) / 2.0;

      double sum = 0;
      foreach (Individual i in curGeneration) {
        i.RelativeFitness = max - i.RawFitness;
        i.AdjustedFitness = 1.0 / (1.0 + i.RelativeFitness);
        sum += i.AdjustedFitness;
      }

      cntAboveAvg = 0;
      double mean = sum / cntIndividuals;
      foreach (Individual i in curGeneration) {
        i.NormalizedFitness = i.AdjustedFitness / sum;
        cntAboveAvg += (i.AdjustedFitness > mean) ? 1 : 0;
      }

      Console.WriteLine("cntAboveAvg = {0}", cntAboveAvg);

      IndividualComparison CompareIndividuals = new IndividualComparison();
      Array.Sort(curGeneration, CompareIndividuals);

    }
  }
}
