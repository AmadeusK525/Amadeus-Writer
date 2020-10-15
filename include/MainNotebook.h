#pragma once
#include "wx/notebook.h"
#include "wx/listctrl.h"
#include "wx/srchctrl.h"
#include "wx/editlbox.h"
#include "wx/wx.h"

#include "CharacterShowcase.h"
#include "LocationShowcase.h"

class MainNotebook : public wxNotebook {
private:
    wxWindow* parent = nullptr;

public:
    CharacterShowcase* charShow = nullptr;
    LocationShowcase* locShow = nullptr;

    wxSearchCtrl* searchBar = nullptr;

    wxArrayString charNames{};
    wxArrayString locNames{};

public:
    MainNotebook(wxWindow* parent, wxWindow* main);

    void onCharRightClick(wxListEvent& event);
    void editCharName(wxListEvent& event);
    void editChar(wxCommandEvent& event);
    void deleteChar(wxCommandEvent& event);
    void openChar(wxListEvent& event);

    void onLocRightClick(wxListEvent& event);
    void editLoc(wxCommandEvent& event);
    void deleteLoc(wxCommandEvent& event);
    void openLoc(wxListEvent& event);

    void charSelected(wxListEvent& event);
    void charDeselected(wxListEvent& event);
    void locSelected(wxListEvent& event);
    void locDeselected(wxListEvent& event);

    void setSearchAC(wxBookCtrlEvent& event);
    void addCharName(string& name);
    void addLocName(string& name);
    void removeCharName(string& name);
    void removeLocName(string& name);

    static void updateLB();

    static wxListView* charList;
    static wxListView* locList;
    static wxListView* itemsList;

    DECLARE_EVENT_TABLE()
};

enum {
    NOTEBOOK_THIS,

    PANEL_Char,

    LIST_CharList,
    LIST_LocList,
    LIST_ItemsList,
    LIST_ScenesList,

    LISTMENU_EditChar,
    LISTMENU_DeleteChar,
    LISTMENU_EditLoc,
    LISTMENU_DeleteLoc
};
