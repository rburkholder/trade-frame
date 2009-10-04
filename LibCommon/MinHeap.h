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

#include <vector>
#include <assert.h>

// http://cis.stvincent.edu/html/tutorials/swd/heaps/heaps.html
// http://en.wikipedia.org/wiki/Binary_heap
// http://en.wikipedia.org/wiki/Heapsort
// http://people.cs.vt.edu/~shaffer/Book/C++2e/progs/minheap.h
// http://www.staroceans.com/minmaxHeap1.htm
// http://www.cppreference.com/wiki/stl/algorithm/is_heap  is_heap()

template<class T, class C> class CMinHeap {
public:
  CMinHeap<T,C>( size_t size );
  CMinHeap<T,C>( void );
  virtual ~CMinHeap( void );
  void Append( T );  // automatic sift up
  T RemoveEnd( void );
  T GetRoot( void ) { assert( 0 != m_cntActiveItems ); return m_vT.front(); };
  void ArchiveRoot( void ); // root item of no further use, swap to end and sift down
  void SiftDown( void ) { SiftDown( 0 ); }; // reorder new root after use and change
  bool Empty( void ) { return m_vT.empty(); };
  size_t Size( void ) { return m_vT.size(); };
protected:
  inline size_t Parent( size_t ix ) { return ( ix - 1 ) / 2; };
  inline size_t RightChild( size_t ix ) { return 2 * ( ix + 1 ); };
  inline size_t LeftChild( size_t ix ) { return ( 2 * ix ) + 1; };
  inline bool isLeaf( size_t ix ) { return LeftChild( ix ) >= m_cntActiveItems; };
  inline bool hasOneLeaf( size_t ix ) { return m_cntActiveItems == RightChild( ix ); };
  inline size_t ixLastItem( void );
  void SiftDown( size_t ix ); // from ix downwards, reordering item
  void SiftUp( size_t ix );   // from ix upwards towards root, when appending new items
  inline void Swap( size_t ix, size_t iy );
private:
  std::vector<T> m_vT;
  size_t m_cntActiveItems;
  bool m_bArchivalStarted; // prevents further Appends
};

template<class T, class C> CMinHeap<T,C>::CMinHeap(size_t size) 
: m_cntActiveItems( 0 ), m_bArchivalStarted( false )
{
  m_vT.reserve( size );
}

template<class T, class C> CMinHeap<T,C>::CMinHeap( void ) 
: m_cntActiveItems( 0 ), m_bArchivalStarted( false )
{
}

template<class T, class C> CMinHeap<T,C>::~CMinHeap() {
}

template<class T, class C> size_t CMinHeap<T,C>::ixLastItem() {
  assert( 0 < m_cntActiveItems );
  return m_cntActiveItems - 1;
}

template<class T, class C> void CMinHeap<T,C>::Append( T item ) {
  assert( !m_bArchivalStarted );
  m_vT.push_back( item );
  ++m_cntActiveItems;
  SiftUp( ixLastItem() );
}

template<class T, class C> T CMinHeap<T,C>::RemoveEnd( void ) {
  assert( !m_vT.empty() );
  T item = m_vT.back();
  m_vT.pop_back();
  return item;
}

template<class T, class C> void CMinHeap<T,C>::ArchiveRoot() {
  // swap with last item and SiftDown
  assert( 0 < m_cntActiveItems );
  m_bArchivalStarted = true;
  Swap( 0, ixLastItem() );
  --m_cntActiveItems;
  if ( 1 < m_cntActiveItems ) { // sift only if 2 or more items
    SiftDown();
  }
}

template<class T, class C> void CMinHeap<T,C>::Swap( size_t ix, size_t iy ) {
  assert( ix < m_cntActiveItems );
  assert( iy < m_cntActiveItems );
  T tmp = m_vT.at( ix );
  m_vT.at( ix ) = m_vT.at( iy );
  m_vT.at( iy ) = tmp;
}

template<class T, class C> void CMinHeap<T,C>::SiftUp( size_t ix ) {
  size_t cur = ix;
  size_t parent;
  while ( 0 != cur ) {
    parent = Parent( cur );
    if ( C::lt( m_vT.at( parent ), m_vT.at( cur ) ) ) {
      break; // done sifting
    }
    else { // swap and try again
      Swap( cur, parent );
      cur = parent;
    }
  }
}

template<class T, class C> void CMinHeap<T,C>::SiftDown( size_t ix ) {
  size_t cur = ix;
  while ( !isLeaf( cur ) ) {
    if ( hasOneLeaf( cur ) ) {
      size_t left = LeftChild( cur );
      if ( C::lt( m_vT.at( left ), m_vT.at( cur ) ) ) {
        Swap( left, cur );
        cur = left;
      }
      else {
        break;
      }
    }
    else { // has two leaves
      size_t right = RightChild( cur );
      size_t left = LeftChild( cur );
      // right side has shorter distance by default for same or greater
      bool bGoRight = !( C::lt( m_vT.at( left ), m_vT.at( right ) ) );
      if ( bGoRight ) {
        if ( C::lt( m_vT.at( right ), m_vT.at( cur ) ) ) {
          Swap( right, cur );
          cur = right;
        }
        else {
          break;
        }
      }
      else { // test with left
        if ( C::lt( m_vT.at( left ), m_vT.at( cur ) ) ) {
          Swap( left, cur );
          cur = left;
        }
        else {
          break;
        }
      }
    }
  }
}

// if we needed to build a heap from pre-assigned vector:
//  for (int i=n/2-1; i>=0; i--) siftdown(i);