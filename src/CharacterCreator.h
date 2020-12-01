#pragma once

#ifndef CHARACTERCREATOR_H_
#define CHARACTERCREATOR_H_

#include "wx/wx.h"
#include "Character.h"
#include "ImagePanel.h"
#include "MainFrame.h"

class CharacterCreator: public wxFrame {
private:
    wxPanel* ncPanel1 = nullptr;
    wxPanel* ncPanel2 = nullptr;
    ImagePanel* ncImagePanel = nullptr;

    wxImage ncImage;

    wxTextCtrl* ncFullName = nullptr;
    wxTextCtrl* ncNickname = nullptr;
    wxTextCtrl* ncAge = nullptr;
    wxTextCtrl* ncNationality = nullptr;
    wxTextCtrl* ncHeight = nullptr;
    wxTextCtrl* ncBackstory = nullptr;
    wxTextCtrl* ncAppearance = nullptr;
    wxTextCtrl* ncPersonality = nullptr;

    wxRadioButton* male = nullptr;
    wxRadioButton* female = nullptr;

    wxRadioButton* main = nullptr;
    wxRadioButton* secon = nullptr;

    wxButton* ncBack = nullptr;
    wxButton* ncCancel = nullptr;
    wxButton* ncNext = nullptr;

    wxButton* ncChooseImage = nullptr;
    wxButton* ncRemoveImage = nullptr;

    Character* charEdit = nullptr;

    ElementsNotebook* notebook;
    MainFrame* mainFrame;

public:
    CharacterCreator(wxWindow* parent, ElementsNotebook* notebook);

    std::vector<std::string> getValues();

    void setEdit(Character* character);
    void nextEdit(wxCommandEvent& event);
    void backEdit(wxCommandEvent& event);
    void edit(wxCommandEvent& event);

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
    BUTTON_BackEdit,
    BUTTON_BackChar,
    BUTTON_CreateEdit,
    BUTTON_NextEdit,
    BUTTON_NextChar,
    BUTTON_CreateChar
};

#endif
