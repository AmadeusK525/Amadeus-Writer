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
    wxPanel* m_panel1 = nullptr,
        * m_panel3 = nullptr;
    wxScrolledWindow* m_panel2 = nullptr;

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

    wxRadioButton* ncMale = nullptr,
        * ncFemale = nullptr;

    wxRadioButton* ncMain = nullptr,
        * ncSupp = nullptr,
        * ncVillian = nullptr,
        * ncSecon = nullptr;

    wxButton* ncBack = nullptr;
    wxButton* ncCancel = nullptr;
    wxButton* ncNext = nullptr;

    wxButton* ncAddCustom = nullptr;
    wxBoxSizer* m_customSizer = nullptr;

    wxButton* ncChooseImage = nullptr;
    wxButton* ncRemoveImage = nullptr;

    Character* charEdit = nullptr;

    amdElementsNotebook* notebook;
    amdMainFrame* mainFrame;

    wxBoxSizer* mainSiz = nullptr;

public:
    CharacterCreator(wxWindow* parent,
        amdElementsNotebook* notebook,
        long id =  wxID_ANY,
        const string& label = "Create character",
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxSize(520, 585),
        long style = wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX |
        wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT);

    vector<string> GetValues();
    vector<pair<string, string>> GetCustom();

    void SetEdit(Character* character);
    void DoEdit(wxCommandEvent& event);

    void AddCustomAttr(wxCommandEvent& event);
    void RemoveCustomAttr(wxCommandEvent& event);

    void Cancel(wxCommandEvent& event);
    void Next(wxCommandEvent& event);
    void Back(wxCommandEvent& event);
    void Create(wxCommandEvent& event);
    void CheckClose(wxCloseEvent& event);

    void SetImage(wxCommandEvent& event);
    void RemoveImage(wxCommandEvent& event);

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
