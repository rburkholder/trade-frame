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
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

// custom on
// http://msdn.microsoft.com/en-us/library/e5ewb1h3.aspx
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// custom off


// following uses as a base:
// http://svn.boost.org/svn/boost/trunk/libs/spirit/example/qi/employee.cpp
// http://svn.boost.org/svn/boost/trunk/libs/spirit/example/qi/calc2.cpp


enum enumNewsConfigLineTypes {
  NC_NOCATEGORY = 0,  // illegal value
  NC_FILLER,
  NC_CATEGORY,
  NC_CATEGORY_DONE,
  NC_MAJORTYPE,
  NC_MAJORTYPE_DONE,
  NC_MINORTYPE,
  NC_DONE
};

struct structNewsConfigInfo {
//  enumNewsConfigLineTypes ncinfo;
  std::string ItemType;
  std::string ItemName;
  std::string ItemAuthCode;
  std::string ItemIcon;
};

BOOST_FUSION_ADAPT_STRUCT(
  structNewsConfigInfo,
//  (enumNewsConfigLineTypes, ncinfo)
  (std::string, ItemType)
  (std::string, ItemName)
  (std::string, ItemAuthCode)
  (std::string, ItemIcon)
)

typedef structNewsConfigInfo nc_rule_t;

template <typename Iterator>
struct structNewsConfiguration: qi::grammar<Iterator, structNewsConfigInfo()> {

  structNewsConfiguration(): structNewsConfiguration::base_type(start) {

    item_quoted =
      qi::lexeme[qi::char_('"') >> *(qi::char_ - qi::char_('"')) >> qi::char_('"')/*[qi::_val = qi::_1]*/];

    item_equal =
      *qi::space >> qi::char_('=') >> *qi::space;

    item_type =
      //        qi::lit("type") >> item_equal >> item_quoted[&CIQFeedNewsQuery<T>::DoItemType];
      qi::lit("type") >> item_equal >> item_quoted;

    item_name =
      //        qi::lit("name") >> item_equal >> item_quoted[&CIQFeedNewsQuery<T>::DoItemName];
      qi::lit("name") >> item_equal >> item_quoted;

    item_icon =
      //        qi::lit("icon_id" ) >> item_equal >> item_quoted[&CIQFeedNewsQuery<T>::DoItemIcon];
      qi::lit("icon_id" ) >> item_equal >> item_quoted;

    item_authcode = 
      //        qi::lit("auth_code") >> item_equal >> item_quoted[&CIQFeedNewsQuery<T>::DoItemAuthCode];
      qi::lit("auth_code") >> item_equal >> item_quoted;

    item_elements =
      item_type >> +qi::space >> item_name >> +qi::space >> item_authcode >> +qi::space >> item_icon;

//    start = qi::eps[qi::_val=NC_NOCATEGORY] // default value if nothing found, which is an error
        start = 
     *qi::space
      >> ( // any one of a set of line styles:
      
      qi::lit("<?xml version='1.0'?>")
    |   qi::lit("<DynamicNewsConf>")    
    | ( qi::lit("<category")             >> +qi::space >> item_name >> *qi::space >> qi::char_('>') )
      |   qi::lit("</category>")          
    | ( qi::lit("<major_type")           >> +qi::space >> item_elements >> *qi::space >> qi::char_('>') )
      |   qi::lit("</major_type>")        
    | ( qi::lit("<minor_type")           >> +qi::space >> item_elements >> *qi::space >> qi::lit("/>") )
      |   qi::lit("</DynamicNewsConf>")   
    |   qi::lit("!ENDMSG!")            
/*
    qi::lit("<?xml version='1.0'?>")[qi::_val=NC_FILLER]
    |   qi::lit("<DynamicNewsConf>")    [qi::_val=NC_FILLER]
    | ( qi::lit("<category")            [qi::_val=NC_CATEGORY] >> +qi::space >> item_name >> *qi::space >> qi::char_('>') )
      |   qi::lit("</category>")          [qi::_val=NC_CATEGORY_DONE]
    | ( qi::lit("<major_type")          [qi::_val=NC_MAJORTYPE] >> +qi::space >> item_elements >> *qi::space >> qi::char_('>') )
      |   qi::lit("</major_type>")        [qi::_val=NC_MAJORTYPE_DONE]
    | ( qi::lit("<minor_type")          [qi::_val=NC_MINORTYPE] >> +qi::space >> item_elements >> *qi::space >> qi::lit("/>") )
      |   qi::lit("</DynamicNewsConf>")   [qi::_val=NC_FILLER]
    |   qi::lit("!ENDMSG!")             [qi::_val=NC_DONE]
*/
    )
      ;
  }

  qi::rule<Iterator> item_elements, item_equal;
  qi::rule<Iterator, std::string()> item_quoted;
  qi::rule<Iterator, std::string()> item_type, item_name, item_icon, item_authcode;
  qi::rule<Iterator, structNewsConfigInfo()> start;
};




template <typename T>
class CIQFeedNewsQuery: public CNetworkClientSkeleton<CIQFeedNewsQuery<T> > {
public:

  typedef typename CNetworkClientSkeleton<CIQFeedNewsQuery<T> > inherited_t;

  struct structMessageDestinations {
    T* owner;
    UINT msgConnected;
    UINT msgSendComplete;
    UINT msgDisconnected;
    UINT msgError;  // not currently forwarded
    structMessageDestinations( void )
      : owner( NULL ), msgConnected( 0 ), msgSendComplete( 0 ), msgDisconnected( 0 ), msgError( 0 )
    {};
    structMessageDestinations( T* owner_, UINT msgConnected_, UINT msgSendComplete_, UINT msgDisconnected_, UINT msgError_ ) 
      : owner( owner_ ), msgConnected( msgConnected_ ), msgSendComplete( msgSendComplete_ ), msgDisconnected( msgDisconnected_ ), msgError( msgError_ )
    {
      BOOST_ASSERT( NULL != owner_ );
    };
  };

  CIQFeedNewsQuery(CAppModule* pModule, const structMessageDestinations& MessageDestinations);
  ~CIQFeedNewsQuery(void );

  void RetrieveStory( const std::string& StoryId, T* owner, LPARAM lParam, UINT MsgIdStoryLine, UINT MsgIdStoryDone );

protected:

  enum enumPrivateMessageTypes { // messages from CNetwork
    WM_NQ_DONE = inherited_t::WM_NCS_ENDMARKER
  };

  BEGIN_MSG_MAP_EX(CIQFeedNewsQuery<T>)
    CHAIN_MSG_MAP(inherited_t)
  END_MSG_MAP()

  enum enumRetrieveType {
    RETRIEVE_IDLE = 0,  // no retrievals in progress
    RETRIEVE_STORY,  // retrieve a story
    RETRIEVE_CONFIG  // retrieve news configuration
  } m_stateRetrieval;

  enum stateStoryRetrieval {
//    IDLE,
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

  UINT m_MsgIdStoryLine;
  UINT m_MsgIdStoryDone;
  T* m_owner;
  LPARAM m_lParam; // passed back to caller as reference to story

  // overloads from CNetworkClientSkeleton
  LRESULT OnConnConnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnDisconnected( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnProcess( UINT, WPARAM, LPARAM, BOOL &bHandled );
  LRESULT OnConnSendDone( UINT, WPARAM, LPARAM, BOOL &bHandled );

  void RetrieveConfiguration( void );

  void ProcessStoryRetrieval(  linebuffer_t* buf, WPARAM wParam );
  void ProcessConfigurationRetrieval(  linebuffer_t* buf );



private:

  CAppModule* m_pModule;
  structMessageDestinations m_structMessageDestinations;

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
          )
        ;
    }
    qi::rule<Iterator, ruleid_t()> start;
  };

  structStoryXmlKeywords<iterator_t> m_grammarStoryKeywords;


  structNewsConfiguration<iterator_t> m_grammarDecodeNewsConfiguration;

  qi::rule<iterator_t> ruleItemEqual;
  qi::rule<iterator_t, std::string()> ruleItemQuoted;
  qi::rule<iterator_t> ruleElements;
  qi::rule<iterator_t, std::string()> ruleItemType, ruleItemName, ruleItemIcon, ruleItemAuthCode;
  //qi::rule<iterator_t, structNewsConfigInfo()> start;
  qi::rule<iterator_t> ruleQueryKeyword;  

  inline void DoCategoryStart( void ) {};
  inline void DoCategoryStop( void ) {};
  inline void DoMajorTypeStart( void ) {};
  inline void DoMajorTypeStop( void ) {};
  inline void DoMinorType( void ) {};
  inline void DoEndMsg( void ) {};

  inline void DoItemType( const std::string& s ) {};
  inline void DoItemName( const std::string& s ) {};
  inline void DoItemIcon( const std::string& s ) {};
  inline void DoItemAuthCode( const std::string& s ) {};

};

template <typename T>
CIQFeedNewsQuery<T>::CIQFeedNewsQuery(WTL::CAppModule *pModule, const structMessageDestinations& MessageDestinations) 
: CNetworkClientSkeleton<CIQFeedNewsQuery<T> >( pModule, "127.0.0.1", 9100 ),
  m_structMessageDestinations( MessageDestinations ),
  m_pModule( pModule ),
  m_stateRetrieval( RETRIEVE_IDLE )
{
  ruleQueryKeyword = 
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
    | ( qi::lit("<minor_type")          [boost::phoenix::bind(&CIQFeedNewsQuery<T>::DoMinorType, this)] 
                                               >> +qi::space >> ruleElements >> *qi::space >> qi::lit("/>") )
    |   qi::lit("</DynamicNewsConf>") 
    |   qi::lit("!ENDMSG!")             [boost::phoenix::bind(&CIQFeedNewsQuery<T>::DoEndMsg, this)]
    );

    ruleItemQuoted =
      qi::lexeme[qi::char_('"') >> *(qi::char_ - qi::char_('"')) >> qi::char_('"')/*[qi::_val = qi::_1]*/];

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
}

template <typename T>
CIQFeedNewsQuery<T>::~CIQFeedNewsQuery() {
}

template <typename T>
LRESULT CIQFeedNewsQuery<T>::OnConnConnected( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgConnected );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CIQFeedNewsQuery<T>::OnConnDisconnected( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgDisconnected );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CIQFeedNewsQuery<T>::OnConnSendDone( UINT, WPARAM wParam, LPARAM lParam, BOOL &bHandled ) {

  m_structMessageDestinations.owner->PostMessage( m_structMessageDestinations.msgSendComplete );

  bHandled = true;
  return 1;
}

template <typename T>
LRESULT CIQFeedNewsQuery<T>::OnConnProcess( UINT, WPARAM wParam, LPARAM, BOOL &bHandled ) {

  linebuffer_t* buf = reinterpret_cast<linebuffer_t*>( wParam );

#if defined _DEBUG
  {
    linebuffer_t::const_iterator bgn = (*buf).begin();
    linebuffer_t::const_iterator end = (*buf).end();

    std::string str( bgn, end );
    str += '\n';
    OutputDebugString( str.c_str() );
//    bgn = (*buf).begin();  // restore the interator for next use
  }
#endif

  switch ( m_stateRetrieval ) {
    case RETRIEVE_STORY:
      ProcessStoryRetrieval( buf, wParam );
      break;
    case RETRIEVE_CONFIG:
      ProcessConfigurationRetrieval( buf );
      break;
    case RETRIEVE_IDLE:
      OutputDebugString( "Unknown CIQFeedNewsQuery<T>::OnConnProcess\n" );
      break;
  }

  PostProcessedMessage( wParam );

  bHandled = true;
  return 1;
}

template <typename T>
void CIQFeedNewsQuery<T>::RetrieveStory( const std::string& StoryId, T* owner, LPARAM lParam, UINT MsgIdStoryLine, UINT MsgIdStoryDone ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedNewsQuery<T>::RetrieveStory: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_STORY;
    m_stateStoryRetrieval = WAIT4XML;
    m_owner = owner;
    m_MsgIdStoryLine = MsgIdStoryLine;
    m_MsgIdStoryDone = MsgIdStoryDone;
    m_lParam = lParam;
    std::string ss;
    ss = "NN:" + StoryId + ";";
    Send( ss );
  }
}

template <typename T>
void CIQFeedNewsQuery<T>::ProcessStoryRetrieval( linebuffer_t* buf, WPARAM wParam ) {

  linebuffer_t::const_iterator bgn = (*buf).begin();
  linebuffer_t::const_iterator end = (*buf).end();

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
            if ( 0 != m_MsgIdStoryLine ) m_owner->PostMessage( m_MsgIdStoryLine, wParam, m_lParam );
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
      if ( id != m_stateStoryRetrieval ) {
        throw std::logic_error( "WAIT4STORY");
      }
      else {
        m_stateStoryRetrieval = WAIT4LINK;
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
        //stateStoryRetrieval = IDLE;
        m_stateRetrieval = RETRIEVE_IDLE;
        if ( 0 != m_MsgIdStoryDone ) m_owner->PostMessage( m_MsgIdStoryDone, NULL, m_lParam );
      }
      break;
//    case IDLE:
//      break;
  }

}

template <typename T>
void CIQFeedNewsQuery<T>::RetrieveConfiguration( void ) {
  if ( RETRIEVE_IDLE != m_stateRetrieval ) {
    throw std::logic_error( "CIQFeedNewsQuery<T>::RetrieveConfiguration: not in IDLE");
  }
  else {
    m_stateRetrieval = RETRIEVE_CONFIG;
    //m_stateCommand = WAIT4XML;
    //m_owner = owner;
    //m_MsgIdStoryLine = MsgIdStoryLine;
    //m_MsgIdStoryDone = MsgIdStoryDone;
    //m_lParam = lParam;
    std::string ss;
    ss = "NC;";
    Send( ss );
  }
}

template <typename T>
void CIQFeedNewsQuery<T>::ProcessConfigurationRetrieval( linebuffer_t* buf ) {

  linebuffer_t::const_iterator bgn = (*buf).begin();
  linebuffer_t::const_iterator end = (*buf).end();

  structNewsConfigInfo info;
  //std::string s;
  //int i;
  //nc_rule_t id;  // id of line marker found during parse
//  bool b = parse( bgn, end, m_grammarDecodeNewsConfiguration, info );

  //OutputDebugString( s.c_str() );

}