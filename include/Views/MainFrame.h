#ifndef MAINFRAME_H_
#define MAINFRAME_H_

#pragma once

#include <wx/wx.h>
#include <wx/simplebook.h>
#include <wx/splitter.h>

#include "Utils/amUtility.h"
#include "ProjectManaging.h"

class amMainFrame : public wxFrame
{
private:
	wxPanel* m_mainPanel = nullptr,
		* m_panel = nullptr;

	amSplitterWindow* m_mainSplitter = nullptr;

	amOverview* m_overview = nullptr;
	amElementNotebook* m_elementNotebook = nullptr;
	amStoryNotebook* m_storyNotebook = nullptr;
	amOutline* m_outline = nullptr;
	amRelease* m_release = nullptr;

	wxVector<wxButton*> m_mainButtons;
	wxPanel* m_selPanel = nullptr;

	wxSimplebook* m_mainBook = nullptr;

	wxBoxSizer* m_holderSizer = nullptr;
	bool m_isFrameFullScreen = false;

	wxToolBar* m_toolBar = nullptr;

	wxBoxSizer* m_mainSizer = nullptr,
		* m_verticalSizer = nullptr,
		* m_buttonSizer = nullptr;

	wxTimer m_timer{ this, -1 };

public:
	amMainFrame(const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize);

	amOverview* GetOverview();
	amElementNotebook* GetElementsNotebook();
	amStoryNotebook* GetStoryNotebook();
	amOutline* GetOutline();
	amRelease* GetRelease();

	wxSimplebook* GetSimplebook() { return m_mainBook; }

	void ClearAll();

	void OnQuit(wxCommandEvent& event);
	void OnNewFile(wxCommandEvent& event);
	void OnOpenFile(wxCommandEvent& event);
	void OnSaveFile(wxCommandEvent& event);
	void OnSaveFileAs(wxCommandEvent& event);
	void OnExportCorkboard(wxCommandEvent& event);

	void EditTitle(wxCommandEvent& event);

	void FullScreen(wxCommandEvent& event);
	void OnNewDocument(wxCommandEvent& event);
	void OnNewCharacter(wxCommandEvent& event);
	void OnNewLocation(wxCommandEvent& event);
	void OnNewItem(wxCommandEvent& event);

	void UpdateElements(wxCommandEvent& event);

	void OnAbout(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);

	void OnMainButtonEnter(wxMouseEvent& event);
	void OnMainButtonLeave(wxMouseEvent& event);
	void OnMainButtons(wxCommandEvent& event);

	void OnSashChanged(wxSplitterEvent& event);

	void Search(wxCommandEvent& event);

	inline wxPanel* GetMainPanel() { return m_mainPanel; }
	inline amSplitterWindow* GetMainSplitter() { return m_mainSplitter; }
	inline wxBoxSizer* GetHolderSizer() { return m_holderSizer; }
	inline bool IsFrameFullScreen() { return m_isFrameFullScreen; }

	DECLARE_EVENT_TABLE()
};

enum
{

	MAIN_Panel = wxID_HIGHEST + 1,

	MENU_NewDocument,
	MENU_NewCharacter,
	MENU_NewLocation,
	MENU_NewItem,

	TOOL_NewDocument,
	TOOL_NewCharacter,
	TOOL_NewLocation,
	TOOL_NewItem,
	TOOL_Search,
	TOOL_Save,
	TOOL_FullScreen,

	MENU_New,
	MENU_Open,
	MENU_Save,
	MENU_SaveAs,
	MENU_Quit,

	MENU_ExportCorkboardPNG,
	MENU_ExportCorkboardBMP,

	MENU_Update,
	MENU_ProjectName,

	MENU_About,

	BUTTON_Overview,
	BUTTON_Elem,
	BUTTON_Documents,
	BUTTON_Release,
	BUTTON_Outline
};
#endif