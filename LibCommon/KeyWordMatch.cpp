#include "StdAfx.h"
#include "KeyWordMatch.h"

#include <stdexcept>

CKeyWordMatch::CKeyWordMatch(void) {
  ClearPatterns();
}

CKeyWordMatch::CKeyWordMatch(size_t size) {
  m_vNodes.reserve( size );
  ClearPatterns();
}

CKeyWordMatch::~CKeyWordMatch(void) {
  m_vNodes.clear();
}

void CKeyWordMatch::ClearPatterns() {
  m_vNodes.clear();
  structNode node;
  m_vNodes.push_back( node ); // root node with nothing
}

// modify this to so we know when max length pattern has been found
// can be used in the rate table lookup for max length match
// set marker for terminal of match (.ixLinkToNextLevel is 0)
// store op, and keep trying for matches on string.
// check for zero matches?

// do matches through indirect case table like the wumanber design

void CKeyWordMatch::AddPattern( 
              const std::string &sPattern, void *object ) {
  std::string::const_iterator iter = sPattern.begin(); 
  if ( sPattern.end() == iter ) {
    throw std::runtime_error( "zero length pattern" );
  }
  size_t ixNode = 0;
  size_t ix;
  bool bDone = false;
  while ( !bDone ) {
    char ch = *iter;
    ix = m_vNodes[ ixNode ].ixLinkToNextLevel;
    if ( 0 == ix ) { // end of chain, so add letter
      structNode node;
      node.chLetter = ch;
      m_vNodes.push_back( node );
      ix = m_vNodes.size() - 1;
      m_vNodes[ ixNode ].ixLinkToNextLevel = ix;
      ixNode = ix;
    }
    else { // find letter at this level
      //ix = m_vNodes[ ixNode ].ixLinkToNextLetter;  // already set
      bool bLevelDone = false;
      size_t ixLevel = ix;  // set from above
      while ( !bLevelDone ) {
        if ( ch == m_vNodes[ ixLevel ].chLetter ) { 
          // found matching character
          ixNode = ixLevel;
          bLevelDone = true;
        }
        else {
          // move onto next node at this level to find character
          size_t ixLinkAtNextSameLevel 
            = m_vNodes[ ixLevel ].ixLinkAtSameLevel;
          if ( 0 == ixLinkAtNextSameLevel ) {
            // add a new node at this level
            structNode node;
            node.chLetter = ch;
            m_vNodes.push_back( node );
            ix = m_vNodes.size() - 1;
            m_vNodes[ ixLevel ].ixLinkAtSameLevel = ix;
            ixNode = ix;
            bLevelDone = true;
          }
          else {
            // check the new node, nothing to do here
            // check next in sequence
            ixLevel = ixLinkAtNextSameLevel;
          }
        }
      }
    }
    ++iter;
    if ( sPattern.end() == iter ) {
      if ( NULL != m_vNodes[ ixNode ].object ) {
        std::runtime_error( "Pattern already present" );
      }
      m_vNodes[ ixNode ].object = object;  // assign and finish
      bDone = true;
    }
  }
}

void *CKeyWordMatch::FindMatch( const std::string &sPattern ) {
  // traverse structure looking for matches
  // need to fix so can return longest match
  std::string::const_iterator iter = sPattern.begin(); 
  if ( sPattern.end() == iter ) {
    throw std::runtime_error( "zero length pattern" );
  }
  void *object = NULL;
  size_t ixNode = 0;
  size_t ix;
  bool bOpFound = true;
  bool bDone = false;
  while ( !bDone ) {
    char ch = *iter;
    ix = m_vNodes[ ixNode ].ixLinkToNextLevel;
    if ( 0 == ix ) {
      bDone = true;  // no more matches to be found so exit
    }
    else {
      // compare characters at this level
      bool bLevelDone = false;
      size_t ixLevel = ix;  // set from above
      while ( !bLevelDone ) {
        if ( ch == m_vNodes[ ixLevel ].chLetter ) {
          ixNode = ixLevel;
          bLevelDone = true;
        }
        else {
          ixLevel = m_vNodes[ ixLevel ].ixLinkAtSameLevel;
          if ( 0 == ixLevel ) {  // no match so end 
            bLevelDone = true;
            bDone = true;
          }
        }
      }
    }
    ++iter;
    if ( sPattern.end() == iter ) {
      object = m_vNodes[ ixNode ].object;
      bDone = true;
    }
  }
  return object;
}


