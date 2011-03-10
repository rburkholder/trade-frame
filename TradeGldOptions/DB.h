/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <OUSQL/Session.h>
#include <OUSqlite/ISqlite3.h>

#include <TFTrading/KeyTypes.h>

#include <TFDatabase/TableDefs.h>

class CDB
{
public:

  CDB(void);
  ~CDB(void);

  void Open( const std::string& sDbName );
  void Close( void );

  bool IsOpen( void ) { return m_bOpened; };

  static ou::tf::keytypes::idPortfolio_t& PortfolioId( void ) { return m_sPortfolioId; };

  void LoadPortfolio( const ou::tf::keytypes::idPortfolio_t& id, ou::tf::CPortfolio::pPortfolio_t& pPortfolio );
  bool LoadUnderlying( const ou::tf::keytypes::idInstrument_t& id, ou::tf::CInstrument::pInstrument_t& pInstrument );
  void SaveInstrument( ou::tf::CInstrument::pInstrument_t& pInstrument );
  bool LoadOptions( const ou::tf::keytypes::idInstrument_t& id, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay ); // uses OnNewInstrument

  typedef FastDelegate1<ou::tf::CInstrument::pInstrument_t> OnNewInstrumentHandler_t;
  void SetOnNewInstrumentHandler( OnNewInstrumentHandler_t function ) {
    OnNewInstrument = function;
  }

protected:
private:

  bool m_bOpened;

  static std::string m_sPortfolioId;

  ou::db::CSession<ou::db::ISqlite3> m_session;

  void Populate( void );

  OnNewInstrumentHandler_t OnNewInstrument;
};

