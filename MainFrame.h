#pragma once

#ifndef MAINFRAME_H_
#define MAINFRAME_H_

#include "wx/menu.h"
#include "wx/textctrl.h"
#include "wx/srchctrl.h"
#include "wx/listctrl.h"
#include "wx/notebook.h"
#include "wx/gdicmn.h"
#include "wx/aboutdlg.h"
#include "wx/toolbook.h"
#include "wx/progdlg.h"

#include <map>
#include <string>
#include <vector>
#include <fstream>

#include "Character.h"
#include "Location.h"
#include "MainNotebook.h"
#include "Release.h"
#include "ChaptersNotebook.h"
#include "Outline.h"

#include "boost/filesystem.hpp"

using std::string;
using std::vector;

class MainToolBar;
class ChaptersGrid;
class ChaptersNotebook;
class Release;

class MainFrame : public wxFrame {
private:
    wxPanel* panel = nullptr;

    wxPanel* mainPanel = nullptr;
    wxPanel* overview = nullptr;
    MainNotebook* elements = nullptr;
    ChaptersNotebook* chaptersNote = nullptr;
    Release* release = nullptr;
    Outline* outline = nullptr;

    wxPanel* pageSel = nullptr;

    wxMenuBar* mainMenu = nullptr;
    wxMenu* projectMenu = nullptr;
    wxMenu* fileMenu = nullptr;
    wxMenu* editMenu = nullptr;
    wxMenu* helpMenu = nullptr;

    string currentDocFile = "";
    string previousDocFile = "dummy";
    string previousDocFolder = "dummy";
    string currentImagePath = "";
    string currentTitle = "New Amadeus project";
    string previousTitle = "";

    wxToolBar* toolBar = nullptr;

    wxBoxSizer* mainSizer = nullptr;
    wxBoxSizer* ver = nullptr;
    wxBoxSizer* buttonSizer = nullptr;

    wxTimer m_timer{this, -1};

public:
    static string currentDocFolder;

    static std::map<string, Character> characters;
    static std::map<string, Location> locations;
    static vector<int> saved;

    static bool isSaved;
    bool showDialog = true;

public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    ChaptersNotebook* getNote() { return chaptersNote; }

    void setLast();
    void getLast();
    void loadFile();

    void quit(wxCommandEvent& event);
    void newFile(wxCommandEvent& event);
    void openFile(wxCommandEvent& event);
    void saveFile(wxCommandEvent& event);
    void saveFileAs(wxCommandEvent& event);

    void editTitle(wxCommandEvent& event);

    void newChar(wxCommandEvent& event);
    void newChap(wxCommandEvent& event);
    void newLoc(wxCommandEvent& event);

    void callUpdate(wxCommandEvent& event);

    void about(wxCommandEvent& event);

    void onQuit(wxCloseEvent& event);

    void notSaved(wxCommandEvent& event);

    void onOverview(wxCommandEvent& event);
    void onElements(wxCommandEvent& event);
    void onChapters(wxCommandEvent& event);
    void onRelease(wxCommandEvent& event);
    void onOutline(wxCommandEvent& event);

    void search(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

enum {

    MAIN_Panel,

    MENU_NewChapter,
    MENU_NewScene,
    MENU_NewCharacter,
    MENU_NewLocation,

    TOOL_NewChapter,
    TOOL_NewScene,
    TOOL_NewCharacter,
    TOOL_NewLocation,
    TOOL_Search,
    TOOL_Save,

    MENU_New,
    MENU_Open,
    MENU_Save,
    MENU_SaveAs,
    MENU_Quit,

    MENU_Update,
    MENU_ProjectName,

    MENU_About,

    BUTTON_Overview,
    BUTTON_Elem,
    BUTTON_Chapters,
    BUTTON_Release,
    BUTTON_Outline
};

#endif
