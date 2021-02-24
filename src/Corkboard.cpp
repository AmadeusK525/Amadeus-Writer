#include "Corkboard.h"

#include "MyApp.h"
#include "MainFrame.h"
#include "ImagePanel.h"
#include "Outline.h"
#include "CorkboardShapes.h"
#include "wxmemdbg.h"

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Corkboard ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(amCorkboard, wxPanel)

EVT_TOOL_RANGE(TOOL_Cursor, TOOL_CorkboardFullScreen, amCorkboard::OnTool)

END_EVENT_TABLE()

amCorkboard::amCorkboard(wxWindow* parent) : wxPanel(parent) {
    this->m_parent = (amOutline*)(parent->GetParent());
	m_manager = amGetManager();

    m_toolBar = new wxToolBar(this, -1);
    m_toolBar->AddRadioTool(TOOL_Cursor, "", wxBITMAP_PNG(cursor), wxNullBitmap, "Default");
    m_toolBar->AddRadioTool(TOOL_NewNote, "", wxBITMAP_PNG(newNote), wxNullBitmap, "New note");
    m_toolBar->AddRadioTool(TOOL_NewImage, "", wxBITMAP_PNG(newImage), wxNullBitmap, "New image");
    m_toolBar->AddRadioTool(TOOL_NewText, "", wxBITMAP_PNG(newText), wxNullBitmap, "New Text");
    m_toolBar->AddRadioTool(TOOL_NewConnection, "", wxBITMAP_PNG(connectionLine), wxNullBitmap, "New connection");
    m_toolBar->AddSeparator();
    m_toolBar->AddSeparator();
    m_toolBar->AddCheckTool(TOOL_CorkboardFullScreen, "", wxBITMAP_PNG(fullScreenPng), wxNullBitmap, "Full screen");
    m_toolBar->Realize();
    m_toolBar->SetBackgroundColour(wxColour(140, 140, 140));

    SetBackgroundColour(wxColour(0, 0, 0));

    m_canvasManager.AcceptShape("All");
    m_canvas = new CorkboardCanvas(&m_canvasManager, this);

    m_corkboardSizer = new wxBoxSizer(wxVERTICAL);
    m_corkboardSizer->Add(m_toolBar, wxSizerFlags(0).Expand().Border(wxALL, 1));
    m_corkboardSizer->Add(m_canvas, wxSizerFlags(1).Expand());

    SetSizer(m_corkboardSizer);
}

void amCorkboard::OnTool(wxCommandEvent& event) {
    if (m_canvas->GetMode() == CorkboardCanvas::modeCREATECONNECTION)
        m_canvas->AbortInteractiveConnection();

    switch (event.GetId()) {
    case TOOL_Cursor:
        m_toolMode = modeDESIGN;
        break;

    case TOOL_NewNote:
        m_toolMode = modeNOTE;
        break;

    case TOOL_NewImage:
        m_toolMode = modeIMAGE;
        break;

    case TOOL_NewText:
        m_toolMode = modeTEXT;
        break;

    case TOOL_NewConnection:
        m_toolMode = modeCONNECTION;
        break;
    
    case TOOL_CorkboardFullScreen:
        CallFullScreen(event);
        break;
    }
}

void amCorkboard::CallFullScreen(wxCommandEvent& event) {
    wxKeyEvent keyEvent;
    keyEvent.m_keyCode = WXK_F11;

    m_canvas->OnKeyDown(keyEvent);
}

void amCorkboard::FullScreen(bool fs) {
    if (fs) {
		m_parent->m_corkHolderSizer->Remove(0);
        Reparent(m_manager->GetMainFrame());
    } else {
        Reparent(m_parent->m_corkboardHolder);
		m_parent->m_corkHolderSizer->Add(this, wxSizerFlags(1).Expand());
    }

    m_toolBar->ToggleTool(TOOL_CorkboardFullScreen, fs);
    Layout();
	m_canvas->Layout();
	m_corkboardSizer->Layout();
}

void amCorkboard::SetToolMode(ToolMode mode) {
    switch (mode) {
    case modeDESIGN:
        m_toolBar->ToggleTool(TOOL_Cursor, true);
        break;
    case modeNOTE:
        m_toolBar->ToggleTool(TOOL_NewNote, true);
        break;
    case modeIMAGE:
        m_toolBar->ToggleTool(TOOL_NewImage, true);
        break;
    case modeTEXT:
        m_toolBar->ToggleTool(TOOL_NewText, true);
        break;
    case modeCONNECTION:
        m_toolBar->ToggleTool(TOOL_NewConnection, true);
        break;
    }

    m_toolMode = mode;
}

void amCorkboard::ExportToImage(wxBitmapType type) {
	wxString name = "Corkboard";
	wxString dummyName = name;

	int i = 0;
	while (wxFileName::Exists(m_manager->GetPath(true).ToStdString() + "Images\\Corkboard\\" + dummyName.ToStdString())) {
		dummyName = dummyName + " (" + std::to_string(i++) + ")";
	}

	name = dummyName;

    switch (type) {
    case wxBITMAP_TYPE_PNG:
        m_canvas->SaveCanvasToImage(m_manager->GetPath(true) +
            "Images\\Corkboard\\" + name + ".png",
            type, true, 1.0);
        break;
    case wxBITMAP_TYPE_BMP:
        m_canvas->SaveCanvasToImage(m_manager->GetPath(true) +
            "Images\\Corkboard\\" + name  + ".bmp",
            type, true, 1.0);
        break;
    }
}

void amCorkboard::Save() {
	amDocument document;
	document.tableName = "outline_corkboards";
	document.name = "Corkboard Canvas";

	document.strings.push_back(pair<wxString, wxString>("content", wxString()));

	wxStringOutputStream sstream(&document.strings[0].second);
	m_canvas->SaveCanvas(sstream);

	m_manager->SaveDocument(document, document);
}

void amCorkboard::Load(wxStringInputStream& stream) {
	if (stream.CanRead())
		m_canvas->LoadCanvas(stream);
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Corkboard Canvas ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(CorkboardCanvas, wxSFShapeCanvas)
EVT_SCROLLWIN(CorkboardCanvas::OnScroll)
END_EVENT_TABLE()

CorkboardCanvas::CorkboardCanvas(wxSFDiagramManager* canvasManager, wxWindow* parent,
	wxWindowID id, const wxPoint& pos, const wxSize& size, long style) :
	wxSFShapeCanvas(canvasManager, parent, id, pos, size, style) {

	m_manager = amGetManager();
	this->parent = (amCorkboard*)parent;

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

	SetScrollRate(25, 25);

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

void CorkboardCanvas::DoFullScreen(bool fs) {
	if (fs) {
		parent->FullScreen(fs);
		m_manager->GetMainFrame()->DoSubWindowFullScreen(fs, m_parent);
	} else {
		m_manager->GetMainFrame()->DoSubWindowFullScreen(fs, m_parent);
		parent->FullScreen(fs);
	}

	m_isFullScreen = !m_isFullScreen;
}

void CorkboardCanvas::OnMenu(wxCommandEvent& event) {
	wxWindowID id = event.GetId();

	if (id == MENU_NoteBlack || id == MENU_NoteBlue || id == MENU_NoteDefault ||
		id == MENU_NoteGreen || id == MENU_NotePink || id == MENU_NoteRed ||
		id == MENU_NoteWhite || id == MENU_NoteYellow)
		((NoteShape*)shapeForMenu)->ChangeColour(id);
	else if (id == MENU_DeleteNote || id == MENU_DeleteImage)
		GetDiagramManager()->RemoveShape(shapeForMenu);

	shapeForMenu = nullptr;
	SaveCanvasState();
}

void CorkboardCanvas::OnLeftDown(wxMouseEvent& event) {
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

		// ... and then perform standard operations provided by the shape canvas:
		Refresh(false);
		parent->SetToolMode(modeDESIGN);
		SaveCanvasState();

		parent->Save();
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
				shape->Create(dlg.GetPath(), wxBITMAP_TYPE_ANY);

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
		parent->SetToolMode(modeDESIGN);

		parent->Save();
		break;
	}

	case modeTEXT:
	{
		TextShape* shape = (TextShape*)GetDiagramManager()->AddShape(CLASSINFO(TextShape),
			event.GetPosition());

		shape->AcceptConnection("All");
		shape->AcceptSrcNeighbour("All");
		shape->AcceptTrgNeighbour("All");

		shape->Scale(2.0, 2.0);
		shape->RemoveStyle(wxSFEditTextShape::sfsSHOW_SHADOW);

		shape->SetText("New text");

		// Show shadows only on the topmost shapes.
		ShowShadows(m_showShadows, wxSFShapeCanvas::shadowTOPMOST);

		// ... and then perform standard operations provided by the shape canvas:
		Refresh(false);
		parent->SetToolMode(modeDESIGN);
		SaveCanvasState();

		parent->Save();
		break;
	}
	case modeCONNECTION:
		if (GetMode() == modeREADY) {
			StartInteractiveConnection(CLASSINFO(wxSFCurveShape), event.GetPosition());
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
		//AutoWrapTextShape::ShouldCountLines(true);

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

				menu.Check(((NoteShape*)shapeForMenu)->m_currentColour, true);
			} else if (classInfo == CLASSINFO(ImageShape)) {
				menu.Append(MENU_DeleteImage, "Delete");
			}

			menu.Bind(wxEVT_MENU, &CorkboardCanvas::OnMenu, this);
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
		m_isDraggingRight = true;
		m_beginDraggingRight = false;
		SetCursor(wxCursor(wxCURSOR_CLOSED_HAND));
	}

	if (m_isDraggingRight) {
		wxPoint toMove = wxGetMousePosition();
		toMove = m_downPos - toMove;

		Scroll((m_scrollbarPos.x + toMove.x) / 25, (m_scrollbarPos.y + toMove.y) / 25);
	}

	// Call default behaviour.
	wxSFShapeCanvas::OnMouseMove(event);
	event.Skip();
}

void CorkboardCanvas::OnMouseCaptureLost(wxMouseCaptureLostEvent& event) {
	m_isDraggingRight = false;
}

void CorkboardCanvas::OnScroll(wxScrollWinEvent& event) {
	if (wxGetKeyState(WXK_CONTROL))
		return;

	event.Skip();
}

void CorkboardCanvas::OnKeyDown(wxKeyEvent& event) {
	switch (event.GetKeyCode()) {
	case WXK_F11:
		DoFullScreen(!m_isFullScreen);
		break;
	case WXK_ESCAPE:
		if (m_isConnecting)
			break;
		if (m_isFullScreen) {
			DoFullScreen(false);
			m_isFullScreen = false;
		}

		break;
	case WXK_DELETE:
		if (m_isConnecting) {
			event.m_keyCode = WXK_ESCAPE;
		}
		parent->Save();
		break;

	case WXK_NUMPAD1:
	case 49:
	case 99:
	case 67:
		parent->SetToolMode(modeDESIGN);
		break;

	case WXK_NUMPAD2:
	case 50:
	case 110:
	case 78:
		parent->SetToolMode(modeNOTE);
		break;

	case WXK_NUMPAD3:
	case 51:
	case 105:
	case 73:
		parent->SetToolMode(modeIMAGE);
		break;

	case WXK_NUMPAD4:
	case 52:
	case 116:
	case 84:
		parent->SetToolMode(modeTEXT);
		break;

	case WXK_NUMPAD5:
	case 53:
	case 108:
	case 76:
		parent->SetToolMode(modeCONNECTION);
		break;
	}

	// Call default behaviour.
	wxSFShapeCanvas::OnKeyDown(event);
}

void CorkboardCanvas::OnTextChange(wxSFEditTextShape* shape) {
	AutoWrapTextShape* pShape = dynamic_cast<AutoWrapTextShape*>(shape);

	if (pShape)
		pShape->CalcWrappedText();

	wxSFShapeCanvas::OnTextChange(shape);
	parent->Save();
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
		parent->Save();
	}

	parent->SetToolMode(modeDESIGN);
	wxSFShapeCanvas::OnConnectionFinished(connection);
}