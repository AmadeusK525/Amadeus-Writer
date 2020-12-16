#ifndef CHARACTERCREATOR_H_
#define CHARACTERCREATOR_H_

#pragma once

#include <wx\wx.h>
#include <wx\scrolwin.h>

#include "Character.h"
#include "ImagePanel.h"
#include "MainFrame.h"

class CharacterCreator: public wxFrame {
private:
    wxPanel* ncPanel1 = nullptr,
        * ncPanel3 = nullptr;
    wxScrolledWindow* ncPanel2 = nullptr;

    ImagePanel* ncImagePanel = nullptr;
    wxImage ncImage;

    wxStaticText* label8 = nullptr;

    wxTextCtrl* ncFullName = nullptr,
        * ncNickname = nullptr,
        * ncAge = nullptr,
        * ncNationality = nullptr,
        * ncHeight = nullptr,
        * ncBackstory = nullptr,
        * ncAppearance = nullptr,
        * ncPersonality = nullptr;

    vector<pair<wxTextCtrl*, wxTextCtrl*>> ncCustom{};
    vector<wxButton*> minusButtons{};

    wxRadioButton* ncMale = nullptr;
    wxRadioButton* ncFemale = nullptr;

    wxRadioButton* ncMain = nullptr;
    wxRadioButton* ncSecon = nullptr;

    wxButton* ncBack = nullptr;
    wxButton* ncCancel = nullptr;
    wxButton* ncNext = nullptr;

    wxButton* ncAddCustom = nullptr;
    wxBoxSizer* ncp2Sizer = nullptr;

    wxButton* ncChooseImage = nullptr;
    wxButton* ncRemoveImage = nullptr;

    Character* charEdit = nullptr;

    ElementsNotebook* notebook;
    MainFrame* mainFrame;

    wxBoxSizer* mainSiz = nullptr;

public:
    CharacterCreator(wxWindow* parent,
        ElementsNotebook* notebook,
        long id =  wxID_ANY,
        const string& label = "Create character",
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(520, 585),
        long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
        wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);

    vector<string> getValues();
    vector<pair<string, string>> getCustom();

    void setEdit(Character* character);
    void doEdit(wxCommandEvent& event);

    void addCustomAttr(wxCommandEvent& event);
    void removeCustomAttr(wxCommandEvent& event);

    void cancel(wxCommandEvent& event);
    void next(wxCommandEvent& event);
    void back(wxCommandEvent& event);
    void create(wxCommandEvent& event);
    void checkClose(wxCloseEvent& event);

    void setImage(wxCommandEvent& event);
    void removeImage(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

enum {
    BUTTON_CharRemoveImage,
    BUTTON_CharImage,
    
    BUTTON_CancelChar,
    
    BUTTON_BackEdit1,
    BUTTON_BackEdit2,
    
    BUTTON_BackChar1,
    BUTTON_BackChar2,
    
    BUTTON_CreateEdit,
    
    BUTTON_NextEdit1,
    BUTTON_NextEdit2,
    
    BUTTON_NextChar1,
    BUTTON_NextChar2,
    
    BUTTON_CreateChar
};

#endif
