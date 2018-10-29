#ifndef OPENCPN_WAYPOINT_LIST_H
#define OPENCPN_WAYPOINT_LIST_H

#include <ocpn_plugin.h>
#include "wx/wxprec.h"

#ifdef WX_PRECOMP
#include "wx/wx.h"
#endif

#include  <wx/wx.h>
#include  <wx/timer.h>
#include <wx/listctrl.h>
#include <vector>
#include <iostream>

using namespace std;

class myFrame:public wxFrame{
public:
	myFrame(const wxString& title, Plugin_WaypointList wp, vector<time_t> t);
	~myFrame(void);

	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	
	void Set(Plugin_WaypointList wp, vector<time_t> t);
	void UpdateListView();

	wxString title;

	Plugin_WaypointList waypointList;
	wxListView *listView = nullptr;
	vector<time_t> tm;

	bool alive;

	enum Column {
        NO = 0,
        LAT = 1,
        LON = 2,
        TIME = 3
    };
    enum Timer {
        wxID_TIMER = 100
    };

DECLARE_EVENT_TABLE()
};

#endif