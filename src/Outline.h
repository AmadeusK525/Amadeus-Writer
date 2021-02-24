#ifndef OUTLINE_H_
#define OUTLINE_H_
#pragma once

#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/progdlg.h>
#include <wx/splitter.h>

#include <wx\sstream.h>

#include "Timeline.h"

class amOutline;
class amOutlineFilesPanel;
class amCorkboard;

class amOutline : public wxNotebook {
private:
    amCorkboard* m_corkboard = nullptr;
    amTimeline* m_timeline = nullptr;
    amOutlineFilesPanel* m_files = nullptr;

public:
    // Hack for doing corkboard fullscreen
    wxPanel* m_corkboardHolder = nullptr;
    wxBoxSizer* m_corkHolderSizer = nullptr;

public:
    amOutline(wxWindow* parent);

    void SaveOutline();
    void LoadOutline(wxStringInputStream& corkboard,
        wxStringInputStream& timeline,
        wxStringInputStream& files);

    amCorkboard* GetCorkboard() { return m_corkboard; }
    amTimeline* GetTimeline() { return m_timeline; }
    amOutlineFilesPanel* GetOutlineFiles() { return m_files; }

    void ClearAll();
};

#endif