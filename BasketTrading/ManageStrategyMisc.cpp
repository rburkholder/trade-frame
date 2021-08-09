===== old selection method

        SymbolSelection selector(
          dtLatestEod, m_setSymbols,
          [this,bAddToList,&vDesired](const IIPivot& iip) {
            if ( bAddToList ) {
              if ( vDesired.end() != vDesired.find( iip.sName ) ) {
                std::cout << "desired: " << iip.sName << std::endl;
//              if (
//                   ( "NEM" != iip.sName ) // NEM has a non-standard strike price: 35.12, etc
//              )
//              {
                // see if we get wider swings with this
//                double dblSum = iip.dblProbabilityAboveAndUp + iip.dblProbabilityBelowAndDown;
//                if ( 1.24 < dblSum ) {
                  m_history.Request( iip.sName, 200 );
                  AddUnderlyingSymbol( iip );
//                }
              }
            }
            else { // simply emit statisitcs
              std::cout
                << iip.sName
                << ": " << iip.dblPV
                << "," << iip.dblProbabilityAboveAndUp
                << "," << iip.dblProbabilityAboveAndDown
                << "," << iip.dblProbabilityBelowAndUp
                << "," << iip.dblProbabilityBelowAndDown
                << std::endl;
            }
          } );


=====

// misc code from other modules to build a different ManageStrategy

/*
  // from MasterPortfolio::Load to run with Darvas symbols, use to select rising collars?

        using setInstrumentInfo_t = SymbolSelection::setIIDarvas_t;
        using InstrumentInfo_t = IIDarvas;
        setInstrumentInfo_t setInstrumentInfo;

        SymbolSelection selector(
          dtLatestEod,
          [&setInstrumentInfo](const InstrumentInfo_t& ii) {
            setInstrumentInfo.insert( ii );
          } );

        if ( bAddToList ) {
          std::for_each( setInstrumentInfo.begin(), setInstrumentInfo.end(),
                        [this](const InstrumentInfo_t& ii){
                          AddSymbol( ii.sName, ii.barLast, ii.dblStop );
                        } );
        }
        else {
          std::cout << "Symbol List: " << std::endl;
          std::for_each(
            setInstrumentInfo.begin(), setInstrumentInfo.end(),
            [this]( const setInstrumentInfo_t::value_type& item ) {
              std::cout << item.sName << std::endl;
            } );
        }
*/


//void MasterPortfolio::GetSentiment( size_t& nUp, size_t& nDown ) const {
//  m_sentiment.Get( nUp, nDown );
//}



//void MasterPortfolio::Start() {

//  if ( m_bStarted ) {
//    std::cout << "MasterPortfolio: already started." << std::endl;
//  }
//  else {
//    std::cout << "m_mapVolatility has " << m_mapVolatility.size() << " entries." << std::endl;
//    m_bStarted = true;
    //m_eAllocate = EAllocate::Done;

    // startup Strategy which has previous positions
//    std::for_each(
//      m_setSymbols.begin(), m_setSymbols.end(),
//      [this,&nToSelect,dblAmountToTradePerInstrument](const setSymbols_t::value_type& vt){
//        Strategy& strategy( m_mapStrategy.find( vt )->second );
//        ou::tf::DatedDatum::volume_t volume = strategy.pManageStrategy->CalcShareCount( dblAmountToTradePerInstrument );
//        std::cout
//          << strategy.iip.sName
//          //<< " ranking=" << strategy.dblBestProbability
//          //<< " direction=" << (int)ranking.direction
//          << " to trade: " << volume
//          << " (from previous)"
//          << std::endl;
//        strategy.pManageStrategy->SetFundsToTrade( dblAmountToTradePerInstrument );
//        m_nSharesTrading += strategy.pManageStrategy->CalcShareCount( dblAmountToTradePerInstrument );
//        strategy.pManageStrategy->Start();
//        m_fSupplyStrategyChart( EStrategyChart::Active, vt, strategy.pChartDataView );
//        if ( 0 < nToSelect ) nToSelect--;
//      }
//    );

//    std::for_each(
//      m_mapVolatility.rbegin(), m_mapVolatility.rend(),
//      [this,&nToSelect,dblAmountToTradePerInstrument](mapVolatility_t::value_type& vt){
//        if ( 0 < nToSelect ) {
//          //Ranking& ranking( vt.second );
//          std::string sName( vt.second );
//          setSymbols_t::const_iterator iterSymbols = m_setSymbols.find( sName );
//          if ( m_setSymbols.end() == iterSymbols ) {
//            Strategy& strategy( m_mapStrategy.find( sName )->second ); // doesn't work as m_mapStrategy uses portfolio name
//            ou::tf::DatedDatum::volume_t volume = strategy.pManageStrategy->CalcShareCount( dblAmountToTradePerInstrument );
//            if ( 100 <= volume ) {
//              std::cout
//                << strategy.iip.sName
//                //<< " ranking=" << strategy.dblBestProbability
//                //<< " direction=" << (int)ranking.direction
//                << " to trade: " << volume
//                << " (new start)"
//                << std::endl;
//              strategy.pManageStrategy->SetFundsToTrade( dblAmountToTradePerInstrument );
//              m_nSharesTrading += strategy.pManageStrategy->CalcShareCount( dblAmountToTradePerInstrument );
//              strategy.pManageStrategy->Start();
//  //            switch ( ranking.direction ) {
//  //              case IIPivot::Direction::Up:
//  //                strategy.pManageStrategy->Start( ManageStrategy::ETradeDirection::Up );
//  //                break;
//  //              case IIPivot::Direction::Down:
//  //                strategy.pManageStrategy->Start( ManageStrategy::ETradeDirection::Down );
//  //                break;
//  //              case IIPivot::Direction::Unknown:
//  //                assert( 0 );
//  //                break;
//  //            }
//              //m_fSupplyStrategyChart( EStrategyChart::Active, vt.second.sName, strategy.pChartDataView );
//              m_fSupplyStrategyChart( EStrategyChart::Active, sName, strategy.pChartDataView );
//              nToSelect--;
//            }
//          }
//        }
//      } );
//    std::cout << "Total Shares to be traded: " << m_nSharesTrading << std::endl;
//  }

//} // Start

