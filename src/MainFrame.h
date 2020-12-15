#ifndef MAINFRAME_H_
#define MAINFRAME_H_

#pragma once

#include <wx\wx.h>
#include <wx\simplebook.h>

#include <map>
#include <string>
#include <vector>
#include <fstream>

#include "Character.h"
#include "Location.h"

#include "boost/filesystem.hpp"

using std::string;
using std::vector;

// Declaring some classes which will be used later. If I don't, there are some issues when including them.
class ElementsNotebook;
class ChaptersNotebook;
class Outline;
class Release;

class MainFrame : public wxFrame {
private:
    wxPanel* mainPanel = nullptr;
    wxPanel* panel = nullptr;

    wxPanel* overview = nullptr;
    ElementsNotebook* elements = nullptr;
    ChaptersNotebook* chaptersNote = nullptr;
    Outline* outline = nullptr;
    Release* release = nullptr;

    vector<wxButton*> buttons;
    wxPanel* pageSel = nullptr;

    wxSimplebook* mainBook = nullptr;

    wxBoxSizer* holderSizer = nullptr;
    bool isFrameFullScreen = false;

    wxMenuBar* mainMenu = nullptr;
    wxMenu* projectMenu = nullptr;
    wxMenu* fileMenu = nullptr;
    wxMenu* editMenu = nullptr;
    wxMenu* helpMenu = nullptr;

    //These are paths used for loading stuff, saving, etc.
    string currentDocFile = "dummy";
    string previousDocFile = "dummy";
    string previousDocFolder = "dummy";
    string currentImagePath = "";
    string currentTitle = "New Amadeus project";
    string previousTitle = "";
    string executablePath = "";

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

    ChaptersNotebook* getNote();
    Outline* getOutline();

    // These three go together when saving / loading. setLast writes to a file the path to the most recently
    // worked on project. When booting up the application, the getLast function will
    // be called and it will get the written path, opening up the project automatically,
    // with no need to load it. Makes things more convenient.
    void setLast();
    void getLast();
    void loadFile();

    bool loadFileFromOpenWith(string& path);
    void setExecPath(string& path);

    void quit(wxCommandEvent& event);
    void newFile(wxCommandEvent& event);
    void openFile(wxCommandEvent& event);
    void saveFile(wxCommandEvent& event);
    void saveFileAs(wxCommandEvent& event);
    void exportCorkboard(wxCommandEvent& event);

    void editTitle(wxCommandEvent& event);

    void fullScreen(wxCommandEvent& event);
    void newChar(wxCommandEvent& event);
    void newChap(wxCommandEvent& event);
    void newLoc(wxCommandEvent& event);

    void callUpdate(wxCommandEvent& event);

    void about(wxCommandEvent& event);

    void onQuit(wxCloseEvent& event);

    void notSaved(wxCommandEvent& event);

    void onMainButtons(wxCommandEvent& event);

    void search(wxCommandEvent& event);

    void corkboardFullScreen(bool fs, wxWindow* toolBar, wxWindow* canvas);

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
    TOOL_FullScreen,

    MENU_New,
    MENU_Open,
    MENU_Save,
    MENU_SaveAs,
    MENU_Quit,

    MENU_ExportCorkboardPNG,
    MENU_ExportCorkboardBMP,

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
