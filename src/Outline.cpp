#include "Outline.h"

#include "MainFrame.h"
#include "Corkboard.h"
#include "OutlineFiles.h"


amOutline::amOutline(wxWindow* parent) : wxNotebook(parent, -1) {
    m_corkboardHolder = new wxPanel(this);
    m_corkboard = new amCorkboard(m_corkboardHolder);
    m_timeline = new amTimeline(this);
    m_files = new amOutlineFilesPanel(this);

    // Hack for doing corkboard fullscreen
    m_corkHolderSizer = new wxBoxSizer(wxVERTICAL);
    m_corkHolderSizer->Add(m_corkboard, wxSizerFlags(1).Expand());
    m_corkboardHolder->SetSizer(m_corkHolderSizer);

    AddPage(m_corkboardHolder, "Corkboard");
    AddPage(m_timeline, "Timeline");
    AddPage(m_files, "Files");

    Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &amOutline::OnPageChange, this);
}

void amOutline::SaveOutline() {
    m_corkboard->Save();
    m_timeline->Save();
    m_files->Save();
}

void amOutline::LoadOutline(wxStringInputStream& corkboard,
    wxStringInputStream& timeline,
    wxStringInputStream& files) {

    m_corkboard->Load(corkboard);
    m_timeline->Load(timeline);
    m_files->Load(files);
}

void amOutline::OnShow() {
    wxBookCtrlEvent event;
    event.SetSelection(GetSelection());

    OnPageChange(event);
}

void amOutline::OnPageChange(wxBookCtrlEvent& event) {
    switch (event.GetSelection()) {
    case 0:
        AutoWrapTextShape::ShouldCountLines(true);
        m_corkboard->Refresh(true);
        break;

    case 1:
        AutoWrapTextShape::ShouldCountLines(true);
        m_timeline->Refresh(true);
        break;

    default:
        AutoWrapTextShape::ShouldCountLines(false);
        break;
    }

    event.Skip();
}

void amOutline::ClearAll() {
    m_corkboard->getCanvas()->GetDiagramManager()->Clear();
    m_corkboard->getCanvas()->Refresh(true);

    m_files->ClearAll();
}