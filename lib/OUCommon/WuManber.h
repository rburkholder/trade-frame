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

// Implementation of Wu Manber's Multi-Pattern Search Algorithm
// Implemented by Ray Burkholder, ray@oneunified.net
// Copyright (2008) One Unified
// For use without restriction but one:  this copyright notice must be preserved.

#include <vector>

namespace ou {

class WuManber {
public:
  WuManber( void );
  ~WuManber( void );
  void Initialize( const std::vector<const char *> &patterns, 
                   bool bCaseSensitive = false, bool bIncludeSpecialCharacters = false, bool bIncludeExtendedAscii = false );
  void Search( std::size_t TextLength, const char *Text, const std::vector<const char *> &patterns );
protected:
private:
  std::size_t m_k;  // number of patterns;
  std::size_t m_lcpl;  // largest common pattern length, 'm' in the reference materials
  static const std::size_t B = 3;  // Wu Manber paper suggests B is 2 or 3 
    // small number of patterns, use B=2, use an exact table
    // for large number of patterns, use B=3 use compressed table (their code uses 400 as a cross over )
    // this class needs to be adjusted for B=2 (in the build shift table portion)
  static unsigned char rchExtendedAscii[];
  static char rchSpecialCharacters[];

  bool m_bInitialized;

  struct structAlphabet {
    char letter;  // letter for matching purposes
    unsigned char offset; // index of character in offsetted alphabet for shift and hash tables
  } m_lu[256]; // defines our alphabet for matching purposes, is LookUp table of letters for pattern/text matching
  unsigned char m_nSizeOfAlphabet;
  unsigned short m_nBitsInShift; // used for hashing blocks of B characters
  std::size_t m_nTableSize;  // size for SHIFT and HASH tables
  std::size_t *m_ShiftTable;  // SHIFT table
  struct structPatternMap { // one struct per pattern for this hash
    std::size_t PrefixHash;  // hash of first two characters of the pattern
    std::size_t ix;  // index into patterns for final comparison
  } m_PatternMapElement;  // termporary area for element storage
  std::vector<structPatternMap> *m_vPatternMap;
    // this is a combination of HASH and PREFIX table
    // the paper suggests shifting hash right by n bits to hash into this table in order to reduce sparseness

};

} // ou