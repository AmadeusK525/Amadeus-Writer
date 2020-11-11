#pragma once
#ifndef MYAPP_H_
#define MYAPP_H_

#include "MainFrame.h"
#include <wx/cmdline.h>

class MyApp: public wxApp {
private:
    MainFrame* mainWin = nullptr;
    wxLocale locale;

public:
    virtual bool OnInit();
    //virtual bool OnInitCmdLine();
    //virtual bool OnParseCmdLine();
};

#endif
