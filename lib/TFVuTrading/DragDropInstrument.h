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

#include <string>

#include <wx/dataobj.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
  
class DragDropDataInstrument: public wxDataObject {
public:
  
  static const wxDataFormat DataFormatInstrumentIQFeedSymbolName;
  static const wxDataFormat DataFormatInstrumentInteractiveBrokersContract;
  static const wxDataFormat DataFormatInstrumentClass;
 
  DragDropDataInstrument( );
  explicit DragDropDataInstrument( const std::string& sIQFeedSymbolName );
  virtual ~DragDropDataInstrument( );
  
  virtual void GetAllFormats (wxDataFormat *formats, Direction dir=Get) const;
  virtual bool GetDataHere (const wxDataFormat &format, void *buf) const;
  virtual size_t GetDataSize (const wxDataFormat &format) const;
  virtual size_t GetFormatCount (Direction dir=Get) const;
  virtual wxDataFormat GetPreferredFormat (Direction dir=Get) const;
  virtual bool 	SetData (const wxDataFormat &format, size_t len, const void *buf);
  bool IsSupported (const wxDataFormat &format, Direction dir=Get) const;
  
  const std::string& GetIQFeedSymbolName() const { return m_sIQFeedSymbolName; } // TODO: need to validate which Format flag was in use
protected:
private:
  static const char szFormatIQFeedSymbolName[];
  static const char szFormatInteractiveBrokersContract[];
  static const char szFormatInstrumentClass[];
  
  std::string m_sIQFeedSymbolName;
};

} // namespace tf
} // namespace ou

#endif /* DRAGDROPINSTRUMENT_H */

