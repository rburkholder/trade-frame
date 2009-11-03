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

#include <string>

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

  void RetrieveStory( const std::string& StoryId, UINT MsgIdStoryLine, UINT MsgIdStoryDone  );

protected:

  enum enumPrivateMessageTypes { // messages from CNetwork
    WM_NQ_DONE = inherited_t::WM_NCS_ENDMARKER
  };

  BEGIN_MSG_MAP_EX(CIQFeedNewsQuery<T>)
    CHAIN_MSG_MAP(inherited_t)
  END_MSG_MAP()

  LRESULT OnConnProcess( UINT, WPARAM, LPARAM, BOOL &bHandled );

  enum stateCommand {
    IDLE,
    WAIT4XML,
    WAIT4STORIES,
    WAIT4STORY,
    WAIT4LINK,
    WAIT4TEXT,
    WAIT4ENDTEXT,
    WAIT4ENDSTORY,
    WAIT4ENDSTORIES,
    WAIT4BLANK,
    WAIT4ENDMSG
  } m_stateCommand;

  UINT m_MsgIdStoryLine;
  UINT m_MsgIdStoryDone;

private:

  CAppModule* m_pModule;
  CBufferRepository<std::string> m_reposStoryTextLines;

};

template <typename T>
CIQFeedNewsQuery<T>::CIQFeedNewsQuery(WTL::CAppModule *pModule) 
: CNetworkClientSkeleton<CIQFeedNewsQuery<T> >( pModule, "127.0.0.1", 9100 ),
  m_pModule( pModule ),
  m_stateCommand( IDLE )
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

  std::string* pLine = m_reposStoryTextLines.CheckOutL();
  pLine->assign( iter, end );

#if defined _DEBUG
  std::string str( *pLine );
  str += '\n';
  OutputDebugString( str.c_str() );
#endif

// do a boost parser for all this with custom iterators which pause when no more input available due to new line

  std::string::size_type ix;
  switch (m_stateCommand ) {
    case IDLE:
      break;
    case WAIT4XML:
      if ( std::string::npos == ( pLine->find( "<?xml version='1.0'?>") ) ) {
        throw std::logic_error( "WAIT4XML");
      }
      else {
        m_stateCommand = WAIT4STORIES;
      }
      break;
    case WAIT4STORIES:
      if ( std::string::npos == ( pLine->find( "<news_stories>") ) ) {
        throw std::logic_error( "WAIT4STORIES");
      }
      else {
        m_stateCommand = WAIT4STORY;
      }
      break;
    case WAIT4STORY:
      if ( std::string::npos == ( pLine->find( "<news_story>") ) ) {
        throw std::logic_error( "WAIT4STORY");
      }
      else {
        m_stateCommand = WAIT4LINK;
      }
      break;
    case WAIT4LINK:
      if ( std::string::npos == ( ix = pLine->find( "<is_link>") ) ) {
        throw std::logic_error( "WAIT4LINK");
      }
      else {
        m_stateCommand = WAIT4TEXT;
      }
      break;
    case WAIT4TEXT:
      if ( std::string::npos == ( ix = pLine->find( "<story_text>") ) ) {
        throw std::logic_error( "WAIT4TEXT");
      }
      else {
        if ( pLine->size() > ( ix + 11 ) ) {
          // emit line
        }
        m_stateCommand = WAIT4ENDTEXT;
      }
      break;
    case WAIT4ENDTEXT:
      if ( std::string::npos == ( pLine->find( "</story_text>") ) ) {
        // emit line
      }
      else {
        m_stateCommand = WAIT4ENDSTORY;
      }
      break;
    case WAIT4ENDSTORY:
      if ( std::string::npos == ( pLine->find( "</news_story>") ) ) {
        throw std::logic_error( "WAIT4ENDSTORY");
      }
      else {
        m_stateCommand = WAIT4ENDSTORIES;
      }
      break;
    case WAIT4ENDSTORIES:
      if ( std::string::npos == ( pLine->find( "</news_stories>") ) ) {
        throw std::logic_error( "WAIT4ENDSTORIES");
      }
      else {
        m_stateCommand = WAIT4BLANK;
      }
      break;
    case WAIT4BLANK:
      if ( 0 != pLine->size() ) {
        throw std::logic_error( "WAIT4BLANK");
      }
      else {
        m_stateCommand = WAIT4ENDMSG;
      }
      break;
    case WAIT4ENDMSG:
      if ( std::string::npos == ( ix = pLine->find( "!ENDMSG!") ) ) {
        throw std::logic_error( "WAIT4ENDMSG");
      }
      else {
        // send end marker
        m_stateCommand = IDLE;
      }
      break;
  }

  PostProcessedMessage( wParam );

  bHandled = true;
  return 1;
}

template <typename T>
void CIQFeedNewsQuery<T>::RetrieveStory( const std::string& StoryId, UINT MsgIdStoryLine, UINT MsgIdStoryDone ) {
  if ( IDLE != m_stateCommand ) {
    throw std::logic_error( "CIQFeedNewsQuery<T>::RetrieveStory: bad state");
  }
  else {
    m_stateCommand = WAIT4XML;
    m_MsgIdStoryLine = MsgIdStoryLine;
    m_MsgIdStoryDone = MsgIdStoryDone;
    std::string ss;
    ss = "NN:" + StoryId + ";";
    Send( ss );
  }
}