#pragma once
#ifndef MYAPP_H_
#define MYAPP_H_

#include "MainFrame.h"

class MyApp: public wxApp {
private:
    MainFrame* mainWin = nullptr;
public:
    virtual bool OnInit();
};

#endif
