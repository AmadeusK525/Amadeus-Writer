#include "CorkboardCanvas.h"

#include <wx\wx.h>

CorkboardCanvas::CorkboardCanvas(wxSFDiagramManager* manager, wxWindow* parent,
	wxWindowID id, const wxPoint& pos, const wxSize& size, long style) :
	wxSFShapeCanvas(manager, parent, id, pos, size, style) {

	// Fill background with gradient color.
	AddStyle(sfsGRADIENT_BACKGROUND);
	SetGradientFrom(wxColour(255, 240, 200));
	SetGradientTo(wxColour(255, 213, 121));

	// Canvas background can be printed/ommited during the canvas printing job.
	AddStyle(sfsPRINT_BACKGROUND);

	// Disable mouse wheel scrolling so it can be used for zooming.
	EnableScrolling(false, false);
	
	// Set distance between grid lines
	SetGridLineMult(10);

	// Process mousewheel for zoom scrolling and set min and max scale.
	AddStyle(sfsPROCESS_MOUSEWHEEL);
	SetMinScale(0.1);
	SetMaxScale(2);

	// Show shadows only on the topmost shapes.
	ShowShadows(true, shadowTOPMOST);

	// Specify accepted shapes.
	GetDiagramManager()->ClearAcceptedShapes();
	GetDiagramManager()->AcceptShape(wxT("All"));

	Bind(wxEVT_MOUSE_CAPTURE_LOST, &CorkboardCanvas::OnMouseCaptureLost, this);
}

void CorkboardCanvas::OnRightDown(wxMouseEvent& event) {
	// Set begin dragging state as true and calculate current mouse position and current
	// scrollbar position.
	m_beginDraggingRight = true;
	m_downPos = wxGetMousePosition();
	CalcUnscrolledPosition(0, 0, &m_scrollbarPos.x, &m_scrollbarPos.y);

	CaptureMouse();

	// Call default behaviour.
	wxSFShapeCanvas::OnRightDown(event);
	event.Skip();
}

void CorkboardCanvas::OnRightUp(wxMouseEvent& event) {
	// Reset everything.
	if (m_isDraggingRight) {
		m_isDraggingRight = false;
		SetCursor(wxCursor(wxCURSOR_DEFAULT));
		ReleaseCapture();
	} else {

	}

	m_beginDraggingRight = false;

	// Call default behaviour.
	wxSFShapeCanvas::OnRightUp(event);
	event.Skip();
}

void CorkboardCanvas::OnMouseMove(wxMouseEvent& event) {
	// If right mouse button has been clicked, calculate drag and begin.

	if (m_beginDraggingRight) {
		m_isDraggingRight = true;
		m_beginDraggingRight = false;
		SetCursor(wxCursor(wxCURSOR_CLOSED_HAND));
	}

	if (m_isDraggingRight) {
		wxPoint toMove = wxGetMousePosition();
		toMove = m_downPos - toMove;

		Scroll((m_scrollbarPos.x + toMove.x) / 5, (m_scrollbarPos.y + toMove.y) / 5);
	}

	// Call default behaviour.
	wxSFShapeCanvas::OnMouseMove(event);
	event.Skip();
}

void CorkboardCanvas::OnMouseWheel(wxMouseEvent& event) {
	// Call default behaviour.
	wxSFShapeCanvas::OnMouseWheel(event);

	// Calculate and apply zoom.
	float newScale = GetScale() + (float)event.GetWheelRotation() / (1500.0 / GetScale());

	if (newScale <= 0.05)
		SetScale(0.05);
	else if (newScale >= 2)
		SetScale(2);
	else
		SetScale(newScale);

	Refresh(false);
	event.Skip();
}

void CorkboardCanvas::OnKeyDown(wxKeyEvent& event) {
	switch (event.GetKeyCode()) {
	case WXK_F11:

		break;
	}

	// Call default behaviour.
	wxSFShapeCanvas::OnKeyDown(event);
	event.Skip();
}

void CorkboardCanvas::OnMouseCaptureLost(wxMouseCaptureLostEvent& event) {
	m_isDraggingRight = false;
	event.Skip();
}
