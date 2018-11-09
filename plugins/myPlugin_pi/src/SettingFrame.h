//
// Created by ZeroHo on 07/11/2018.
//

#ifndef OPENCPN_SETTINGFRAME_H
#define OPENCPN_SETTINGFRAME_H

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <functional>


///////////////////////////////////////////////////////////////////////////////
/// Class SettingFrame
///////////////////////////////////////////////////////////////////////////////
class SettingFrame: public wxFrame
{
private:

protected:
    wxStaticText* m_staticText1;
    wxTextCtrl* hostInput;
    wxStaticText* m_staticText2;
    wxTextCtrl* portInput;
    wxStaticText* m_staticText3;
    wxTextCtrl* intervalInput;
    wxStaticText* m_staticText4;
    wxStaticText* m_staticText5;
    wxStaticText* m_staticText6;
    wxStaticText* hostText;
    wxStaticText* portText;
    wxStaticText* intervalText;
    wxButton* cancelBtn;
    wxButton* applyBtn;

    std::function<void()> applyButtonCallback;

public:

    SettingFrame( wxWindow* parent, wxWindowID id = wxID_ANY,
            const wxString& title = wxEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxSize( 500,300 ),
            long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

    void SetApplyButtonClickCallback(std::function<void()>& callback);
    void SetHostText(wxString host);
    void SetHostPortText(unsigned  short hostPort);
    void SetRequestIntervalText(unsigned int interval);

    wxString GetHost();
    unsigned short GetHostPort();
    unsigned int GetRequestInterval();


    void OnApplyButtonClick(wxCommandEvent& event);

    ~SettingFrame();

    enum ID {
        wxID_APPLY_BUTTON = 1
    };

    DECLARE_EVENT_TABLE()
};

#endif //OPENCPN_SETTINGFRAME_H
