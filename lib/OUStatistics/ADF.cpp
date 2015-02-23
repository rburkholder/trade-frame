#include "ADF.h"

#include <fstream>
#include <math.h>

#include "NewMat/newmat.h"
#include "NewMat/newmatio.h"

#ifdef use_namespace
using namespace RBD_LIBRARIES;
#endif

//ReturnMatrix OLS(Matrix &x,Matrix &y);
//ReturnMatrix OLSError(Matrix& x,Matrix& y,ColumnVector& beta,int df);
void GetNeighbourIndices(int n,double* inArr,double x,int* lx,int* ux);

ReturnMatrix OLS(Matrix &x,Matrix &y) {
  Matrix ma=(((x.t()*x).i())*x.t())*y;
  ma.Release();
  return ma;
}

ReturnMatrix OLSError(Matrix& x,Matrix& y,ColumnVector& beta,int df) {
  ColumnVector residuals=y+((x*beta)*(-1));
  Real residualSumOfSquares=(residuals.t()*residuals).AsScalar();
  Real sigma2=residualSumOfSquares/df;
  DiagonalMatrix diag;  diag<<((x.t()*x).i())*sigma2;

  for(int i=1;i<=diag.Nrows();i++)
    diag(i)=pow(diag(i),0.5);

  residuals.Release();
  diag.Release();
  return diag;
}

void adfTest(double* x,int obs,int k,double* dfs,double* pv) {
  double xAxis[8]={
    0.01,0.025,0.05,0.1,0.9,0.95,0.975,0.99
  };
  double yAxis[6]={
    25.0,50.0,100.0,250.0,500.0,10000.0
  };
  double zSurface[6][8]={
    -4.38,-3.95,-3.60,-3.24,-1.14,-0.80,-0.50,-0.15,
    -4.15,-3.80,-3.50,-3.18,-1.19,-0.87,-0.58,-0.24,
    -4.04,-3.73,-3.45,-3.15,-1.22,-0.90,-0.62,-0.28,
    -3.99,-3.69,-3.43,-3.13,-1.23,-0.92,-0.64,-0.31,
    -3.98,-3.68,-3.42,-3.13,-1.24,-0.93,-0.65,-0.32,
    -3.96,-3.66,-3.41,-3.12,-1.25,-0.94,-0.66,-0.33,
  };

  int lags=k+1;
  int cols=(lags-1)+3;
  int rows=obs-lags;

  Matrix xMat(rows,cols);
  ColumnVector yMat(rows);

  double* delta=new double[obs-1];
  for(int i=0;i<(obs-1);i++)
    delta[i]=x[i+1]-x[i];

  xMat.Column(1)=1.0;
  for(int i=0;i<rows;i++)
  {
    yMat(i+1)=delta[lags+i-1];
    xMat(i+1,2)=x[lags+i-1];
    xMat(i+1,3)=lags+i;
    for(int j=1;j<lags;j++)
      xMat(i+1,j+3)=delta[lags+i-j-1];
  }

  int df=rows-cols;
  ColumnVector beta=OLS(xMat,yMat);
  DiagonalMatrix stderror=OLSError(xMat,yMat,beta,df);

  int lx,ux;
  double* zSection=new double[8];
  Real yLookup=rows-1;
  GetNeighbourIndices(6,yAxis,yLookup,&lx,&ux);
  for(int i=0;i<8;i++)
  {
    if(lx==ux)
    {
      zSection[i]=zSurface[lx][i];
    }else
    {
      double y1=yAxis[lx];
      double y2=yAxis[ux];
      double z1=zSurface[lx][i];
      double z2=zSurface[ux][i];
      double y=yLookup;
      zSection[i]=z1+(z2-z1)*((y-y1)/(y2-y1));
    }
  }

  int lz,uz;
  double pValue=0.0;
  Real tStatistic=beta(2)/stderror(2);
  GetNeighbourIndices(8,zSection,tStatistic,&lz,&uz);
  if(lz==uz)
  {
    pValue=xAxis[lz];
  }else
  {
    double y1=zSection[lz];
    double y2=zSection[uz];
    double z1=xAxis[lz];
    double z2=xAxis[uz];
    double y=tStatistic;
    pValue=z1+(z2-z1)*((y-y1)/(y2-y1));
  }

  *dfs=tStatistic;
  *pv=pValue;
  delete[] zSection;
  delete[] delta;
}

void GetNeighbourIndices(int n,double* inArr,double x,int* lx,int* ux) {

  int lowerX(0);
  int upperX(0);

  if(x<=inArr[0])
  {
    lowerX=0;
    upperX=0;
  }else if(x>=inArr[n-1])
  {
    lowerX=n-1;
    upperX=n-1;
  }else
  {
    for(int i=1;i<n;i++)
    {
      if(x<inArr[i])
      {
        lowerX=i-1;
        upperX=i;
        break;
      }else if(x==inArr[i])
      {
        lowerX=i;
        upperX=i;
        break;
      }//END OF IF
    }//END OF FOR
  }//END OF IF
  *lx=lowerX;
  *ux=upperX;
}

