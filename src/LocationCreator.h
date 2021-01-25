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
    amdMainFrame* mainFrame = nullptr;
    amdElementsNotebook* notebook = nullptr;

    wxPanel* nlPanel1 = nullptr,
        * nlPanel3 = nullptr;
    wxScrolledWindow* nlPanel2 = nullptr;

    ImagePanel* nlImagePanel = nullptr;
    wxImage nlImage;

    wxTextCtrl* nlName = nullptr,
        * nlGeneral = nullptr,
        * nlNatural = nullptr,
        * nlArchitecture = nullptr,
        * nlPolitics = nullptr,
        * nlEconomy = nullptr,
        * nlCulture = nullptr;

    wxButton* nlCancel = nullptr,
        * nlNext = nullptr,
        * nlBack = nullptr;

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
    LocationCreator(wxWindow* parent,
        amdElementsNotebook* notebook,
        wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
        wxCLIP_CHILDREN | wxFRAME_SHAPED | wxFRAME_FLOAT_ON_PARENT | wxBORDER_SIMPLE);

    vector<string> GetValues();
    vector<pair<string, string>> GetCustom();

    void SetEdit(Location* location);
    void DoEdit(wxCommandEvent& event);

    void SetImage(wxCommandEvent& event);
    void RemoveImage(wxCommandEvent& event);

    void AddCustomAttr(wxCommandEvent& event);
    void RemoveCustomAttr(wxCommandEvent& event);
    void recolorCustoms();

    void CheckClose(wxCloseEvent& event);
    void Next(wxCommandEvent& event);
    void Back(wxCommandEvent& event);
    void Create(wxCommandEvent& event);
    void Cancel(wxCommandEvent& event);

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
