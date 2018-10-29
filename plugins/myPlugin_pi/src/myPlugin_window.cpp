//
// Created by ZeroHo on 23/05/2018.
//

#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/window.h>
#include <wx/wx.h>
#include "myPlugin_window.h"
#include <wx/aui/aui.h>
#include <chart1.h>
#include <wx/listctrl.h>
#include "wx/jsonreader.h"
#include "wxWTranslateCatalog.h"
#include <wx/sckipc.h>
#include <wx/sstream.h>
#include <wx/protocol/http.h>
#include <thread>


#define TAG "[myPlugin_window] "
#define CLI_DEBUG 0
#define WX_DEBUG 1

wxString json(_T("{\n"
                 "\t\"items\": [{\n"
                 "\t\t\t\"tag_id\": \"A1\",\n"
                 "\t\t\t\"lat\": \"35.17\",\n"
                 "\t\t\t\"lon\": \"129.0\",\n"
                 "\t\t\t\"time\": \"2018-08-14 17:00:10\"\n"
                 "\t\t}, {\n"
                 "\t\t\t\"tag_id\": \"A1\",\n"
                 "\t\t\t\"lat\": \"34.17\",\n"
                 "\t\t\t\"lon\": \"128.0\",\n"
                 "\t\t\t\"time\": \"2018-08-14 17:00:20\"\n"
                 "\t\t}, {\n"
                 "\t\t\t\"tag_id\": \"A2\",\n"
                 "\t\t\t\"lat\": \"33.17\",\n"
                 "\t\t\t\"lon\": \"127.0\",\n"
                 "\t\t\t\"time\": \"2018-08-14 17:01:10\"\n"
                 "\t\t}, {\n"
                 "\t\t\t\"tag_id\": \"A2\",\n"
                 "\t\t\t\"lat\": \"35.22\",\n"
                 "\t\t\t\"lon\": \"129.9\",\n"
                 "\t\t\t\"time\": \"2018-08-14 17:00:30\"\n"
                 "\t\t}, {\n"
                 "\t\t\t\"tag_id\": \"A2\",\n"
                 "\t\t\t\"lat\": \"35.54\",\n"
                 "\t\t\t\"lon\": \"129.8\",\n"
                 "\t\t\t\"time\": \"2018-08-14 17:00:10\"\n"
                 "\t}]\n"
                 "}"));

//----------------------------------------------------------------
//
//    demo Window Implementation
//
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(myPlugin_window, wxWindow)
                EVT_PAINT(myPlugin_window::OnPaint)
                EVT_SIZE(myPlugin_window::OnSize)
                EVT_BUTTON(myPlugin_window::ADD_BUTTON, myPlugin_window::OnAddButtonClick)
                EVT_BUTTON(myPlugin_window::REMOVE_BUTTON, myPlugin_window::OnRemoveButtonClick)
                EVT_BUTTON(myPlugin_window::CONNECT_BUTTON, myPlugin_window::OnConnectButtonClick)
                EVT_SOCKET(myPlugin_window::wxID_CLIENT_SOCKET, myPlugin_window::OnClientSocketEvent)
                EVT_TIMER(myPlugin_window::wxID_TIMER, myPlugin_window::OnTimer)
                EVT_TIMER(myPlugin_window::wxID_REFRESH, myPlugin_window::OnRefresh)
                EVT_LIST_ITEM_ACTIVATED(myPlugin_window::TAG_LIST, myPlugin_window::OnListDoubleClick)
END_EVENT_TABLE()




myPlugin_window::myPlugin_window(wxWindow *pparent, wxWindowID id)
        : wxWindow(pparent, id, wxPoint(-1, -1), wxSize(500, 600),
                   wxSIMPLE_BORDER, _T("OpenCPN PlugIn")) {

    auto logWindow = new if\(WX_DEBUG\)\twxLogWindow(NULL, _T("log window"));
    logWindow->Show(true);

    for(int i = 0; i < route.size(); i++){
        route[i] = nullptr;
    }
    parent = pparent;

    CreateListView();
    UpdateListView();
    SetTimer();

}

myPlugin_window::~myPlugin_window() {
}

void myPlugin_window::OnSize(wxSizeEvent &event) {
//    if\(WX_DEBUG\)\twxLogMessage(_T(TAG"onSize"));
}

void myPlugin_window::OnAddButtonClick(wxCommandEvent &event) {
    if(WX_DEBUG)	wxLogMessage(_T(TAG"AddButtonClick"));

    AddWayPoints();
}


void myPlugin_window::OnRemoveButtonClick(wxCommandEvent &event) {
    if(WX_DEBUG)	wxLogMessage(_T(TAG"RemoveButtonClick"));

    RemoveWayPoints();
}

void myPlugin_window::OnConnectButtonClick(wxCommandEvent &event) {
    if(WX_DEBUG)	wxLogMessage(_T(TAG"OnConnectButtonClick"));

    if(isConnected) {
//        client->Destroy();
        connectButton->SetLabel(_T("Connect"));
    } else {
//        GetWayPointsFromSocket();
        GetWayPointsFromHTTP();
    }
}


void myPlugin_window::SetSentence(wxString &sentence) {
}

void myPlugin_window::OnPaint(wxPaintEvent &event) {
    if(WX_DEBUG)	wxLogMessage(_T(TAG "onPaint"));

    wxPaintDC dc(this);

}
void myPlugin_window::GetWayPointsFromJSON(wxString json) {
    if(WX_DEBUG)	wxLogMessage(_T(TAG"GetWayPointsFromJSON"));
    if(WX_DEBUG)	wxLogMessage(json);
    cout << json << endl;

    wxJSONValue root;
    wxJSONReader reader;

    int numErrors = reader.Parse(json, &root);
    if(numErrors > 0) {
        if(WX_DEBUG)	wxLogMessage(_T(TAG"ERROR: the json document is not well-formed"));
        const wxArrayString errors = reader.GetErrors();

        for(auto error: errors) {
            if(WX_DEBUG)	wxLogMessage(error);
        }
        return;
    }

    wxJSONValue items = root["items"];
    map<wxString, vector<double> > values;
    map<wxString, time_t> timeValues;
    map<wxString, vector<double> >::iterator it;

    for(int i = 0; i < items.Size(); i++) {
        if(items[i]["tag_id"].AsString().Len() < 1){
            if(WX_DEBUG)	wxLogMessage("Name string < 1");
            continue;
        }

        wxString tmpLatStr = items[i]["lat"].AsString();
        wxString tmpLonStr = items[i]["lon"].AsString();
        wxString tmpName = items[i]["tag_id"].AsString();
        struct tm tm;
        strptime(items[i]["time"].AsString(), "%F %H:%M:%S", &tm);
        time_t tmpTime = mktime(&tm);

        double lat, lon;

        tmpLatStr.ToDouble(&lat);
        tmpLonStr.ToDouble(&lon);

        // Save coordinate that most recently
        if(timeValues.find(tmpName) == timeValues.end()){
            values[tmpName] = {lat, lon};
            timeValues[tmpName] = tmpTime;
        }
        else{
            if(timeValues[tmpName] < tmpTime){
                timeValues[tmpName] = tmpTime;
                values[tmpName] = {lat, lon};
            }
        }

        // Save m_tag name(A1, A2, ...)
        if(find(name.begin(), name.end(), items[i]["tag_id"].AsString()) == name.end()){
            name.push_back(tmpName);
        }
    }

// Print saved value
    for(it = values.begin(); it != values.end(); it++){
        if(WX_DEBUG)	wxLogMessage("name: %s, lat:%f, lon:%f", it->first, it->second[0], it->second[1]);
        cout << "name: " << it->first << ", lat: " << it->second[0] << ", lon: " << it->second[1] << ", time: " << timeValues[it->first] << endl;
    }

// Create waypoint & Append to waypoint list
    for(it = values.begin(); it != values.end(); it++){
        PlugIn_Waypoint *waypoint = new PlugIn_Waypoint(
                it->second[0],
                it->second[1],
                _T("Red X"),
                it->first,
                GetNewGUID()); //// Causing duplicated waypoints

        wayPointList[it->first].wp.Append(waypoint);
        wayPointList[it->first].t.push_back(timeValues[it->first]);
    }

    if(WX_DEBUG)	wxLogMessage(wxT(TAG"Done with extracting way points from json"));
}

void myPlugin_window::CreateListView() {
    if(WX_DEBUG)	wxLogMessage(_T(TAG "Create list view"));

    if(listView == nullptr) {
        listView = new wxListView(this, TAG_LIST,
                                  wxPoint(5, 5), wxSize(500, 500),
                                  wxLC_REPORT, wxDefaultValidator, wxT("myListView"));
    }

// listView->InsertColumn(Column::NO, wxT("No"), wxLIST_FORMAT_LEFT);
    listView->InsertColumn(Column::NAME, wxT("TAG"), wxLIST_FORMAT_LEFT);
    listView->InsertColumn(Column::LAT, wxT("Lat"), wxLIST_FORMAT_LEFT);
    listView->InsertColumn(Column::LON, wxT("Lon"), wxLIST_FORMAT_LEFT);
    listView->InsertColumn(Column::TIME, wxT("Time"), wxLIST_FORMAT_LEFT, 250);
}

void myPlugin_window::AddWayPoints() {
    if(WX_DEBUG)	wxLogMessage(_T(TAG"Add Way Points"));
    for(int i = 0; i < name.size(); i++){
        for(auto wayPoint: wayPointList[name[i]].wp) {
            AddSingleWaypoint(wayPoint, false);
        }
    }
    if(WX_DEBUG)	wxLogMessage(_T(TAG"Done with adding way points on the map"));
}

void myPlugin_window::AddRoutes(){
// Delete all route
    for(int i = 0; i < route.size(); i++){
        DeletePlugInRoute(route[i]->m_GUID);
    }
    route.clear();

// Create route with name(A1, A2, ...) using iterator
    map<wxString, Waypoint_st>::iterator it;
    for(it = wayPointList.begin(); it != wayPointList.end(); it++){
        PlugIn_Route *newRoute = nullptr;
        newRoute = new PlugIn_Route();
        newRoute->m_NameString = it->first;
        newRoute->m_StartString = wxString(wxT("Start"));
        newRoute->m_EndString = wxString(wxT("End"));
        newRoute->m_GUID = GetNewGUID();
        newRoute->pWaypointList = &(it->second.wp);

        AddPlugInRoute(newRoute, false);
        route.push_back(newRoute);
    }
    cout << "route: " << route.size() << endl;
    parent->Refresh();
}

void myPlugin_window::RemoveWayPoints() {
    if(WX_DEBUG)	wxLogMessage(_T(TAG"Remove Way Points"));
    for(int i = 0; i < name.size(); i++){
        for(auto wayPoint: wayPointList[name[i]].wp) {
            DeleteSingleWaypoint(wayPoint->m_GUID);
        }
    }
// wayPointList.clear();
}

void myPlugin_window::UpdateWayPointsAll(wxString json) {
    if(WX_DEBUG)	wxLogMessage(_T(TAG"Update all way points"));

// RemoveWayPoints();
    GetWayPointsFromJSON(json);
// AddWayPoints();
    AddRoutes();
}

void myPlugin_window::CreateWayPointAddButton() {
    if(wayPointAddButton == nullptr) {
        wayPointAddButton = new wxButton(this, ADD_BUTTON, _T("Add"), wxPoint(5, 505), wxSize(50, 50));
    }
}

void myPlugin_window::CreateWayPointRemoveButton() {
    if(wayPointRemoveButton == nullptr) {
        wayPointRemoveButton = new wxButton(this, REMOVE_BUTTON, _T("Remove"), wxPoint(60, 505), wxSize(100, 50));
    }
}

void myPlugin_window::CreateConnectButton() {
    if(connectButton == nullptr) {
        connectButton = new wxButton(this, CONNECT_BUTTON, _T("Connect"),
                                     wxPoint(100, 505), wxSize(100, 50));
    }
}

// void myPlugin_window::UpdateListView() {
// 	if\(WX_DEBUG\)\twxLogMessage(_T(TAG"Update list view"));
// 	if(listView == nullptr) {
// 		if\(WX_DEBUG\)\twxLogMessage(_T(TAG"No list view"));
// 		return;
// 	}

// 	long index = 0;
// 	if\(WX_DEBUG\)\twxLogMessage(_T(TAG"IN UpdateListView: wayPointList items: "), wayPointList.size());
// 	for (auto wayPoint: wayPointList) {
// 		listView->InsertItem(index, wxString::Format(wxT("%d"), index + 1));
// 		listView->SetItem(index, Column::LAT, wxString::Format(wxT("%f"), wayPoint->m_lat));
// 		listView->SetItem(index, Column::LON, wxString::Format(wxT("%f"), wayPoint->m_lon));
// 		listView->SetItem(index, Column::NAME, wxString::Format(wxT("%s"), wayPoint->m_MarkName));
// 		index++;
// 	}
// 	if\(WX_DEBUG\)\twxLogMessage(wxString::Format(wxT(TAG"The number of list view items: %d"), listView->GetItemCount()));
// }

void myPlugin_window::UpdateListView() {
    if(WX_DEBUG)	wxLogMessage(_T(TAG"Update list view"));
    if(listView == nullptr) {
        if(WX_DEBUG)	wxLogMessage(_T(TAG"No list view"));
        return;
    }

    long index = 0;

    for(int i = 0; i < name.size(); i++){
        listView->InsertItem(index, wxString::Format(wxT("%d"), index + 1));
        listView->SetItem(index, Column::NAME, wxString::Format(wxT("%s"), name[i]));
        listView->SetItem(index, Column::LAT, wxString::Format(wxT("%f"), wayPointList[name[i]].wp.back()->m_lat));
        listView->SetItem(index, Column::LON, wxString::Format(wxT("%f"), wayPointList[name[i]].wp.back()->m_lon));
        listView->SetItem(index, Column::TIME, wxDateTime(wayPointList[name[i]].t.back()).Format());
        index++;
    }
    if(WX_DEBUG)	wxLogMessage(wxString::Format(wxT(TAG"The number of list view items: %d"), listView->GetItemCount()));
}

void myPlugin_window::RemoveListViewItems() {
    if(WX_DEBUG)	wxLogMessage(_T(TAG"Remove List View Items"));

    if(listView != nullptr) {
        listView->DeleteAllItems();
    }

}


void myPlugin_window::OnClientSocketEvent(wxSocketEvent &event) {
    wxSocketBase *Sock = event.GetSocket();
    if(WX_DEBUG)	wxLogMessage(wxT(TAG"[CLIENT] OnClientSocketEvent"));

    char buffer[10000];

    switch(event.GetSocketEvent()) {
        case wxSOCKET_CONNECTION: {
            if(WX_DEBUG)	wxLogMessage(wxT(TAG"[CLIENT] wxSOCKET_CONNECTION"));
//            char mychar = '0';
//
//            for(int i=0; i<10; ++i) {
//                buffer[i] = mychar++;
//            }
//
//            Sock->Write(buffer, sizeof(buffer));
            break;
        }

        case wxSOCKET_INPUT: {
            if(WX_DEBUG)	wxLogMessage(wxT(TAG"[CLIENT] wxSOCKET_INPUT"));
            Sock->Read(buffer, sizeof(buffer));

            GetWayPointsFromJSON(wxString(buffer));
//            try {
//                GetWayPointsFromJSON(wxString(buffer));
//            } catch (int e) {
//                if\(WX_DEBUG\)\twxLogMessage(wxT(TAG"[EXCEPTION]"));
//            }
            UpdateListView();
            AddWayPoints();
            break;
        }

        case wxSOCKET_LOST: {
            if(WX_DEBUG)	wxLogMessage(wxT(TAG"[CLIENT] wxSOCKET_LOST"));
            Sock->Destroy();
            break;
        }
    }
}


void myPlugin_window::GetWayPointsFromSocket() {
    auto socketConn = new wxSocketClient();
    wxIPV4address addr;
    addr.Hostname(wxT("172.30.1.17"));
    addr.Service(5555);

    socketConn->SetEventHandler(*this, wxID_CLIENT_SOCKET);
    socketConn->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    socketConn->Notify(true);

    if(!socketConn->Connect(addr, true)) {
        if(WX_DEBUG)	wxLogMessage(_T(TAG"ERROR: Socket Connection Failed"));
        connectButton->SetLabel(_T("Disconnect"));
        isConnected = true;
        return;
    }
    if(WX_DEBUG)	wxLogMessage(_T(TAG"Socket Connection Success"));


//    socketConn->SetNotify(wxSOCKET_CONNECTION);
//    socketConn->Notify(true);
//

}


wxString myPlugin_window::GetWayPointsFromHTTP() {
    if(WX_DEBUG)	wxLogMessage(_T(TAG"Get Request"));
    wxHTTP get;
    get.SetHeader(_T("Content-type"), _T("text/html; charset=utf-8"));
    get.SetTimeout(5);

    if(!get.Connect(_T("192.168.88.1"), 8008)) return wxString(_T(""));


//wxApp::IsMainLoopRunning();

    wxInputStream *httpStream = get.GetInputStream(_T("/"));

    wxString res;
    if(get.GetError() == wxPROTO_NOERR) {
        wxStringOutputStream outStream(&res);
        httpStream->Read(outStream);
    } else {
        if(WX_DEBUG)	wxLogMessage(_T("Unable to connect"));
    }

    wxDELETE(httpStream);
    get.Close();
    return res;

}

void myPlugin_window::OnTimer(wxTimerEvent &event) {
    if\(WX_DEBUG\)\twxLogMessage(_T(TAG"Timer: tick!"));
// UpdateWayPointsAll(GetWayPointsFromHTTP());
    UpdateWayPointsAll(json);
    RemoveListViewItems();
    UpdateListView();

// for(int i = 0; i < tagFrame.size();){
// 	if(tagFrame[i]->alive == false){
// 		cout << "erase frame" << endl;
// 		tagFrame.erase(tagFrame.begin() + i);
// 	}
// 	else{
// 		i++;
// 	}
// }

// cout << "frame size: " << tagFrame.size() << endl;
// for(int i = 0; i < tagFrame.size();){
// 	if(tagFrame[i]->alive){
// 		tagFrame[i]->Set(wayPointList[tagFrame[i]->title].wp, wayPointList[tagFrame[i]->title].t);
// 		i++;
// 	}
// }
}

void myPlugin_window::OnRefresh(wxTimerEvent &event){
    parent->Refresh();
}

void myPlugin_window::SetTimer() {
    if(timer == nullptr){
        if(WX_DEBUG)	wxLogMessage(_T(TAG"Timer: start"));
        timer = new wxTimer(this, wxID_TIMER);
        timer->Start(10000);
    }
    if(refresh == nullptr){
        refresh = new wxTimer(this, wxID_REFRESH);
        refresh->Start(1000);
    }
}

void myPlugin_window::OnListDoubleClick(wxListEvent &event){
    long index = event.GetIndex();
    wxString selected = event.GetItem().GetText();

    myFrame *newFrame = new myFrame(selected, wayPointList[selected].wp, wayPointList[selected].t);
    newFrame->Show(true);
// tagFrame.push_back(newFrame);
}