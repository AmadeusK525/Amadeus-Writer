#ifndef MYAPP_H_
#define MYAPP_H_

#pragma once

#include <wx/wx.h>

#include "ProjectManager.h"
#include "ProjectWizard.h"

class MyApp: public wxApp {
private:
    amProjectManager* m_manager = nullptr;
    amProjectWizard* m_wizard = nullptr;
    wxLocale m_locale{};

public:
    virtual bool OnInit();
    virtual int OnExit();

    amProjectManager* GetManager() { return m_manager; }
    void OnWizardFinished(wxWizardEvent& event);
    void OnWizardCanceled(wxWizardEvent& event);
};

wxDECLARE_APP(MyApp);

amProjectManager* amGetManager();
#endif
