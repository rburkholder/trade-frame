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
  HANDLE m_hWaitEvent;
  CWinThread *m_pThreadPeriodic;
	DECLARE_MESSAGE_MAP()
  virtual LRESULT OnPeriodicRefresh( WPARAM w, LPARAM l );
  //virtual afx_msg void OnDestroy();  // doesn't get called
private:
  static UINT TriggerRefresh( LPVOID );
  bool m_bKeepTimerActive;
  //static int m_cntInstances;
};
