/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// TestGpFusion.cpp : Defines the entry point for the console application.
// Tests library use of boost::fusion with genetic programming
// 2012/04/22
//

#include "stdafx.h"

#include <iostream>

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/vector.hpp>

#include <boost/fusion/view/transform_view.hpp>
#include <boost/fusion/include/transform_view.hpp>

#include <boost/fusion/sequence/io.hpp>
#include <boost/fusion/include/io.hpp>

#include <boost/fusion/include/size.hpp>

#include <boost/type_traits.hpp>

#include <lib/OUGP/NodeDouble.h>
#include <lib/OUGP/TreeBuilder.h>

using namespace boost::fusion;

  struct square {
    template<typename Sig>
    struct result;

    template<typename U>
    struct result<square(U)> : boost::remove_reference<U>     {};

    template <typename T>
    T operator()(T x) const
    {
        return x * x;
    }
  };



int _tmain(int argc, _TCHAR* argv[]) {

  std::stringstream ss;

  typedef boost::fusion::vector<int, short, double> vector_type;
  vector_type vec(2, 5, 3.3);

  boost::fusion::transform_view<vector_type, square> transform(vec, square());
  std::cout << transform << std::endl;

  typedef boost::fusion::result_of::size<vector_type> whatsit;

  std::cout << whatsit::value << std::endl;

  ou::gp::TreeBuilder tb;

  //ou::gp::Node* node = tb.CreateChild( false, true, 1, 5, tb.m_vNodeFactoryBoolean );

//  ou::gp::NodeDoubleAdd node;

//  ss << *node;

//  node.EvaluateDouble();


	return 0;
}

