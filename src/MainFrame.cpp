#include "MainFrame.h"

#include "Overview.h"
#include "ElementsNotebook.h"
#include "StoryNotebook.h"
#include "Outline.h"
#include "Corkboard.h"
#include "Release.h"
#include "SwitchCtrl.h"

#include "DocumentCreator.h"
#include "ElementCreators.h"

#include "amUtility.h"

#include <wx\richtext\richtextxml.h>
#include <wx\richtext\richtexthtml.h>
#include <wx\aboutdlg.h>

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(amMainFrame, wxFrame)

EVT_MENU(MENU_New, amMainFrame::OnNewFile)
EVT_MENU(MENU_Open, amMainFrame::OnOpenFile)
EVT_MENU(MENU_Save, amMainFrame::OnSaveFile)
EVT_MENU(MENU_SaveAs, amMainFrame::OnSaveFileAs)
EVT_MENU(MENU_ExportCorkboardPNG, amMainFrame::OnExportCorkboard)
EVT_MENU(MENU_ExportCorkboardBMP, amMainFrame::OnExportCorkboard)
EVT_MENU(MENU_Quit, amMainFrame::OnQuit)

EVT_MENU(MENU_Update, amMainFrame::UpdateElements)
EVT_MENU(MENU_ProjectName, amMainFrame::EditTitle)

EVT_MENU(MENU_NewDocument, amMainFrame::OnNewDocument)
EVT_MENU(MENU_NewCharacter, amMainFrame::OnNewCharacter)
EVT_MENU(MENU_NewLocation, amMainFrame::OnNewLocation)
EVT_MENU(MENU_NewItem, amMainFrame::OnNewItem)

EVT_TOOL(TOOL_NewDocument, amMainFrame::OnNewDocument)
EVT_TOOL(TOOL_NewCharacter, amMainFrame::OnNewCharacter)
EVT_TOOL(TOOL_NewLocation, amMainFrame::OnNewLocation)
EVT_TOOL(TOOL_NewItem, amMainFrame::OnNewItem)

EVT_TOOL(TOOL_Save, amMainFrame::OnSaveFile)
EVT_MENU(TOOL_FullScreen, amMainFrame::FullScreen)

EVT_MENU(wxID_ABOUT, amMainFrame::OnAbout)

EVT_BUTTON(BUTTON_Overview, amMainFrame::OnMainButtons)
EVT_BUTTON(BUTTON_Elem, amMainFrame::OnMainButtons)
EVT_BUTTON(BUTTON_Documents, amMainFrame::OnMainButtons)
EVT_BUTTON(BUTTON_Release, amMainFrame::OnMainButtons)
EVT_BUTTON(BUTTON_Outline, amMainFrame::OnMainButtons)

EVT_SEARCH(TOOL_Search, amMainFrame::Search)

EVT_CLOSE(amMainFrame::OnClose)

END_EVENT_TABLE()

amMainFrame::amMainFrame(const wxString& title, amProjectManager* manager, const wxPoint& pos, const wxSize& size) :
	wxFrame(nullptr, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
	m_manager = manager;
	Hide();

	m_mainPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize);
	m_mainPanel->SetBackgroundColour(wxColour(40, 40, 40));

	m_holderSizer = new wxBoxSizer(wxHORIZONTAL);
	m_holderSizer->Add(m_mainPanel, wxSizerFlags(1).Expand());
	SetSizer(m_holderSizer);

	amSplitterWindow* splitter = new amSplitterWindow(m_mainPanel, -1, wxDefaultPosition,
		wxDefaultSize, wxSP_NOBORDER | wxSP_NO_XP_THEME | wxSP_THIN_SASH | wxSP_LIVE_UPDATE);
	splitter->SetBackgroundColour(wxColour(20, 20, 20));
	splitter->SetMinimumPaneSize(50);
	splitter->Bind(wxEVT_SPLITTER_SASH_POS_CHANGING, &amMainFrame::OnSashChanged, this);

	m_selPanel = new wxPanel(splitter, wxID_ANY);
	m_selPanel->SetBackgroundColour(wxColour(40, 40, 40));
	m_selPanel->SetDoubleBuffered(true);

	wxFont font(wxFontInfo(16).Family(wxFONTFAMILY_MODERN).Bold().AntiAliased());

	for ( int i = 0; i < 5; i++ )
		m_mainButtons.push_back(nullptr);

	// These are the buttons located on the left pane.
	m_mainButtons[0] = new wxButton(m_selPanel, BUTTON_Overview, "Overview", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_mainButtons[1] = new wxButton(m_selPanel, BUTTON_Elem, "Elements", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_mainButtons[2] = new wxButton(m_selPanel, BUTTON_Documents, "Story", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_mainButtons[3] = new wxButton(m_selPanel, BUTTON_Outline, "Outline", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	m_mainButtons[4] = new wxButton(m_selPanel, BUTTON_Release, "Release", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);

	for ( wxButton* button : m_mainButtons )
	{
		button->SetFont(font);
		button->SetBackgroundColour(wxColour(20, 20, 20));
		button->SetForegroundColour(wxColour(245, 245, 245));
		button->Bind(wxEVT_ENTER_WINDOW, amOnEnterDarkButton);
		button->Bind(wxEVT_LEAVE_WINDOW, amOnLeaveDarkButton);
	}
	m_mainButtons[0]->SetBackgroundColour(wxDarkenColour(wxColour(130, 0, 0), 28));

	m_buttonSizer = new wxBoxSizer(wxVERTICAL);
	m_buttonSizer->Add(m_mainButtons[0], wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
	m_buttonSizer->Add(m_mainButtons[1], wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
	m_buttonSizer->Add(m_mainButtons[2], wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
	m_buttonSizer->Add(m_mainButtons[3], wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
	m_buttonSizer->Add(m_mainButtons[4], wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
	m_buttonSizer->AddStretchSpacer(1);

	m_selPanel->SetSizer(m_buttonSizer);

	m_mainBook = new wxSimplebook(splitter);
	m_mainBook->SetBackgroundColour(wxColour(40, 40, 40));

	m_overview = new amOverview(m_mainBook, m_manager);
	m_overview->Show();

	//Setting up notebook Elements page
	m_elementNotebook = new amElementNotebook(m_mainBook);
	m_elementNotebook->Hide();

	m_storyNotebook = new amStoryNotebook(m_mainBook, m_manager);
	m_storyNotebook->Hide();

	m_outline = new amOutline(m_mainBook);
	m_outline->Hide();

	m_release = new amRelease(m_mainBook);
	m_release->Hide();

	m_mainBook->ShowNewPage(m_overview);
	m_mainBook->ShowNewPage(m_elementNotebook);
	m_mainBook->ShowNewPage(m_storyNotebook);
	m_mainBook->ShowNewPage(m_outline);
	m_mainBook->ShowNewPage(m_release);

	m_mainBook->ChangeSelection(0);

	// Create menus and such
	wxMenuBar* mainMenu = new wxMenuBar();

	wxMenu* fileMenu = new wxMenu();
	wxMenu* exportCanvas = new wxMenu();
	exportCanvas->Append(MENU_ExportCorkboardPNG, "PNG image", "Export the contents of the Canvas to an image file, in this case, PNG");
	exportCanvas->Append(MENU_ExportCorkboardBMP, "BMP image", "Export the contents of the Canvas to an image file, in this case, BMP");

	fileMenu->Append(MENU_New, "&New project", "Create a new project");
	fileMenu->Append(MENU_Open, "&Open project", "Open an existing project");
	fileMenu->Append(MENU_Save, "&Save", "Save the current sqlEntry project");
	fileMenu->Append(MENU_SaveAs, "Save &As");
	fileMenu->AppendSubMenu(exportCanvas, "Export corkboard to...");
	fileMenu->Append(MENU_Quit, "&Quit", "Quit the editor");

	wxMenu* projectMenu = new wxMenu();

	projectMenu->Append(MENU_NewDocument, "New &Document", "Create a new document");
	projectMenu->AppendSeparator();
	projectMenu->Append(MENU_NewCharacter, "New Character", "Create a new character");
	projectMenu->Append(MENU_NewLocation, "New &Location", "Create a new location");
	projectMenu->Append(MENU_NewItem, "New &Item", "Create a new item");

	wxMenu* editMenu = new wxMenu();
	editMenu->Append(MENU_Update, "&Update", "Update");
	editMenu->Append(MENU_ProjectName, "&Project Name", "Edit project name");

	wxMenu* helpMenu = new wxMenu();
	helpMenu->Append(wxID_ABOUT, "&About", "Shows information about the application");

	wxMenu* viewMenu = new wxMenu();
	viewMenu->AppendRadioItem(wxID_ANY, "&Dark theme");
	viewMenu->AppendRadioItem(-1, "&Light theme");

	mainMenu->Append(fileMenu, _("File"));
	mainMenu->Append(editMenu, "Edit");
	mainMenu->Append(viewMenu, "View");
	mainMenu->Append(projectMenu, "Project");
	mainMenu->Append(helpMenu, "Help");
	SetMenuBar(mainMenu);

	// Creating toolbar and setting tools

	m_toolBar = new wxToolBar(m_mainPanel, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT);
	m_toolBar->SetBackgroundColour(wxColour(100, 100, 100));

	m_toolBar->AddTool(TOOL_NewDocument, wxEmptyString, wxBITMAP_PNG(documentPng), "Add new document", wxITEM_NORMAL);
	m_toolBar->AddTool(TOOL_NewCharacter, wxEmptyString, wxBITMAP_PNG(characterPng), "Add new character", wxITEM_NORMAL);
	m_toolBar->AddTool(TOOL_NewLocation, wxEmptyString, wxBITMAP_PNG(locationPng), "Add new location", wxITEM_NORMAL);
	m_toolBar->AddTool(TOOL_NewItem, wxEmptyString, wxBITMAP_PNG(itemPng), "Add new item", wxITEM_NORMAL);

	m_toolBar->AddSeparator();
	m_toolBar->AddTool(TOOL_Save, "", wxBITMAP_PNG(savePng), "Save project", wxITEM_NORMAL);
	m_toolBar->AddSeparator();
	m_toolBar->AddCheckTool(TOOL_FullScreen, "", wxBITMAP_PNG(fullScreenPng), wxNullBitmap, "Toggle Full Screen");

	m_toolBar->AddStretchableSpace();

	// Initializing the search bar. It's actually owned by the another class, but it's initialized in this constructor.

	m_elementNotebook->m_searchBar = new wxSearchCtrl(m_toolBar, TOOL_Search,
		wxEmptyString, wxDefaultPosition, FromDIP(wxSize(250, -1)), wxTE_CAPITALIZE | wxBORDER_SIMPLE);
	m_elementNotebook->m_searchBar->SetBackgroundColour(wxColour(30, 30, 30));
	m_elementNotebook->m_searchBar->SetForegroundColour(wxColour(255, 255, 255));
	m_elementNotebook->m_searchBar->ShowCancelButton(true);

	m_toolBar->AddControl(m_elementNotebook->m_searchBar);
	m_toolBar->AddSeparator();

	m_toolBar->Realize();

	splitter->SplitVertically(m_selPanel, m_mainBook, FromDIP(150));
	splitter->SetSashGravity(0.0);

	m_verticalSizer = new wxBoxSizer(wxVERTICAL);
	m_verticalSizer->Add(m_toolBar, wxSizerFlags(0).Expand());
	m_verticalSizer->Add(splitter, wxSizerFlags(1).Expand());
	m_mainPanel->SetSizer(m_verticalSizer);

	SetIcon(wxICON(amadeus));

	wxRichTextBuffer::AddHandler(new wxRichTextXMLHandler);
	wxRichTextBuffer::AddHandler(new wxRichTextHTMLHandler);

	// Initialize maximized
	Maximize();
	m_verticalSizer->Layout();

	Show();
}

void amMainFrame::OnNewFile(wxCommandEvent& event)
{
	// Updating everything that needs to be reset.
	m_elementNotebook->ClearAll();
	m_storyNotebook->ClearAll();
	m_outline->ClearAll();

	// Clearing all paths and setting window title as generic.
	SetTitle("New Amadeus project");
}

void amMainFrame::OnOpenFile(wxCommandEvent& event)
{
	wxFileDialog openDialog(this, "Choose a file to open", wxEmptyString, wxEmptyString,
		"Amadeus Project files (*.amp)|*.amp",
		wxFD_OPEN, wxDefaultPosition);

	if ( openDialog.ShowModal() == wxID_OK )
		m_manager->DoLoadProject(openDialog.GetPath());
}

void amMainFrame::OnSaveFile(wxCommandEvent& event)
{
	// First, check whether the current path of the project exists. If it doesn't,
	// the "OnSaveFileAs" is called, which calls back the "saveAs" function.
	if ( !wxFileName::Exists(m_manager->GetPath(false).ToStdString()) )
	{
		amMainFrame::OnSaveFileAs(event);
	}
	else
	{
		m_manager->SaveProject();
		SetFocus();
	}
	event.Skip();
}

void amMainFrame::OnSaveFileAs(wxCommandEvent& event)
{
	wxFileDialog saveDialog(this, "Save file as...", wxEmptyString, wxEmptyString,
		"Amadeus Project files (*.amp)|*.amp",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

	if ( saveDialog.ShowModal() == wxID_OK )
	{
		m_manager->SetProjectFileName(saveDialog.GetPath());
		OnSaveFile(event);
	}

	event.Skip();
}

void amMainFrame::OnExportCorkboard(wxCommandEvent& event)
{
	wxBitmapType type;
	switch ( event.GetId() )
	{
	case MENU_ExportCorkboardPNG:
		type = wxBITMAP_TYPE_PNG;
		break;
	case MENU_ExportCorkboardBMP:
		type = wxBITMAP_TYPE_BMP;
		break;
	}

	m_outline->GetCorkboard()->ExportToImage(type);
}

void amMainFrame::OnClose(wxCloseEvent& event)
{

	/*if (event.CanVeto()) {
		wxMessageDialog saveBefore(this, "Project has been modified and not saved.\nDo you want to save before quitting?",
			"Quit", wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxICON_EXCLAMATION);

		switch (saveBefore.ShowModal()) {
		case wxID_YES: {
			OnSaveFile(wxCommandEvent());

			if (m_manager->IsSaved()) {
				Destroy();
			} else {
				event.Veto();
			}
			break;
		}

		case wxID_NO:
			Destroy();
			break;

		case wxID_CANCEL:
			break;
		}
	} else {
		Destroy();
	}*/

	event.Skip();
}

void amMainFrame::OnQuit(wxCommandEvent& event)
{
	Close(true);
}

void amMainFrame::EditTitle(wxCommandEvent& event)
{
	/*wxString temp(m_manager->GetTitle());
	temp.erase(m_manager->GetTitle().size() - 4, wxString::npos);

	wxTextEntryDialog newTitle(this, "Change project name - '" +
		temp + "':", "New title", "Teste");

	if (newTitle.ShowModal() == wxID_OK) {
		currentTitle = newTitle.GetValue() + ".amp";
		this->SetTitle(currentTitle);

		fs::rename(currentDocFile, currentDocFolder + "\\" + currentTitle);
		currentDocFile = currentDocFolder + "\\" + currentTitle;

		currentDocFolder.erase(currentDocFolder.size() - previousTitle.size() + 4, wxString::npos);
		currentDocFolder += newTitle.GetValue();

		fs::rename(previousDocFolder, currentDocFolder);

		currentImagePath = currentDocFolder + "\\Images";

		previousDocFile = currentDocFile;
		previousDocFolder = currentDocFolder;
		previousTitle = currentTitle;

		OnSaveFile(event);
	}

	Show();*/
}

void amMainFrame::FullScreen(wxCommandEvent& event)
{
	ShowFullScreen(!IsFullScreen());
	m_isFrameFullScreen = !m_isFrameFullScreen;
}

void amMainFrame::OnAbout(wxCommandEvent& event)
{
	wxAboutDialogInfo info;
	info.SetName("AmadeusWriter");
	info.SetVersion("0.5.2");
	info.SetDescription("The most incredible writer program out there. Well, according to me hehe.");
	info.AddDeveloper("Leonardo Bronstein Franceschetti");
	info.AddArtist("Leonardo Bronstein Franceschetti");
	info.AddArtist("Carolina Perez Avante");
	info.SetIcon(wxICON(amadeus));
	info.SetLicence("Te amo, Carol!!!");

	wxAboutBox(info, this);
}

void amMainFrame::OnMainButtonEnter(wxMouseEvent& event)
{
	((wxButton*)event.GetEventObject())->SetForegroundColour(wxColour(145, 145, 145));
}

void amMainFrame::OnMainButtonLeave(wxMouseEvent& event)
{
	((wxButton*)event.GetEventObject())->SetForegroundColour(wxColour(245, 245, 245));
}

void amMainFrame::OnMainButtons(wxCommandEvent& event)
{
	int page;
	bool showToolBar;
	bool showSearch;

	switch ( event.GetId() )
	{
	case BUTTON_Overview:
		page = 0;
		showToolBar = true;
		showSearch = false;
		break;
	case BUTTON_Elem:
		page = 1;
		showToolBar = true;
		showSearch = true;
		break;
	case BUTTON_Documents:
		page = 2;
		showToolBar = true;
		showSearch = false;
		break;
	case BUTTON_Outline:
		page = 3;
		showToolBar = true;
		showSearch = false;
		break;
	case BUTTON_Release:
		page = 4;
		showToolBar = false;
		showSearch = false;
		break;
	default:
		page = 0;
		showToolBar = true;
		showSearch = false;
		break;
	}

	for ( int i = 0; i < 5; i++ )
	{
		if ( i == page )
			m_mainButtons[i]->SetBackgroundColour(wxColour(130, 0, 0));
		else
			m_mainButtons[i]->SetBackgroundColour(wxColour(20, 20, 20));
	}

	m_mainBook->ChangeSelection(page);
	m_toolBar->Show(showToolBar);
	m_elementNotebook->m_searchBar->Show(showSearch);
	m_verticalSizer->Layout();
}

// These next 3 functions are for opening up the frames used on creating characters, locations and documents.
void amMainFrame::OnNewDocument(wxCommandEvent& event)
{
	DocumentCreator* create = new DocumentCreator(this, m_manager);
	create->Show();
	create->SetFocus();
	Enable(false);
	event.Skip();
}

void amMainFrame::OnNewCharacter(wxCommandEvent& event)
{
	amCharacterCreator* create = new amCharacterCreator(this, m_manager, -1,
		"Create character", wxDefaultPosition, FromDIP(wxSize(650, 650)));
	create->Show();
	create->SetFocus();
	Enable(false);
	event.Skip();
}

void amMainFrame::OnNewLocation(wxCommandEvent& event)
{
	amLocationCreator* create = new amLocationCreator(this, m_manager, -1, "Create location",
		wxDefaultPosition, FromDIP(wxSize(900, 650)));
	create->Show();
	create->SetFocus();
	Enable(false);
	event.Skip();
}

void amMainFrame::OnNewItem(wxCommandEvent& event)
{
	amItemCreator* create = new amItemCreator(this, m_manager, -1, "Create item",
		wxDefaultPosition, FromDIP(wxSize(900, 720)));
	create->Show();
	create->SetFocus();
	Enable(false);
	event.Skip();
}

void amMainFrame::OnSashChanged(wxSplitterEvent& event)
{
	if ( m_storyNotebook->IsShown() )
		m_storyNotebook->LayoutGrid();
}

void amMainFrame::Search(wxCommandEvent& event)
{
	// Get which page (Characters, Locations or Items) is currently displayed.
	int sel = m_elementNotebook->GetSelection();
	int item;

	// Get the searched wxString.
	wxString nameSearch = event.GetString();
	wxMenu menu;

	// Switch accordingly to current page being displayed, then either display a message
	// saying that it couldn't be found or select it on the list and make it visible.
	switch ( sel )
	{
	case 0:
		item = m_elementNotebook->m_charList->FindItem(-1, nameSearch, true);

		if ( item == -1 )
		{
			wxMessageBox("Character """ + nameSearch + """ could not be found", "Not found!",
				wxOK | wxICON_INFORMATION | wxCENTER);
		}
		else
		{
			m_elementNotebook->m_charList->Select(item, true);
			m_elementNotebook->m_charList->EnsureVisible(item);
			m_elementNotebook->m_charList->SetFocus();
		}

		break;

	case 1:
		item = m_elementNotebook->m_locList->FindItem(-1, event.GetString(), true);

		if ( item == -1 )
		{
			wxMessageBox("Location """ + nameSearch + """ could not be found", "Not found!",
				wxOK | wxICON_INFORMATION | wxCENTER);
		}
		else
		{
			m_elementNotebook->m_locList->Select(item, true);
			m_elementNotebook->m_locList->EnsureVisible(item);
			m_elementNotebook->m_locList->SetFocus();
		}

		break;

	case 2:
		break;

	case 3:
		break;
	}
}

amOverview* amMainFrame::GetOverview()
{
	return m_overview;
}

amElementNotebook* amMainFrame::GetElementsNotebook()
{
	return m_elementNotebook;
}

amStoryNotebook* amMainFrame::GetStoryNotebook()
{
	return m_storyNotebook;
}

amOutline* amMainFrame::GetOutline()
{
	return m_outline;
}

amRelease* amMainFrame::GetRelease()
{
	return m_release;
}

// I don't think I actually use this function since I made "UpdateAll" static,
// will probably get rid of it after further investigating.
void amMainFrame::UpdateElements(wxCommandEvent& event)
{
	m_elementNotebook->UpdateAll();
}