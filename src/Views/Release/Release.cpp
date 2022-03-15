#include "Views/Release/Release.h"

#include "Utils/wxmemdbg.h"

BEGIN_EVENT_TABLE(amRelease, wxPanel)

//EVT_BUTTON(BUTTON_NextPage, amRelease::OnNext)
//EVT_BUTTON(BUTTON_PreviousPage, amRelease::OnPrevious)

EVT_KEY_UP(amRelease::OnKeyPressed)

END_EVENT_TABLE()

amRelease::amRelease(wxWindow* parent) :
	wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
	SetBackgroundColour(wxColour(20, 20, 20));
	wxStaticText* pWIP = new wxStaticText(this, -1, "Work in Progress");
	pWIP->SetFont(wxFontInfo(15).Bold());
	pWIP->SetForegroundColour(wxColour(255, 255, 255));

	wxBoxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);
	pSizer->Add(pWIP, wxSizerFlags(0).Bottom().Border(wxALL, 5));

	SetSizer(pSizer);
}

amRelease::~amRelease()
{
}

void amRelease::ClearAll()
{
}

void amRelease::SetBookData(am::Book* book)
{
}

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
		//OnNext(wxCommandEvent());
		return;
	}

    if ( event.GetKeyCode() == WXK_RIGHT ) {}
		//OnPrevious(wxCommandEvent());
}
