//
// Created by ZeroHo on 23/05/2018.
//

#ifndef OPENCPN_MYPLUGIN_WINDOW_H
#define OPENCPN_MYPLUGIN_WINDOW_H

#include <wx/window.h>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <ocpn_plugin.h>
#include "wx/wxprec.h"
#include <wx/sckipc.h>

#ifdef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "version.h"
#include <time.h>
#include "waypoint_list.h"

#include <map>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

class myPlugin_window: public wxWindow {
public:
    myPlugin_window(wxWindow *pparent, wxWindowID id);
    ~myPlugin_window();

    /* Function */
    void AddWayPoints();
    void RemoveWayPoints();
    void AddRoutes();

    /* Event handler */
    void OnPaint(wxPaintEvent &event);
    void SetSentence(wxString &sentence);
    void OnSize(wxSizeEvent &event);
    void OnAddButtonClick(wxCommandEvent &event);
    void OnRemoveButtonClick(wxCommandEvent &event);
    void OnConnectButtonClick(wxCommandEvent &event);
    void OnTimer(wxTimerEvent &event);
    void OnRefresh(wxTimerEvent &event);
    void OnListDoubleClick(wxListEvent &event);

    /* Data */
    void GetWayPointsFromJSON(wxString json);
    void GetWayPointsFromSocket();
    wxString GetWayPointsFromHTTP();


    /* UI */
    void CreateListView();
    void UpdateListView();
    void CreateWayPointAddButton();
    void CreateWayPointRemoveButton();
    void CreateConnectButton();
    void UpdateWayPointsAll(wxString json);
    void RemoveListViewItems();

    /* Setter */
    void SetTimer();



    /* Getter */


    /* Variable */
    enum {
        ADD_BUTTON = wxID_HIGHEST + 1,
        REMOVE_BUTTON,
        CONNECT_BUTTON,
        TAG_LIST,
    };

    enum Column {
        // NO = 0,
                NAME = 0,
        LAT = 1,
        LON = 2,
        TIME = 3
    };

    typedef struct Waypoint_st{
        Plugin_WaypointList wp;
        vector<time_t> t;
    }Waypoint_st;

    // Currently not used
//    NMEA0183 m_NMEA0183;                 // Used to parse NMEA Sentences
    wxString m_NMEASentence;

    wxWindow *parent;

    map<wxString, Waypoint_st> wayPointList;
    vector<wxString> name;
    vector<PlugIn_Route *> route;

    wxListView *listView = nullptr;
    wxButton *wayPointAddButton = nullptr;
    wxButton *wayPointRemoveButton = nullptr;
    wxButton *connectButton = nullptr;
    wxTimer *timer = nullptr;
    wxTimer *refresh = nullptr;

    vector<myFrame *> tagFrame;

    wxSocketClient *client;
    void OnClientSocketEvent(wxSocketEvent &event);
    enum Socket {
        wxID_SOCKET = 0,
        wxID_BUTSTART,
        wxID_DESC,
        wxID_CLIENT_SOCKET,
        wxID_HTTP_REQUEST
    };
    enum Timer {
        wxID_TIMER = 100,
        wxID_REFRESH
    };
    bool isConnected = false;

    enum Execption {
        JSON_PARSE_EXCEPTION = 1
    };



DECLARE_EVENT_TABLE()
};

#endif //OPENCPN_MYPLUGIN_WINDOW_H
