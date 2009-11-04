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

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;


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
    WAIT4ENDMSG,
    STORYLINE
  } m_stateCommand;

  UINT m_MsgIdStoryLine;
  UINT m_MsgIdStoryDone;

private:

  CAppModule* m_pModule;
  //CBufferRepository<std::string> m_reposStoryTextLines;

  typedef stateCommand ruleid_t;

  template <typename Iterator>
  struct structStoryXmlKeywords: qi::grammar<Iterator, ruleid_t()> {
    structStoryXmlKeywords(): structStoryXmlKeywords::base_type(start) {
      start = qi::eps[qi::_val=STORYLINE] 
        >> *qi::space
        >> (
            qi::lit("<?xml version='1.0'?>")[qi::_val=WAIT4XML]
          | qi::lit("<news_stories>")       [qi::_val=WAIT4STORIES]
          | qi::lit("<news_story>")         [qi::_val=WAIT4STORY]
          | qi::lit("<is_link>")            [qi::_val=WAIT4LINK]
          | qi::lit("<story_text>")         [qi::_val=WAIT4TEXT]
          | qi::lit("</story_text>")        [qi::_val=WAIT4ENDTEXT]
          | qi::lit("</news_story>")        [qi::_val=WAIT4ENDSTORY]
          | qi::lit("</news_stories>")      [qi::_val=WAIT4ENDSTORIES]
          | qi::lit("!ENDMSG!")             [qi::_val=WAIT4ENDMSG]
          )
        ;
    }
    qi::rule<Iterator, ruleid_t()> start;
  };

  typedef typename inherited_t::linebuffer_t::const_iterator iterator_t;
  structStoryXmlKeywords<iterator_t> m_grammarStoryKeywords;

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
  inherited_t::linebuffer_t::const_iterator bgn = (*buf).begin();
  inherited_t::linebuffer_t::const_iterator end = (*buf).end();

#if defined _DEBUG
  {
    std::string str( bgn, end );
    str += '\n';
    OutputDebugString( str.c_str() );
    bgn = (*buf).begin();  // restore the interator for next use
  }
#endif

// do a boost parser for all this with custom iterators which pause when no more input available due to new line

  ruleid_t id;
  bool b = parse( bgn, end, m_grammarStoryKeywords, id );

  switch (m_stateCommand ) {
    case IDLE:
      break;
    case WAIT4XML:
      if ( id != m_stateCommand ) {
        throw std::logic_error( "WAIT4XML");
      }
      else {
        m_stateCommand = WAIT4STORIES;
        PostProcessedMessage( wParam );
      }
      break;
    case WAIT4STORIES:
      if ( id != m_stateCommand ) {
        throw std::logic_error( "WAIT4STORIES");
      }
      else {
        m_stateCommand = WAIT4STORY;
        PostProcessedMessage( wParam );
      }
      break;
    case WAIT4STORY:
      if ( id != m_stateCommand ) {
        throw std::logic_error( "WAIT4STORY");
      }
      else {
        m_stateCommand = WAIT4LINK;
        PostProcessedMessage( wParam );
      }
      break;
    case WAIT4LINK:
      if ( id != m_stateCommand ) {
        throw std::logic_error( "WAIT4LINK");
      }
      else {
        m_stateCommand = WAIT4TEXT;
        PostProcessedMessage( wParam );
      }
      break;
    case WAIT4TEXT:
      if ( WAIT4ENDTEXT == id ) {
        m_stateCommand = WAIT4ENDSTORY;
        PostProcessedMessage( wParam );
      }
      else {
        if ( id != m_stateCommand ) {
          throw std::logic_error( "WAIT4TEXT");
        }
        else {  // anything else on the line?
          if ( bgn == end ) {
            // nothing else to process
            PostProcessedMessage( wParam );
          }
          else {
            buf->erase( buf->begin(), bgn );  // erase the keyword and send buffer onwards
            // emit line
          }
        }
      }
      break;
    case WAIT4ENDSTORY:
      if ( id != m_stateCommand ) {
        throw std::logic_error( "WAIT4ENDSTORY");
      }
      else {
        m_stateCommand = WAIT4ENDSTORIES;
        PostProcessedMessage( wParam );
      }
      break;
    case WAIT4ENDSTORIES:
      if ( id != m_stateCommand ) {
        throw std::logic_error( "WAIT4ENDSTORIES");
      }
      else {
        m_stateCommand = WAIT4BLANK;
        PostProcessedMessage( wParam );
      }
      break;
    case WAIT4BLANK:
      if ( bgn != end ) {
        throw std::logic_error( "WAIT4BLANK");
      }
      else {
        m_stateCommand = WAIT4ENDMSG;
        PostProcessedMessage( wParam );
      }
      break;
    case WAIT4ENDMSG:
      if ( id != m_stateCommand ) {
        throw std::logic_error( "WAIT4ENDMSG");
      }
      else {
        // send end marker
        m_stateCommand = IDLE;
        PostProcessedMessage( wParam );
      }
      break;
  }

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