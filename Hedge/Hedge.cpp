// Hedge.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include <wx/wx.h>

class MyApp : public wxApp {
   virtual bool OnInit();
   virtual int OnExit();
};
 
IMPLEMENT_APP(MyApp)

// Implements MyApp& wxGetApp()
DECLARE_APP(MyApp)
 
class MyFrame : public wxFrame {
public:
   MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
protected:
   // Do we really need to expose the implementation detail? I guess not.
   void OnQuit(wxCommandEvent& event);
   void OnAbout(wxCommandEvent& event);
private:
   enum {ID_Quit=wxID_HIGHEST + 1, ID_About};
};
 
bool MyApp::OnInit() {
   wxFrame *frame = new MyFrame("Hello World", wxPoint(50,50), wxSize(450,350));
     frame->Show(TRUE);
     SetTopWindow(frame);
     return TRUE;
}

int MyApp::OnExit() {
  return 0;
}
 
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame((wxFrame*)NULL, -1, title, pos, size)
{
  // create menubar
  wxMenuBar* menuBar = new wxMenuBar;
  // create menu
  wxMenu* menuFile = new wxMenu;
  // append menu entries
  menuFile->Append(ID_About, "&About...\tF1");
  menuFile->AppendSeparator();
  menuFile->Append(ID_Quit, "E&xit\tAlt-X");
  // append menu to menubar
  menuBar->Append(menuFile, "&File");
  // set frame menubar
  SetMenuBar(menuBar);
 
  Bind( wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnQuit, this, ID_Quit );
  Bind( wxEVT_COMMAND_MENU_SELECTED, &MyFrame::OnAbout, this, ID_About );
 
  CreateStatusBar();
  SetStatusText("Welcome to wxWidgets");
}
 
void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
   Close(TRUE);
}
 
void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
   wxMessageBox("wxWidgets Hello World example.","About Hello World",
        wxOK|wxICON_INFORMATION, this);
}
