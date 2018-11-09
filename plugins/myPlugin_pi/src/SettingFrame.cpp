///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "SettingFrame.h"
#include "wx/string.h"
#include <iostream>

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(SettingFrame, wxFrame)
    EVT_BUTTON(SettingFrame::wxID_APPLY_BUTTON, SettingFrame::OnApplyButtonClick)
END_EVENT_TABLE()

SettingFrame::SettingFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* bSizer3;
    bSizer3 = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* bSizer41;
    bSizer41 = new wxBoxSizer( wxVERTICAL );

    m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Host"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText1->Wrap( -1 );
    bSizer41->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

    hostInput = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    bSizer41->Add( hostInput, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );


    bSizer3->Add( bSizer41, 5, wxALL|wxEXPAND, 5 );

    wxBoxSizer* bSizer5;
    bSizer5 = new wxBoxSizer( wxVERTICAL );

    m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText2->Wrap( -1 );
    bSizer5->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

    portInput = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    bSizer5->Add( portInput, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );


    bSizer3->Add( bSizer5, 1, wxALL|wxEXPAND, 5 );

    wxBoxSizer* bSizer6;
    bSizer6 = new wxBoxSizer( wxVERTICAL );

    m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Interval"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText3->Wrap( -1 );
    bSizer6->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

    intervalInput = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    bSizer6->Add( intervalInput, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );


    bSizer3->Add( bSizer6, 1, wxALL|wxEXPAND, 5 );


    bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );

    wxBoxSizer* bSizer7;
    bSizer7 = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* bSizer8;
    bSizer8 = new wxBoxSizer( wxVERTICAL );

    m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Host"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText4->Wrap( -1 );
    bSizer8->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

    m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText5->Wrap( -1 );
    bSizer8->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

    m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Interval"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText6->Wrap( -1 );
    bSizer8->Add( m_staticText6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );


    bSizer7->Add( bSizer8, 1, wxALL|wxEXPAND, 5 );

    wxBoxSizer* bSizer9;
    bSizer9 = new wxBoxSizer( wxVERTICAL );

    hostText = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    hostText->Wrap( -1 );
    bSizer9->Add( hostText, 0, wxALL, 5 );

    portText = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    portText->Wrap( -1 );
    bSizer9->Add( portText, 0, wxALL, 5 );

    intervalText = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    intervalText->Wrap( -1 );
    bSizer9->Add( intervalText, 0, wxALL, 5 );


    bSizer7->Add( bSizer9, 5, wxALL|wxEXPAND, 5 );


    bSizer1->Add( bSizer7, 1, wxALL|wxEXPAND, 5 );

    wxBoxSizer* bSizer4;
    bSizer4 = new wxBoxSizer( wxHORIZONTAL );

    cancelBtn = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( cancelBtn, 0, wxALL|wxALIGN_BOTTOM, 5 );

    applyBtn = new wxButton( this, wxID_APPLY_BUTTON, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer4->Add( applyBtn, 0, wxALL|wxALIGN_BOTTOM, 5 );


    bSizer1->Add( bSizer4, 1, wxALIGN_BOTTOM|wxALIGN_RIGHT, 5 );


    this->SetSizer( bSizer1 );
    this->Layout();

    this->Centre( wxBOTH );
}

void SettingFrame::SetHostText(wxString host) {
    hostText->SetLabelText(host);
}
void SettingFrame::SetHostPortText(unsigned short hostPort) {
    wxString portStr = wxString::Format(wxT("%d"), hostPort);
    portText->SetLabelText(portStr);
}
void SettingFrame::SetRequestIntervalText(const unsigned int interval) {
    wxString intervalStr = wxString::Format(wxT("%d"), interval);
    intervalText->SetLabelText(intervalStr);
}
void SettingFrame::SetApplyButtonClickCallback(std::function<void()>& callback) {
    applyButtonCallback = callback;
}

void SettingFrame::OnApplyButtonClick(wxCommandEvent &event) {
    applyButtonCallback();
//    Close(true);
}

wxString SettingFrame::GetHost() {
    return hostInput->GetLineText(0);
}

unsigned short SettingFrame::GetHostPort() {
    unsigned long ret = 1;
    if(!portInput->GetLineText(0).ToULong(&ret) && ret > 0) {
        return (unsigned short)ret;
    }
    return 3000;
}

unsigned int SettingFrame::GetRequestInterval() {
    unsigned long ret = 10;
    wxString str = intervalInput->GetValue();
    using namespace std;
    cout << "str: " << str << endl;
    if(!str.ToULong(&ret) && ret > 0) {
        cout << "pass" << endl;
       return (unsigned int)ret;
    }
    cout << "blocked" << endl;
//    if(!intervalInput->GetLineText(0).ToULong(&ret) && ret > 0) {
//        return (unsigned int)ret;
//    }
    return 10;
}


SettingFrame::~SettingFrame()
{
}
