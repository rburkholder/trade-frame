/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

/*
 * File:    HiPass.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: February 6, 2023  18:54:43
 */

#pragma once

#include <string>

#include <OUCharting/ChartEntryIndicator.h>

class HiPass {
public:
  HiPass();
  void Init( int nPeriods, ou::Colour::EColour colour, const std::string& sName );
  void Update( boost::posix_time::ptime dt, double price );
protected:
private:

  ou::ChartEntryIndicator m_ceEhlersHiPassFilter;
  ou::ChartEntryIndicator m_ceEhlersHiPassFilterSlope;
  //ou::ChartEntryIndicator m_ceEhlersLoPassFilter;

  // ehlers, cybernetic analsys, eqn 2.7, eqn 2.9
  double m_alpha;
  double m_one_minus_alpha;
  double m_alpha_by_two;
  double m_one_minus_alpha_by_two;
  double m_alpha_squared;

  double m_dblPrice0, m_dblPrice1, m_dblPrice2;
  double m_dblHPF0, m_dblHPF1, m_dblHPF2;
  //double m_dblLPF0, m_dblLPF1, m_dblLPF2;

  double m_dblHPF_Slope0, m_dblHPF_Slope1;


};

