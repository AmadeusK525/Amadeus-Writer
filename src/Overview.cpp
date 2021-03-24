#include "Overview.h"
#include "amUtility.h"

#include "wxmemdbg.h"

amBookPicker::amBookPicker(amOverview* parent) :
	wxScrolledWindow(parent, -1, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxALWAYS_SHOW_SB | wxBORDER_SIMPLE) {
	m_parent = (amOverview*)parent;
	SetBackgroundColour(wxColour(30, 30, 30));
	SetDoubleBuffered(true);
	ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_NEVER);
	SetScrollRate(10, 10);

	m_bookCoverSize = { Book::GetCoverSize().x / 6, Book::GetCoverSize().y / 6};
	m_buttonSize = m_bookCoverSize + wxSize(15, 15);

	SetMinClientSize(wxSize(m_buttonSize.x + 5, m_buttonSize.y + 5));

	m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(m_mainSizer);
}

void amBookPicker::AddButton(Book& book, int index) {
	wxBitmapToggleButton* button = new wxBitmapToggleButton(this, -1,
		amGetScaledImage(m_bookCoverSize.x, m_bookCoverSize.y, book.cover),
		wxDefaultPosition, m_buttonSize);
	button->Bind(wxEVT_TOGGLEBUTTON, &amBookPicker::OnButtonPress, this);

	if (index >= m_bookButtons.size())
		m_bookButtons.push_back(button);
	else {
		if (index < 0)
			index = 0;

		m_bookButtons.insert(m_bookButtons.begin() + index, button);
	}
	m_mainSizer->Insert(index, button, wxSizerFlags().Border(wxBOTTOM | wxLEFT | wxTOP, 5));
	
	FitInside();
}

void amBookPicker::OnButtonPress(wxCommandEvent& event) {
	wxBitmapToggleButton* button = (wxBitmapToggleButton*)event.GetEventObject();

	for (wxBitmapToggleButton* buttonIt : m_bookButtons) {
		if (!buttonIt->GetValue() && buttonIt == button)
			buttonIt->SetValue(true);
		else if (buttonIt->GetValue() && buttonIt != button)
			buttonIt->SetValue(false);
	}

	event.Skip();
}

void amBookPicker::SetSelection(int bookPos) {
	for (int i = 0; i < m_bookButtons.size(); i++) {
		if (bookPos == i + 1)
			m_bookButtons[i]->SetValue(true);
		else
			m_bookButtons[i]->SetValue(false);
	}
}


/////////////////////////////////////////////////////////////////
/////////////////////////// Overview ////////////////////////////
/////////////////////////////////////////////////////////////////


amOverview::amOverview(wxWindow* parent, amProjectManager* manager) : wxPanel(parent) {
	m_manager = manager;

	SetBackgroundColour(wxColour(20, 20, 20));
	m_bookPicker = new amBookPicker(this);

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(m_bookPicker, wxSizerFlags(1).Top());
	sizer->AddStretchSpacer(1);

	SetSizer(sizer);
}

void amOverview::LoadOverview() {
	wxVector<Book>& books = m_manager->GetBooks();

	int i = 0;
	for (Book& book : books)
		m_bookPicker->AddButton(book, i++);

	m_bookPicker->SetSelection(m_manager->GetCurrentBookPos());
}
