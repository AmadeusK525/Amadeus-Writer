#ifndef MYAPP_H_
#define MYAPP_H_

#pragma once

#include <wx/wx.h>

#include "ProjectManager.h"
#include "ProjectWizard.h"

class MyApp: public wxApp {
private:
    amdProjectManager* m_manager = nullptr;
    amdProjectWizard* m_wizard = nullptr;
    wxLocale m_locale{};

public:
    virtual bool OnInit();
    virtual int OnExit();

    amdProjectManager* GetManager() { return m_manager; }
    void OnWizardFinished(wxWizardEvent& event);
    void OnWizardCanceled(wxWizardEvent& event);
};

wxDECLARE_APP(MyApp);

amdProjectManager* amdGetManager();
#endif
