#include "Overview.h"
#include "amUtility.h"
#include "MyApp.h"

#include "wxmemdbg.h"

amBookPicker::amBookPicker(amOverview* parent) :
	wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE)
{
	m_overview = parent;

	m_mainWindow = new wxScrolledWindow(this, -1, wxDefaultPosition, wxDefaultSize,
		wxHSCROLL | wxALWAYS_SHOW_SB | wxBORDER_SIMPLE);

	m_mainWindow->SetBackgroundColour(wxColour(30, 30, 30));
	m_mainWindow->SetDoubleBuffered(true);
	m_mainWindow->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_NEVER);
	m_mainWindow->SetScrollRate(10, 10);

	m_bookCoverSize = { Book::GetCoverSize().x / 6, Book::GetCoverSize().y / 6 };
	m_buttonSize = m_bookCoverSize + wxSize(15, 15);

	m_mainWindow->SetMinClientSize(wxSize(-1, m_buttonSize.y + 10));

	wxButton* addBook = new wxButton(this, -1, "", wxDefaultPosition, wxSize(25, 25));
	addBook->SetBitmap(wxBITMAP_PNG(plus));
	wxButton* removeBook = new wxButton(this, -1, "", wxDefaultPosition, wxSize(25, 25));
	removeBook->SetBitmap(wxBITMAP_PNG(minus));

	wxBoxSizer* bottomLine = new wxBoxSizer(wxHORIZONTAL);
	bottomLine->AddStretchSpacer(1);
	bottomLine->Add(removeBook, wxSizerFlags(0).CenterVertical());
	bottomLine->Add(addBook, wxSizerFlags(0).CenterVertical());

	m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
	m_mainWindow->SetSizer(m_mainSizer);

	wxBoxSizer* vertical = new wxBoxSizer(wxVERTICAL);
	vertical->Add(m_mainWindow, wxSizerFlags(1).Expand());
	vertical->Add(bottomLine, wxSizerFlags(0).Expand());
	SetSizerAndFit(vertical);
}

void amBookPicker::AddButton(Book* book, int index)
{
	amBookButton* button = new amBookButton(m_mainWindow, book,
		amGetScaledImage(m_bookCoverSize.x, m_bookCoverSize.y, book->cover),
		wxDefaultPosition, m_buttonSize);
	button->Bind(wxEVT_TOGGLEBUTTON, &amBookPicker::OnBookClicked, this);

	if ( index >= m_bookButtons.size() )
		m_bookButtons.push_back(button);
	else
	{
		if ( index < 0 )
			index = 0;

		m_bookButtons.insert(m_bookButtons.begin() + index, button);
	}
	m_mainSizer->Insert(index, button, wxSizerFlags().Border(wxBOTTOM | wxLEFT | wxTOP, 5));

	m_mainWindow->Fit();
}

void amBookPicker::OnBookClicked(wxCommandEvent& event)
{
	amBookButton* button = (amBookButton*)event.GetEventObject();
	Book* currentBook = amGetManager()->GetCurrentBook();

	if ( currentBook != button->book )
	{
		wxBusyCursor cursor;
		if ( !amGetManager()->SetCurrentBook(button->book->pos) )
		{
			for ( amBookButton*& pButton : m_bookButtons )
				if ( pButton->book == currentBook )
					button = pButton;
		}
		else
		{
			currentBook = amGetManager()->GetCurrentBook();
		}
	}

	for ( amBookButton* buttonIt : m_bookButtons )
	{
		if ( buttonIt->book == currentBook )
		{
			buttonIt->SetValue(true);
			buttonIt->SetFocus();
		}
		else
		{
			buttonIt->SetValue(false);
		}
	}

	event.Skip();
}

void amBookPicker::SetSelection(int bookPos)
{
	for ( int i = 0; i < m_bookButtons.size(); i++ )
	{
		if ( bookPos == i + 1 )
			m_bookButtons[i]->SetValue(true);
		else
			m_bookButtons[i]->SetValue(false);
	}
}


/////////////////////////////////////////////////////////////////
/////////////////////////// Overview ////////////////////////////
/////////////////////////////////////////////////////////////////


amOverview::amOverview(wxWindow* parent, amProjectManager* manager) : wxPanel(parent)
{
	m_manager = manager;

	SetBackgroundColour(wxColour(20, 20, 20));
	m_bookPicker = new amBookPicker(this);

	m_stBookTitle = new wxStaticText(this, -1, "", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
	m_stBookTitle->SetBackgroundColour(wxColour(0, 0, 0));
	m_stBookTitle->SetForegroundColour(wxColour(255, 255, 255));
	m_stBookTitle->SetFont(wxFontInfo(20).Bold());

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(m_bookPicker, wxSizerFlags(1).Top());
	sizer->Add(m_stBookTitle, wxSizerFlags(1).Top());

	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	m_mainSizer->Add(sizer, wxSizerFlags(0).Expand());

	SetSizer(m_mainSizer);
}

void amOverview::SetBookData(Book* book)
{
	Freeze();

	m_stBookTitle->SetLabel(book->title);
	Layout();
	m_stBookTitle->Wrap(m_stBookTitle->GetSize().x);
	Layout();

	Thaw();
}

void amOverview::LoadBookContainer()
{
	wxVector<Book*>& books = m_manager->GetBooks();

	int i = 0;
	for ( Book*& book : books )
		m_bookPicker->AddButton(book, i++);

	m_bookPicker->SetSelection(m_manager->GetCurrentBookPos());
}
