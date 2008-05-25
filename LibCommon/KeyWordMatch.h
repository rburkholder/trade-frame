#pragma once

// this is kind of a subset of Aho Corasick algorithm
// only full keyword matching, no text searches
// no on failure coding

// turn into a template for <object>

#include <vector>
#include <string>

class CKeyWordMatch {
public:
  CKeyWordMatch(void);
  CKeyWordMatch(size_t size);
  virtual ~CKeyWordMatch(void);
  void ClearPatterns( void );
  void AddPattern( const std::string &sPattern, void *object );
  void *FindMatch( const std::string &sMatch );
protected:
  struct structNode {
    size_t ixLinkToNextLevel;  // next letter of same word
    size_t ixLinkAtSameLevel;  // look other letters at same location
    void *object;  // upon match, (returned when keyword found)
    char chLetter;  // the letter at this node
    structNode() : ixLinkToNextLevel( 0 ), ixLinkAtSameLevel( 0 ), 
      object( NULL ), chLetter( 0 ) {};
  };
  std::vector<structNode> m_vNodes;
private:
};
