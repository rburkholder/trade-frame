#pragma once

#include "Delegate.h"
#include "GuiThreadCrossing.h"

//#include <boost/pool/detail/singleton.hpp>
//using boost::details::pool;
// returns a reference to singleton with: singleton_default<CGeneratePeriodicRefresh>::instance()

//class CGeneratePeriodicRefresh;  // can't do this, generated outside of main
//typedef boost::details::pool::singleton_default<CGeneratePeriodicRefresh> singletonPeriodicRefresh_t;

class CGeneratePeriodicRefresh: public CGuiThreadCrossing {
  DECLARE_DYNAMIC(CGeneratePeriodicRefresh)
public:
  CGeneratePeriodicRefresh( void );
  virtual ~CGeneratePeriodicRefresh(void);
  typedef Delegate<CGeneratePeriodicRefresh *> delegateRefresh_t;
  void Add( delegateRefresh_t::OnMessageHandler handler );
  void Remove( delegateRefresh_t::OnMessageHandler handler );
protected:
  delegateRefresh_t m_OnRefresh;
  //HANDLE hScreenRefreshThread;
  //DWORD RefreshThreadId;
  HANDLE m_hTimerQueue;
  HANDLE m_hTimer;
	DECLARE_MESSAGE_MAP()
  virtual LRESULT OnPeriodicRefresh( WPARAM w, LPARAM l );
  virtual afx_msg void OnDestroy();
private:
  static VOID CALLBACK TriggerRefresh( LPVOID, BOOLEAN );
  bool m_bKeepTimerActive;
};
