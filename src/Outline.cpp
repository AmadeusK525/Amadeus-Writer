#include "Outline.h"

#include "MainFrame.h"
#include "Corkboard.h"
#include "Timeline.h"
#include "OutlineFiles.h"

#include "wxmemdbg.h"

amOutline::amOutline(wxWindow* parent) : wxNotebook(parent, -1) {
    m_corkboardHolder = new wxPanel(this);
    m_corkboard = new amCorkboard(m_corkboardHolder);
    m_timeline = new amTLTimeline(this);
    m_files = new amOutlineFilesPanel(this);

    // Hack for doing corkboard fullscreen
    m_corkHolderSizer = new wxBoxSizer(wxVERTICAL);
    m_corkHolderSizer->Add(m_corkboard, wxSizerFlags(1).Expand());
    m_corkboardHolder->SetSizer(m_corkHolderSizer);

    AddPage(m_corkboardHolder, "Corkboard");
    AddPage(m_timeline, "Timeline");
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