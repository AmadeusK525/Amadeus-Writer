#ifndef OUTLINE_H_
#define OUTLINE_H_
#pragma once

#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/progdlg.h>
#include <wx/splitter.h>

#include <fstream>

class amOutline;
class amOutlineFilesPanel;
class Corkboard;

class amOutline : public wxNotebook {
private:
    Corkboard* m_corkboard = nullptr;
    wxPanel* m_timeline = nullptr;
    amOutlineFilesPanel* m_files = nullptr;

public:
    // Hack for doing corkboard fullscreen
    wxPanel* m_corkboardHolder = nullptr;
    wxBoxSizer* m_corkHolderSizer = nullptr;

public:
    amOutline(wxWindow* parent);

    void SaveOutline(int& progress, wxProgressDialog* dialog);
    void LoadOutline(int& progress, wxProgressDialog* dialog);

    Corkboard* GetCorkboard() { return m_corkboard; }
    amOutlineFilesPanel* GetOutlineFiles() { return m_files; }

    void ClearAll();
};

#endif