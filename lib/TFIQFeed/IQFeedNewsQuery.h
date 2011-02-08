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
// put parsers in separate compilation units to cut down on compile time

#include <string>
#include <vector>
#include <cassert>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

#include <OUCommon/Network.h>


// custom on
// http://msdn.microsoft.com/en-us/library/e5ewb1h3.aspx
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
// custom off


// following uses as a base:
// http://svn.boost.org/svn/boost/trunk/libs/spirit/example/qi/employee.cpp
// http://svn.boost.org/svn/boost/trunk/libs/spirit/example/qi/calc2.cpp

namespace ou { // One Unified
namespace tf { // TradeFrame

template <typename T>
class CIQFeedNewsQuery: public ou::CNetwork<CIQFeedNewsQuery<T> > {
  friend ou::CNetwork<CIQFeedNewsQuery<T> >;
public:

  typedef typename ou::CNetwork<CIQFeedNewsQuery<T> > inherited_t;
  typedef typename inherited_t::linebuffer_t linebuffer_t;

  struct structNCElements {
    std::string ItemName;
    std::string ItemType;
    std::string ItemAuthCode;
    std::string ItemIcon;
  };

  struct structNCCategory {
    structNCElements Elements;
    //std::vector<vNCMajorType_t::size_type> vMajorTypes;
  };
  typedef std::vector<structNCCategory> vNCCategory_t;

  struct structNCMajorType {
    typename vNCCategory_t::size_type ixCategory;
    structNCElements Elements;
    //std::vector<vNCMinorType_t::size_type> vMinorTypes;
    structNCMajorType( typename vNCCategory_t::size_type ixCategory_ ): ixCategory( ixCategory_ ) {};;
  };
  typedef std::vector<structNCMajorType> vNCMajorType_t;

  struct structNCMinorType {
    typename vNCMajorType_t::size_type ixMajorItem;
    structNCElements Elements;
    structNCMinorType( typename vNCMajorType_t::size_type ixMajorItem_ ): ixMajorItem( ixMajorItem_ ) {};;
  }; 
  typedef std::vector<structNCMinorType> vNCMinorType_t;

  CIQFeedNewsQuery( void );
  ~CIQFeedNewsQuery( void );

  void RetrieveStory( const std::string& StoryId, LPARAM lParam );

  vNCCategory_t& GetConfigCategories( void ) { return m_vNCCategory; };
  vNCMajorType_t& GetConfigMajorTypes( void ) { return m_vNCMajorType; };
  vNCMinorType_t& GetConfigMinorTypes( void ) { return m_vNCMinorType; };

protected:

  enum enumRetrieveType {  // activity in progress on this port
    RETRIEVE_IDLE = 0,  // no retrievals in progress
    RETRIEVE_STORY,  // retrieve a story
    RETRIEVE_CONFIG  // retrieve news configuration
  } m_stateRetrieval;

  enum stateStoryRetrieval {
    WAIT4XML,
    WAIT4STORIES,
    WAIT4STORY,
    WAIT4LINK,
    WAIT4TEXT,
    WAIT4ENDTEXT,
    WAIT4ENDSTORY,
    WAIT4SYMBOLS,
    WAIT4ENDSTORIES,
    WAIT4BLANK,
    WAIT4ENDMSG
  } m_stateStoryRetrieval;

  LPARAM m_lParam; // passed back to caller as reference to story, therefore only one story at a time, which is inherent in protocol anyway

  // CRTP based dummy callbacks
  void OnNewsQueryError( size_t ) {};
  void OnNewsQueryConnected( void ) {};
  void OnNewsQueryDisconnected( void ) {};
  void OnNewsQuerySendDone( void ) {};
  void OnNewsQueryStoryLine( linebuffer_t* buf, LPARAM lParam ) {};
  void OnNewsQueryStoryDone( LPARAM lParam ) {};
  void OnNewsQueryNewsConfigDone( void ) {};

  // called by CNetwork via CRTP
  void OnNetworkConnected(void) {
    RetrieveConfiguration();
    if ( &CIQFeedNewsQuery<T>::OnNewsQueryConnected != &T::OnNewsQueryConnected ) {
      static_cast<T*>( this )->OnNewsQueryConnected();
    }
  };
  void OnNetworkDisconnected(void) {
    if ( &CIQFeedNewsQuery<T>::OnNewsQueryDisconnected != &T::OnNewsQueryDisconnected ) {
      static_cast<T*>( this )->OnNewsQueryDisconnected();
    }
  };
  void OnNetworkError( size_t e ) {
    if ( &CIQFeedNewsQuery<T>::OnNewsQueryError != &T::OnNewsQueryError ) {
      static_cast<T*>( this )->OnNewsQueryError(e);
    }
  };
  void OnNetworkSendDone(void) {
    if ( &CIQFeedNewsQuery<T>::OnNewsQuerySendDone != &T::OnNewsQuerySendDone ) {
      static_cast<T*>( this )->OnNewsQuerySendDone();
    }
  };
  void OnNetworkLineBuffer( linebuffer_t* );  // new line available for processing

  // internal use only to obtain news type configurations
  void RetrieveConfiguration( void );

  // internal procedures for each retrieveal state
  void ProcessStoryRetrieval(  linebuffer_t* buf );
  void ProcessConfigurationRetrieval(  linebuffer_t* buf );

private:

  typedef stateStoryRetrieval ruleid_t;
  typedef typename inherited_t::linebuffer_t linebuffer_t;
  typedef typename inherited_t::linebuffer_t::const_iterator iterator_t;

  template <typename Iterator>
  struct structStoryXmlKeywords: qi::grammar<Iterator, ruleid_t()> {
    structStoryXmlKeywords(): structStoryXmlKeywords::base_type(start) {
      start = qi::eps[qi::_val=WAIT4TEXT]  // default value if nothing else found
        >> *qi::space  // eat up any leading spaces
        >> (  // any one of a set of line markers:
            qi::lit("<?xml version='1.0'?>")[qi::_val=WAIT4XML]
          | qi::lit("<news_stories>")       [qi::_val=WAIT4STORIES]
          | qi::lit("<news_story>")         [qi::_val=WAIT4STORY]
          | qi::lit("<is_link>")            [qi::_val=WAIT4LINK]
          | qi::lit("<story_text>")         [qi::_val=WAIT4TEXT]
          | qi::lit("</story_text>")        [qi::_val=WAIT4ENDTEXT]
          | qi::lit("</news_story>")        [qi::_val=WAIT4ENDSTORY]
          | qi::lit("</news_stories>")      [qi::_val=WAIT4ENDSTORIES]
          | qi::lit("<symbols>")            [qi::_val=WAIT4SYMBOLS]
          | qi::lit("!ENDMSG!")             [qi::_val=WAIT4ENDMSG]
          | ( ( +( qi::char_ >> !qi::lit("</story_text>") ) ) >> qi::char_ >> qi::lit("</story_text>") )
                                            [qi::_val=WAIT4ENDTEXT]
          )
        ;
    }
    qi::rule<Iterator, ruleid_t()> start;
  };

  structStoryXmlKeywords<iterator_t> m_grammarStoryKeywords;

  structNCElements *m_pElements;
  vNCCategory_t m_vNCCategory;
  vNCMajorType_t m_vNCMajorType;
  vNCMinorType_t m_vNCMinorType;

  qi::rule<iterator_t> ruleItemEqual, ruleItemQuote;
  qi::rule<iterator_t, std::string()> ruleItemQuoted;
  qi::rule<iterator_t> ruleElements;
  qi::rule<iterator_t, std::string()> ruleItemType, ruleItemName, ruleItemIcon, ruleItemAuthCode;
  qi::rule<iterator_t> ruleNewsConfigKeyword;  

  inline void DoCategoryStart( void ) {
    structNCCategory Category;
    m_vNCCategory.push_back( Category );
    m_pElements = &m_vNCCategory.back().Elements;
  };
  inline void DoCategoryStop( void ) {};
  inline void DoMajorTypeStart( void ) {
    structNCMajorType MajorType( m_vNCCategory.size()-1 );
    m_vNCMajorType.push_back( MajorType );
    m_pElements = &m_vNCMajorType.back().Elements;
  };
  inline void DoMajorTypeStop( void ) {};
  inline void DoMinorTypeStart( void ) {
    structNCMinorType MinorType( m_vNCMajorType.size()-1 );
    m_vNCMinorType.push_back( MinorType );
    m_pElements = &m_vNCMinorType.back().Elements;
  };
  inline void DoMinorStop( void ) {};
  inline void DoEndMsg( void ) {
    m_stateRetrieval = RETRIEVE_IDLE;
    if ( &CIQFeedNewsQuery<T>::OnNewsQueryNewsConfigDone != &T::OnNewsQueryNewsConfigDone ) {
      static_cast<T*>( this )->OnNewsQueryNewsConfigDone();
    }
  };

  inline void DoItemType( const std::string& str ) {
    m_pElements->ItemType = str;
  };
  inline void DoItemName( const std::string& str ) {
    m_pElements->ItemName = str;
  };
  inline void DoItemIcon( const std::string& str ) {
    m_pElements->ItemIcon = str;
  };
  inline void DoItemAuthCode( const std::string& str ) {
    m_pElements->ItemAuthCode = str;
  };

};

template <typename T>
CIQFeedNewsQuery<T>::CIQFeedNewsQuery( void ) 
: CNetwork<CIQFeedNewsQuery<T> >( "127.0.0.1", 9100 ),
  m_stateRetrieval( RETRIEVE_IDLE ), 
  m_lParam( 0 )
{

  // other examples:
  //  http://svn.boost.org/svn/boost/trunk/libs/spirit/example/qi/
  // trunk documentation:
  //  http://svn.boost.org/svn/boost/trunk/libs/spirit/doc/html/index.html

  ruleItemQuote = qi::char_('"');

  ruleItemQuoted =
    ruleItemQuote >> qi::lexeme[*(qi::char_ - qi::char_('"'))] >> ruleItemQuote;

  ruleItemEqual =
    *qi::space >> qi::char_('=') >> *qi::space;

  ruleItemType =
    qi::lit("type") >> ruleItemEqual >> ruleItemQuoted[boost::phoenix::bind(&CIQFeedNewsQuery<T>::DoItemType, this, qi::_1)];

  ruleItemName =
    qi::lit("name") >> ruleItemEqual >> ruleItemQuoted[boost::phoenix::bind(&CIQFeedNewsQuery<T>::DoItemName, this, qi::_1)];

  ruleItemIcon =
    qi::lit("icon_id" ) >> ruleItemEqual >> ruleItemQuoted[boost::phoenix::bind(&CIQFeedNewsQuery<T>::DoItemIcon, this, qi::_1)];

  ruleItemAuthCode = 
    qi::lit("auth_code") >> ruleItemEqual >> ruleItemQuoted[boost::phoenix::bind(&CIQFeedNewsQuery<T>::DoItemAuthCode, this, qi::_1)];

  ruleElements =
    ruleItemType >> +qi::space >> ruleItemName >> +qi::space >> ruleItemAuthCode >> +qi::space >> ruleItemIcon;

  ruleNewsConfigKeyword = 
    *qi::space
    >> ( // any one of a set of line styles:
    qi::lit("<?xml version='1.0'?>")
    |   qi::lit("<DynamicNewsConf>")    
    | ( qi::lit("<category")            [boost::phoenix::bind(&CIQFeedNewsQuery<T>::DoCategoryStart, this)] 
          >> +qi::space >> ruleItemName >> *qi::space >> qi::char_('>') )
    |   qi::lit("</category>")          [boost::phoenix::bind(&CIQFeedNewsQuery<T>::DoCategoryStop, this)]
    | ( qi::lit("<major_type")          [boost::phoenix::bind(&CIQFeedNewsQuery<T>::DoMajorTypeStart, this)] 
          >> +qi::space >> ruleElements >> *qi::space >> qi::char_('>') )
    |   qi::lit("</major_type>")        [boost::phoenix::bind(&CIQFeedNewsQuery<T>::DoMajorTypeStop, this)]
    | ( qi::lit("<minor_type")          [boost::phoenix::bind(&CIQFeedNewsQuery<T>::DoMinorTypeStart, this)] 
          >> +qi::space >> ruleElements >> *qi::space >> qi::lit("/>") )
    |   qi::lit("</DynamicNewsConf>") 
    |   qi::lit("!ENDMSG!")             [boost::phoenix::bind(&CIQFeedNewsQuery<T>::DoEndMsg, this)]
  );
}

template <typename T>
CIQFeedNewsQuery<T>::~CIQFeedNewsQuery() {
}

template <typename T>
//LRESULT CIQFeedNewsQuery<T>::OnConnProcess( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {
void CIQFeedNewsQuery<T>::OnNetworkLineBuffer( linebuffer_t* buf ) {

#if defined _DEBUG
  {
    linebuffer_t::const_iterator bgn = (*buf).begin();
    linebuffer_t::const_iterator end = (*buf).end();

    std::string str( bgn, end );
    str += "\n";
    OutputDebugString( str.c_str() );
  }
#endif

  switch ( m_stateRetrieval ) {
    case RETRIEVE_STORY:
      ProcessStoryRetrieval( buf );
      //      ReturnLineBuffer( buf ); // is handled specially inside method
      break;
    case RETRIEVE_CONFIG:
      ProcessConfigurationRetrieval( buf );
      GiveBackBuffer( buf );
      break;
    case RETRIEVE_IDLE:
      OutputDebugString( "Unknown CIQFeedNewsQuery<T>::OnConnProcess\n" );
      GiveBackBuffer( buf );
      break;
  }
}

template <typename T>
void CIQFeedNewsQuery<T>::RetrieveStory( const std::string& StoryId, LPARAM lParam ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedNewsQuery<T>::RetrieveStory: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_STORY;
    m_stateStoryRetrieval = WAIT4XML;
    m_lParam = lParam;
    std::string ss;
    ss = "NN:" + StoryId + ";";
    Send( ss );
  }
}

template <typename T>
void CIQFeedNewsQuery<T>::ProcessStoryRetrieval( linebuffer_t* buf ) {

  linebuffer_t::const_iterator bgn = (*buf).begin();
  linebuffer_t::const_iterator end = (*buf).end();

  bool bReturnTheBuffer = true;

  ruleid_t id;  // id of line marker found during parse
  bool b = parse( bgn, end, m_grammarStoryKeywords, id );

  // do his with custom iterators which pause when no more input available due to new line

  switch ( m_stateStoryRetrieval ) {  //match where we are with what we are expecting
    case WAIT4TEXT:  // this one is encountered most often so first in switch statement
      if ( WAIT4ENDTEXT == id ) {
        m_stateStoryRetrieval = WAIT4ENDSTORY;
      }
      else {
        if ( id != m_stateStoryRetrieval ) {
          throw std::logic_error( "WAIT4TEXT");
        }
        else {  // anything else on the line?
          if ( bgn == end ) {
            // nothing else to process
          }
          else { // erase the keyword, if any, and send buffer onwards
            buf->erase( buf->begin(), bgn );  
            // emit line
            if ( &CIQFeedNewsQuery<T>::OnNewsQueryStoryLine != &T::OnNewsQueryStoryLine ) {
              static_cast<T*>( this )->OnNewsQueryStoryLine( buf, m_lParam );
              bReturnTheBuffer = false;  // message handler needs to return the buffer once it's processing is complete
            }
          }
        }
      }
      break;
    case WAIT4XML:
      if ( id != m_stateStoryRetrieval ) {
        throw std::logic_error( "WAIT4XML");
      }
      else {
        m_stateStoryRetrieval = WAIT4STORIES;
      }
      break;
    case WAIT4STORIES:
      if ( id != m_stateStoryRetrieval ) {
        throw std::logic_error( "WAIT4STORIES");
      }
      else {
        m_stateStoryRetrieval = WAIT4STORY;
      }
      break;
    case WAIT4STORY:
      switch ( id ) {
        case WAIT4STORY:
          m_stateStoryRetrieval = WAIT4LINK;
          break;
        case WAIT4ENDSTORIES:  // copied from below
          m_stateStoryRetrieval = WAIT4BLANK;
          break;
        default:
          throw std::logic_error( "WAIT4STORY");
      }
      break;
    case WAIT4LINK:
      if ( id != m_stateStoryRetrieval ) {
        throw std::logic_error( "WAIT4LINK");
      }
      else {
        m_stateStoryRetrieval = WAIT4TEXT;
      }
      break;
    case WAIT4ENDSTORY:
      switch ( id ) {
        case WAIT4ENDSTORY:
          m_stateStoryRetrieval = WAIT4ENDSTORIES;
          break;
        case WAIT4SYMBOLS:
          m_stateStoryRetrieval = WAIT4ENDSTORY;
          break;
        default:
          throw std::logic_error( "WAIT4ENDSTORY");
          break;
          }
      break;
    case WAIT4ENDSTORIES:
      if ( id != m_stateStoryRetrieval ) {
        throw std::logic_error( "WAIT4ENDSTORIES");
      }
      else {
        m_stateStoryRetrieval = WAIT4BLANK;
      }
      break;
    case WAIT4BLANK:
      if ( bgn != end ) {
        throw std::logic_error( "WAIT4BLANK");
      }
      else {
        m_stateStoryRetrieval = WAIT4ENDMSG;
      }
      break;
    case WAIT4ENDMSG:
      if ( id != m_stateStoryRetrieval ) {
        throw std::logic_error( "WAIT4ENDMSG");
      }
      else {
        // send end marker
        m_stateRetrieval = RETRIEVE_IDLE;
        if ( &CIQFeedNewsQuery<T>::OnNewsQueryStoryDone != &T::OnNewsQueryStoryDone ) {
          static_cast<T*>( this )->OnNewsQueryStoryDone( m_lParam );
        }
      }
      break;
  }
  if ( bReturnTheBuffer ) GiveBackBuffer( buf );

}

template <typename T>
void CIQFeedNewsQuery<T>::RetrieveConfiguration( void ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedNewsQuery<T>::RetrieveConfiguration: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_CONFIG;
    std::string ss;
    ss = "NC;";
    Send( ss );
  }
}

template <typename T>
void CIQFeedNewsQuery<T>::ProcessConfigurationRetrieval( linebuffer_t* buf ) {

  linebuffer_t::const_iterator bgn = (*buf).begin();
  linebuffer_t::const_iterator end = (*buf).end();

  bool b = parse( bgn, end, ruleNewsConfigKeyword);

}

} // namespace tf
} // namespace ou
