/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

// accepts headlines
// matches up story text, calculates hashes to reduce redundant stories
// performs some sentiment analysis
// handles refresh of 'sources' list

#include <LibWtlCommon/NetworkClientSkeleton.h>

#include <LibCommon/ReusableBuffers.h>

// custom on
// http://msdn.microsoft.com/en-us/library/e5ewb1h3.aspx
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// custom off

template <typename T>
class CIQFeedNewsQuery: public CNetworkClientSkeleton<CIQFeedNewsQuery<T> > {
public:

  typedef typename CNetworkClientSkeleton<CIQFeedNewsQuery<T> > inherited_t;

  CIQFeedNewsQuery(CAppModule* pModule);
  ~CIQFeedNewsQuery(void );

protected:

  enum enumPrivateMessageTypes { // messages from CNetwork
    WM_NQ_DONE = inherited_t::WM_NCS_ENDMARKER
  };

  BEGIN_MSG_MAP_EX(CIQFeedNewsQuery<T>)
    CHAIN_MSG_MAP(inherited_t)
  END_MSG_MAP()

  LRESULT OnConnProcess( UINT, WPARAM, LPARAM, BOOL &bHandled );

private:

  CAppModule* m_pModule;

};

template <typename T>
CIQFeedNewsQuery<T>::CIQFeedNewsQuery(WTL::CAppModule *pModule) 
: CNetworkClientSkeleton<CIQFeedNewsQuery<T> >( pModule, "127.0.0.1", 9100 ),
  m_pModule( pModule )
{
}

template <typename T>
CIQFeedNewsQuery<T>::~CIQFeedNewsQuery() {
}

template <typename T>
LRESULT CIQFeedNewsQuery<T>::OnConnProcess( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  inherited_t::linebuffer_t* buf = reinterpret_cast<inherited_t::linebuffer_t*>( wParam );
  inherited_t::linebuffer_t::iterator iter = (*buf).begin();
  inherited_t::linebuffer_t::iterator end = (*buf).end();

#if defined _DEBUG
  std::string str( iter, end );
  str += '\n';
  OutputDebugString( str.c_str() );
#endif

//  BOOST_ASSERT( iter != end );  // can't use this as there are blank lines

  // code here

  PostProcessedMessage( wParam );

  bHandled = true;
  return 1;
}

