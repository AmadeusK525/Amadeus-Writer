#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "MyApp.h"

IMPLEMENT_APP(MyApp)


bool MyApp::OnInit() {
    //throw 8;
    wxInitAllImageHandlers();

    MainFrame* mainWin = new MainFrame("New Amadeus project", wxDefaultPosition, wxSize(800, 700));

    mainWin->Show(true);
    SetTopWindow(mainWin);

    std::cout << "Hola" << std::endl;

    return true;
}


