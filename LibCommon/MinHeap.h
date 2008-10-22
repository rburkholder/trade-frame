#pragma once

#include <vector>
#include <stdexcept>

template<class T> class CMinHeap {
public:
  CMinHeap<t>( size_t size );
  virtual ~CMinHeap( void );
protected:
private:
  std::vector<T> m_vT;
};

template<class T> CMinHeap<T>::CMinHeap(size_t size) {
}

template<class T> CMinHeap<T>::~CMinHeap() {
}