/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

class CRunningStats {
public:
  CRunningStats(void);
  CRunningStats(double BBMultiplier);
  virtual ~CRunningStats(void);
  void SetBBMultiplier( double dbl ) { m_BBMultiplier = dbl; };
  double GetBBMultiplier( void ) { return m_BBMultiplier; };

  void Add( double, double );
  void Remove( double, double );
  virtual void CalcStats( void );

  double b2; // acceleration
  double b1; // slope
  double b0; // offset

  double meanY;

  double RR;
  double R;

  double SD;

  double BBUpper, BBLower;

protected:
  unsigned int nX, nY;
  double SumXX, SumX, SumXY, SumY, SumYY;
  double m_BBMultiplier;
private:
};
