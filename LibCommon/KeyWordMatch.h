#pragma once

// this is kind of a subset of Aho Corasick algorithm
// only full keyword matching, no text searches
// no on failure coding

// turn into a template for <op>

#include <vector>
#include <string>

class CKeyWordMatch {
public:
  CKeyWordMatch(void);
  virtual ~CKeyWordMatch(void);
  void ClearPatterns( void );
  void AddPattern( const std::string &sPattern, unsigned short op );
  unsigned short FindMatch( const std::string &sMatch );
protected:
  struct structNode {
    size_t ixLinkToNextLevel;  // next letter of same word
    size_t ixLinkAtSameLevel;  // look for letter at same level
    unsigned short ixOperation;  // upon match, perform this operation (returned when keyword found)
    char chLetter;  // the letter at this node
    structNode() : ixLinkToNextLevel( 0 ), ixLinkAtSameLevel( 0 ), ixOperation( 0 ), chLetter( 0 ) {};
  };
  std::vector<structNode> m_vNodes;
private:
};
