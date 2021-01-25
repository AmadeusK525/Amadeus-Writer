#include "MyApp.h"

#include <iostream>

wxIMPLEMENT_APP(MyApp);

/**
Global function that returns the application manager, gotten from wxGetApp().
*/
amdProjectManager* amdGetManager() {
    return 	wxGetApp().GetManager();
}

bool MyApp::OnInit() {
    m_locale.Init(wxLANGUAGE_DEFAULT);
    wxInitAllImageHandlers();

    m_manager = new amdProjectManager();
    m_manager->SetExecutablePath(argv.GetArguments()[0]);

    if (argc > 1)
        m_manager->SetCurrentDocPath(argv.GetArguments()[1]);

    m_manager->Init();

  /*  if (argc > 1)
        m_manager = new amdProjectManager((string)argv.GetArguments()[1]);
    else*/
       
    return true;
}

int MyApp::OnExit() {
    if (m_manager)
        delete m_manager;

    return 0;
}
