/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#pragma once

#include <OUWtlCommon/ThreadCrossingMessage.h>

#include <TFTimeSeries/MergeDatedDatums.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class CCrossThreadMerge:
  public CMergeDatedDatums,
  public CThreadCrossingMessage<CCrossThreadMerge>
{
public:

  using inherited_t = CThreadCrossingMessage<CCrossThreadMerge>;

  CCrossThreadMerge();
  ~CCrossThreadMerge();

protected:

  enum enumMessageIds {
    UWM_CARRIEREVENT = inherited_t::UWM_THREADCROSSINGMESSAGE_NEXT // background thread posts dateddatums to foreground
  };

  BEGIN_MSG_MAP_EX(CCrossThreadMerge)
    MESSAGE_HANDLER(UWM_CARRIEREVENT, ProcessCarrierInForeGround)
  END_MSG_MAP()

  void ProcessCarrierInBackGround( CMergeCarrierBase *pCarrier );
  LRESULT ProcessCarrierInForeGround( UINT, WPARAM, LPARAM, BOOL &bHandled );
private:
  HANDLE m_hSignal;  // used to sync between threads (prevents too many messages from piling up)
};

} // namespace tf
} // namespace ou
