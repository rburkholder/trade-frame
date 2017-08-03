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
 * File:   ShimEvents.h
 * Author: raymond@burkholder.net
 * 
 * Created on August 3, 2017, 12:22 PM
 */

#pragma once
class InterfaceBoundEvents {
public:
  InterfaceBoundEvents();
  virtual ~InterfaceBoundEvents();
  virtual void BindEvents() = 0;
  virtual void UnbindEvents() = 0;
private:
};
