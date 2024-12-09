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

class CListViewCtrl_Headlines : public CWindowImpl<CListViewCtrl_Headlines, CListViewCtrl> {
public:
  //CListViewCtrl_Headlines(HWND hWnd = NULL);
//  CListViewCtrl_Headlines(void) {};
//  ~CListViewCtrl_Headlines(void) {};
  CListViewCtrl_Headlines& operator=( HWND hWnd ) {
    CListViewCtrl::operator =( hWnd );
    return *this;
  }
protected:
  BEGIN_MSG_MAP_EX(CListViewCtrl_Headlines)
    // put your message handler entries here
//    MSG_WM_MOUSEMOVE(OnMouseMove)
//    MSG_WM_MOUSEWHEEL(OnMouseWheel)
//    MSG_WM_MOUSEHOVER(OnMouseHover)
    MSG_WM_DESTROY(OnDestroy)
  END_MSG_MAP()

  void OnMouseMove(UINT nFlags, CPoint point);
  BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  void OnMouseHover(WPARAM wParam, CPoint ptPos);
  void OnDestroy();

private:
};
