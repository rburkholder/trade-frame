/************************************************************************
 * Copyright(c) 2017, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

/* 
 * File:   TSAllocator.h
 * Author: Raymond Burkholder
 * Created on February 5, 2017, 8:43 PM
 */

#pragma once

// http://en.cppreference.com/w/cpp/concept/Allocator
// https://www.youtube.com/watch?v=WjTrfoiB0MQ
// https://www.codeproject.com/Articles/4795/C-Standard-Allocator-An-Introduction-and-Implement
// http://docs.roguewave.com/sourcepro/11.1/html/toolsug/11-6.html
// http://jrruethe.github.io/blog/2015/11/22/allocators/

//#include <OUCommon/FastDelegate.h>

namespace ou { // One Unified

template<typename T>
class object_traits {
public:

   typedef T type;

   template<typename U>
   struct rebind {
      typedef object_traits<U> other;
   };

   // Constructor
   object_traits(void){}

   // Copy Constructor
   template<typename U>
   object_traits(object_traits<U> const& other){}

   // Address of object
   type*       address(type&       obj) const {return &obj;}
   type const* address(type const& obj) const {return &obj;}

   // Construct object
   void construct(type* ptr, type const& ref) const {
      // In-place copy construct
      new(ptr) type(ref);
   }

   // Destroy object
   void destroy(type* ptr) const {
      // Call destructor
      ptr->~type();
   }
};

#define ALLOCATOR_TRAITS(T)                \
typedef T                 type;            \
typedef type              value_type;      \
typedef value_type*       pointer;         \
typedef value_type const* const_pointer;   \
typedef value_type&       reference;       \
typedef value_type const& const_reference; \
typedef std::size_t       size_type;       \
typedef std::ptrdiff_t    difference_type; \

template<typename T>
struct max_allocations {
  enum{value = static_cast<std::size_t>(-1) / sizeof(T)};
};

template<typename T>
class heap {
public:

  ALLOCATOR_TRAITS(T)
  
  //typedef fastdelegate::FastDelegate0<void> lockRequest_t;
  //typedef fastdelegate::FastDelegate1<size_type> OnDeallocate_t;

  template<typename U>
  struct rebind {
      typedef heap<U> other;
  };

  // Default Constructor
  heap(void){}

  // Copy Constructor
  template<typename U>
  heap(heap<U> const& other){}
  
  //lockRequest_t lockRequest;

  // Allocate memory
  pointer allocate(size_type count, const_pointer /* hint */ = 0) {
    //if ( 0 != lockRequest ) lockRequest();  // allows external coarse lock
    if(count > max_size()){throw std::bad_alloc();}
    return static_cast<pointer>(::operator new(count * sizeof(type), ::std::nothrow));
  }
  
  //OnDeallocate_t OnDeallocate;

  // Delete memory
  void deallocate(pointer ptr, size_type count ) {
    //if ( 0 != lockRequest ) lockRequest();  // allows external coarse lock
    ::operator delete(ptr);
  }

  // Max number of objects that can be allocated in one call
  size_type max_size(void) const {return max_allocations<T>::value;}
  
};

#define FORWARD_ALLOCATOR_TRAITS(C)                  \
typedef typename C::value_type      value_type;      \
typedef typename C::pointer         pointer;         \
typedef typename C::const_pointer   const_pointer;   \
typedef typename C::reference       reference;       \
typedef typename C::const_reference const_reference; \
typedef typename C::size_type       size_type;       \
typedef typename C::difference_type difference_type; \

template<typename T,
         typename PolicyT = heap<T>,
         typename TraitsT = object_traits<T> >
class allocator : public PolicyT,
                  public TraitsT
{
public:

    // Template parameters
    typedef PolicyT Policy;
    typedef TraitsT Traits;

    FORWARD_ALLOCATOR_TRAITS(Policy)

    template<typename U>
    struct rebind {
       typedef allocator<U,
                         typename Policy::template rebind<U>::other,
                         typename Traits::template rebind<U>::other
                        > other;
    };

    // Constructor
    allocator(void){}

    // Copy Constructor
    template<typename U,
             typename PolicyU,
             typename TraitsU>
    allocator(allocator<U,
                        PolicyU,
                        TraitsU> const& other) :
       Policy(other),
       Traits(other)
    {}
};

// Two allocators are not equal unless a specialization says so
template<typename T, typename PolicyT, typename TraitsT,
         typename U, typename PolicyU, typename TraitsU>
bool operator==(allocator<T, PolicyT, TraitsT> const& left,
                allocator<U, PolicyU, TraitsU> const& right)
{
   return false;
}

// Also implement inequality
template<typename T, typename PolicyT, typename TraitsT,
         typename U, typename PolicyU, typename TraitsU>
bool operator!=(allocator<T, PolicyT, TraitsT> const& left,
                allocator<U, PolicyU, TraitsU> const& right)
{
   return !(left == right);
}

// Comparing an allocator to anything else should not show equality
template<typename T, typename PolicyT, typename TraitsT,
         typename OtherAllocator>
bool operator==(allocator<T, PolicyT, TraitsT> const& left,
                OtherAllocator const& right)
{
   return false;
}

// Also implement inequality
template<typename T, typename PolicyT, typename TraitsT,
         typename OtherAllocator>
bool operator!=(allocator<T, PolicyT, TraitsT> const& left,
                OtherAllocator const& right)
{
  return !(left == right);
}

// Specialize for the heap policy
template<typename T, typename TraitsT,
         typename U, typename TraitsU>
bool operator==(allocator<T, heap<T>, TraitsT> const& left,
                allocator<U, heap<U>, TraitsU> const& right)
{
   return true;
}

// Also implement inequality
template<typename T, typename TraitsT,
         typename U, typename TraitsU>
bool operator!=(allocator<T, heap<T>, TraitsT> const& left,
                allocator<U, heap<U>, TraitsU> const& right)
{
   return !(left == right);
}

} // namespace ou
