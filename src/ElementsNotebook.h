#ifndef ELEMENTSNOTEBOOK_H_
#define ELEMENTSNOTEBOOK_H_

#pragma once

#include <wx\wx.h>
#include <wx\listctrl.h>
#include <wx\srchctrl.h>

#include "MainFrame.h"

class CharacterShowcase;
class LocationShowcase;

class ElementsNotebook : public wxNotebook {
private:
    MainFrame* mainFrame = nullptr;

public:
    CharacterShowcase* charShow = nullptr;
    LocationShowcase* locShow = nullptr;

    wxSearchCtrl* searchBar = nullptr;

    wxArrayString charNames{};
    wxArrayString locNames{};

public:
    ElementsNotebook(wxWindow* parent);

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
    void locSelected(wxListEvent& event);

    void setSearchAC(wxBookCtrlEvent& event);
    void addCharName(string& name);
    void addLocName(string& name);
    void removeCharName(string& name);
    void removeLocName(string& name);

    void clearAll();
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

#endif