#include "Outline.h"

#include "MainFrame.h"
#include "Corkboard.h"
#include "Timeline.h"
#include "OutlineFiles.h"

#include "wxmemdbg.h"

amOutline::amOutline(wxWindow* parent) : wxAuiNotebook(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
	wxAuiSimpleTabArt* pArt = new wxAuiSimpleTabArt();
	pArt->SetColour(wxColour(50, 50, 50));
	pArt->SetActiveColour(wxColour(30, 30, 30));
	SetArtProvider(pArt);

	GetAuiManager().GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
	GetAuiManager().GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);

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

void amOutline::SaveOutline()
{
	m_corkboard->Save();
	m_timeline->Save();
	m_files->Save();
}

void amOutline::LoadOutline(am::ProjectSQLDatabase* db)
{
	m_corkboard->Load(db);
	m_timeline->Load(db);
	m_files->Load(db);
}

void amOutline::ClearAll()
{
	m_corkboard->getCanvas()->GetDiagramManager()->Clear();
	m_corkboard->getCanvas()->Refresh(true);

	m_files->ClearAll();
}