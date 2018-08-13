/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "PanelOptionCombo.h"
#include "PanelOptionCombo_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelOptionCombo::PanelOptionCombo(void) {
  Init();
}

PanelOptionCombo::PanelOptionCombo( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

PanelOptionCombo::~PanelOptionCombo(void) {
  //std::cout << "PanelOptionCombo deleted" << std::endl;
}

void PanelOptionCombo::Init() {
  m_pimpl.reset( new PanelOptionCombo_impl( *this ) ); 
}

bool PanelOptionCombo::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

    wxPanel::Create( parent, id, pos, size, style );

    m_pimpl->CreateControls();
    if (GetSizer()) {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

ou::tf::PortfolioGreek::pPortfolioGreek_t& PanelOptionCombo::GetPortfolioGreek( void ) { 
  return m_pimpl->m_pPortfolioGreek; 
}

void PanelOptionCombo::SetPortfolioGreek( pPortfolioGreek_t pPortfolioGreek ) {
  m_pimpl->SetPortfolioGreek( pPortfolioGreek );
}

void PanelOptionCombo::AddPositionGreek( pPositionGreek_t pPositionGreek ) {
  m_pimpl->AddPositionGreek( pPositionGreek );
}

void PanelOptionCombo::SaveColumnSizes( ou::tf::GridColumnSizer& gcs ) const {
  m_pimpl->SaveColumnSizes( gcs );
}

void PanelOptionCombo::SetColumnSizes( ou::tf::GridColumnSizer& gcs ) {
  m_pimpl->SetColumnSizes( gcs );
}
	
void PanelOptionCombo::UpdateGui( void ) {
  m_pimpl->UpdateGui();
}

wxBitmap PanelOptionCombo::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelOptionCombo::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

  template<typename Archive>
  void PanelOptionCombo::save( Archive& ar, const unsigned int version ) const {
    ar & *m_pimpl.get();
  }

  template<typename Archive>
  void PanelOptionCombo::load( Archive& ar, const unsigned int version ) {
    ar & *m_pimpl.get();
  }

// https://www.boost.org/doc/libs/1_67_0/libs/serialization/doc/pimpl.html

  template void PanelOptionCombo::save<boost::archive::text_iarchive>( boost::archive::text_iarchive& ar, const unsigned int version ) const;
  template void PanelOptionCombo::save<boost::archive::text_oarchive>( boost::archive::text_oarchive& ar, const unsigned int version ) const;

  template void PanelOptionCombo::load<boost::archive::text_iarchive>( boost::archive::text_iarchive& ar, const unsigned int version );
  template void PanelOptionCombo::load<boost::archive::text_oarchive>( boost::archive::text_oarchive& ar, const unsigned int version );


} // namespace tf
} // namespace ou
