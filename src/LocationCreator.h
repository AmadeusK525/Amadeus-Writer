#ifndef LOCATIONCREATOR_H_
#define LOCATIONCREATOR_H_
#pragma once

#include "MainFrame.h"
#include "ImagePanel.h"
#include "Location.h"

class LocationCreator: public wxFrame {
private:
    MainFrame* mainFrame = nullptr;
    ElementsNotebook* notebook = nullptr;

    wxPanel* nlPanel1 = nullptr,
        * nlPanel2 = nullptr;
    ImagePanel* nlImagePanel = nullptr;

    wxImage nlImage;

    wxTextCtrl* nlName = nullptr,
        * nlHBack = nullptr,
        * nlNatural = nullptr,
        * nlArquitecture = nullptr,
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

    Location* locationEdit = nullptr;

public:
    LocationCreator(wxWindow* parent, ElementsNotebook* notebook);

    std::vector<std::string> getValues();

    void setEdit(Location* location);
    void nextEdit(wxCommandEvent& event);
    void backEdit(wxCommandEvent& event);
    void edit(wxCommandEvent& event);

    void setImage(wxCommandEvent& event);
    void removeImage(wxCommandEvent& event);

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
    BUTTON_BackEditLoc,
    BUTTON_BackLoc,
    BUTTON_NextEditLoc,
    BUTTON_NextLoc,
    BUTTON_CreateLoc
};

#endif
