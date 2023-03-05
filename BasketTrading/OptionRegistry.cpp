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
 * File:    OptionRegistry.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrader
 * Created: March 4, 2023 19:42:36
 */

 #include <TFVuTrading/TreeItem.hpp>

#include "OptionRegistry.hpp"

OptionRegistry::OptionRegistry(
    fRegisterOption_t&& fRegisterOption
  , fStartCalc_t&& fStartCalc
  , fStopCalc_t&& fStopCalc
  , fSetChartDataView_t&& fSetChartDataView
  ) :
    m_fRegisterOption( std::move( fRegisterOption ) )
  , m_fStartCalc( std::move( fStartCalc ) )
  , m_fStopCalc( std::move( fStopCalc ) )
  , m_fSetChartDataView( std::move( fSetChartDataView ) )
  {
    assert( nullptr != m_fStartCalc );
    assert( nullptr != m_fStopCalc );
    assert( nullptr != m_fRegisterOption );
  }

  OptionRegistry::~OptionRegistry() {
    for ( mapOption_t::value_type& vt: m_mapOption ) { // TODO: fix, isn't the best place?
      m_fStopCalc( vt.second->Option(), m_pWatchUnderlying );
    }
    m_mapOption.clear();
  }

  // don't worry about reference counting, options in a strategy need to be unique
  // however, may need to worry about the option if from previous time frame

  void OptionRegistry::Add( pOption_t pOption, pPosition_t pPosition, const std::string& sLegName, ou::tf::option::Combo::vMenuActivation_t&& ma ) {

    const std::string& sOptionName( pOption->GetInstrument()->GetInstrumentName() );

    mapOption_t::iterator iterOption = m_mapOption.find( sOptionName );
    if ( m_mapOption.end() == iterOption ) {

      try {
        m_fRegisterOption( pOption );
      }
      catch( std::runtime_error& e ) {
        std::cout << "OptionRepository::Add: " << e.what() << std::endl;
        // simply telling us we are already registered, convert from error to status?
      }

      pOptionStatistics_t pOptionStatistics = OptionStatistics::Factory( pOption );
      m_mapOption[ sOptionName ] = pOptionStatistics;
      ou::tf::TreeItem* pti = m_ptiParent->AppendChild(
        pOption->GetInstrumentName() + " (" + sLegName + ")",
        [this,pOptionStatistics]( ou::tf::TreeItem* ){
          m_fSetChartDataView( pOptionStatistics->ChartDataView() );
        },
        [this,&sOptionName, ma_=std::move(ma)]( ou::tf::TreeItem* pti ) {
          pti->NewMenu();
          for ( const ou::tf::option::Combo::vMenuActivation_t::value_type& vt: ma_  ) {
            pti->AppendMenuItem(
              vt.sLabel,
              //[this,&sOptionName,ma_f=std::move( vt.fMenuActivation )]( ou::tf::TreeItem* pti ){
              [this,&sOptionName,ma_f=&vt.fMenuActivation]( ou::tf::TreeItem* pti ){
                (*ma_f)();
              });
          }
        }
      );
      pOptionStatistics->Set( pti );
      pOptionStatistics->Set( pPosition );

      std::cout << "OptionRepository::Add " << pOption->GetInstrumentName() << std::endl;

      m_fStartCalc( pOption, m_pWatchUnderlying );
    }
    else {
      std::cout << "OptionRepository::Add: error, duplicate entry: " << sOptionName << std::endl;
    }

  }

  void OptionRegistry::Remove( pOption_t pOption ) {

    const std::string& sOptionName( pOption->GetInstrument()->GetInstrumentName() );
    std::cout << "OptionRepository::Remove: " <<sOptionName << std::endl;

    mapOption_t::iterator iterOption = m_mapOption.find( sOptionName );
    if ( m_mapOption.end() != iterOption ) {

      m_fStopCalc( pOption, m_pWatchUnderlying );
      //iterOption->second->GetTreeItem()->Delete(); // this needs to be tested prior to activation, what happens if this treeitem is visible?
      m_mapOption.erase( iterOption );

    }
    else {
      std::cout << "OptionRepository::Remove: error, option not found: " << sOptionName << std::endl;
    }

  }
