#ifndef MYAPP_H_
#define MYAPP_H_

#pragma once

#include <wx/wx.h>
#include "MainFrame.h"

class MyApp: public wxApp {
private:
    MainFrame* mainWin = nullptr;
    wxLocale locale;

public:
    virtual bool OnInit();
};

wxDECLARE_APP(MyApp);
#endif
