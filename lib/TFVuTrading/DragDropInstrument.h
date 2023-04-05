/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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
 * File:   DragDropInstrument.h
 * Author: raymond@burkholder.net
 *
 * Created on July 6, 2018, 11:25 AM
 */

#ifndef DRAGDROPINSTRUMENT_H
#define DRAGDROPINSTRUMENT_H

#include <wx/dataobj.h>

#include <TFTrading/Instrument.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class DragDropInstrument: public wxDataObject {
public:

  typedef Instrument::pInstrument_t pInstrument_t;
  typedef pInstrument_t pOptionInstrument_t;
  typedef pInstrument_t pUnderlyingInstrument_t;

  static const wxDataFormat DataFormatIQFeedSymbolName;
  static const wxDataFormat DataFormatClass;
  static const wxDataFormat DataFormatFunction_Instrument;
  static const wxDataFormat DataFormatFunction_OptionUnderlying;

  typedef std::function<void(pInstrument_t)> fOnInstrumentRetrieveComplete_t;
  typedef std::function<void(fOnInstrumentRetrieveComplete_t)> fOnInstrumentRetrieveInitiate_t;
  typedef fOnInstrumentRetrieveInitiate_t* fOnInstrumentRetrieveInitiate_ptr;

  typedef std::function<void(pOptionInstrument_t, pUnderlyingInstrument_t)> fOnOptionUnderlyingRetrieveComplete_t;
  typedef std::function<void(fOnOptionUnderlyingRetrieveComplete_t)> fOnOptionUnderlyingRetrieveInitiate_t;
  typedef fOnOptionUnderlyingRetrieveInitiate_t* fOnOptionUnderlyingRetrieveInitiate_ptr;

  DragDropInstrument();
  explicit DragDropInstrument( const DragDropInstrument& ) = delete;
  explicit DragDropInstrument( const DragDropInstrument&& ) = delete;
  explicit DragDropInstrument( const std::string& sIQFeedSymbolName );
  explicit DragDropInstrument( pInstrument_t );
  explicit DragDropInstrument( fOnInstrumentRetrieveInitiate_t&& );
  explicit DragDropInstrument( fOnOptionUnderlyingRetrieveInitiate_t&& );
  virtual ~DragDropInstrument();

  virtual void GetAllFormats (wxDataFormat *formats, Direction dir=Get) const;
  virtual bool GetDataHere (const wxDataFormat &format, void *buf) const;
  virtual size_t GetDataSize (const wxDataFormat &format) const;
  virtual size_t GetFormatCount (Direction dir=Get) const;
  virtual wxDataFormat GetPreferredFormat (Direction dir=Get) const;
  virtual bool SetData (const wxDataFormat &format, size_t len, const void *buf);
  bool IsSupported (const wxDataFormat &format, Direction dir=Get) const;

  const std::string& GetIQFeedSymbolName() const; // TODO: need to validate which Format flag was in use
  pInstrument_t GetInstrument();
  fOnInstrumentRetrieveInitiate_t& GetInstrumentRetrieveInitiate();  // TODO: change to a move as the constructor  is a move
  fOnOptionUnderlyingRetrieveInitiate_t& GetOptionUnderlyingRetrieveInitiate();  // TODO: change to a move as the constructor  is a move

protected:
private:
  static const char szFormatIQFeedSymbolName[];
  static const char szFormatClass[];
  static const char szFormatFunction_Instrument[];
  static const char szFormatFunction_OptionUnderlying[];

  wxDataFormat m_DataFormat;
  std::string m_sIQFeedSymbolName;
  pInstrument_t m_pInstrument;
  fOnInstrumentRetrieveInitiate_t m_fOnInstrumentRetrieveInitiate;
  fOnOptionUnderlyingRetrieveInitiate_t m_fOnOptionUnderlyingRetrieveInitiate;
};

} // namespace tf
} // namespace ou

#endif /* DRAGDROPINSTRUMENT_H */

