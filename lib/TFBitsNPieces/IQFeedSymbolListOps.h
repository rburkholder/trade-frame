/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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
// Started 2013/11/14

#pragma once

#include <set>
#include <string>

#include <boost/atomic.hpp>
#include <boost/signals2.hpp>

#include <OUCommon/Worker.h>

#include <TFIQFeed/LoadMktSymbols.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class IQFeedSymbolListOps {
public:

  using vExchanges_t = std::set<std::string>;
  using vSymbolList_t = std::set<std::string>;
  using classifier_t =  ou::tf::iqfeed::ESecurityType;
  using vClassifiers_t = std::set<classifier_t>;
  using trd_t = ou::tf::iqfeed::InMemoryMktSymbolList::trd_t;

  // used in conjuction with InMemoryMktSymbolList::SelectSymbolsByExchange
  // should this structure be here or in InMemoryMktSymbolList?
  struct SelectSymbols {
    bool m_bSelectWithOptions;
    const vClassifiers_t& m_classifiers;
    ou::tf::iqfeed::InMemoryMktSymbolList& m_selected;

    SelectSymbols( const vClassifiers_t& classifiers, ou::tf::iqfeed::InMemoryMktSymbolList& selected, bool bSelectWithOptions = false )
      : m_selected( selected ), m_classifiers( classifiers ), m_bSelectWithOptions( bSelectWithOptions )  {  };

    void operator() ( const trd_t& trd ) {
      if ( m_classifiers.end() != m_classifiers.find( trd.sc ) ) {
        if ( m_bSelectWithOptions ) {
          if ( trd.bHasOptions ) {
            m_selected( trd );
          }
          else {
            // skip symbol if it has no options
          }
        }
        else {
          m_selected( trd );
        }
      }
    }
  };

  typedef boost::signals2::signal<void( const std::string&)> signalStatus_t;
  typedef signalStatus_t::slot_type slotStatus_t;

  enum ECompletionCode { ccDone, ccSaved, ccCleared };

  typedef boost::signals2::signal<void( ECompletionCode )> signalDone_t;
  typedef signalDone_t::slot_type slotDone_t;

  IQFeedSymbolListOps( ou::tf::iqfeed::InMemoryMktSymbolList& );
  ~IQFeedSymbolListOps(void);

  bool Exists( const std::string& sName );

  signalStatus_t Status;
  signalDone_t Done;

  void ObtainNewIQFeedSymbolListRemote( void );
  void ObtainNewIQFeedSymbolListLocal( void );
  void LoadIQFeedSymbolList( void );
  void SaveSymbolSubset( const std::string& sFileName, const ou::tf::iqfeed::InMemoryMktSymbolList& subset );  // sample
  void LoadSymbolSubset( const std::string& sFileName );  // sample
  void ClearIQFeedSymbolList( void );

protected:
private:

  boost::atomic<int> m_fenceWorker;
  ou::action::Worker m_worker;
  ou::tf::iqfeed::InMemoryMktSymbolList& m_listIQFeedSymbols;

  void StatusBusy();
  void StatusDone();

  void WorkerObtainNewIQFeedSymbolListRemote( void );
  void WorkerObtainNewIQFeedSymbolListLocal( void );
  void WorkerLoadIQFeedSymbolList( void );
};

} // namespace tf
} // namespace ou
