#include "Outline.h"

#include "MainFrame.h"
#include "Corkboard.h"
#include "Timeline.h"
#include "OutlineFiles.h"

#include "wxmemdbg.h"

amOutline::amOutline(wxWindow* parent) : wxNotebook(parent, -1) {
    m_corkboardHolder = new wxPanel(this);
    m_corkboard = new amCorkboard(m_corkboardHolder);

    m_timelineHolder = new wxPanel(this);
    m_timeline = new amTLTimeline(m_timelineHolder);

    m_files = new amOutlineFilesPanel(this);

    // Hack for doing corkboard fullscreen
    m_corkHolderSizer = new wxBoxSizer(wxVERTICAL);
    m_corkHolderSizer->Add(m_corkboard, wxSizerFlags(1).Expand());
    m_corkboardHolder->SetSizer(m_corkHolderSizer);

    // Hack for doing corkboard fullscreen
    m_timelineHolderSizer = new wxBoxSizer(wxVERTICAL);
    m_timelineHolderSizer->Add(m_timeline, wxSizerFlags(1).Expand());
    m_timelineHolder->SetSizer(m_timelineHolderSizer);

    AddPage(m_corkboardHolder, "Corkboard");
    AddPage(m_timelineHolder, "Timeline");
    AddPage(m_files, "Files");
}

void amOutline::SaveOutline() {
    m_corkboard->Save();
    m_timeline->Save();
    m_files->Save();
}

void amOutline::LoadOutline(wxStringInputStream& corkboard,
    wxStringInputStream& timelineCanvas,
    wxStringInputStream& timelineElements,
    wxStringInputStream& files) {

    m_corkboard->Load(corkboard);
    m_timeline->Load(timelineCanvas, timelineElements);
    m_files->Load(files);
}

void amOutline::ClearAll() {
    m_corkboard->getCanvas()->GetDiagramManager()->Clear();
    m_corkboard->getCanvas()->Refresh(true);

    m_files->ClearAll();
}