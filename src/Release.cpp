#include "Release.h"

BEGIN_EVENT_TABLE(Release, wxPanel)

EVT_BUTTON(BUTTON_NextPage, Release::next)
EVT_BUTTON(BUTTON_PreviousPage, Release::previous)

EVT_KEY_UP(Release::keyPressed)

END_EVENT_TABLE()

Release::Release(wxWindow* parent) :
    wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN) {
    SetBackgroundColour(wxColour(20, 20, 20));
}

Release::~Release() {

}

void Release::updateContent() {

}

void Release::next(wxCommandEvent& event) {
    
}

void Release::previous(wxCommandEvent& event) {

}

void Release::keyPressed(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_LEFT) {
        next(wxCommandEvent());
        return;
    }

    if (event.GetKeyCode() == WXK_RIGHT)
        previous(wxCommandEvent());
}
