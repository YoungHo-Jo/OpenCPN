/******************************************************************************
 * updated: 4-5-2012  
 * Project:  OpenCPN
 * Purpose:  demo Plugin
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 */


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP

#include "wx/wx.h"

#endif //precompiled headers

#include <wx/aui/aui.h>
#include <chart1.h>
#include <wx/listctrl.h>
#include "wx/jsonreader.h"
#include "myPlugin_pi.h"
#include "wxWTranslateCatalog.h"
#include <wx/datetime.h>
#include <map>

using namespace std;

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin *create_pi(void *ppimgr) {
    return new demo_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin *p) {
    delete p;
}



//---------------------------------------------------------------------------------------------------------
//
//    demo PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------


int demo_pi::Init(void) {
    AddLocaleCatalog(PLUGIN_CATALOG_NAME);

    m_pdemo_window = NULL;

    // Get a pointer to the opencpn display canvas, to use as a parent for windows created
    m_parent_window = GetOCPNCanvasWindow();

    // Create the Context Menu Items

    //    In order to avoid an ASSERT on msw debug builds,
    //    we need to create a dummy menu to act as a surrogate parent of the created MenuItems
    //    The Items will be re-parented when added to the real context meenu
    wxMenu dummy_menu;

    wxMenuItem *pmi = new wxMenuItem(&dummy_menu, -1, _("Show PlugIn MyPlugin"));
    m_show_id = AddCanvasContextMenuItem(pmi, this);
    SetCanvasContextMenuItemViz(m_show_id, true);

    wxMenuItem *pmih = new wxMenuItem(&dummy_menu, -1, _("Hide PlugIn MyPlugin"));
    m_hide_id = AddCanvasContextMenuItem(pmih, this);
    SetCanvasContextMenuItemViz(m_hide_id, false);

    m_pdemo_window = new myPlugin_window(m_parent_window, wxID_ANY);

    m_AUImgr = GetFrameAuiManager();
    m_AUImgr->AddPane(m_pdemo_window);
    m_AUImgr->GetPane(m_pdemo_window).Name(_T("MyPlugin"));

    m_AUImgr->GetPane(m_pdemo_window).Float();
    m_AUImgr->GetPane(m_pdemo_window).FloatingPosition(300, 30);

    m_AUImgr->GetPane(m_pdemo_window).Caption(_T("AUI Managed Demo Window"));
    m_AUImgr->GetPane(m_pdemo_window).CaptionVisible(true);
    m_AUImgr->GetPane(m_pdemo_window).GripperTop(true);
    m_AUImgr->GetPane(m_pdemo_window).CloseButton(true);
    m_AUImgr->GetPane(m_pdemo_window).Show(false);
    m_AUImgr->Update();

    return (
            WANTS_OVERLAY_CALLBACK |
            INSTALLS_CONTEXTMENU_ITEMS |
            WANTS_NMEA_SENTENCES |
            USES_AUI_MANAGER
    );
}

bool demo_pi::DeInit(void) {
    m_AUImgr->DetachPane(m_pdemo_window);
    if (m_pdemo_window) {
        m_pdemo_window->Close();
//          m_pdemo_window->Destroy(); //Gives a Segmentation fault
    }
    return true;
}

int demo_pi::GetAPIVersionMajor() {
    return MY_API_VERSION_MAJOR;
}

int demo_pi::GetAPIVersionMinor() {
    return MY_API_VERSION_MINOR;
}

int demo_pi::GetPlugInVersionMajor() {
    return PLUGIN_VERSION_MAJOR;
}

int demo_pi::GetPlugInVersionMinor() {
    return PLUGIN_VERSION_MINOR;
}

wxString demo_pi::GetCommonName() {
    return _("MyPlugin");
}

wxString demo_pi::GetShortDescription() {
    return _("MyPlugIn for SOOSANG");
}

wxString demo_pi::GetLongDescription() {
    return _("Demo PlugIn for OpenCPN\n\
demonstrates PlugIn processing of NMEA messages.");

}

void demo_pi::SetNMEASentence(wxString &sentence) {
//      printf("demo_pi::SetNMEASentence\n");
    if (m_pdemo_window) {
        m_pdemo_window->SetSentence(sentence);
    }
}


void demo_pi::OnContextMenuItemCallback(int id) {
    wxLogMessage(_T(TAG"OnContextMenuCallBack()"));
    ::wxBell();

    //  Note carefully that this is a "reference to a wxAuiPaneInfo classs instance"
    //  Copy constructor (i.e. wxAuiPaneInfo pane = m_AUImgr->GetPane(m_pdemo_window);) will not work

    wxAuiPaneInfo &pane = m_AUImgr->GetPane(m_pdemo_window);
    if (!pane.IsOk())
        return;

    if (!pane.IsShown()) {
//            printf("show\n");
        SetCanvasContextMenuItemViz(m_hide_id, true);
        SetCanvasContextMenuItemViz(m_show_id, false);

        pane.Show(true);
        m_AUImgr->Update();

    } else {
//            printf("hide\n");
        SetCanvasContextMenuItemViz(m_hide_id, false);
        SetCanvasContextMenuItemViz(m_show_id, true);

        pane.Show(false);
        m_AUImgr->Update();
    }

/*
      if(NULL == m_pdemo_window)
      {
            m_pdemo_window = new demoWindow(m_parent_window, wxID_ANY);

            SetCanvasContextMenuItemViz(m_hide_id, true);
            SetCanvasContextMenuItemViz(m_show_id, false);
      }
      else
      {
            m_pdemo_window->Close();
            m_pdemo_window->Destroy();
            m_pdemo_window = NULL;

            SetCanvasContextMenuItemViz(m_hide_id, false);
            SetCanvasContextMenuItemViz(m_show_id, true);
      }
*/
}

void demo_pi::UpdateAuiStatus(void) {
    //    This method is called after the PlugIn is initialized
    //    and the frame has done its initial layout, possibly from a saved wxAuiManager "Perspective"
    //    It is a chance for the PlugIn to syncronize itself internally with the state of any Panes that
    //    were added to the frame in the PlugIn ctor.

    //    We use this callback here to keep the context menu selection in sync with the window state


    wxAuiPaneInfo &pane = m_AUImgr->GetPane(m_pdemo_window);
    if (!pane.IsOk())
        return;

    printf("update %d\n", pane.IsShown());

    SetCanvasContextMenuItemViz(m_hide_id, pane.IsShown());
    SetCanvasContextMenuItemViz(m_show_id, !pane.IsShown());

}

void DrawTextString(wxDC& dc, wxString& text, wxPoint& pt)
{
    wxFont font(13, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL);
    dc.SetFont(font);
    dc.SetBackgroundMode(wxTRANSPARENT);
    dc.SetTextForeground(*wxBLACK);
    dc.SetTextBackground(*wxWHITE);
    dc.DrawText(text, pt);
}

bool demo_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp) {
    if(m_pdemo_window){
        wxString cur_time = wxDateTime::Now().Format();

        wxPoint pt(0, 50);
        DrawTextString(dc, cur_time, pt);

        long posY = 100;
        // time, name, lat, lon
        for(int i = 0; i < m_pdemo_window->name.size(); i++){
            wxString n = m_pdemo_window->name[i];
            double lat = m_pdemo_window->wayPointList[n].wp.front()->m_lat;
            double lon = m_pdemo_window->wayPointList[n].wp.front()->m_lon;
            time_t t = m_pdemo_window->wayPointList[n].t.front();

            wxString text = wxString::Format(wxT("%s: %s (%f, %f)"), n, wxDateTime(t).Format(), lat, lon);
            pt = wxPoint(0, posY + i * 15);
            DrawTextString(dc, text, pt);
        }

        // pt = wxPoint(0, 70);
        // DrawTextString(dc, lat, pt);
        // pt = wxPoint(0, 90);
        // DrawTextString(dc, lon, pt);
        // pt = wxPoint(0, 110);
        // DrawTextString(dc, speed, pt);
    }

    return false;
}

void demo_pi::SetCursorLatLon(double lat, double lon) {

}

bool demo_pi::RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp) {
    /*   if(m_pGribDialog && m_pGRIBOverlayFactory)
       {
             if(m_pGRIBOverlayFactory->IsReadyToRender())
             {
                   m_pGRIBOverlayFactory->RenderGLGribOverlay ( pcontext, vp );
                   return true;
             }
             else
                   return false;
       }
       else*/
    return false;

}

int demo_pi::GetToolbarToolCount(void) {
    return 1;
}

void demo_pi::ShowPreferencesDialog(wxWindow *parent) {

}

void demo_pi::OnToolbarToolCallback(int id) {

}

void demo_pi::SetPluginMessage(wxString &message_id, wxString &message_body) {

}

void demo_pi::SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix) {

}



