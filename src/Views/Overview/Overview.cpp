#include "Views/Overview/Overview.h"
#include "Style/Style.hpp"
#include "Utils/amUtility.h"
#include "MyApp.h"

#include "Utils/wxmemdbg.h"

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

	m_bookCoverSize = { am::Book::GetCoverSize().x / 6, am::Book::GetCoverSize().y / 6 };
	m_buttonSize = m_bookCoverSize + FromDIP(wxSize(15, 15));

	m_mainWindow->SetMinClientSize(FromDIP(wxSize(-1, m_buttonSize.y + 10)));

	const wxSize buttonSize(24, 24);
	wxButton* addBook = new wxButton(this, -1, "", wxDefaultPosition, buttonSize);
	addBook->SetBitmap(Style::Icon::GetBmp(IconName::Plus, buttonSize));
	addBook->Bind(wxEVT_BUTTON, &amBookPicker::OnCreateBook, this);
	wxButton* removeBook = new wxButton(this, -1, "", wxDefaultPosition, buttonSize);
	removeBook->SetBitmap(Style::Icon::GetBmp(IconName::Minus, buttonSize));
	removeBook->Bind(wxEVT_BUTTON, &amBookPicker::OnDeleteBook, this);

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

void amBookPicker::AddButton(am::Book* book, size_t index)
{
	amBookButton* button = new amBookButton(m_mainWindow, book,
		am::GetScaledImage(m_bookCoverSize.x, m_bookCoverSize.y, book->cover),
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
	amBookButton* pButton = (amBookButton*)event.GetEventObject();
	am::Book* pCurrentBook = am::GetCurrentBook();

	if ( pCurrentBook != pButton->book )
	{
		wxBusyCursor cursor;
		if ( !am::SetCurrentBook(pButton->book->pos) )
		{
			SetSelectionByBook(am::GetCurrentBook());
		}
	}

	event.Skip();
}

void amBookPicker::SetSelectionByBook(am::Book* book)
{
	for ( size_t i = 0; i < m_bookButtons.size(); i++ )
	{
		if ( book == m_bookButtons[i]->book )
		{
			m_bookButtons[i]->SetValue(true);
			m_bookButtons[i]->SetFocus();
		}
		else
			m_bookButtons[i]->SetValue(false);
	}
}

void amBookPicker::OnCreateBook(wxCommandEvent& event)
{
	am::StartCreatingBook();
}

void amBookPicker::OnDeleteBook(wxCommandEvent & event)
{}

void amBookPicker::DeleteAllButtons()
{
	for ( amBookButton*& pButton : m_bookButtons )
		pButton->Destroy();

	m_bookButtons.clear();
}


/////////////////////////////////////////////////////////////////
/////////////////////////// Overview ////////////////////////////
/////////////////////////////////////////////////////////////////


amOverview::amOverview(wxWindow* parent) : wxPanel(parent)
{
	SetBackgroundColour(wxColour(20, 20, 20));
	m_bookPicker = new amBookPicker(this);

	m_stBookTitle = new wxStaticText(this, -1, "", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
	m_stBookTitle->SetBackgroundColour(wxColour(0, 0, 0));
	m_stBookTitle->SetForegroundColour(wxColour(255, 255, 255));
	m_stBookTitle->SetFont(wxFontInfo(20).Bold());
	m_stBookTitle->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_stBookTitle->SetDoubleBuffered(true);

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(m_bookPicker, wxSizerFlags(1).Top());
	sizer->Add(m_stBookTitle, wxSizerFlags(1).Top());

	wxStaticText* pRecentDocumentsLabel = new wxStaticText(this, -1, _("Recent Documents:"));
	pRecentDocumentsLabel->SetBackgroundColour(wxColour(60, 60, 60));
	pRecentDocumentsLabel->SetForegroundColour(wxColour(240,240,240));
	pRecentDocumentsLabel->SetFont(wxFontInfo(10).Bold());

	m_recentDocumentsModel = new StoryTreeModel;

#ifdef __WXMSW__
	m_recentDocumentsDVC = m_recentDocumentsHandler.Create(this, -1, m_recentDocumentsModel.get(),
		wxBORDER_NONE | wxDV_MULTIPLE);
#else
	m_recentDocumentsDVC = new wxDataViewCtrl(this, -1, wxDefaultPosition, wxDefaultSize,
		wxDV_SINGLE | wxBORDER_NONE);
	m_recentDocumentsDVC->AssociateModel(m_recentDocumentsModel.get());
#endif

	m_recentDocumentsDVC->SetBackgroundColour(wxColour(90, 90, 90));
	m_recentDocumentsDVC->SetMinSize(wxSize(FromDIP(250), -1));
	m_recentDocumentsDVC->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &amOverview::OnRecentDocument, this);

	wxDataViewColumn* pColumn = new wxDataViewColumn(_("Document name"), new wxDataViewIconTextRenderer(wxDataViewIconTextRenderer::GetDefaultType(),
		wxDATAVIEW_CELL_EDITABLE), 0, FromDIP(170), wxALIGN_LEFT);
	m_recentDocumentsDVC->AppendColumn(pColumn);

	pColumn = new wxDataViewColumn(_("Words"), new wxDataViewTextRenderer(wxDataViewTextRenderer::GetDefaultType(),
		wxDATAVIEW_CELL_INERT), 1, FromDIP(80), wxALIGN_CENTER);
	m_recentDocumentsDVC->AppendColumn(pColumn);

	wxBoxSizer* pRecentDocumentsSizer = new wxBoxSizer(wxVERTICAL);
	pRecentDocumentsSizer->Add(pRecentDocumentsLabel, wxSizerFlags(0).Expand());
	pRecentDocumentsSizer->Add(m_recentDocumentsDVC, wxSizerFlags(1));

	wxStaticText* pWIP = new wxStaticText(this, -1, "Work in Progress");
	pWIP->SetFont(wxFontInfo(15).Bold());
	pWIP->SetForegroundColour(wxColour(255, 255, 255));

	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	m_mainSizer->Add(sizer, wxSizerFlags(0).Expand());
	m_mainSizer->AddSpacer(10);
	m_mainSizer->Add(pRecentDocumentsSizer, wxSizerFlags(1).Left());
	m_mainSizer->Add(pWIP, wxSizerFlags(0).Left().Border(wxALL, 5));

	SetSizer(m_mainSizer);
	Layout();
}

void amOverview::ClearAll()
{
	m_bookPicker->DeleteAllButtons();
	m_recentDocumentsModel->ClearAll();
	m_stBookTitle->SetLabel("");
}

void amOverview::SetBookData(am::Book* book)
{
	Freeze();

	m_bookPicker->SetSelectionByBook(book);

	m_stBookTitle->SetLabel(book->title);
	Layout();
	m_stBookTitle->Wrap(m_stBookTitle->GetSize().x);
	Layout();

	LoadRecentDocuments(book);

	Thaw();
}

void amOverview::LoadRecentDocuments(am::Book* book)
{
	Freeze();

	m_recentDocumentsModel->ClearAll();

	for ( int i = book->vRecentDocuments.size() - 1; i >= 0; i-- )
	{
		wxDataViewItem docItem = m_recentDocumentsModel->AddDocument(book->vRecentDocuments[i], wxDataViewItem(nullptr));
	}

	Thaw();
}

void amOverview::OnRecentDocument(wxDataViewEvent& event)
{
	StoryTreeModelNode* pNode = (StoryTreeModelNode*)event.GetItem().GetID();
	if ( !pNode )
		return;

	if ( pNode->GetDocument() )
		am::OpenDocument(pNode->GetDocument());

	event.Skip();
}

void amOverview::LoadBookContainer()
{
	m_bookPicker->Freeze();
	m_bookPicker->DeleteAllButtons();

	wxVector<am::Book*>& books = am::GetBooks();

	int i = 0;
	for ( am::Book*& book : books )
		m_bookPicker->AddButton(book, i++);

	m_bookPicker->SetSelectionByBook(am::GetCurrentBook());
	m_bookPicker->Layout();

	m_bookPicker->Thaw();
}
