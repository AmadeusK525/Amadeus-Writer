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

	bool m_isFullScreen = false;

	bool m_beginDraggingRight = false;
	bool m_isDraggingRight = false;
	wxPoint m_downPos{};
	wxPoint m_scrollbarPos{};

public:
	CorkboardCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id = -1,
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxHSCROLL | wxVSCROLL);

	void doFullScreen(bool fs);

	virtual void OnRightDown(wxMouseEvent& event);
	virtual void OnRightUp(wxMouseEvent& event);
	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnMouseWheel(wxMouseEvent& event);
	virtual void OnKeyDown(wxKeyEvent& event);

	void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
};
#endif