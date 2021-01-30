#include "MainFrame.h"

#include "ElementsNotebook.h"
#include "ChaptersGrid.h"
#include "Outline.h"
#include "Corkboard.h"
#include "Release.h"

#include "ChapterCreator.h"
#include "ElementCreators.h"



#include <wx\richtext\richtextxml.h>
#include <wx\richtext\richtexthtml.h>
#include <wx\aboutdlg.h>

#include "wxmemdbg.h"

#ifndef BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_FILESYSTEM_NO_DEPRECATED
#endif

namespace fs = boost::filesystem;

BEGIN_EVENT_TABLE(amdMainFrame, wxFrame)

EVT_MENU(MENU_New, amdMainFrame::OnNewFile)
EVT_MENU(MENU_Open, amdMainFrame::OnOpenFile)
EVT_MENU(MENU_Save, amdMainFrame::OnSaveFile)
EVT_MENU(MENU_SaveAs, amdMainFrame::OnSaveFileAs)
EVT_MENU(MENU_ExportCorkboardPNG, amdMainFrame::OnExportCorkboard)
EVT_MENU(MENU_ExportCorkboardBMP, amdMainFrame::OnExportCorkboard)
EVT_MENU(MENU_Quit, amdMainFrame::OnQuit)

EVT_MENU(MENU_Update, amdMainFrame::UpdateElements)
EVT_MENU(MENU_ProjectName, amdMainFrame::EditTitle)

EVT_MENU(MENU_NewChapter, amdMainFrame::OnNewChapter)
EVT_MENU(MENU_NewCharacter, amdMainFrame::OnNewCharacter)
EVT_MENU(MENU_NewLocation, amdMainFrame::OnNewLocation)
EVT_MENU(MENU_NewItem, amdMainFrame::OnNewItem)

EVT_TOOL(TOOL_NewChapter, amdMainFrame::OnNewChapter)
EVT_TOOL(TOOL_NewCharacter, amdMainFrame::OnNewCharacter)
EVT_TOOL(TOOL_NewLocation, amdMainFrame::OnNewLocation)
EVT_TOOL(TOOL_NewItem, amdMainFrame::OnNewItem)

EVT_TOOL(TOOL_Save, amdMainFrame::OnSaveFile)
EVT_MENU(TOOL_FullScreen, amdMainFrame::FullScreen)

EVT_MENU(wxID_ABOUT, amdMainFrame::OnAbout)

EVT_BUTTON(BUTTON_Overview, amdMainFrame::OnMainButtons)
EVT_BUTTON(BUTTON_Elem, amdMainFrame::OnMainButtons)
EVT_BUTTON(BUTTON_Chapters, amdMainFrame::OnMainButtons)
EVT_BUTTON(BUTTON_Release, amdMainFrame::OnMainButtons)
EVT_BUTTON(BUTTON_Outline, amdMainFrame::OnMainButtons)

EVT_SEARCH(TOOL_Search, amdMainFrame::Search)

EVT_CLOSE(amdMainFrame::OnClose)

END_EVENT_TABLE()

amdMainFrame::amdMainFrame(const wxString& title, amdProjectManager* manager, const wxPoint& pos, const wxSize& size) :
    wxFrame(nullptr, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE) {

    // This entire constructor makes the cProtagonist Frame of the application look like it does.
    // I see no vantage in doing all of this in a separate "Init()" function and
    // calling it in the constructor.
    m_manager = manager;
    Hide();

    m_mainPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize);
    m_mainPanel->SetBackgroundColour(wxColour(40, 40, 40));

    m_holderSizer = new wxBoxSizer(wxHORIZONTAL);
    m_holderSizer->Add(m_mainPanel, wxSizerFlags(1).Expand());
    SetSizer(m_holderSizer);

    m_selPanel = new wxPanel(m_mainPanel, wxID_ANY);
    m_selPanel->SetBackgroundColour(wxColour(40, 40, 40));

    wxFont font(wxFontInfo(16).Family(wxFONTFAMILY_MODERN).Bold().AntiAliased());

    for (int i = 0; i < 5; i++)
        m_mainButtons.push_back(nullptr);

    // These are the buttons located on the left pane.
    m_mainButtons[0] = new wxButton(m_selPanel, BUTTON_Overview, "Overview");
    m_mainButtons[0]->SetFont(font);
    m_mainButtons[0]->SetBackgroundColour(wxColour(130, 0, 0));
    m_mainButtons[0]->SetForegroundColour(wxColour(245, 245, 245));
    
    m_mainButtons[1] = new wxButton(m_selPanel, BUTTON_Elem, "Elements");
    m_mainButtons[1]->SetFont(font);
    m_mainButtons[1]->SetBackgroundColour(wxColour(255, 255, 255));
    
    m_mainButtons[2] = new wxButton(m_selPanel, BUTTON_Chapters, "Chapters");
    m_mainButtons[2]->SetFont(font);
    m_mainButtons[2]->SetBackgroundColour(wxColour(255, 255, 255));
    
    m_mainButtons[3] = new wxButton(m_selPanel, BUTTON_Outline, "Outline");
    m_mainButtons[3]->SetFont(font);
    m_mainButtons[3]->SetBackgroundColour(wxColour(255, 255, 255));
    
    m_mainButtons[4] = new wxButton(m_selPanel, BUTTON_Release, "Release");
    m_mainButtons[4]->SetFont(font);
    m_mainButtons[4]->SetBackgroundColour(wxColour(255, 255, 255));

    m_buttonSizer = new wxBoxSizer(wxVERTICAL);
    m_buttonSizer->Add(m_mainButtons[0], wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
    m_buttonSizer->Add(m_mainButtons[1], wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
    m_buttonSizer->Add(m_mainButtons[2], wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
    m_buttonSizer->Add(m_mainButtons[3], wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
    m_buttonSizer->Add(m_mainButtons[4], wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
    m_buttonSizer->AddStretchSpacer(1);

    m_selPanel->SetSizer(m_buttonSizer);

    m_mainBook = new wxSimplebook(m_mainPanel);
    m_mainBook->SetBackgroundColour(wxColour(40, 40, 40));

    m_overview = new wxPanel(m_mainBook);
    m_overview->SetBackgroundColour(wxColour(20, 20, 20));
    m_overview->Show();

    //Setting up notebook Elements page
    m_elements = new amdElementsNotebook(m_mainBook);
    m_elements->Hide();

    m_chaptersNote = new amdChaptersNotebook(m_mainBook, m_manager);
    m_chaptersNote->Hide();

    m_outline = new amdOutline(m_mainBook);
    m_outline->Hide();

    m_release = new amdRelease(m_mainBook);
    m_release->Hide();

    m_mainBook->ShowNewPage(m_overview);
    m_mainBook->ShowNewPage(m_elements);
    m_mainBook->ShowNewPage(m_chaptersNote);
    m_mainBook->ShowNewPage(m_outline);
    m_mainBook->ShowNewPage(m_release);

    m_mainBook->ChangeSelection(0);

    m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
    m_mainSizer->Add(m_selPanel, wxSizerFlags(1).Expand());
    m_mainSizer->Add(m_mainBook, wxSizerFlags(1).Expand());

    // Create menus and such
    wxMenuBar* mainMenu = new wxMenuBar();

    wxMenu* fileMenu = new wxMenu();
    wxMenu* exportCanvas = new wxMenu();
    exportCanvas->Append(MENU_ExportCorkboardPNG, "PNG image", "Export the contents of the Canvas to an image file, in this case, PNG");
    exportCanvas->Append(MENU_ExportCorkboardBMP, "BMP image", "Export the contents of the Canvas to an image file, in this case, BMP");

    fileMenu->Append(MENU_New, "&New project", "Create a new project");
    fileMenu->Append(MENU_Open, "&Open project", "Open an existing project");
    fileMenu->Append(MENU_Save, "&Save", "Save the current document project");
    fileMenu->Append(MENU_SaveAs, "Save &As");
    fileMenu->AppendSubMenu(exportCanvas, "Export corkboard to...");
    fileMenu->Append(MENU_Quit, "&Quit", "Quit the editor");

    wxMenu* projectMenu = new wxMenu();

    projectMenu->Append(MENU_NewChapter, "New &Chapter", "Create a new chapter");
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

    m_toolBar = new wxToolBar(m_mainPanel, -1);
    m_toolBar->SetBackgroundColour(wxColour(100, 100, 100));

    m_toolBar->AddTool(TOOL_NewChapter, wxEmptyString, wxBITMAP_PNG(chapterPng), "Add new chapter", wxITEM_NORMAL);
    m_toolBar->AddTool(TOOL_NewCharacter, wxEmptyString, wxBITMAP_PNG(characterPng), "Add new character", wxITEM_NORMAL);
    m_toolBar->AddTool(TOOL_NewLocation, wxEmptyString, wxBITMAP_PNG(locationPng), "Add new location", wxITEM_NORMAL);
    m_toolBar->AddTool(TOOL_NewItem, wxEmptyString, wxBITMAP_PNG(itemPng), "Add new item", wxITEM_NORMAL);

    m_toolBar->AddSeparator();
    m_toolBar->AddSeparator();
    m_toolBar->AddTool(TOOL_Save, "", wxBITMAP_PNG(savePng), "Save project", wxITEM_NORMAL);
    m_toolBar->AddSeparator();
    m_toolBar->AddCheckTool(TOOL_FullScreen, "", wxBITMAP_PNG(fullScreenPng), wxNullBitmap, "Toggle Full Screen");

    m_toolBar->AddStretchableSpace();

    // Initializing the search bar. It's actually owned by the another class, but it's initialized in this constructor.

    m_elements->m_searchBar = new wxSearchCtrl(m_toolBar, TOOL_Search, wxEmptyString, wxDefaultPosition, wxSize(250, -1), wxTE_CAPITALIZE);
    m_elements->m_searchBar->ShowCancelButton(true);

    m_toolBar->AddControl(m_elements->m_searchBar);
    m_toolBar->AddSeparator();

    m_toolBar->Realize();

    m_verticalSizer = new wxBoxSizer(wxVERTICAL);
    m_verticalSizer->Add(m_toolBar, wxSizerFlags(0).Expand());
    m_verticalSizer->Add(m_mainSizer,wxSizerFlags(1).Expand());
    m_mainPanel->SetSizer(m_verticalSizer);

    SetIcon(wxICON(amadeus));

    wxRichTextBuffer::AddHandler(new wxRichTextXMLHandler);
    wxRichTextBuffer::AddHandler(new wxRichTextHTMLHandler);

    // Initialize maximized
    Maximize();
    m_overview->Layout();

    Show();
}

void amdMainFrame::OnNewFile(wxCommandEvent& event) {
    // Updating everything that needs to be reset.
    m_elements->ClearAll();
    m_outline->ClearAll();
    m_chaptersNote->ClearAll();

    // Clearing all paths and setting window title as generic.
    SetTitle("New Amadeus project");
}

void amdMainFrame::OnOpenFile(wxCommandEvent& event) {
    wxFileDialog openDialog(this, "Choose a file to open", wxEmptyString, wxEmptyString,
        "Amadeus Project files (*.amp)|*.amp",
        wxFD_OPEN, wxDefaultPosition);

    if (openDialog.ShowModal() == wxID_OK)
        m_manager->DoLoadProject(openDialog.GetPath());
}

void amdMainFrame::OnSaveFile(wxCommandEvent& event) {
    // First, check whether the current path of the project exists. If it doesn't,
    // the "OnSaveFileAs" is called, which calls back the "saveAs" function.
    if (!fs::exists(m_manager->GetPath(false).ToStdString())) {
        amdMainFrame::OnSaveFileAs(event);
    } else {
        m_manager->SaveProject();
        SetFocus();
    }
    event.Skip();
}

void amdMainFrame::OnSaveFileAs(wxCommandEvent& event) {
    wxFileDialog saveDialog(this, "Save file as...", wxEmptyString, wxEmptyString,
        "Amadeus Project files (*.amp)|*.amp",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

    if (saveDialog.ShowModal() == wxID_OK) {
        wxString fullPath(saveDialog.GetPath().ToStdString());
        fullPath.erase(fullPath.size() - 4, 4);
        fullPath += "\\" + saveDialog.GetFilename();

        m_manager->SetCurrentDocPath(fullPath);

        fullPath = m_manager->GetPath(true);

        fs::create_directory(fullPath.ToStdString());
        fs::create_directory(fullPath.ToStdString() + "Images");
        fs::create_directory(fullPath.ToStdString() + "Images\\Characters");
        fs::create_directory(fullPath.ToStdString() + "Images\\Locations");
        fs::create_directory(fullPath.ToStdString() + "Images\\Corkboard");
        fs::create_directory(fullPath.ToStdString() + "Files");
        fs::create_directory(fullPath.ToStdString() + "Files\\Corkboard");
        fs::create_directory(fullPath.ToStdString() + "Files\\Chapters");
        fs::create_directory(fullPath.ToStdString() + "Files\\Outline");


        OnSaveFile(event);
    }

    event.Skip();
}

void amdMainFrame::OnExportCorkboard(wxCommandEvent& event) {
    wxBitmapType type;
    switch (event.GetId()) {
    case MENU_ExportCorkboardPNG:
        type = wxBITMAP_TYPE_PNG;
        break;
    case MENU_ExportCorkboardBMP:
        type = wxBITMAP_TYPE_BMP;
        break;
    }

    m_outline->GetCorkboard()->exportToImage(type);
}

void amdMainFrame::OnClose(wxCloseEvent& event) {

    if (event.CanVeto() && !m_manager->IsSaved()) {
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
    }
}

void amdMainFrame::OnQuit(wxCommandEvent& event) {
    Close(true);
}

void amdMainFrame::EditTitle(wxCommandEvent& event) {
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

void amdMainFrame::FullScreen(wxCommandEvent& event) {
    ShowFullScreen(!IsFullScreen());
    m_isFrameFullScreen = !m_isFrameFullScreen;
}

void amdMainFrame::OnAbout(wxCommandEvent& event) {
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

void amdMainFrame::OnMainButtons(wxCommandEvent& event) {
    int page;
    bool showToolBar;
    bool showSearch;

    switch (event.GetId()) {
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
    case BUTTON_Chapters:
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

    for (int i = 0; i < 5; i++) {
        if (i == page) {
            m_mainButtons[i]->SetBackgroundColour(wxColour(130, 0, 0));
            m_mainButtons[i]->SetForegroundColour(wxColour(245, 245, 245));
        } else {
            m_mainButtons[i]->SetBackgroundColour(wxColour(255, 255, 255));
            m_mainButtons[i]->SetForegroundColour(wxColour(10, 10, 10));
        }
    }

    m_mainBook->ChangeSelection(page);
    m_toolBar->Show(showToolBar);
    m_elements->m_searchBar->Show(showSearch);
    m_verticalSizer->Layout();
}

// These next 3 functions are for opening up the frames used on creating characters, locations and chapters.
void amdMainFrame::OnNewChapter(wxCommandEvent& event) {
    if (!fs::is_directory(m_manager->GetPath(true).ToStdString() + "Files")) {
        wxMessageDialog* first = new wxMessageDialog(this, "It seems like you haven't saved your project yet.\nPlease do before writing any chapters.",
            "Save before", wxOK | wxCANCEL | wxOK_DEFAULT);
        first->SetOKCancelLabels("Save", "Cancel");
        int aa = first->ShowModal();
        if (aa == wxID_OK) {
           OnSaveFile(event);

            if (!m_manager->IsSaved())
                return;
        } else {
            event.Skip();
            return;
        }

        if (first)
            delete first;
    }

    ChapterCreator* create = new ChapterCreator(this, m_manager);
    create->Show();
    create->SetFocus();
    Enable(false);
    event.Skip();
}

void amdMainFrame::OnNewCharacter(wxCommandEvent& event) {
    amdCharacterCreator* create = new amdCharacterCreator(this, m_manager, -1,
        "Create character", wxDefaultPosition, FromDIP(wxSize(650, 650)));
    create->Show();
    create->SetFocus();
    Enable(false);
    event.Skip();
}

void amdMainFrame::OnNewLocation(wxCommandEvent& event) {
    amdLocationCreator* create = new amdLocationCreator(this, m_manager, -1, "Create location",
        wxDefaultPosition, FromDIP(wxSize(900, 650)));
    create->Show();
    create->SetFocus();
    Enable(false);
    event.Skip();
}

void amdMainFrame::OnNewItem(wxCommandEvent& event) {
    amdItemCreator* create = new amdItemCreator(this, m_manager, -1, "Create item",
        wxDefaultPosition, FromDIP(wxSize(900, 720)));
    create->Show();
    create->SetFocus();
    Enable(false);
    event.Skip();
}

void amdMainFrame::Search(wxCommandEvent& event) {
    // Get which page (Characters, Locations or Items) is currently displayed.
    int sel = m_elements->GetSelection();
    int item;

    // Get the searched wxString.
    wxString nameSearch = event.GetString();
    wxMenu menu;

    // Switch accordingly to current page being displayed, then either display a message
    // saying that it couldn't be found or select it on the list and make it visible.
    switch(sel) {
    case 0:
        item = m_elements->m_charList->FindItem(-1, nameSearch, true);

        if (item == -1) {
            wxMessageBox("Character """ + nameSearch + """ could not be found", "Not found!",
                wxOK | wxICON_INFORMATION | wxCENTER);
        } else {
            m_elements->m_charList->Select(item, true);
            m_elements->m_charList->EnsureVisible(item);
            m_elements->m_charList->SetFocus();
        }

        break;

    case 1:
        item = m_elements->m_locList->FindItem(-1, event.GetString(), true);
        
        if (item == -1) {
            wxMessageBox("Location """ + nameSearch + """ could not be found", "Not found!",
                wxOK | wxICON_INFORMATION | wxCENTER);
        } else {
            m_elements->m_locList->Select(item, true);
            m_elements->m_locList->EnsureVisible(item);
            m_elements->m_locList->SetFocus();
        }

        break;

    case 2:
        break;

    case 3:
        break;
    }
}

void amdMainFrame::DoCorkboardFullScreen(bool doFullScreen, wxWindow* toolBar, wxWindow* canvas) {
    m_mainPanel->Show(!doFullScreen);
    
    if (doFullScreen) {
        m_holderSizer->Replace(m_mainPanel, m_outline->GetCorkboard());

        ShowFullScreen(true);
    } else {
        m_holderSizer->Replace(m_outline->GetCorkboard(), m_mainPanel);
        
        if (!m_isFrameFullScreen)
            ShowFullScreen(false);
    }

    Layout();
}

//void amdMainFrame::setLast() {
//    // This function is called at every Save and it writes a file named 88165468
//    // next to the executable. In the file, there are paths to the project.
//    // These paths are used when loading so that the user doesn't need to manually
//    // load a project at startup, it automatically loads the last project that was worked on.
//
//    std::ofstream last(m_manager->GetExecutablePath().ToStdString() + "\\88165468", std::ios::binary | std::ios::out);
//
//    char size = m_manager->GetFullPath().size() + 1;
//
//    last.write((char*)&size, sizeof(int));
//    last.write(m_manager->GetFullPath().c_str(), size);
//
//    last.close();
//}

//void amdMainFrame::getLast() {
//    // Nothing special here, just reads the 88165468 file and, if succesful, calls the load function.
//    // Else, just clear the paths and load a standard new project.
//    std::ifstream last(m_manager->GetExecutablePath().ToStdString() + "\\88165468", std::ios::binary | std::ios::in);
//
//    if (last.is_open()) {
//
//        char size;
//        char* data;
//
//        last.read((char*)&size, sizeof(int));
//        data = new char[size];
//        last.read(data, size);
//        m_manager->SetCurrentDocPath(data);
//        delete data;
//       
//        last.close();
//    }
//
//    if (fs::exists(m_manager->GetFullPath().ToStdString()))
//        m_manager->LoadProject(m_manager->GetFullPath());
//    else
//        m_manager->ClearPath();
//    
//}

amdElementsNotebook* amdMainFrame::GetElementsNotebook() {
    return m_elements;
}

amdChaptersNotebook* amdMainFrame::GetChaptersNotebook() {
    return m_chaptersNote;
}

amdOutline* amdMainFrame::GetOutline() {
    return m_outline;
}

amdRelease* amdMainFrame::GetRelease() {
    return m_release;
}

// I don't think I actually use this function since I made "UpdateAll" static,
// will probably get rid of it after further investigating.
void amdMainFrame::UpdateElements(wxCommandEvent& event) {
    m_elements->UpdateAll();
}
