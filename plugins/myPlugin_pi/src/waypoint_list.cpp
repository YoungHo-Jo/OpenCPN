#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "waypoint_list.h"
#include "wxWTranslateCatalog.h"


BEGIN_EVENT_TABLE(myFrame, wxFrame)
	EVT_PAINT(myFrame::OnPaint)
	EVT_SIZE(myFrame::OnSize)

END_EVENT_TABLE()

myFrame::myFrame(const wxString& title, Plugin_WaypointList wp, vector<time_t> t)
	:wxFrame(NULL, wxID_ANY, title){
	SetMinSize(wxSize(500, 100));
	alive = true;
	this->title = title;
	waypointList = wp;

	tm.resize((int)(t.size()));
	copy(t.begin(), t.end(), tm.begin());

	if(listView == nullptr) {
		listView = new wxListView(this, wxID_ANY,
								  wxDefaultPosition, wxDefaultSize,
								  wxLC_REPORT, wxDefaultValidator, wxT("myListView"));
	}

	listView->InsertColumn(Column::NO, wxT("No"), wxLIST_FORMAT_LEFT);
	listView->InsertColumn(Column::LAT, wxT("Lat"), wxLIST_FORMAT_LEFT);
	listView->InsertColumn(Column::LON, wxT("Lon"), wxLIST_FORMAT_LEFT);
	listView->InsertColumn(Column::TIME, wxT("Time"), wxLIST_FORMAT_LEFT, 250);

	UpdateListView();
}


myFrame::~myFrame(void){
	alive = false;
}

void myFrame::OnPaint(wxPaintEvent& event){

}

void myFrame::OnSize(wxSizeEvent& event){
	
}

void myFrame::Set(Plugin_WaypointList wp, vector<time_t> t){
	waypointList = wp;
	tm.resize((int)(t.size()));
	copy(t.begin(), t.end(), tm.begin());
	listView->DeleteAllItems();
	UpdateListView();
}

void myFrame::UpdateListView(){
	int index = 0;
	for(int i = 0; i < waypointList.size(); i++){
		listView->InsertItem(index, wxString::Format(wxT("%d"), index + 1));
		listView->SetItem(index, Column::NO, wxString::Format(wxT("%d"), index+1));
		listView->SetItem(index, Column::LAT, wxString::Format(wxT("%f"), waypointList[i]->m_lat));
		listView->SetItem(index, Column::LON, wxString::Format(wxT("%f"), waypointList[i]->m_lon));
		listView->SetItem(index, Column::TIME, wxDateTime(tm[i]).Format());
		index++;
	}
}