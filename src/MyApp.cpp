#include "MyApp.h"

#include <iostream>

wxIMPLEMENT_APP(MyApp);

/// <summary>
/// Global function that returns the application manager, gotten from wxGetApp().
/// </summary>
/// <returns>The active amdProjectManager object pointer</returns>
amdProjectManager* amdGetManager() {
    return 	wxGetApp().GetManager();
}

bool MyApp::OnInit() {
    m_locale.Init(wxLANGUAGE_DEFAULT);
    wxInitAllImageHandlers();

    m_manager = new amdProjectManager();
    m_manager->SetExecutablePath(argv.GetArguments()[0]);

    if (argc > 1)
        m_manager->SetProjectFileName(argv.GetArguments()[1]);
    else
        m_manager->GetLastSave();

    if (!m_manager->Init()) {
        m_wizard = new amdProjectWizard(nullptr, 1234);
        m_wizard->Bind(wxEVT_WIZARD_FINISHED, &MyApp::OnWizardFinished, this);
        m_wizard->Bind(wxEVT_WIZARD_CANCEL, &MyApp::OnWizardCanceled, this);
        m_wizard->Show();
        m_wizard->RunWizard(m_wizard->GetFirstPage());
    }

    return true;
}

int MyApp::OnExit() {
    if (m_manager)
        delete m_manager;

    return 0;
}

void MyApp::OnWizardFinished(wxWizardEvent& event) {
    if (!wxFileName::Exists(m_wizard->GetFileName().GetPath())) {
        wxMessageBox("There was an error.");
        return;
    }

    m_manager->SetProjectFileName(m_wizard->GetFileName());
    m_manager->Init();

    m_wizard->Destroy();
}

void MyApp::OnWizardCanceled(wxWizardEvent& event) {
    m_wizard->Destroy();
}