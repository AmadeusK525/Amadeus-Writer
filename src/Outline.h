#pragma once

#include "wx/notebook.h"
#include "wx/panel.h"
#include "wx/progdlg.h"

#include <fstream>

#include "Corkboard.h"

class Outline : public wxNotebook {
private:
    Corkboard* corkboard = nullptr;
    wxPanel* timeline = nullptr;

public:
    // Hack for doing corkboard fullscreen
    wxPanel* corkHolder = nullptr;
    wxBoxSizer* corkHolderSizer = nullptr;

public:
    Outline(wxWindow* parent);
    
    void saveOutline(int& progress, wxProgressDialog* dialog);
    void loadOutline(int& progress, wxProgressDialog* dialog);

    Corkboard* getCorkboard() { return corkboard; }
};

