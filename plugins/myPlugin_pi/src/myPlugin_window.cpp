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
#include "SettingFrame.h"


#define TAG "[myPlugin_window] "
#define CLI_DEBUG 0
#define WX_DEBUG 1

#if WX_DEBUG == 1
#define D(s) s
#else
#define D(s)
#endif

const unsigned int TAG_INFO_REQUEST_INTERVAL = 10000;
const unsigned int MAP_UPDATE_INTERVAL = 1000;


//--------------------------------------------------------------
//
//              Dummy data
//
//--------------------------------------------------------------

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

wxDECLARE_EVENT(wxEVT_COMMAND_WOKRERTHREAD_COMPLETED, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_WOKRERTHREAD_UPDATE, wxCommandEvent);

BEGIN_EVENT_TABLE(myPlugin_window, wxWindow)
                EVT_PAINT(myPlugin_window::OnPaint)
                EVT_SIZE(myPlugin_window::OnSize)
                EVT_TIMER(myPlugin_window::wxID_TIMER, myPlugin_window::OnTimer)
                EVT_TIMER(myPlugin_window::wxID_REFRESH, myPlugin_window::OnRefresh)
                EVT_LIST_ITEM_ACTIVATED(myPlugin_window::TAG_LIST, myPlugin_window::OnListDoubleClick)
                EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_WOKRERTHREAD_COMPLETED, myPlugin_window::OnThreadCompletion)
                EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_WOKRERTHREAD_UPDATE, myPlugin_window::OnThreadUpdate)
                EVT_BUTTON(myPlugin_window::wxID_SETTING_BUTTON, myPlugin_window::OnSettingBtnClick)
END_EVENT_TABLE()

wxDEFINE_EVENT(wxEVT_COMMAND_WOKRERTHREAD_COMPLETED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_WOKRERTHREAD_UPDATE, wxCommandEvent);




myPlugin_window::myPlugin_window(wxWindow *pparent, wxWindowID id)
        : wxWindow(pparent, id, wxPoint(-1, -1), wxSize(500, 600),
                   wxSIMPLE_BORDER, _T("OpenCPN PlugIn")) {

    D(
        auto logWindow = new wxLogWindow(NULL, _T("log window"));
        logWindow->Show(true);
    )

    for(int i = 0; i < route.size(); i++){
        route[i] = nullptr;
    }
    parent = pparent;

    CreateListView();
    UpdateListView();
    SetTimer();
    CreateSettingButton();
}

myPlugin_window::~myPlugin_window() {
}

// When the window size has changed
void myPlugin_window::OnSize(wxSizeEvent &event) {
}

void myPlugin_window::SetSentence(wxString &sentence) {
}

// When the window has getting updated
void myPlugin_window::OnPaint(wxPaintEvent &event) {
    D(wxLogMessage(_T(TAG "onPaint"));)

    wxPaintDC dc(this);

}
void myPlugin_window::GetWayPointsFromJSON(wxString& json) {
    D(wxLogMessage(_T(TAG"GetWayPointsFromJSON"));)
    D(wxLogMessage(json);)

    wxJSONValue root;
    wxJSONReader reader;

    int numErrors = reader.Parse(json, &root);
    if(numErrors > 0) { // Error occured
        D(wxLogMessage(_T(TAG"ERROR: the json document is not well-formed"));)
        const wxArrayString errors = reader.GetErrors();

        for(auto error: errors) {
            D(wxLogMessage(error);)
        }
        return;
    }

    wxJSONValue items = root["items"];
    map<wxString, vector<double> > values;
    map<wxString, time_t> timeValues;

    for(int i = 0; i < items.Size(); i++) {
        if(items[i]["tag_id"].AsString().Len() < 1){
            D(wxLogMessage("Name string < 1");)
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

        // Save the coordinate that is most recently
        if(timeValues.find(tmpName) == timeValues.end()){ // Newest
            values[tmpName] = {lat, lon};
            timeValues[tmpName] = tmpTime;
        }
        else {
            if(timeValues[tmpName] < tmpTime){ // The stored one is later than this
                timeValues[tmpName] = tmpTime;
                values[tmpName] = {lat, lon};
            }
        }

        // Save m_tag name(A1, A2, ...)
        if(find(name.begin(), name.end(), items[i]["tag_id"].AsString()) == name.end()){
            name.push_back(tmpName);
        }
    }

// Create waypoint & Append to waypoint list
    for(auto it = values.begin(); it != values.end(); it++){
        PlugIn_Waypoint *waypoint = new PlugIn_Waypoint(
                it->second[0],
                it->second[1],
                _T("Red X"), // Set the icon
                it->first,
                GetNewGUID()); //// Causing duplicated waypoints

        wayPointList[it->first].wp.Append(waypoint);
        wayPointList[it->first].t.push_back(timeValues[it->first]);
    }

    D(wxLogMessage(wxT(TAG"Done: Extracting way points from the json"));)
}

void myPlugin_window::CreateListView() {
    D(wxLogMessage(_T(TAG "Create list view"));)

    if(listView == nullptr) {
        listView = new wxListView(this, TAG_LIST,
                                  wxPoint(5, 5), wxSize(500, 500),
                                  wxLC_REPORT, wxDefaultValidator, wxT("myListView"));
    }

    listView->InsertColumn(Column::NAME, wxT("TAG"), wxLIST_FORMAT_LEFT);
    listView->InsertColumn(Column::LAT, wxT("Lat"), wxLIST_FORMAT_LEFT);
    listView->InsertColumn(Column::LON, wxT("Lon"), wxLIST_FORMAT_LEFT);
    listView->InsertColumn(Column::TIME, wxT("Time"), wxLIST_FORMAT_LEFT, 250);
}

void myPlugin_window::AddWayPoints() {
    D(wxLogMessage(_T(TAG"Add Way Points"));)
    for(int i = 0; i < name.size(); i++){
        for(auto wayPoint: wayPointList[name[i]].wp) {
            AddSingleWaypoint(wayPoint, false);
        }
    }
    D(wxLogMessage(_T(TAG"Done: Adding way points on the map"));)
}

void myPlugin_window::AddRoutes(){
// Delete all route
    for(int i = 0; i < route.size(); i++){
        delete route[i]->pWaypointList;
        DeletePlugInRoute(route[i]->m_GUID);
    }
    route.clear();

// Create route with name(A1, A2, ...) using iterator
    const unsigned int MAX_ROUTE_LENGTH = 10;
    map<wxString, Waypoint_st>::iterator it;
    for(it = wayPointList.begin(); it != wayPointList.end(); it++){
        PlugIn_Route *newRoute = nullptr;
        newRoute = new PlugIn_Route();
        newRoute->m_NameString = it->first;
        newRoute->m_StartString = wxString(wxT("Start"));
        newRoute->m_EndString = wxString(wxT("End"));
        newRoute->m_GUID = GetNewGUID();
        Plugin_WaypointList *newList = new Plugin_WaypointList;
        newList->assign(it->second.wp.begin(), it->second.wp.end());

        //// TODO: NOT GOOD
//        while(newList->size() <= MAX_ROUTE_LENGTH) {
//           newList->pop_front();
//        }
//        newRoute->pWaypointList = &(it->second.wp);
        newRoute->pWaypointList = newList;

        AddPlugInRoute(newRoute, false);
        route.push_back(newRoute);
    }
    parent->Refresh();
}

void myPlugin_window::RemoveWayPoints() {
    D(wxLogMessage(_T(TAG"Remove Way Points"));)
    for(int i = 0; i < name.size(); i++){
        for(auto wayPoint: wayPointList[name[i]].wp) {
            DeleteSingleWaypoint(wayPoint->m_GUID);
        }
    }
// wayPointList.clear();
}

void myPlugin_window::UpdateWayPointsAll(wxString& json) {
    D(wxLogMessage(_T(TAG"Update all way points"));)

    GetWayPointsFromJSON(json);
    AddRoutes();
}

void myPlugin_window::UpdateListView() {
    D(wxLogMessage(_T(TAG"Update list view"));)
    if(listView == nullptr) {
        D(wxLogMessage(_T(TAG"No list view"));)
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
    D(wxLogMessage(wxString::Format(wxT(TAG"The number of list view items: %d"), listView->GetItemCount()));)
}

void myPlugin_window::RemoveListViewItems() {
    D(wxLogMessage(_T(TAG"Remove List View Items"));)

    if(listView != nullptr) {
        listView->DeleteAllItems();
    }
}


void myPlugin_window::OnTimer(wxTimerEvent &event) {
    D(wxLogMessage(_T(TAG"Timer: tick!"));)
    DoStartThread();
}

void myPlugin_window::OnRefresh(wxTimerEvent &event){
    parent->Refresh();
}

void myPlugin_window::SetTimer() {
    if(timer == nullptr){
        D(wxLogMessage(_T(TAG"Timer: start"));)
        timer = new wxTimer(this, wxID_TIMER);
        timer->Start(requestInterval*1000);
    }
    if(refresh == nullptr){
        refresh = new wxTimer(this, wxID_REFRESH);
        refresh->Start(MAP_UPDATE_INTERVAL);
    }
}

void myPlugin_window::OnListDoubleClick(wxListEvent &event){
    wxString selected = event.GetItem().GetText();

    myFrame *newFrame = new myFrame(selected, wayPointList[selected].wp, wayPointList[selected].t);
    newFrame->Show(true);
// tagFrame.push_back(newFrame);
}

void myPlugin_window::DoStartThread() {
    D(wxLogMessage(TAG"Start Thread");)
    if (msgQueue == nullptr) {
        msgQueue = new wxMessageQueue<wxString>;
        while(msgQueue->IsOk() == false) {
            delete msgQueue;
            msgQueue = new wxMessageQueue<wxString>;
        }
    }

    // This is required to open sockets in a thread
    wxSocketBase::Initialize();
    auto workerThread = new WorkerThread(this, msgQueue, host, hostPort, requestTimeout);
    if(workerThread->Run() != wxTHREAD_NO_ERROR) {
        D(wxLogMessage(TAG"Can't create the thread!");)
        delete msgQueue;
        delete workerThread;
        msgQueue = nullptr;
    }
}


void myPlugin_window::OnThreadCompletion(wxCommandEvent &event) {
    D(wxLogMessage(TAG"Thread exited!");)
}

void myPlugin_window::OnThreadUpdate(wxCommandEvent &event) {
    D(wxLogMessage(TAG"Thread update...");)
    wxString res;
    try {
        msgQueue->ReceiveTimeout(500, res);
        UpdateWayPointsAll(res);
        RemoveListViewItems();
        UpdateListView();
    } catch(wxMessageQueueError e) {
        cout << e << endl;
        D(wxLogMessage(TAG"ERROR: ", e);)
    }
}

void myPlugin_window::OnClose(wxCloseEvent& event) {
//   wxCriticalSectionLocker enter(m_pThreadCS);
//   if(m_pThread) {
//       wxLogMessage(TAG"Deleting thread");
//       if(m_pThread->Delete() != wxTHREAD_NO_ERROR) {
//           wxLogError(TAG"Can't delete the thread!");
//       }
//   }
//   while(true) {
//       {
//           wxCriticalSectionLocker enter(m_pThreadCS);
//           if(!m_pThread) break;
//       }
//       wxThread::This()->Sleep(1);
//   }
   Destroy();
}

void myPlugin_window::CreateSettingButton() {
    if(settingBtn == nullptr) {
       settingBtn = new wxButton(this, wxID_SETTING_BUTTON, wxT("Setting"), wxPoint(10, 500), wxDefaultSize, 0);
    }
}

void myPlugin_window::OnSettingBtnClick(wxCommandEvent &event) {
    SettingFrame *settingFrame = new SettingFrame(this);
    settingFrame->SetHostText(host);
    settingFrame->SetHostPortText(hostPort);
    settingFrame->SetRequestIntervalText(requestInterval);

    function<void()> set = [&]() {
        cout << "hi" << endl;
//        host = settingFrame->GetHost();
        cout << host << endl;
//        hostPort = settingFrame->GetHostPort();
        cout << hostPort << endl;
        requestInterval = settingFrame->GetRequestInterval();
        cout << requestInterval << endl;
    };

    settingFrame->SetApplyButtonClickCallback(set);
    settingFrame->Show(true);
}


wxThread::ExitCode WorkerThread::Entry() {
    while(!TestDestroy()) {
        wxString res = this->GetWayPointsFromHTTP();
        msgQueue->Post(res);
        wxQueueEvent(m_pHandler, new wxThreadEvent(wxEVT_COMMAND_WOKRERTHREAD_UPDATE));
        break; // Do it Once
    }

    wxQueueEvent(m_pHandler, new wxThreadEvent(wxEVT_COMMAND_WOKRERTHREAD_COMPLETED));
    return (wxThread::ExitCode)0; // success
}

wxString WorkerThread::GetWayPointsFromHTTP() {
    D(wxLogMessage(_T(TAG"Get Request in Thread"));)
    wxHTTP get;
    get.SetHeader(_T("Content-type"), _T("text/html; charset=utf-8"));
    get.SetTimeout(this->requestTimeout);

    wxString res = wxEmptyString;

    get.Connect(this->host, this->hostPort);
    wxInputStream *httpStream = get.GetInputStream(_T("/"));

    if(get.GetError() == wxPROTO_NOERR) {
        wxStringOutputStream outStream(&res);
        httpStream->Read(outStream);
    } else {
        res = wxEmptyString;
        D(wxLogMessage(_T("Unable to connect"));)
    }

    wxDELETE(httpStream);
    get.Close();
    return res;
}

WorkerThread::~WorkerThread() {
}



