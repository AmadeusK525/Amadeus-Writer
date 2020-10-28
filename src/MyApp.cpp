#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "MyApp.h"

#include "wx/cmdline.h"
#include "wx/filename.h"
#include <iostream>

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
    wxInitAllImageHandlers();

    MainFrame* mainFrame = new MainFrame("New Amadeus project", wxDefaultPosition, wxSize(800, 700));

    mainFrame->Show(true);
    SetTopWindow(mainFrame);
    
    // This catches the path to the executable (the first cmd line argument) and sets it.
    // It'll be used in the getLast() and setLast() functions.
    mainFrame->setExecPath((string)argv.GetArguments()[0]);

    // This loads the file opened with "Open with..." and, if it fails,
    // it gets the most recent worked on project and loads it.
    if (argc > 1)
        mainFrame->loadFileFromOpenWith((string)argv.GetArguments()[1]);
    
    return true;
}
