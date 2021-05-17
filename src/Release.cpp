#include "Release.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(amRelease, wxPanel)

EVT_BUTTON(BUTTON_NextPage, amRelease::OnNext)
EVT_BUTTON(BUTTON_PreviousPage, amRelease::OnPrevious)

EVT_KEY_UP(amRelease::OnKeyPressed)

END_EVENT_TABLE()

amRelease::amRelease(wxWindow* parent) :
	wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN)
{
	SetBackgroundColour(wxColour(20, 20, 20));
}

amRelease::~amRelease()
{

}

void amRelease::SetBookData(Book* book)
{}

void amRelease::UpdateContent()
{

}

void amRelease::OnNext(wxCommandEvent& event)
{

}

void amRelease::OnPrevious(wxCommandEvent& event)
{

}

void amRelease::OnKeyPressed(wxKeyEvent& event)
{
	if ( event.GetKeyCode() == WXK_LEFT )
	{
		OnNext(wxCommandEvent());
		return;
	}

	if ( event.GetKeyCode() == WXK_RIGHT )
		OnPrevious(wxCommandEvent());
}
