#ifndef OUTLINE_H_
#define OUTLINE_H_
#pragma once

#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/progdlg.h>
#include <wx/splitter.h>

#include <fstream>

class Outline;
class OutlineFilesPanel;
class Corkboard;

class Outline : public wxNotebook {
private:
    Corkboard* corkboard = nullptr;
    wxPanel* timeline = nullptr;
    OutlineFilesPanel* files = nullptr;

public:
    // Hack for doing corkboard fullscreen
    wxPanel* corkHolder = nullptr;
    wxBoxSizer* corkHolderSizer = nullptr;

public:
    Outline(wxWindow* parent);

    void saveOutline(std::ofstream& out, int& progress, wxProgressDialog* dialog);
    void loadOutline(std::ifstream& in, int& progress, wxProgressDialog* dialog);

    Corkboard* getCorkboard() { return corkboard; }
    OutlineFilesPanel* getOutlineFiles() { return files; }

    void clearAll();
};

#endif