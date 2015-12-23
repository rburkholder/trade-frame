/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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

// started December 13, 2015, 8:16 PM

#include "TreeItemInstrument.h"

void TreeItemInstrument::Init() {
  /*
   * work on loading instrument list in background
   * confirm that subsets can be saved and loaded
   * 
   * need instrument list
   * pop up to ask for input for symbol, for now just get name, then incrementally work on parallel lookups
   * then build instrument with iqfeed, ib
   * send off for ib contract
   * need to look in instrument table first
   */
  m_pInstrument = m_resources.signalNewInstrument(); 
  if ( 0 != m_pInstrument.get() ) {
    m_baseResources.signalSetItemText( m_id, m_pInstrument->GetInstrumentName() );
  }
}