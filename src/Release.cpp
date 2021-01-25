#include "Release.h"

BEGIN_EVENT_TABLE(amdRelease, wxPanel)

EVT_BUTTON(BUTTON_NextPage, amdRelease::OnNext)
EVT_BUTTON(BUTTON_PreviousPage, amdRelease::OnPrevious)

EVT_KEY_UP(amdRelease::OnKeyPressed)

END_EVENT_TABLE()

amdRelease::amdRelease(wxWindow* parent) :
    wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN) {
    SetBackgroundColour(wxColour(20, 20, 20));
}

amdRelease::~amdRelease() {

}

void amdRelease::UpdateContent() {

}

void amdRelease::OnNext(wxCommandEvent& event) {
    
}

void amdRelease::OnPrevious(wxCommandEvent& event) {

}

void amdRelease::OnKeyPressed(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_LEFT) {
        OnNext(wxCommandEvent());
        return;
    }

    if (event.GetKeyCode() == WXK_RIGHT)
        OnPrevious(wxCommandEvent());
}
