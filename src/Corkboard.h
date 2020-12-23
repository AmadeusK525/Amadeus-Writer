#ifndef CORKBOARD_H_
#define CORKBOARD_H_

#pragma once

#include <wx/panel.h>
#include <wx/toolbar.h>
#include <wx/dcclient.h>

#include <wx\wxsf\wxShapeFramework.h>

#include "MainFrame.h"
#include "Outline.h"

#include <vector>
#include <memory>

class Corkboard;
class CorkboardCanvas;

using std::vector;

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

class Corkboard : public wxPanel {
private:
    Outline* parent = nullptr;
    wxToolBar* toolBar = nullptr;

    wxBoxSizer* corkboardSizer = nullptr;

    CorkboardCanvas* canvas = nullptr;
    wxSFDiagramManager manager;

    ToolMode toolMode = modeDESIGN;
    bool isDraggingRight = false;

    int currentImage = 1;

public:
    Corkboard(wxWindow* parent);

    void onTool(wxCommandEvent& event);

    void callFullScreen(wxCommandEvent& event);
    void fullScreen(bool fs);

    void setToolMode(ToolMode mode);
    ToolMode getToolMode() { return toolMode; }
    wxToolBar* getToolbar() { return toolBar; }

    void exportToImage(wxBitmapType type);

    void save();
    void load();
    
    CorkboardCanvas* getCanvas() { return canvas; }

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

class CorkboardCanvas : public wxSFShapeCanvas {
private:
	Corkboard* parent = nullptr;
	MainFrame* mainFrame = nullptr;

	wxSFShapeBase* shapeForMenu = nullptr;

	bool m_isFullScreen = false;
	bool m_isConnecting = false;
	bool m_showShadows = false;

	bool m_beginDraggingRight = false;
	bool m_isDraggingRight = false;
	wxPoint m_downPos{};
	wxPoint m_scrollbarPos{};

public:
	CorkboardCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id = -1,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxHSCROLL | wxVSCROLL);

	void doFullScreen(bool fs);
	void onMenu(wxCommandEvent& event);

	virtual void OnLeftDown(wxMouseEvent& event);
	virtual void OnLeftUp(wxMouseEvent& event);

	virtual void OnRightDown(wxMouseEvent& event);
	virtual void OnRightUp(wxMouseEvent& event);

	virtual void OnUpdateVirtualSize(wxRect& vrtrect);
	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnMouseWheel(wxMouseEvent& event);
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);

	virtual void OnKeyDown(wxKeyEvent& event);

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