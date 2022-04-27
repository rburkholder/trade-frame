
/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    MacroStrand.h
 * Author:  raymond@burkholder.net
 * Project: lib/TFTrading
 * Created: April 26, 2022 19:46
 */

#pragma once

#include <boost/asio/post.hpp>

#define STRAND( command ) \
  if ( m_bStrand ) {      \
    boost::asio::post(    \
      *m_pStrand,         \
      [this](){           \
        command;          \
      }                   \
      );                  \
  }                       \
  else {                  \
    command;              \
  }

#define STRAND_CAPTURE( command, capture ) \
  if ( m_bStrand ) {      \
    boost::asio::post(    \
      *m_pStrand,         \
      [this,capture](){   \
        command;          \
      }                   \
      );                  \
  }                       \
  else {                  \
    command;              \
  }


