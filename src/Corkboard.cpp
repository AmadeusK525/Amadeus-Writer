#include "Corkboard.h"

#include "MyApp.h"
#include "MainFrame.h"
#include "ImagePanel.h"
#include "Outline.h"
#include "NoteShape.h"
#include "ImageShape.h"

#include "wxmemdbg.h"

namespace fs = boost::filesystem;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Corkboard ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(Corkboard, wxPanel)

EVT_TOOL_RANGE(TOOL_Cursor, TOOL_CorkboardFullScreen, Corkboard::onTool)

END_EVENT_TABLE()

Corkboard::Corkboard(wxWindow* parent) : wxPanel(parent) {
    this->parent = (Outline*)(parent->GetParent());

    toolBar = new wxToolBar(this, -1);
    toolBar->AddRadioTool(TOOL_Cursor, "", wxBITMAP_PNG(cursor), wxNullBitmap, "Default");
    toolBar->AddRadioTool(TOOL_NewNote, "", wxBITMAP_PNG(newNote), wxNullBitmap, "New note");
    toolBar->AddRadioTool(TOOL_NewImage, "", wxBITMAP_PNG(newImage), wxNullBitmap, "New image");
    toolBar->AddRadioTool(TOOL_NewText, "", wxBITMAP_PNG(newText), wxNullBitmap, "New Text");
    toolBar->AddRadioTool(TOOL_NewConnection, "", wxBITMAP_PNG(connectionLine), wxNullBitmap, "New connection");
    toolBar->AddSeparator();
    toolBar->AddSeparator();
    toolBar->AddCheckTool(TOOL_CorkboardFullScreen, "", wxBITMAP_PNG(fullScreenPng), wxNullBitmap, "Full screen");
    toolBar->Realize();
    toolBar->SetBackgroundColour(wxColour(140, 140, 140));

    SetBackgroundColour(wxColour(0, 0, 0));

    manager.AcceptShape("All");
    canvas = new CorkboardCanvas(&manager, this);

    corkboardSizer = new wxBoxSizer(wxVERTICAL);
    corkboardSizer->Add(toolBar, wxSizerFlags(0).Expand().Border(wxALL, 1));
    corkboardSizer->Add(canvas, wxSizerFlags(1).Expand());

    SetSizer(corkboardSizer);
}

void Corkboard::onTool(wxCommandEvent& event) {
    if (canvas->GetMode() == CorkboardCanvas::modeCREATECONNECTION)
        canvas->AbortInteractiveConnection();

    switch (event.GetId()) {
    case TOOL_Cursor:
        toolMode = modeDESIGN;
        break;

    case TOOL_NewNote:
        toolMode = modeNOTE;
        break;

    case TOOL_NewImage:
        toolMode = modeIMAGE;
        break;

    case TOOL_NewText:
        toolMode = modeTEXT;
        break;

    case TOOL_NewConnection:
        toolMode = modeCONNECTION;
        break;
    
    case TOOL_CorkboardFullScreen:
        callFullScreen(event);
        break;
    }
}

void Corkboard::callFullScreen(wxCommandEvent& event) {
    wxKeyEvent keyEvent;
    keyEvent.m_keyCode = WXK_F11;

    canvas->OnKeyDown(keyEvent);
}

void Corkboard::fullScreen(bool fs) {
    AutoWrapTextShape::willCountLines(false);

    if (fs) {
        parent->corkHolderSizer->Remove(0);
        Reparent(wxGetApp().GetTopWindow());
    } else {
        Reparent(parent->corkHolder);
        parent->corkHolderSizer->Add(this, wxSizerFlags(1).Expand());
    }

    toolBar->ToggleTool(TOOL_CorkboardFullScreen, fs);
    canvas->Layout();

    //AutoWrapTextShape::willCountLines(true);
}

void Corkboard::setToolMode(ToolMode mode) {
    switch (mode) {
    case modeDESIGN:
        toolBar->ToggleTool(TOOL_Cursor, true);
        break;
    case modeNOTE:
        toolBar->ToggleTool(TOOL_NewNote, true);
        break;
    case modeIMAGE:
        toolBar->ToggleTool(TOOL_NewImage, true);
        break;
    case modeTEXT:
        toolBar->ToggleTool(TOOL_NewText, true);
        break;
    case modeCONNECTION:
        toolBar->ToggleTool(TOOL_NewConnection, true);
        break;
    }

    toolMode = mode;
}

void Corkboard::exportToImage(wxBitmapType type) {
	string name = "Corkboard";
	string dummyName = name;

	int i = 0;
	while (fs::exists(MainFrame::currentDocFolder + "\\Images\\Corkboard\\" + dummyName)) {
		dummyName = dummyName + " (" + std::to_string(i++) + ")";
	}

	name = dummyName;

    switch (type) {
    case wxBITMAP_TYPE_PNG:
        canvas->SaveCanvasToImage(MainFrame::currentDocFolder +
            "\\Images\\Corkboard\\" + name + ".png",
            type, true, 1.0);
        break;
    case wxBITMAP_TYPE_BMP:
        canvas->SaveCanvasToImage(MainFrame::currentDocFolder +
            "\\Images\\Corkboard\\" + name  + ".bmp",
            type, true, 1.0);
        break;
    }
}

void Corkboard::save() {
    canvas->SaveCanvas(MainFrame::currentDocFolder + "\\Files\\Outline\\Corkboard Canvas.xml");
}

void Corkboard::load() {
    if (fs::exists(MainFrame::currentDocFolder + "\\Files\\Outline\\Corkboard Canvas.xml")) {
        canvas->LoadCanvas(MainFrame::currentDocFolder + "\\Files\\Outline\\Corkboard Canvas.xml");
        canvas->SetScale(0.1);
        canvas->RefreshCanvas(true, canvas->GetTotalBoundingBox());
    } else
        canvas->GetDiagramManager()->Clear();
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Corkboard Canvas ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

CorkboardCanvas::CorkboardCanvas(wxSFDiagramManager* manager, wxWindow* parent,
	wxWindowID id, const wxPoint& pos, const wxSize& size, long style) :
	wxSFShapeCanvas(manager, parent, id, pos, size, style) {

	mainFrame = (MainFrame*)wxGetApp().GetTopWindow();
	this->parent = (Corkboard*)parent;

	// Remove grid lines.
	RemoveStyle(sfsGRID_SHOW);
	RemoveStyle(sfsGRID_USE);
	RemoveStyle(sfsMULTI_SIZE_CHANGE);

	// Fill background with gradient color.
	AddStyle(sfsGRADIENT_BACKGROUND);
	SetGradientFrom(wxColour(255, 250, 220));
	SetGradientTo(wxColour(240, 150, 70));

	// Canvas background can be printed/ommited during the canvas printing job.
	AddStyle(sfsPRINT_BACKGROUND);

	// Edited default shadow.
	SetShadowFill(wxBrush(wxColour(110, 70, 60)));
	//SetShadowOffset(wxRealPoint(4.5, 4.5));

	// Disable mouse wheel scrolling so it can be used for zooming.
	EnableScrolling(false, false);

	// Process mousewheel for zoom scrolling and set min and max scale.
	AddStyle(sfsPROCESS_MOUSEWHEEL);
	SetMinScale(0.1);
	SetMaxScale(2);
	SetScale(0.75);

	// Specify accepted shapes.
	GetDiagramManager()->ClearAcceptedShapes();
	GetDiagramManager()->AcceptShape("All");

	Bind(wxEVT_MOUSE_CAPTURE_LOST, &CorkboardCanvas::OnMouseCaptureLost, this);

	// Use wxGC for drawing everything. This looks a lot nicer, but may be
	// slower, but imo definitely worth keeping on.
	wxSFShapeCanvas::EnableGC(true);
}

void CorkboardCanvas::doFullScreen(bool fs) {
	if (fs) {
		parent->fullScreen(fs);
		mainFrame->corkboardFullScreen(fs, parent->getToolbar(), this);
	} else {
		mainFrame->corkboardFullScreen(fs, parent->getToolbar(), this);
		parent->fullScreen(fs);
	}

	m_isFullScreen = !m_isFullScreen;
}

void CorkboardCanvas::onMenu(wxCommandEvent& event) {
	wxWindowID id = event.GetId();

	if (id == MENU_NoteBlack || id == MENU_NoteBlue || id == MENU_NoteDefault ||
		id == MENU_NoteGreen || id == MENU_NotePink || id == MENU_NoteRed ||
		id == MENU_NoteWhite || id == MENU_NoteYellow)
		((NoteShape*)shapeForMenu)->changeColour(id);
	else if (id == MENU_DeleteNote || id == MENU_DeleteImage)
		GetDiagramManager()->RemoveShape(shapeForMenu);

	shapeForMenu = nullptr;
	SaveCanvasState();
}

void CorkboardCanvas::OnLeftDown(wxMouseEvent& event) {
	AutoWrapTextShape::willCountLines(false);

	switch (parent->getToolMode()) {
	case modeNOTE:
	{
		NoteShape* shape = (NoteShape*)GetDiagramManager()->AddShape(CLASSINFO(NoteShape),
			ScreenToClient(wxGetMousePosition()));

		shape->AcceptConnection("All");
		shape->AcceptSrcNeighbour("All");
		shape->AcceptTrgNeighbour("All");

		// Show shadows only on the topmost shapes.
		ShowShadows(m_showShadows, wxSFShapeCanvas::shadowTOPMOST);

		AutoWrapTextShape::willCountLines(true);
		// ... and then perform standard operations provided by the shape canvas:
		Refresh(false);
		parent->setToolMode(modeDESIGN);
		SaveCanvasState();

		MainFrame::isSaved = false;
		break;
	}
	case modeIMAGE:
	{
		wxFileDialog dlg(this, wxT("Load image..."), wxGetCwd(), wxT(""),
			wxT("BMP Files, JPEG Files, JPG Files,  PNG Files (*.bmp;*.jpeg;*.jpg;*.png)|*.bmp;*.jpeg;*.jpg;*.png"),
			wxFD_FILE_MUST_EXIST);

		if (dlg.ShowModal() == wxID_OK) {
			ImageShape* shape = (ImageShape*)(GetDiagramManager()->AddShape(CLASSINFO(ImageShape),
				event.GetPosition(), sfDONT_SAVE_STATE));

			if (shape) {
				// Create image from file.
				shape->create(dlg.GetPath(), wxBITMAP_TYPE_ANY);

				wxRect bb = shape->GetBoundingBox();
				double ratio;

				if (bb.width > bb.height)
					ratio = 250.0 / (double)bb.width;
				else
					ratio = 250.0 / (double)bb.height;

				shape->Scale(ratio, ratio);

				// Set shape policy.
				shape->AcceptConnection("All");
				shape->AcceptSrcNeighbour("All");
				shape->AcceptTrgNeighbour("All");
				shape->AcceptChild("All");
			}
		}

		// Show shadows only on the topmost shapes.
		ShowShadows(m_showShadows, wxSFShapeCanvas::shadowTOPMOST);

		// ... and then perform standard operations provided by the shape canvas:
		Refresh(false);
		SaveCanvasState();
		parent->setToolMode(modeDESIGN);

		MainFrame::isSaved = false;
		break;
	}

	case modeTEXT:
	{
		wxSFEditTextShape* shape = (wxSFEditTextShape*)GetDiagramManager()->AddShape(CLASSINFO(wxSFEditTextShape),
			event.GetPosition());

		shape->AcceptConnection("All");
		shape->AcceptSrcNeighbour("All");
		shape->AcceptTrgNeighbour("All");

		shape->RemoveStyle(wxSFEditTextShape::sfsSHOW_SHADOW);

		shape->SetText("New text");

		// Show shadows only on the topmost shapes.
		ShowShadows(m_showShadows, wxSFShapeCanvas::shadowTOPMOST);

		// ... and then perform standard operations provided by the shape canvas:
		Refresh(false);
		parent->setToolMode(modeDESIGN);
		SaveCanvasState();

		MainFrame::isSaved = false;
		break;
	}
	case modeCONNECTION:
		if (GetMode() == modeREADY) {
			StartInteractiveConnection(CLASSINFO(wxSFCurveShape), event.GetPosition());
			AutoWrapTextShape::willCountLines(false);
			m_isConnecting = true;
		} else
			wxSFShapeCanvas::OnLeftDown(event);

		SaveCanvasState();
		break;
	case modeDESIGN:
		wxSFShapeCanvas::OnLeftDown(event);
		break;
	}
}

void CorkboardCanvas::OnLeftUp(wxMouseEvent& event) {
	//if (!m_isConnecting)
		//AutoWrapTextShape::willCountLines(true);

	wxSFShapeCanvas::OnLeftUp(event);
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
	} else {
		shapeForMenu = GetShapeUnderCursor();

		if (shapeForMenu) {
			wxClassInfo* classInfo = shapeForMenu->GetClassInfo();
			wxMenu menu;
			if (classInfo == CLASSINFO(NoteShape)) {
				menu.AppendRadioItem(MENU_NoteDefault, "Default");
				menu.AppendRadioItem(MENU_NoteBlack, "Black");
				menu.AppendRadioItem(MENU_NoteWhite, "White");
				menu.AppendRadioItem(MENU_NoteRed, "Red");
				menu.AppendRadioItem(MENU_NoteGreen, "Green");
				menu.AppendRadioItem(MENU_NoteBlue, "Blue");
				menu.AppendRadioItem(MENU_NoteYellow, "Yellow");
				menu.AppendRadioItem(MENU_NotePink, "Pink");
				menu.AppendSeparator();
				menu.Append(MENU_DeleteNote, "Delete");

				menu.Check(((NoteShape*)shapeForMenu)->curColour, true);
			} else if (classInfo == CLASSINFO(ImageShape)) {
				menu.Append(MENU_DeleteImage, "Delete");
			}

			menu.Bind(wxEVT_MENU, &CorkboardCanvas::onMenu, this);
			PopupMenu(&menu);
		}
	}

	if (HasCapture())
		ReleaseCapture();

	m_beginDraggingRight = false;

	// Call default behaviour.
	//wxSFShapeCanvas::OnRightUp(event);
	event.Skip();
}

void CorkboardCanvas::OnUpdateVirtualSize(wxRect& vrtrect) {
	vrtrect.width += 500;
	vrtrect.height += 500;
}

void CorkboardCanvas::OnMouseMove(wxMouseEvent& event) {
	// If right mouse button has been clicked, calculate drag and begin.

	if (m_beginDraggingRight) {
		AutoWrapTextShape::willCountLines(false);
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
	AutoWrapTextShape::willCountLines(false);

	// Call default behaviour.
	event.SetControlDown(true);
	wxSFShapeCanvas::OnMouseWheel(event);
	event.SetControlDown(false);

	//int xs, ys;

	//GetScrollPixelsPerUnit(&xs, &ys);

	//wxPoint pos = ScreenToClient(wxGetMousePosition());
	//Scroll(pos.x / xs, pos.y / ys);
}

void CorkboardCanvas::OnMouseCaptureLost(wxMouseCaptureLostEvent& event) {
	m_isDraggingRight = false;
}

void CorkboardCanvas::OnKeyDown(wxKeyEvent& event) {
	switch (event.GetKeyCode()) {
	case WXK_F11:
		doFullScreen(!m_isFullScreen);
		break;
	case WXK_ESCAPE:
		if (m_isConnecting)
			break;
		if (m_isFullScreen) {
			doFullScreen(false);
			m_isFullScreen = false;
		}

		break;
	case WXK_DELETE:
		if (m_isConnecting) {
			event.m_keyCode = WXK_ESCAPE;
		}
		MainFrame::isSaved = false;
		break;

	case WXK_NUMPAD1:
	case 49:
	case 99:
	case 67:
		parent->setToolMode(modeDESIGN);
		break;

	case WXK_NUMPAD2:
	case 50:
	case 110:
	case 78:
		parent->setToolMode(modeNOTE);
		break;

	case WXK_NUMPAD3:
	case 51:
	case 105:
	case 73:
		parent->setToolMode(modeIMAGE);
		break;

	case WXK_NUMPAD4:
	case 52:
	case 116:
	case 84:
		parent->setToolMode(modeTEXT);
		break;

	case WXK_NUMPAD5:
	case 53:
	case 108:
	case 76:
		parent->setToolMode(modeCONNECTION);
		break;
	}

	// Call default behaviour.
	wxSFShapeCanvas::OnKeyDown(event);
}

void CorkboardCanvas::OnConnectionFinished(wxSFLineShape* connection) {
	if (connection) {
		connection->SetLinePen(wxPen(wxColour(150, 0, 0), 3));
		connection->SetTrgArrow(CLASSINFO(wxSFSolidArrow));

		connection->AcceptChild("wxSFTextShape");
		connection->AcceptChild("wxSFEditTextShape");

		connection->AcceptConnection("NoteShape");
		connection->AcceptConnection("ImageShape");
		connection->AcceptConnection("wxSFEditTextShape");
		connection->AcceptConnection("wxSFCurveShape");

		connection->AcceptSrcNeighbour("NoteShape");
		connection->AcceptSrcNeighbour("ImageShape");
		connection->AcceptSrcNeighbour("wxSFEditTextShape");
		connection->AcceptSrcNeighbour("wxSFCurveShape");

		connection->AcceptTrgNeighbour("NoteShape");
		connection->AcceptTrgNeighbour("ImageShape");
		connection->AcceptTrgNeighbour("wxSFEditTextShape");
		connection->AcceptTrgNeighbour("wxSFCurveShape");
		//connection->AcceptSrcNeighbour("All");
		//connection->AcceptTrgNeighbour("All");

		m_isConnecting = false;
		MainFrame::isSaved = false;
	}

	parent->setToolMode(modeDESIGN);
	wxSFShapeCanvas::OnConnectionFinished(connection);
}