#ifndef OUTLINE_H_
#define OUTLINE_H_
#pragma once

#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/progdlg.h>
#include <wx/splitter.h>

#include <wx\sstream.h>

class amOutline;
class amCorkboard;
class amTLTimeline;
class amOutlineFilesPanel;

class amOutline : public wxNotebook {
private:
    amCorkboard* m_corkboard = nullptr;
    amTLTimeline* m_timeline = nullptr;
    amOutlineFilesPanel* m_files = nullptr;

public:
    // Hack for doing corkboard fullscreen
    wxPanel* m_corkboardHolder = nullptr;
    wxBoxSizer* m_corkHolderSizer = nullptr;

public:
    amOutline(wxWindow* parent);

    void SaveOutline();
    void LoadOutline(wxStringInputStream& corkboard,
        wxStringInputStream& timelineCanvas,
        wxStringInputStream& timelineElements,
        wxStringInputStream& files);

    amCorkboard* GetCorkboard() { return m_corkboard; }
    amTLTimeline* GetTimeline() { return m_timeline; }
    amOutlineFilesPanel* GetOutlineFiles() { return m_files; }

    void ClearAll();
};

#endif