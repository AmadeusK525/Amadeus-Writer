#include "MainFrame.h"
#include "ChapterCreator.h"
#include "ChaptersGrid.h"
#include "CharacterCreator.h"
#include "CharacterShowcase.h"
#include "LocationCreator.h"
#include "MovableNote.h"

#include "wxmemdbg.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED

using std::string;
using std::vector;

namespace fs = boost::filesystem;

BEGIN_EVENT_TABLE(MainFrame, wxFrame)

EVT_MENU(MENU_New, MainFrame::newFile)
EVT_MENU(MENU_Open, MainFrame::openFile)
EVT_MENU(MENU_Save, MainFrame::saveFile)
EVT_MENU(MENU_SaveAs, MainFrame::saveFileAs)
EVT_MENU(MENU_Quit, MainFrame::quit)

EVT_MENU(MENU_Update, MainFrame::callUpdate)
EVT_MENU(MENU_ProjectName, MainFrame::editTitle)

EVT_MENU(MENU_NewCharacter, MainFrame::newChar)
EVT_MENU(MENU_NewChapter, MainFrame::newChap)
EVT_MENU(MENU_NewLocation, MainFrame::newLoc)

EVT_TOOL(TOOL_NewChapter, MainFrame::newChap)
EVT_TOOL(TOOL_NewCharacter, MainFrame::newChar)
EVT_TOOL(TOOL_NewLocation, MainFrame::newLoc)

EVT_TOOL(TOOL_Save, MainFrame::saveFile)

EVT_MENU(wxID_ABOUT, MainFrame::about)

EVT_BUTTON(BUTTON_Overview, MainFrame::onOverview)
EVT_BUTTON(BUTTON_Elem, MainFrame::onElements)
EVT_BUTTON(BUTTON_Chapters, MainFrame::onChapters)
EVT_BUTTON(BUTTON_Release, MainFrame::onRelease)
EVT_BUTTON(BUTTON_Outline, MainFrame::onOutline)

EVT_SEARCH(TOOL_Search, MainFrame::search)

EVT_CLOSE(MainFrame::onQuit)

END_EVENT_TABLE()

string MainFrame::currentDocFolder;
std::map<string, Character> MainFrame::characters;
std::map<string, Location> MainFrame::locations;
vector<int> MainFrame::saved;

bool MainFrame::isSaved;

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :
    wxFrame(nullptr, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE) {

    currentDocFolder = "";
    Hide();

    mainPanel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize);
    mainPanel->SetBackgroundColour(wxColour(150, 150, 150));

    panel = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    panel->Show();
    panel->SetBackgroundColour("WHITE");

    overview = new wxPanel(panel, -1);
    overview->Show();

    mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(overview, 1, wxEXPAND);

    //Setting up notebook Elements page
    elements = new MainNotebook(panel, this);
    elements->Hide();

    wxPanel* panel2 = new wxPanel(mainPanel, wxID_ANY);
    panel2->SetBackgroundColour(wxColour(220, 220, 220));
    pageSel = new wxPanel(panel2, wxID_ANY);
    pageSel->SetBackgroundColour(wxColour(70, 70, 70));

    wxFont font(wxFontInfo(16).Family(wxFONTFAMILY_MODERN).Bold().AntiAliased());

    wxButton* button0 = new wxButton(pageSel, BUTTON_Overview, "Overview");
    button0->SetFont(font);
    button0->SetBackgroundColour(wxColour(255, 255, 255));
    wxButton* button1 = new wxButton(pageSel, BUTTON_Elem, "Elements");
    button1->SetFont(font);
    button1->SetBackgroundColour(wxColour(255, 255, 255));
    wxButton* button2 = new wxButton(pageSel, BUTTON_Chapters, "Chapters");
    button2->SetFont(font);
    button2->SetBackgroundColour(wxColour(255, 255, 255));
    wxButton* button3 = new wxButton(pageSel, BUTTON_Release, "Release");
    button3->SetFont(font);
    button3->SetBackgroundColour(wxColour(255, 255, 255));
    wxButton* button4 = new wxButton(pageSel, BUTTON_Outline, "Outline");
    button4->SetFont(font);
    button4->SetBackgroundColour(wxColour(255, 255, 255));


    buttonSizer = new wxBoxSizer(wxVERTICAL);
    buttonSizer->Add(button0, wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
    buttonSizer->Add(button1, wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
    buttonSizer->Add(button2, wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
    buttonSizer->Add(button4, wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
    buttonSizer->Add(button3, wxSizerFlags(1).Expand().Border(wxTOP | wxLEFT | wxRIGHT, 10));
    buttonSizer->AddStretchSpacer(1);

    pageSel->SetSizer(buttonSizer);

    wxBoxSizer* pageSizer = new wxBoxSizer(wxVERTICAL);
    pageSizer->Add(pageSel, 1, wxEXPAND | wxTOP | wxBOTTOM, 2);
    panel2->SetSizer(pageSizer);

    chaptersNote = new ChaptersNotebook(panel);
    chaptersNote->Hide();

    release = new Release(panel);
    release->Hide();

    outline = new Outline(panel);
    outline->Hide();

    panel->SetSizer(mainSizer);

    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->SetMinSize(500, 400);
    topSizer->Add(panel2, 1, wxGROW);
    topSizer->Add(panel, 4, wxEXPAND);

    //Create menus and such

    mainMenu = new wxMenuBar();

    fileMenu = new wxMenu();

    fileMenu->Append(MENU_New, "&New project", "Create a new project");
    fileMenu->Append(MENU_Open, "&Open project", "Open an existing project");
    fileMenu->Append(MENU_Save, "&Save", "Save the current document project");
    fileMenu->Append(MENU_SaveAs, "Save &As");
    fileMenu->Append(MENU_Quit, "&Quit", "Quit the editor");

    projectMenu = new wxMenu();

    projectMenu->Append(MENU_NewChapter, "New &Chapter", "Create a new chapter");
    projectMenu->AppendSeparator();
    projectMenu->Append(MENU_NewCharacter, "New Character", "Create a new character");
    projectMenu->Append(MENU_NewLocation, "New &Location", "Create a new location");

    editMenu = new wxMenu();
    editMenu->Append(MENU_Update, "&Update", "Update");
    editMenu->Append(MENU_ProjectName, "&Project Name", "Edit project name");
    
    helpMenu = new wxMenu();
    helpMenu->Append(wxID_ABOUT, "&About", "Shows information about the application");

    wxMenu* viewMenu = new wxMenu();
    viewMenu->AppendRadioItem(wxID_ANY, "&Dark theme");
    viewMenu->AppendRadioItem(-1, "&Light theme");

    mainMenu->Append(fileMenu, "File");
    mainMenu->Append(editMenu, "Edit");
    mainMenu->Append(viewMenu, "View");
    mainMenu->Append(projectMenu, "Project");
    mainMenu->Append(helpMenu, "Help");
    SetMenuBar(mainMenu);

    //Creating toolbar and setting tools

    toolBar = new wxToolBar(mainPanel, wxID_ANY);
    toolBar->SetBackgroundColour(wxColour(120, 120, 120));

    toolBar->AddTool(TOOL_NewChapter, wxEmptyString, wxBITMAP_PNG(chapterPng), "Add new chapter", wxITEM_NORMAL);
    toolBar->AddTool(TOOL_NewCharacter, wxEmptyString, wxBITMAP_PNG(characterPng), "Add new character", wxITEM_NORMAL);
    toolBar->AddTool(TOOL_NewLocation, wxEmptyString, wxBITMAP_PNG(locationPng), "Add new location", wxITEM_NORMAL);

    toolBar->AddSeparator();
    toolBar->AddSeparator();
    toolBar->AddTool(TOOL_Save, "", wxBITMAP_PNG(savePng), "Save project", wxITEM_NORMAL);

    toolBar->AddStretchableSpace();

    elements->searchBar = new wxSearchCtrl(toolBar, TOOL_Search, wxEmptyString, wxDefaultPosition, wxSize(250, -1), wxTE_CAPITALIZE);
    elements->searchBar->ShowCancelButton(true);

    toolBar->AddControl(elements->searchBar);
    toolBar->AddSeparator();

    toolBar->Realize();

    ver = new wxBoxSizer(wxVERTICAL);
    ver->Add(toolBar, 0, wxGROW);
    ver->Add(topSizer, 1, wxGROW);
    mainPanel->SetSizerAndFit(ver);

    SetIcon(wxICON(amadeus));

    //Set file state as saved
    isSaved = true;

    for (int i = 0; i < 4; i++) {
        saved.push_back(0);
    }

    //Initialize maximized
    Maximize();
    outline->initCork(overview->GetSize(), wxBITMAP_PNG(test).ConvertToImage());
    elements->Hide();
    overview->Layout();

    Show();

    getLast();
}

void MainFrame::newFile(wxCommandEvent& event) {
    if (!isSaved) {
        wxMessageDialog* saveBefore = new wxMessageDialog(this, "Do you want to save before creating a new project?",
            "New project", wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxICON_EXCLAMATION);

        switch (saveBefore->ShowModal()) {
        case wxID_YES: {
            saveFile(event);
            break;
        }

        case wxID_NO:
            saveBefore->Destroy();
            break;

        case wxID_CANCEL:
            return;
            break;
        }

    }

    isSaved = true;

    saved.clear();

    for (int i = 0; i < 4; i++) {
        saved.push_back(0);
    }

    characters.clear();
    locations.clear();
    chaptersNote->getGrid()->clearAll();
    outline->clearAll();
    MainNotebook::charList->DeleteAllItems();
    MainNotebook::locList->DeleteAllItems();

    currentDocFolder = "a";
    currentDocFile = "";
    previousDocFile = "dummy";
    previousDocFolder = "dummy";
    currentImagePath = "";
    currentTitle = "New Amadeus project";
    previousTitle = "";

    SetTitle("New Amadeus project");
}

void MainFrame::openFile(wxCommandEvent& event) {
    wxFileDialog* openDialog = new wxFileDialog(this, "Choose a file to open", wxEmptyString, wxEmptyString,
        "Amadeus Project files (*.amp)|*.amp",
        wxFD_OPEN, wxDefaultPosition);

    if (openDialog->ShowModal() == wxID_OK) {
        currentDocFile = openDialog->GetPath();
        currentTitle = openDialog->GetFilename();
        previousTitle = currentTitle;

        currentDocFolder = openDialog->GetDirectory();
        previousDocFolder = currentDocFolder;

        currentImagePath = currentDocFolder + "\\Images";
        loadFile();

        setLast();
    }

    openDialog->Destroy();
}

void MainFrame::saveFile(wxCommandEvent& event) {
    if (!fs::exists(currentDocFolder)) {
        MainFrame::saveFileAs(event);
    } else {
        std::ofstream file(currentDocFile, std::ios::binary | std::ios::out);

        for (unsigned int i = 0; i < 4; i++) {
            file.write((char*)&saved[i], sizeof(int));
        }    

        outline->saveQuantities(file);

        int progressSize = saved[0] + saved[1] + saved[2] + saved[3]
            + outline->getCount();
        int currentSize = 0;

        wxProgressDialog* progress = new wxProgressDialog("Saving project...", currentDocFile, progressSize, this,
            wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_SMOOTH);

        string imagePath;

        for (auto it = characters.begin(); it != characters.end(); it++) {
            it->second.save(file);

            imagePath = currentImagePath + "\\Characters\\" + it->second.name + ".jpg";
            if (fs::exists(imagePath) && !it->second.image.IsOk()) {
                fs::remove(imagePath);
            } else {
                it->second.image.SaveFile(imagePath);
            }
            progress->Update(currentSize++);
        }

        for (auto it = locations.begin(); it != locations.end(); it++) {
            it->second.save(file);

            imagePath = currentImagePath + "\\Locations\\" + it->second.name + ".jpg";
            if (fs::exists(imagePath) && !it->second.image.IsOk()) {
                fs::remove(imagePath);
            } else {
                it->second.image.SaveFile(imagePath);
            }
            progress->Update(currentSize++);
        }

        for (auto it = chaptersNote->getGrid()->chapters.begin(); it != chaptersNote->getGrid()->chapters.end(); it++) {
            it->save(file);
            progress->Update(currentSize++);
        }

        outline->saveOutline(file, currentSize, progress);

        file.close();

        std::ofstream note1(currentImagePath + "\\Characters\\!!!Note!!!.txt", std::ios::out);
        note1 << "Please, NEVER change the names of the images nor move them somewhere else!";
        note1.close();

        progress->Update(progressSize);

        std::ofstream note2(currentImagePath + "\\Locations\\!!!Note!!!.txt", std::ios::out);
        note2 << "Please, NEVER change the names of the images nor move them somewhere else!";
        note2.close();

        std::ofstream note3(currentImagePath + "\\Outline\\!!!Note!!!.txt", std::ios::out);
        note3 << "Please, NEVER change the names of the images nor move them somewhere else!";
        note3.close();

        std::ofstream note4(currentDocFolder + "\\Chapters\\!!!Note!!!.txt", std::ios::out);
        note4 << "Please, NEVER change the names of the chapters nor move them somewhere else!";
        note4.close();

        progress->Destroy();

        isSaved = true;
        showDialog = true;

        SetTitle(currentTitle + " - Amadeus Writer");
        SetFocus();
        setLast();
    }
    event.Skip();
}

void MainFrame::saveFileAs(wxCommandEvent& event) {
    wxFileDialog* saveDialog = new wxFileDialog(this, "Save file as...", wxEmptyString, wxEmptyString,
        "Amadeus Project files (*.amp)|*.amp",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

    if (saveDialog->ShowModal() == wxID_OK) {
        currentTitle = saveDialog->GetFilename();
        currentDocFile = saveDialog->GetPath();
        currentDocFolder = currentDocFile;
        currentDocFolder.erase(currentDocFolder.size() - 4, string::npos);

        if (currentDocFile == previousDocFile) {
            currentDocFolder = saveDialog->GetDirectory();
        } else {
            fs::create_directory(currentDocFolder);

            currentImagePath = currentDocFolder + "\\Images";

            fs::create_directory(currentImagePath);
            fs::create_directory(currentImagePath + "\\Characters");
            fs::create_directory(currentImagePath + "\\Locations");
            fs::create_directory(currentImagePath + "\\Outline");
            fs::create_directory(currentDocFolder + "\\Chapters");
            //fs::permissions(currentDocFolder + "\\Chapters", fs::all_all);

            currentDocFile = currentDocFolder + "\\" + currentTitle;
        }

        saveFile(event);
    }

    if (saveDialog)
        delete saveDialog;

    event.Skip();
}

void MainFrame::onQuit(wxCloseEvent& event) {

    if (event.CanVeto() && !isSaved) {
        wxMessageDialog* saveBefore = new wxMessageDialog(this, "Project has been modified and not saved.\nDo you want to save before quitting?",
            "Quit", wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxICON_EXCLAMATION);

        switch (saveBefore->ShowModal()) {
        case wxID_YES: {
            saveFile(wxCommandEvent());

            if (isSaved) {
                Destroy();
            } else {
                event.Veto();
            }

            saveBefore->Destroy();

            break;
        }

        case wxID_NO:
            saveBefore->Destroy();
            Destroy();
            break;

        case wxID_CANCEL:
            saveBefore->Destroy();
            break;
        }
    } else {
        Destroy();
    }
}

void MainFrame::quit(wxCommandEvent& event) {
    Close(true);
}

void MainFrame::editTitle(wxCommandEvent& event) {
    string temp(currentTitle);
    temp.erase(currentTitle.size() - 4, string::npos);

    wxTextEntryDialog* newTitle = new wxTextEntryDialog(this, "Change project name - '" +
        temp + "':", "New title", "Teste");

    if (newTitle->ShowModal() == wxID_OK) {
        currentTitle = newTitle->GetValue() + ".amp";
        this->SetTitle(currentTitle);

        fs::rename(currentDocFile, currentDocFolder + "\\" + currentTitle);
        currentDocFile = currentDocFolder + "\\" + currentTitle;

        currentDocFolder.erase(currentDocFolder.size() - previousTitle.size() + 4, string::npos);
        currentDocFolder += newTitle->GetValue();

        fs::rename(previousDocFolder, currentDocFolder);

        currentImagePath = currentDocFolder + "\\Images";

        previousDocFile = currentDocFile;
        previousDocFolder = currentDocFolder;
        previousTitle = currentTitle;

        saveFile(event);
    }

    Show();
}

void MainFrame::about(wxCommandEvent& event) {
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

void MainFrame::notSaved(wxCommandEvent& event) {
    isSaved = false;
    event.Skip();
}

void MainFrame::onOverview(wxCommandEvent& event) {
    if (!overview->IsShown()) {
        mainSizer->Add(overview, 1, wxGROW);
        mainSizer->Remove(0);
        elements->Hide();
        chaptersNote->Hide();
        release->Hide();
        outline->Hide();
        overview->Show();

        elements->searchBar->Hide();

        if (ver->GetItemCount() == 1) {
            ver->Insert(0, toolBar, wxSizerFlags(0).Expand());
            toolBar->Show();
        }

    }

    ver->Layout();
    event.Skip();
}

void MainFrame::onElements(wxCommandEvent& event) {

    if (!elements->IsShown()) {
        mainSizer->Add(elements, 1, wxGROW);
        mainSizer->Remove(0);
        overview->Hide();
        chaptersNote->Hide();
        release->Hide();
        outline->Hide();
        elements->Show();

        if (ver->GetItemCount() == 1) {
            ver->Insert(0, toolBar, wxSizerFlags(0).Expand());
            toolBar->Show();
        }

        elements->searchBar->Show();
    }

    ver->Layout();
    event.Skip();
}

void MainFrame::onChapters(wxCommandEvent& event) {
    if (!chaptersNote->IsShown()) {
        mainSizer->Add(chaptersNote, 1, wxGROW);
        mainSizer->Remove(0);
        overview->Hide();
        release->Hide();
        elements->Hide();
        outline->Hide();
        chaptersNote->Show();
        elements->searchBar->Hide();

        if (ver->GetItemCount() == 1) {
            ver->Insert(0, toolBar, wxSizerFlags(0).Expand());
            toolBar->Show();
        }
    }

    ver->Layout();
    event.Skip();
}

void MainFrame::onRelease(wxCommandEvent& event) {
    if (!release->IsShown()) {
        mainSizer->Add(release, 1, wxGROW, 5);
        mainSizer->Remove(0);
        overview->Hide();
        elements->Hide();
        chaptersNote->Hide();
        outline->Hide();
        release->Show();
        elements->searchBar->Hide();

        if (ver->GetItemCount() == 2) {
            ver->Remove(0);
            toolBar->Hide();
        }
    }

    ver->Layout();
    event.Skip();
}

void MainFrame::onOutline(wxCommandEvent& event) {
    if (!outline->IsShown()) {
        mainSizer->Add(outline, 1, wxGROW);
        mainSizer->Remove(0);
        overview->Hide();
        release->Hide();
        elements->Hide();
        chaptersNote->Hide();
        outline->Show();
        elements->searchBar->Hide();

        if (ver->GetItemCount() == 1) {
            ver->Insert(0, toolBar, wxSizerFlags(0).Expand());
            toolBar->Show();
        }
    }

    ver->Layout();
    event.Skip();
}

void MainFrame::newChar(wxCommandEvent& event) {
    CharacterCreator* create = new CharacterCreator(this, elements);
    create->Show();
    create->SetFocus();
    event.Skip();
}

void MainFrame::newChap(wxCommandEvent& event) {
    ChapterCreator* create = new ChapterCreator(this, chaptersNote->getGrid());
    create->Show();
    create->SetFocus();
    event.Skip();
}

void MainFrame::newLoc(wxCommandEvent& event) {
    LocationCreator* create = new LocationCreator(this, elements);
    create->SetFocus();
    event.Skip();
}

void MainFrame::search(wxCommandEvent& event) {
    int sel = elements->GetSelection();
    int item;
    string nameSearch = event.GetString();
    wxMenu menu;

    switch(sel) {
    case 0:
        item = elements->charList->FindItem(-1, nameSearch, true);

        if (item == -1) {
            wxMessageBox("Character """ + nameSearch + """ could not be found", "Not found!", wxOK | wxICON_INFORMATION | wxCENTER);
        } else {
            elements->charList->Select(item, true);
            elements->charList->EnsureVisible(item);
            elements->charList->SetFocus();
        }

        break;

    case 1:
        item = elements->locList->FindItem(-1, event.GetString(), true);
        elements->locList->Select(item, true);
        elements->locList->EnsureVisible(item);
        elements->locList->SetFocus();
        break;

    case 2:
        break;

    case 3:
        break;
    }
}

void MainFrame::setLast() {
    std::ofstream last("88165468", std::ios::binary | std::ios::out);

    int size = currentDocFile.size() + 1;

    last.write(reinterpret_cast<char*>(&size), sizeof(int));
    last.write(currentDocFile.c_str(), size);

    size = currentTitle.size() + 1;
    last.write(reinterpret_cast<char*>(&size), sizeof(int));
    last.write(currentTitle.c_str(), size);

    size = currentImagePath.size() + 1;
    last.write(reinterpret_cast<char*>(&size), sizeof(int));
    last.write(currentImagePath.c_str(), size);

    size = currentDocFolder.size() + 1;
    last.write(reinterpret_cast<char*>(&size), sizeof(int));
    last.write(currentDocFolder.c_str(), size);

    last.close();
}

void MainFrame::getLast() {
    std::ifstream last("88165468", std::ios::binary | std::ios::in);

    if (last.is_open()) {

        int size;
        char* data;

        last.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        last.read(data, size);
        currentDocFile = data;
        delete data;
        previousDocFile = currentDocFile;

        last.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        last.read(data, size);
        currentTitle = data;
        delete data;
        previousTitle = currentTitle;

        last.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        last.read(data, size);
        currentImagePath = data;
        delete data;

        last.read(reinterpret_cast<char*>(&size), sizeof(int));
        data = new char[size];
        last.read(data, size);
        currentDocFolder = data;
        delete[] data;
        previousDocFolder = currentDocFolder;

        last.close();
    }

    if (fs::exists(currentDocFile)) {
        loadFile();
    } else {
        currentDocFile = "";
        previousDocFile = "dummy";
        currentTitle = "";
        previousTitle = "";
        currentImagePath = "";
        currentDocFolder = "";
        previousDocFolder = "dummy";
    }
}

void MainFrame::loadFile() {
    std::ifstream file(currentDocFile, std::ios::in | std::ios::binary);

    saved.clear();

    int progressSize = 0;
    int currentSize = 0;

    for (int i = 0; i < 4; i++) {
        saved.push_back(0);
        file.read((char*)&saved[i], sizeof(int));
        progressSize += saved[i];
    }

    outline->loadQuantities(file);
    progressSize += outline->getCount();

    wxProgressDialog* progress = new wxProgressDialog("Loading project...", currentDocFile, progressSize, this,
        wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_SMOOTH);

    characters.clear();

    elements->charNames.clear();
    for (int i = 0; i < saved[0]; i++) {
        Character character;
        character.load(file);

        if (fs::exists(currentImagePath + "\\Characters\\" +
            character.name + ".jpg")) {
            character.image = wxImage(currentImagePath + "\\Characters\\" +
                character.name + ".jpg");
        }

        characters[character.role + character.name] = character;
        elements->charNames.Add(character.name);
        progress->Update(currentSize++);
    }

    locations.clear();

    elements->locNames.clear();
    for (int i = 0; i < saved[1]; i++) {
        Location location;
        location.load(file);

        if (fs::exists(currentImagePath + "\\Locations\\" +
            location.name + ".jpg")) {
            location.image = wxImage(currentImagePath + "\\Locations\\" +
                location.name + ".jpg");
        }

        locations[location.importance + location.name] = location;
        elements->locNames.Add(location.name);
        progress->Update(currentSize++);
    }

    chaptersNote->getGrid()->clearAll();

    for (int i = 0; i < saved[2]; i++) {
        Chapter chapter;
        chapter.load(file);
        chaptersNote->getGrid()->chapters.push_back(chapter);
        chaptersNote->getGrid()->addButton();

        progress->Update(currentSize++);
    }

    outline->loadOutline(file, currentSize, progress);

    file.close();

    release->updateContent();

    SetTitle(currentTitle + " - Amadeus Writer");

    elements->setSearchAC(wxBookCtrlEvent());

    MainNotebook::updateLB();

    progress->Update(progressSize);
    progress->Hide();
    delete progress;

    isSaved = true;
}

void MainFrame::callUpdate(wxCommandEvent& event) {
    MainNotebook::updateLB();
}
