/* Copyright(c) 2017, One Unified. All rights reserved.                 *
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
 * File:   IQFeedInstrumentBuild.h
 * Author: raymond@burkholder.net
 *
 * Created on August 19, 2017, 7:12 PM
 */

#ifndef IQFEEDINSTRUMENTBUILD_H
#define IQFEEDINSTRUMENTBUILD_H

#include <functional>

#include <TFTrading/TradingEnumerations.h>
#include <TFTrading/Instrument.h>

#include <TFVuTrading/DialogPickSymbol.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class IQFeedInstrumentBuild {
public:

  typedef ou::tf::Instrument::idInstrument_t idInstrument_t;
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;

  IQFeedInstrumentBuild( wxWindow* pParentForDialog );
  virtual ~IQFeedInstrumentBuild();

  typedef DialogPickSymbol::DataExchange::functionLookupIQFeedDescription_t functionLookupIQFeedDescription_t;
  functionLookupIQFeedDescription_t fLookupIQFeedDescription;

  struct ValuesForBuildInstrument {
    const std::string& sKey;
    const std::string& sIQF;
    const std::string& sIB;
    pInstrument_t& pInstrument;
    boost::uint16_t day;
    ValuesForBuildInstrument( const std::string& sKey_, const std::string& sIQF_, const std::string& sIB_, pInstrument_t& pInstrument_, boost::uint16_t day_)
      : sKey( sKey_ ), sIQF( sIQF_ ), sIB( sIB_ ), pInstrument( pInstrument_ ), day( day_ )
      {}
  };

  typedef std::function<void(ValuesForBuildInstrument&)> functionBuildInstrument_t;
  functionBuildInstrument_t fBuildInstrument;

  pInstrument_t HandleNewInstrumentRequest(
    const ou::tf::Allowed::EInstrument selector,
    const wxString& sUnderlying );

  void InstrumentUpdated( pInstrument_t ); // typically:  the ib contract id has arrived

protected:
private:

  wxWindow* m_pParentForDialog;

  pInstrument_t m_pDialogPickSymbolCreatedInstrument;

  ou::tf::DialogPickSymbol* m_pDialogPickSymbol;
  DialogPickSymbol::DataExchange m_de;

  void HandleComposeIQFeedFullName( ou::tf::DialogPickSymbol::DataExchange* );
  void HandleLookUpDescription( const std::string&, std::string& );

  void BuildInstrument( const DialogPickSymbol::DataExchange& pde, pInstrument_t& pInstrument );

};

} // namespace tf
} // namespace ou

#endif /* IQFEEDINSTRUMENTBUILD_H */

