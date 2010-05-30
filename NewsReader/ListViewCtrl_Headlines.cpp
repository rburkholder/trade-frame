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

#include "StdAfx.h"

#include <sstream>

#include "ListViewCtrl_Headlines.h"


void CListViewCtrl_Headlines::OnMouseMove(UINT nFlags, CPoint point) {
  std::stringstream ss;
  ss << "HeadLine Mouse Move " << point.x << "," << point.y << std::endl;
  OutputDebugString( ss.str().c_str() );
}

BOOL CListViewCtrl_Headlines::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  std::stringstream ss;
  ss << "HeadLine Mouse Scroll " << pt.x << "," << pt.y << "," << zDelta << std::endl;
  OutputDebugString( ss.str().c_str() );
  return TRUE;
}

void CListViewCtrl_Headlines::OnMouseHover(WPARAM wParam, CPoint ptPos) {
  std::stringstream ss;
  ss << "HeadLine Mouse Hover " << ptPos.x << "," << ptPos.y << std::endl;
  OutputDebugString( ss.str().c_str() );
}

void CListViewCtrl_Headlines::OnDestroy() {
//  DestroyWindow();
}