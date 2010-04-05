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

// this is kind of a subset of Aho Corasick algorithm
// only full keyword matching, no text searches
// no on-failure coding

#include <string>
#include <vector>
#include <stdexcept>
//#include <iostream>

template<typename T> 
class CKeyWordMatch {
  // T is something to be returned once a match is found, requires copy constructor and operator!=()
  // example usage is to return per minute rate for longest match on telephone number prefix
public:
  explicit CKeyWordMatch<T>( T initializer, size_t size );  // initializer is some default value of T
  ~CKeyWordMatch<T>(void);
  void ClearPatterns( void );
  void AddPattern( const std::string &sPattern, T object );
  size_t GetNodeCount( void ) { return m_vNodes.size(); };
  size_t GetPatternCount( void ) { return m_cntPatterns; };
  T FindMatch( const std::string &sMatch );
protected:
	T m_Initializer;
  struct structNode {
    size_t ixLinkToNextLevel;  // next letter of same word
    size_t ixLinkAtSameLevel;  // look for other letters at same location
    T object;  // upon match, (returned when keyword found)
    char chLetter;  // the letter at this node
    explicit structNode( T initializer ) : ixLinkToNextLevel( 0 ), ixLinkAtSameLevel( 0 ), 
      object( initializer ), chLetter( 0 ) {};
  };
private:
  std::vector<structNode> m_vNodes;
  size_t m_cntPatterns;
};

template<typename T> CKeyWordMatch<T>::CKeyWordMatch( T initializer, size_t size )
: m_Initializer( initializer ), m_cntPatterns( 0 )
{
  m_vNodes.reserve( size );
  ClearPatterns();
}

template<typename T> CKeyWordMatch<T>::~CKeyWordMatch(void) {
  m_vNodes.clear();
}

template<typename T> void CKeyWordMatch<T>::ClearPatterns() {
  m_vNodes.clear();
  structNode node( m_Initializer );
  m_vNodes.push_back( node ); // root node with nothing
  m_cntPatterns = 0;
}

template<typename T> void CKeyWordMatch<T>::AddPattern( 
              const std::string &sPattern, T object ) {
  std::string::const_iterator iter = sPattern.begin(); 
  if ( sPattern.end() == iter ) {
    throw std::invalid_argument( "zero length pattern" );
  }
  size_t ixNode = 0;
  size_t ix;
  bool bDone = false;
  while ( !bDone ) {
    char ch = *iter;
    ix = m_vNodes[ ixNode ].ixLinkToNextLevel;
    if ( 0 == ix ) { // end of chain, so add letter
      structNode node( m_Initializer );
      node.chLetter = ch;
      m_vNodes.push_back( node );
      ix = m_vNodes.size() - 1;
      m_vNodes[ ixNode ].ixLinkToNextLevel = ix;
      ixNode = ix;
    }
    else { // find letter at this level
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
            structNode node( m_Initializer );
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
      if ( m_Initializer != m_vNodes[ ixNode ].object ) {
        throw std::domain_error( "Pattern already present" );
      }
      m_vNodes[ ixNode ].object = object;  // assign and finish
      bDone = true;
    }
  }
  ++m_cntPatterns;
}

template<typename T> T CKeyWordMatch<T>::FindMatch( const std::string &sPattern ) {
  // traverse structure looking for matches, object at longest match is returned
  std::string::const_iterator iter = sPattern.begin(); 
  if ( sPattern.end() == iter ) {
    throw std::runtime_error( "zero length pattern" );
  }
  T object = m_Initializer;
  size_t ixNode = 0;
  size_t ix;
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
        	if ( m_Initializer != m_vNodes[ ixLevel ].object )
        		object = m_vNodes[ ixLevel ].object;
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
      bDone = true;
    }
  }
  return object;
}
