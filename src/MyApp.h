#ifndef MYAPP_H_
#define MYAPP_H_

#pragma once

#include <wx/wx.h>

#include "ProjectManager.h"

class MyApp: public wxApp {
private:
    amdProjectManager* m_manager;
    wxLocale m_locale{};

public:
    virtual bool OnInit();
    virtual int OnExit();

    amdProjectManager* GetManager() { return m_manager; }
};

wxDECLARE_APP(MyApp);

amdProjectManager* amdGetManager();
#endif
