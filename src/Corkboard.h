#ifndef CORKBOARD_H_
#define CORKBOARD_H_

#pragma once

#include <wx/panel.h>
#include <wx/toolbar.h>
#include <wx/dcclient.h>

#include <wx\wxsf\wxShapeFramework.h>

#include "MainFrame.h"
#include "Outline.h"
#include "amUtility.h"

#include <memory>

class amCorkboard;

enum ToolMode {
    modeDESIGN,
    modeNOTE,
    modeIMAGE,
    modeTEXT,
    modeCONNECTION
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Corkboard ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

class amCorkboard : public wxPanel {
public:
	friend class CorkboardCanvas;

private:
	amProjectManager* m_manager = nullptr;
    amOutline* m_parent = nullptr;
    wxToolBar* m_toolBar = nullptr;

    wxBoxSizer* m_corkboardSizer = nullptr;

    CorkboardCanvas* m_canvas = nullptr;
    wxSFDiagramManager m_canvasManager;

    ToolMode m_toolMode = modeDESIGN;
    bool m_isDraggingRight = false;

    int m_currentImage = 1;

	bool m_isSaving = false;

public:
    amCorkboard(wxWindow* parent);

    void OnTool(wxCommandEvent& event);

    void CallFullScreen(wxCommandEvent& event);
    void FullScreen(bool fs);

    void SetToolMode(ToolMode mode);
    ToolMode getToolMode() { return m_toolMode; }
    wxToolBar* getToolbar() { return m_toolBar; }

    void ExportToImage(wxBitmapType type);

    void Save();
    void Load(wxStringInputStream& stream);
    
    CorkboardCanvas* getCanvas() { return m_canvas; }

	enum {
		TOOL_Cursor,
		TOOL_NewNote,
		TOOL_NewImage,
		TOOL_NewText,
		TOOL_NewConnection,
		TOOL_CorkboardFullScreen
	};

    DECLARE_EVENT_TABLE()
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Corkboard Canvas ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

class CorkboardCanvas : public amSFShapeCanvas {
private:
	amCorkboard* parent = nullptr;
	amProjectManager* m_manager = nullptr;

	wxSFShapeBase* shapeForMenu = nullptr;

	bool m_isFullScreen = false;
	bool m_isConnecting = false;
	bool m_showShadows = false;

public:
	CorkboardCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id = -1,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxHSCROLL | wxVSCROLL);

	void DoFullScreen(bool fs);
	void OnMenu(wxCommandEvent& event);

	virtual void OnLeftDown(wxMouseEvent& event);
	virtual void OnLeftUp(wxMouseEvent& event);

	virtual void OnRightUp(wxMouseEvent& event);

	virtual void OnUpdateVirtualSize(wxRect& vrtrect);
	virtual void OnKeyDown(wxKeyEvent& event);
	virtual void OnTextChange(wxSFEditTextShape* shape);
	virtual void OnConnectionFinished(wxSFLineShape* connection);

	enum {
		MENU_DeleteNote,
		MENU_DeleteImage,

		MENU_NoteRed,
		MENU_NoteGreen,
		MENU_NoteBlue,
		MENU_NotePink,
		MENU_NoteYellow,
		MENU_NoteWhite,
		MENU_NoteBlack,
		MENU_NoteDefault
	};
};
#endif