#ifndef OUTLINE_H_
#define OUTLINE_H_
#pragma once

#include <wx/aui/aui.h>
#include <wx/panel.h>
#include <wx/progdlg.h>
#include <wx/splitter.h>

#include <wx/sstream.h>

#include "ProjectManager.h"

class amOutline;
class amCorkboard;
class amTLTimeline;
class amOutlineFilesPanel;

class amOutline : public wxAuiNotebook
{
private:
	amCorkboard* m_corkboard = nullptr;
	amTLTimeline* m_timeline = nullptr;
	amOutlineFilesPanel* m_files = nullptr;

public:
	// Hack for doing corkboard fullscreen
	wxPanel* m_corkboardHolder = nullptr,
		* m_timelineHolder = nullptr;

	wxBoxSizer* m_corkHolderSizer = nullptr,
		* m_timelineHolderSizer = nullptr;

public:
	amOutline(wxWindow* parent);

	void SaveOutline();
	void LoadOutline(amProjectSQLDatabase* db);

	inline amCorkboard* GetCorkboard() { return m_corkboard; }
	inline amTLTimeline* GetTimeline() { return m_timeline; }
	inline amOutlineFilesPanel* GetOutlineFiles() { return m_files; }

	inline wxPanel* GetCorkboardHolder() { return m_corkboardHolder; }
	inline wxBoxSizer* GetCorkHolderSizer() { return m_corkHolderSizer; }
	inline wxPanel* GetTimelineHolder() { return m_timelineHolder; }
	inline wxBoxSizer* GetTimelineHolderSizer() { return m_timelineHolderSizer; }

	void ClearAll();
};

#endif