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
#include <wx/thread.h>
#include <wx/msgqueue.h>
#include <wx/protocol/http.h>

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

class WorkerThread;
class myPlugin_window: public wxWindow {
public:
    myPlugin_window(wxWindow *pparent, wxWindowID id);
    ~myPlugin_window();

    /* Function */
    void AddWayPoints();
    void RemoveWayPoints();
    void AddRoutes();
    void DoStartThread();

    /* Event handler */
    void OnPaint(wxPaintEvent &event);
    void SetSentence(wxString &sentence);
    void OnSize(wxSizeEvent &event);
    void OnTimer(wxTimerEvent &event);
    void OnRefresh(wxTimerEvent &event);
    void OnListDoubleClick(wxListEvent &event);
    void OnThreadCompletion(wxCommandEvent &event);
    void OnThreadUpdate(wxCommandEvent &event);
    void OnClose(wxCloseEvent &event);
    void OnSettingBtnClick(wxCommandEvent& event);

    /* Data */
    void GetWayPointsFromJSON(wxString& json);

    /* UI */
    void CreateListView();
    void UpdateListView();
    void UpdateWayPointsAll(wxString& json);
    void RemoveListViewItems();
    void CreateSettingButton();

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
    } Waypoint_st;

    wxWindow *parent;
    wxMessageQueue<wxString> *msgQueue = nullptr;
    wxString host = _T("127.0.0.1"); // Default to localhost
    unsigned short hostPort = 3000;
    unsigned int requestTimeout = 1;
    unsigned int requestInterval = 10;

    map<wxString, Waypoint_st> wayPointList;
    vector<wxString> name;
    vector<PlugIn_Route *> route;

    wxListView *listView = nullptr;
    wxButton *settingBtn = nullptr;

    wxTimer *timer = nullptr;
    wxTimer *refresh = nullptr;

    vector<myFrame *> tagFrame;

    enum Timer {
        wxID_TIMER = 100,
        wxID_REFRESH
    };

    enum ID {
       wxID_SETTING_BUTTON = 200
    };


DECLARE_EVENT_TABLE()
};

class WorkerThread: public wxThread {
public:
    WorkerThread(myPlugin_window *handler, wxMessageQueue<wxString> *msgQueue,
            wxString host, unsigned short hostPort, unsigned int requestTimeout):
        wxThread(wxTHREAD_DETACHED), m_pHandler(handler), msgQueue(msgQueue),
        host(host), hostPort(hostPort), requestTimeout(requestTimeout) {};
    ~WorkerThread();
protected:
    myPlugin_window *m_pHandler;
    wxMessageQueue<wxString> *msgQueue;
    wxHTTP *get;

    wxString host;
    unsigned short hostPort;
    unsigned int requestTimeout;

    virtual ExitCode Entry();
    wxString GetWayPointsFromHTTP();

};


#endif //OPENCPN_MYPLUGIN_WINDOW_H
