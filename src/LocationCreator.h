#pragma once
#ifndef LOCATIONCREATOR_H_
#define LOCATIONCREATOR_H_

#include "wx/wx.h"
#include "MainFrame.h"
#include "ImagePanel.h"
#include "Location.h"

class LocationCreator: public wxFrame {
private:
    wxPanel* nlPanel1 = nullptr;
    wxPanel* nlPanel2 = nullptr;
    ImagePanel* nlImagePanel = nullptr;

    wxImage nlImage;

    wxTextCtrl* nlName = nullptr;
    wxTextCtrl* nlHBack = nullptr;
    wxTextCtrl* nlNatural = nullptr;
    wxTextCtrl* nlArquitecture = nullptr;
    wxTextCtrl* nlEconomy = nullptr;
    wxTextCtrl* nlCulture = nullptr;

    wxButton* nlCancel = nullptr;
    wxButton* nlNext = nullptr;
    wxButton* nlBack = nullptr;

    wxRadioButton* nlPrivate = nullptr;
    wxRadioButton* nlPublic = nullptr;

    wxRadioButton* nlHigh = nullptr;
    wxRadioButton* nlLow = nullptr;

    wxButton* nlChooseImage = nullptr;
    wxButton* nlRemoveImage = nullptr;

    Location* locationEdit = nullptr;

    ElementsNotebook* notebook = nullptr;

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
