#ifndef CORKBOARDCANVAS_H_
#define CORKBOARDCANVAS_H_

#pragma once

#include <wx\wxsf\wxShapeFramework.h>
#include "Corkboard.h"
#include "MainFrame.h"

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
	//wxPoint curMousePos{};

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