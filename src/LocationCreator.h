#ifndef LOCATIONCREATOR_H_
#define LOCATIONCREATOR_H_
#pragma once

#include "MainFrame.h"
#include "ImagePanel.h"
#include "Location.h"

#include <wx\wrapsizer.h>

#include <vector>

using std::string;
using std::vector;
using std::pair;

class LocationCreator: public wxFrame {
private:
    MainFrame* mainFrame = nullptr;
    ElementsNotebook* notebook = nullptr;

    wxPanel* nlPanel1 = nullptr,
        * nlPanel3 = nullptr;
    wxScrolledWindow* nlPanel2 = nullptr;

    ImagePanel* nlImagePanel = nullptr;
    wxImage nlImage;

    wxTextCtrl* nlName = nullptr,
        * nlHBack = nullptr,
        * nlNatural = nullptr,
        * nlArchitecture = nullptr,
        * nlEconomy = nullptr,
        * nlCulture = nullptr;

    wxButton* nlCancel = nullptr,
        * nlNext = nullptr,
        * nlBack = nullptr;

    wxRadioButton* nlPrivate = nullptr,
        * nlPublic = nullptr;

    wxRadioButton* nlHigh = nullptr,
        * nlLow = nullptr;

    wxButton* nlChooseImage = nullptr,
        * nlRemoveImage = nullptr;

    Location* m_locationEdit = nullptr;

    vector<pair<wxTextCtrl*, wxTextCtrl*>> nlCustom{};
    vector<wxButton*> m_minusButtons{};

    wxStaticText* label6 = nullptr;
    wxWrapSizer* m_customSizer = nullptr;
    wxBoxSizer* m_mainSiz = nullptr;

public:
    LocationCreator(wxWindow* parent, ElementsNotebook* notebook);

    std::vector<std::string> getValues();

    void setEdit(Location* location);
    void doEdit(wxCommandEvent& event);

    void setImage(wxCommandEvent& event);
    void removeImage(wxCommandEvent& event);

    void addCustomAttr(wxCommandEvent& event);
    void removeCustomAttr(wxCommandEvent& event);
    void recolorCustoms();

    void checkClose(wxCloseEvent& event);
    void next(wxCommandEvent& event);
    void back(wxCommandEvent& event);
    void create(wxCommandEvent& event);
    void cancel(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

enum {
    BUTTON_RemoveImageLoc,
    BUTTON_ChooseImageLoc,
    BUTTON_CancelLoc,
    BUTTON_CreateEditLoc,
    BUTTON_BackEditLoc1,
    BUTTON_BackEditLoc2,
    BUTTON_BackLoc1,
    BUTTON_BackLoc2,
    BUTTON_NextEditLoc1,
    BUTTON_NextEditLoc2,
    BUTTON_NextLoc1,
    BUTTON_NextLoc2,
    BUTTON_CreateLoc
};

#endif
