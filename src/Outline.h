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
    
    int non = 0;
    int noi = 0;
    int nob = 0;

    int pnoi = 0;

public:
    Outline(wxWindow* parent);

    void clearAll();
    int  getCount() { return non + noi + nob; }

    void setNon(int n) { non = n; }
    void setNoi(int n) { noi = n; }
    void setNob(int n) { nob = n; }

    void saveOutline(std::ofstream& out, int& progress, wxProgressDialog* dialog);
    void loadOutline(std::ifstream& in, int& progress, wxProgressDialog* dialog);

    void saveQuantities(std::ofstream& out);
    void loadQuantities(std::ifstream& in);
};

