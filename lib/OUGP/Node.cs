using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;
using System.Reflection;
using System.Runtime;

//using SmartQuant.FIX;
//using SmartQuant.Data;
//using SmartQuant.Series;
//using SmartQuant.Trading;
//using SmartQuant.Charting;
//using SmartQuant.Execution;
//using SmartQuant.Providers;
//using SmartQuant.Indicators;
//using SmartQuant.Instruments;
//using SmartQuant.Optimization;

namespace OneUnified.GeneticProgramming {


  public class Node {

    public enum EParentLink {
      None = 0, Left, Center, Right
    }

    protected Node ChildLeft = null;
    protected Node ChildCenter = null;
    protected Node ChildRight = null;
    public Node Parent = null;
    public EParentLink ParentSide;
    protected bool Terminal = false;

    // nodes for use in population (these are the return types of the nodes)
    protected static ArrayList alBoolNodes;
    protected static ArrayList alDoubleNodes;

    protected int cntNodes = 2;  // how many child nodes by default (0 for terminal nodes)

    static protected Random random;

    static Node() {
      alBoolNodes = new ArrayList(20);
      alDoubleNodes = new ArrayList(30);
      random = new Random();
      DoubleNode dn = new DoubleNode();
      dn = null;
      BoolNode bn = new BoolNode();
      bn = null;
      Console.WriteLine("bool nodes {0} double nodes {1}", alBoolNodes.Count, alDoubleNodes.Count);
    }

    public Node() {
      Parent = null;
      ParentSide = EParentLink.None;
    }

    public bool IsTerminal {
      get { return Terminal; }
    }

    public virtual string TreeToString() {
      string value = "";
      if (IsTerminal) {
        value = ToString();
      }
      else {
        switch (cntNodes) {
          case 1:
            value = ToString() + ChildCenter.TreeToString();
            break;
          case 2:
            value = "(" + ChildLeft.TreeToString() + ToString() + ChildRight.TreeToString() + ")";
            break;
        }
      }
      return value;
    }

    public void AddLeft( Node node ) {
      ChildLeft = node;
      node.Parent = this;
      node.ParentSide = EParentLink.Left;
    }

    public void AddCenter( Node node ) {
      ChildCenter = node;
      node.Parent = this;
      node.ParentSide = EParentLink.Center;
    }

    public void AddRight( Node node ) {
      ChildRight = node;
      node.Parent = this;
      node.ParentSide = EParentLink.Right;
    }

    public virtual void AddRandomChildren( bool UseTerminal, bool UseNode, int Depth, int MaxDepth ) {
      throw new NotSupportedException("AddRandomChildren called from virtual, no override");
    }

    protected virtual void AddRandomChildren(
      bool UseTerminal, bool UseNode, int Depth, int MaxDepth, ArrayList al
      // al is the list from which strongly typed elements are chosen:  doubles or boolean
      ) {
      
      if (!UseTerminal && !UseNode) {
        throw new NotSupportedException("One or both of UseTerminal, UseNode must be set.");
      }
      if (Depth > MaxDepth) {
        throw new NotSupportedException("Depth (" + Depth.ToString() + ") > MaxDepth (" + MaxDepth.ToString() + ")");
      }

      switch (cntNodes) {
        case 1:
          AddCenter(CreateChild(UseTerminal, UseNode, Depth, MaxDepth, al));
          if (!ChildCenter.IsTerminal)
            ChildCenter.AddRandomChildren(UseTerminal, UseNode, Depth + 1, MaxDepth);
          break;
        case 2:
          AddRight(CreateChild(UseTerminal, UseNode, Depth, MaxDepth, al));
          if (!ChildRight.IsTerminal)
            ChildRight.AddRandomChildren(UseTerminal, UseNode, Depth + 1, MaxDepth);

          AddLeft(CreateChild(UseTerminal, UseNode, Depth, MaxDepth, al));
          if (!ChildLeft.IsTerminal)
            ChildLeft.AddRandomChildren(UseTerminal, UseNode, Depth + 1, MaxDepth);
          break;
      }
    }

    private Node CreateChild( bool UseTerminal, bool UseNode, int Depth, int MaxDepth, ArrayList al ) {
      /*               | !depth==max | depth==max |
       *               |-------------|------------|
       * !term * !node | illegal     | illegal    |
       *  term * !node | term        | term       |
       * !term *  node | node        | term       |
       *  term *  node | node, term  | term       |
       */
      bool bEnd = false;
      bool terminal = ( 1 == Depth && Depth < MaxDepth ) ? false : UseTerminal;  // don't use a terminal on first node
      Node node = null;
      while (!bEnd) {
        int i = random.Next(al.Count);
        Type t = al[i] as Type;
        node = Activator.CreateInstance(t) as Node;
        if (UseNode && Depth < MaxDepth) {
          if (!terminal && node.IsTerminal) {
            // reject and try for another
          }
          else {
            bEnd = true;
          }
        }
        else {
          if (node.IsTerminal) {
            bEnd = true;
          }
          else {
          }
        }
        if (!bEnd) node = null;
      }
      if (null == node) {
        throw new Exception("node is null in CreateChild");
      }
      return node;
    }

    public virtual void AddAsCandidateNodeTo( RootNode root ) {
      throw new NotSupportedException("AddAsCandidatenodeTo called from virtual, no override");
    }

    public int AddCandidates( RootNode root, int Depth ) {
      int level = Depth;
      AddAsCandidateNodeTo(root);
      if (null != ChildLeft) {
        level = Math.Max( level, ChildLeft.AddCandidates(root, Depth + 1) );
      }
      if (null != ChildCenter){
        level = Math.Max(level, ChildCenter.AddCandidates(root, Depth + 1));
      }
      if (null != ChildRight) {
        level = Math.Max(level, ChildRight.AddCandidates(root, Depth + 1));
      }
      return level;
    }

    public void DeleteChildren() {
      if (null != ChildRight) {
        ChildRight.DeleteChildren();
        ChildRight = null;
      }
      if (null != ChildCenter) {
        ChildCenter.DeleteChildren();
        ChildCenter = null;
      }
      if (null != ChildLeft) {
        ChildLeft.DeleteChildren();
        ChildLeft = null;
      }
    }

    public virtual Node Replicate( bool CopyValues ) {

      Node NewNode = null;
      NewNode = Activator.CreateInstance(this.GetType()) as Node;
      if ( CopyValues) CopyValuesTo(NewNode);

      if (null != ChildLeft) {
        NewNode.ChildLeft = ChildLeft.Replicate(CopyValues);
        NewNode.ChildLeft.Parent = NewNode;
        NewNode.ChildLeft.ParentSide = EParentLink.Left;
      }
      if (null != ChildCenter) {
        NewNode.ChildCenter = ChildCenter.Replicate(CopyValues);
        NewNode.ChildCenter.Parent = NewNode;
        NewNode.ChildCenter.ParentSide = EParentLink.Center;
      }
      if (null != ChildRight) {
        NewNode.ChildRight = ChildRight.Replicate(CopyValues);
        NewNode.ChildRight.Parent = NewNode;
        NewNode.ChildRight.ParentSide = EParentLink.Right;
      }

      return NewNode;
    }

    protected virtual void CopyValuesTo( Node node ) {
      // copy this.values to replicated copy
    }

    public virtual bool EvaluateBool( object o ) {
      throw new NotSupportedException("Can not call EvaluateBool on void base.");
    }

    public virtual double EvaluateDouble( object o ) {
      throw new NotSupportedException("Can not call EvaluateDouble on void base.");
    }

  }

  #region BoolNode

  public class BoolNode : Node {
    // base
    // input: one or two bool
    // output:  one bool

    static BoolNode() {
      RootNode rn = new RootNode();
      rn = null;
      BoolFalseNode bfn = new BoolFalseNode();
      bfn = null;
      BoolTrueNode btn = new BoolTrueNode();
      btn = null;
      BoolNotNode bnn = new BoolNotNode();
      bnn = null;
      BoolAndNode ban = new BoolAndNode();
      ban = null;
      BoolOrNode bon = new BoolOrNode();
      bon = null;
    }

    public BoolNode() {
    }

    public override void AddAsCandidateNodeTo( RootNode root ) {
      root.AddCandidateBoolNode(this);
    }

    public override bool EvaluateBool( object o ) {
      throw new NotSupportedException("Can not call EvaluateBool on BoolNode base.");
    }

    public override void AddRandomChildren( bool UseTerminal, bool UseNode, int Depth, int MaxDepth ) {
      AddRandomChildren(UseTerminal, UseNode, Depth, MaxDepth, alBoolNodes);
    }

  }

  public class RootNode : BoolNode {
    // input: one bool
    // output: one bool

    // these three arrays take care of handling strongly typed gentic crossovers
    public ArrayList alAllCandidateNodes;
    public ArrayList alBoolCandidateNodes;
    public ArrayList alDblCandidateNodes;

    public ArrayList alFunctionNodes;
    public ArrayList alTerminalNodes;

    public int NodeCount = 1;
    public int TreeDepth = 1;

    public RootNode() {

      cntNodes = 1;

      alAllCandidateNodes = new ArrayList(500);
      alBoolCandidateNodes = new ArrayList(500);
      alDblCandidateNodes = new ArrayList(500);

      alFunctionNodes = new ArrayList(500);
      alTerminalNodes = new ArrayList(500);

    }

    public override string ToString() {
      return "root=";
    }

    public override bool EvaluateBool( object o ) {
      Node node = ChildCenter;
      bool b = node.EvaluateBool(o);
      return b;
    }

    public void BuildTree( bool UseTerminal, bool UseNode, int MaxDepth ) {
      AddRandomChildren(UseTerminal, UseNode, 1, MaxDepth);
      RebuildCandidateLists();
    }

    private void AddTerminalFunctionNode( Node node ) {
      if (node.IsTerminal) {
        alTerminalNodes.Add(node);
      }
      else {
        alFunctionNodes.Add(node);
      }
      NodeCount++;
    }

    public void AddCandidateBoolNode( Node node) {
      alAllCandidateNodes.Add(node);
      alBoolCandidateNodes.Add(node);
      AddTerminalFunctionNode(node);
    }

    public void AddCandidateDblNode( Node node) {
      alAllCandidateNodes.Add(node);
      alDblCandidateNodes.Add(node);
      AddTerminalFunctionNode(node);
    }

    public void RebuildCandidateLists() {

      for (int i = alAllCandidateNodes.Count - 1; i >= 0; i--) {
        alAllCandidateNodes[i] = null;
      }
      alAllCandidateNodes.Clear();

      for (int i = alBoolCandidateNodes.Count - 1; i >= 0; i--) {
        alBoolCandidateNodes[i] = null;
      }
      alBoolCandidateNodes.Clear();

      for (int i = alDblCandidateNodes.Count - 1; i >= 0; i--) {
        alDblCandidateNodes[i] = null;
      }
      alDblCandidateNodes.Clear();

      for (int i = alFunctionNodes.Count - 1; i >= 0; i--) {
        alFunctionNodes[i] = null;
      }
      alFunctionNodes.Clear();

      for (int i = alTerminalNodes.Count - 1; i >= 0; i--) {
        alTerminalNodes[i] = null;
      }
      alTerminalNodes.Clear();

      NodeCount = 1;

      TreeDepth = ChildCenter.AddCandidates(this, 1);
    }

  }

  
  public class BoolFalseNode : BoolNode {
    // input:  none
    // output: one bool

    static BoolFalseNode() { 
      alBoolNodes.Add(typeof(BoolFalseNode));
    }

    public BoolFalseNode() {
      Terminal = true;
      cntNodes = 0;
    }


    public override string ToString() {
      return "false";
    }

    public override bool EvaluateBool( object o ) {
      return false;
    }
  }

  public class BoolTrueNode : BoolNode {
    // input:  none
    // output: one bool

    static BoolTrueNode() {
      alBoolNodes.Add(typeof(BoolTrueNode));
    }

    public BoolTrueNode() {
      Terminal = true;
      cntNodes = 0;
    }

    public override string ToString() {
      return "true";
    }

    public override bool EvaluateBool( object o ) {
      return true;
    }
  }
  

  public class BoolNotNode : BoolNode {
    // input:  one bool
    // output:  one bool


    public override string ToString() {
      return "!";
    }

    static BoolNotNode() {
      alBoolNodes.Add(typeof(BoolNotNode));
    }

    public BoolNotNode() {
      cntNodes = 1;
    }
    public override bool EvaluateBool( object o ) {
      Node node = ChildCenter;
      bool b = node.EvaluateBool(o);
      // return !bn.Evaluate(strategy);
      return b;
      //return !GetChildCenter.Evaluate(strategy);
    }
  }

  public class BoolAndNode : BoolNode {
    // input:  two bool
    // output:  one bool


    public override string ToString() {
      return "&&";
    }

    static BoolAndNode() {
      alBoolNodes.Add(typeof(BoolAndNode));
    }

    public override bool EvaluateBool( object o ) {
      Node node1 = ChildLeft;
      Node node2 = ChildRight;

      bool b1 = node1.EvaluateBool(o);
      bool b2 = node2.EvaluateBool(o);
      return b1 && b2;
    }
  }

  public class BoolOrNode : BoolNode {
    // input:  two bool
    // output:  one bool


    public override string ToString() {
      return "||";
    }

    static BoolOrNode() {
      alBoolNodes.Add(typeof(BoolOrNode));
    }

    public override bool EvaluateBool( object o ) {
      Node node1 = ChildLeft;
      Node node2 = ChildRight;
      bool b1 = node1.EvaluateBool(o);
      bool b2 = node2.EvaluateBool(o);
      return b1 || b2;
    }
  }

  #endregion BoolNode

  #region DoubleNode

  public class DoubleNode : Node {
    // base
    // output none
    // input one or two doubles
    static DoubleNode() {
      CompareNode cn = new CompareNode();
      cn = null;
      MathNode mn = new MathNode();
      mn = null;
    }

    public DoubleNode() {
    }

    public override void AddAsCandidateNodeTo( RootNode root ) {
      root.AddCandidateDblNode(this);
    }

    public override double EvaluateDouble( object o ) {
      throw new NotSupportedException("Can not call Evaluate on DoubleNode base.");
    }

    public override void AddRandomChildren( bool UseTerminal, bool UseNode, int Depth, int MaxDepth ) {
      AddRandomChildren(UseTerminal, UseNode, Depth, MaxDepth, alDoubleNodes);
    }

  }

  #region CompareNode

  public class CompareNode : BoolNode {
    // base
    // output bool
    // input one or two doubles
    static CompareNode() {
      CompareGTNode gt = new CompareGTNode();
      gt = null;
      CompareGENode ge = new CompareGENode();
      ge = null;
      //CompareEQNode eq = new CompareEQNode();
      //eq = null;
      CompareLENode le = new CompareLENode();
      le = null;
      CompareLTNode lt = new CompareLTNode();
      lt = null;
    }

    public override bool EvaluateBool( object o ) {
      throw new NotSupportedException("Can not call Evaluate on CompareNode base.");
    }

    public override void AddRandomChildren( bool UseTerminal, bool UseNode, int Depth, int MaxDepth ) {
      AddRandomChildren(UseTerminal, UseNode, Depth, MaxDepth, alDoubleNodes);
    }
  }

  public class CompareGTNode : CompareNode {
    // output bool
    // input two doubles


    public override string ToString() {
      return ">";
    }

    static CompareGTNode() {
      alBoolNodes.Add(typeof(CompareGTNode));
    }

    public override bool EvaluateBool( object o ) {
      Node node1 = ChildLeft;
      Node node2 = ChildRight;
      double d1 = node1.EvaluateDouble(o);
      double d2 = node2.EvaluateDouble(o);
      return d1 > d2;
    }
  }

  public class CompareGENode : CompareNode {
    // output bool
    // input two doubles


    public override string ToString() {
      return ">=";
    }

    static CompareGENode() {
      alBoolNodes.Add(typeof(CompareGENode));
    }

    public override bool EvaluateBool( object o ) {
      Node node1 = ChildLeft;
      Node node2 = ChildRight;
      double d1 = node1.EvaluateDouble(o);
      double d2 = node2.EvaluateDouble(o);
      return d1 >= d2;
    }
  }

  /*
  public class CompareEQNode : CompareNode {
    // output bool
    // input two doubles


    public override string ToString() {
      return "==";
    }

    static CompareEQNode() {
      alBoolNodes.Add(typeof(CompareEQNode));
    }

    public override bool EvaluateBool( GPStrategy strategy ) {
      Node node1 = ChildLeft;
      Node node2 = ChildRight;
      double d1 = node1.EvaluateDouble(strategy);
      double d2 = node2.EvaluateDouble(strategy);
      return d1 == d2;
    }
  }
  */
  public class CompareLENode : CompareNode {
    // output bool
    // input two doubles


    public override string ToString() {
      return "<=";
    }

    static CompareLENode() {
      alBoolNodes.Add(typeof(CompareLENode));
    }

    public override bool EvaluateBool( object o ) {
      Node node1 = ChildLeft;
      Node node2 = ChildRight;
      double d1 = node1.EvaluateDouble(o);
      double d2 = node2.EvaluateDouble(o);
      return d1 <= d2;
    }
  }

  public class CompareLTNode : CompareNode {
    // output bool
    // input two doubles


    public override string ToString() {
      return "<";
    }

    static CompareLTNode() {
      alBoolNodes.Add(typeof(CompareLTNode));
    }

    public override bool EvaluateBool( object o ) {
      Node node1 = ChildLeft;
      Node node2 = ChildRight;
      double d1 = node1.EvaluateDouble(o);
      double d2 = node2.EvaluateDouble(o);
      return d1 < d2;
    }
  }

  #endregion CompareNode

  #region MathNode

  public class MathNode : DoubleNode {
    // base
    // output one double
    // input one or two doubles
    static MathNode() {
      MathAddNode man = new MathAddNode();
      man = null;
      MathSubNode msn = new MathSubNode();
      msn = null;
    }

    public override double EvaluateDouble( object o ) {
      throw new NotSupportedException("Can not call Evaluate on MathNode base.");
    }
  }

  public class MathAddNode : MathNode {
    // output one double
    // input two doubles

    public override string ToString() {
      return "+";
    }

    static MathAddNode() {
      alDoubleNodes.Add(typeof(MathAddNode));
    }

    public override double EvaluateDouble( object o ) {
      Node node1 = ChildLeft;
      Node node2 = ChildRight;
      double d1 = node1.EvaluateDouble(o);
      double d2 = node2.EvaluateDouble(o);
      return d1 + d2;
    }
  }

  public class MathSubNode : MathNode {
    // output one double
    // input two doubles

    public override string ToString() {
      return "-";
    }

    static MathSubNode() {
      alDoubleNodes.Add(typeof(MathSubNode));
    }

    public override double EvaluateDouble( object o ) {
      Node node1 = ChildLeft;
      Node node2 = ChildRight;
      double d1 = node1.EvaluateDouble(o);
      double d2 = node2.EvaluateDouble(o);
      return d1 - d2;
    }
  }

  public class MathMltNode : MathNode {
    // output one double
    // input two doubles

    public override string ToString() {
      return "*";
    }

    static MathMltNode() {
      alDoubleNodes.Add(typeof(MathMltNode));
    }

    public override double EvaluateDouble( object o ) {
      Node node1 = ChildLeft;
      Node node2 = ChildRight;
      double d1 = node1.EvaluateDouble(o);
      double d2 = node2.EvaluateDouble(o);
      return d1 * d2;
    }
  }

  public class MathDvdNode : MathNode {
    // output one double
    // input two doubles

    public override string ToString() {
      return "/";
    }

    static MathDvdNode() {
      alDoubleNodes.Add(typeof(MathDvdNode));
    }

    public override double EvaluateDouble( object o ) {
      Node node1 = ChildLeft;
      Node node2 = ChildRight;
      double d1 = node1.EvaluateDouble(o);
      double d2 = node2.EvaluateDouble(o);
      return d1 / d2;
    }
  }

  #endregion MathNode

  #endregion DoubleNode
}
