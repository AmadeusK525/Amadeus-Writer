#include "BookWizard.h"

BookWizardTreeModel::BookWizardTreeModel(am::Book* pBookToInsert) : m_pBookToInsert(pBookToInsert)
{
	for ( int i = 0; i <= am::GetBookCount(); i++ )
	{
		am::Book* pBook;

		if ( i == am::GetBookCount() )
			pBook = m_pBookToInsert;
		else
			pBook = am::GetBooks()[i];

		StoryTreeModelNode* pBookNode = new StoryTreeModelNode(pBook);
		pBookNode->SetContainer(false);

		m_vBooks.push_back(pBookNode);
		wxDataViewItem bookItem(pBookNode);

		ItemAdded(wxDataViewItem(nullptr), bookItem);
	}
}

void BookWizardTreeModel::OnBeginDrag(wxDataViewEvent& event, wxDataViewCtrl* dvc)
{
	if ( ((StoryTreeModelNode*)event.GetItem().GetID())->GetBook() != m_pBookToInsert )
	{
		event.Veto();
		return;
	}

	event.SetDataObject(new wxTextDataObject(m_pBookToInsert->title));
	event.SetDataFormat(wxDF_UNICODETEXT);
	event.SetDragFlags(wxDrag_AllowMove);
}

void BookWizardTreeModel::OnDropPossible(wxDataViewEvent& event, wxDataViewCtrl* dvc)
{
	event.SetDropEffect(wxDragMove);
}

void BookWizardTreeModel::OnDrop(wxDataViewEvent& event, wxDataViewCtrl* dvc)
{
	int nProposedIndex = event.GetProposedDropIndex();
	if ( nProposedIndex == -1 )
	{
		if ( event.GetItem().IsOk() )
		{
			nProposedIndex = 0;
			wxDataViewItemArray arrSiblings;
			GetChildren(GetParent(event.GetItem()), arrSiblings);

			for ( wxDataViewItem& item : arrSiblings )
			{
				nProposedIndex++;

				if ( item == event.GetItem() )
					break;
			}
		}
		else
			return;
	}

	StoryTreeModelNode* pBookToInsertNode = nullptr;
	for ( StoryTreeModelNode*& pBookNode : m_vBooks )
	{
		if ( pBookNode->GetBook() == m_pBookToInsert )
		{
			pBookToInsertNode = pBookNode;
			break;
		}
	}

	if ( Reposition(wxDataViewItem(pBookToInsertNode), nProposedIndex) )
	{
		wxDataViewItemArray arrSiblings;
		GetChildren(wxDataViewItem(nullptr), arrSiblings);

		int nPos = 1;
		for ( wxDataViewItem& item : arrSiblings )
		{
			if ( item == wxDataViewItem(pBookToInsertNode) )
			{
				m_pBookToInsert->pos = nPos;
				break;
			}

			nPos++;
		}
	}
}


amBookWizard::amBookWizard(wxWindow* parent)
{
	Create(parent, -1, "Create new Book", wxNullBitmap, wxDefaultPosition,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	m_btnNext->Bind(wxEVT_BUTTON, &amBookWizard::OnNextButton, this);

	SetPageSize(FromDIP(wxSize(400, 350)));
	SetIcon(wxICON(amadeus));
	SetBackgroundColour(wxColour(40, 40, 40));

	m_pBookToInsert = new am::Book;
	m_pBookToInsert->pos = am::GetBookCount() + 1;

	m_pPosPage = new wxWizardPageSimple(this);

	wxStaticText* pPosLabel = new wxStaticText(m_pPosPage, -1, _("Drag the new Book to the desired position:"));
	pPosLabel->SetForegroundColour(wxColour(255, 255, 255));
	pPosLabel->SetFont(wxFontInfo(10));

	m_posDVCModel = new BookWizardTreeModel(m_pBookToInsert);

#ifdef __WXMSW__
	m_pPosDVC = m_posDVCHandler.Create(m_pPosPage, -1, m_posDVCModel.get(),
		wxBORDER_NONE | wxDV_MULTIPLE);
#else
	m_pPosDVC = new wxDataViewCtrl(m_pPosPage, -1, wxDefaultPosition, wxDefaultSize,
		wxDV_SINGLE | wxBORDER_NONE);
	m_pPosDVC->AssociateModel(&m_posDVCModel);
#endif

	m_pPosDVC->SetBackgroundColour(wxColour(90, 90, 90));
	m_pPosDVC->SetMinSize(wxSize(FromDIP(250), -1));
	m_pPosDVC->EnableDragSource(wxDF_UNICODETEXT);
	m_pPosDVC->EnableDropTarget(wxDF_UNICODETEXT);
	//m_pPosDVC->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &amOverview::OnRecentDocument, this);

	wxDataViewColumn* pColumn = new wxDataViewColumn(_("Books"), new wxDataViewIconTextRenderer(wxDataViewIconTextRenderer::GetDefaultType()), 0, FromDIP(170), wxALIGN_LEFT);
	m_pPosDVC->AppendColumn(pColumn);

	wxBoxSizer* pPosSizer = new wxBoxSizer(wxVERTICAL);
	pPosSizer->Add(pPosLabel, wxSizerFlags(0).Expand());
	pPosSizer->AddSpacer(10);
	pPosSizer->Add(m_pPosDVC, wxSizerFlags(1).Expand());
	
	m_pPosPage->SetSizer(pPosSizer);

	m_pMainPage = new amFirstBookWizardPage(this, m_pPosPage);

	m_pPosPage->SetNext(nullptr);
	m_pPosPage->SetPrev(m_pMainPage);

	GetPageAreaSizer()->Add(m_pMainPage);
}

amBookWizard::~amBookWizard()
{
	if ( !m_bSucceeded )
		delete m_pBookToInsert;
}

bool amBookWizard::CanCreateBook()
{
	return m_pMainPage->CanCreateBook();
}

void amBookWizard::OnNextButton(wxCommandEvent& event)
{
	if ( !GetCurrentPage()->GetNext() )
	{
		if ( !CanCreateBook() )
		{
			wxMessageBox(_("You cannot create a Book without filling out all of the necessary fields (the ones that start with a *)"),
				_("Error when creating Book"));
			
			return;
		}
		else
		{
			am::AddBook(m_pBookToInsert);
			m_bSucceeded = true;
		}
	}
	else
	{
		if ( m_pBookToInsert->title != m_pMainPage->m_tcTitle->GetValue() )
			m_pBookToInsert->title = m_pMainPage->m_tcTitle->GetValue();

		if ( m_pBookToInsert->title != m_pMainPage->m_tcTitle->GetValue() )
			m_pBookToInsert->title = m_pMainPage->m_tcTitle->GetValue();

		if ( m_pBookToInsert->title != m_pMainPage->m_tcTitle->GetValue() )
			m_pBookToInsert->title = m_pMainPage->m_tcTitle->GetValue();

		if ( m_pBookToInsert->title != m_pMainPage->m_tcTitle->GetValue() )
			m_pBookToInsert->title = m_pMainPage->m_tcTitle->GetValue();

		if ( m_pBookToInsert->title != m_pMainPage->m_tcTitle->GetValue() )
			m_pBookToInsert->title = m_pMainPage->m_tcTitle->GetValue();
	}

	event.Skip();
}


amFirstBookWizardPage::amFirstBookWizardPage(wxWizard* parent, wxWizardPage* next) : wxWizardPage(parent), m_pNextPage(next)
{
	wxStaticText* pMainLabel = new wxStaticText(this, -1, _("Book"));
	pMainLabel->SetForegroundColour(wxColour(255, 255, 255));
	pMainLabel->SetFont(wxFontInfo(18).Bold().FaceName("Times New Roman"));

	wxSize szMinLabelSize(FromDIP(wxSize(80, -1)));
	wxColour colTextCtrlBg(20, 20, 20);
	wxColour colTextCtrlFg(200, 200, 200);

	wxStaticText* pTitleLabel = new wxStaticText(this, -1, _("*Title:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	pTitleLabel->SetForegroundColour(wxColour(255, 255, 255));
	pTitleLabel->SetMinSize(szMinLabelSize);
	m_tcTitle = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_tcTitle->SetBackgroundColour(colTextCtrlBg);
	m_tcTitle->SetForegroundColour(colTextCtrlFg);

	wxBoxSizer* pTitleSizer = new wxBoxSizer(wxHORIZONTAL);
	pTitleSizer->Add(pTitleLabel, wxSizerFlags(0).CenterVertical());
	pTitleSizer->AddSpacer(5);
	pTitleSizer->Add(m_tcTitle, wxSizerFlags(2).CenterVertical());

	wxStaticText* pAuthorLabel = new wxStaticText(this, -1, _("*Author:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	pAuthorLabel->SetForegroundColour(wxColour(255, 255, 255));
	pAuthorLabel->SetMinSize(szMinLabelSize);
	m_tcAuthor = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_tcAuthor->SetBackgroundColour(colTextCtrlBg);
	m_tcAuthor->SetForegroundColour(colTextCtrlFg);

	wxBoxSizer* pAuthorSizer = new wxBoxSizer(wxHORIZONTAL);
	pAuthorSizer->Add(pAuthorLabel, wxSizerFlags(0).CenterVertical());
	pAuthorSizer->AddSpacer(5);
	pAuthorSizer->Add(m_tcAuthor, wxSizerFlags(2).CenterVertical());

	wxStaticText* pGenreLabel = new wxStaticText(this, -1, _("Genre:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	pGenreLabel->SetForegroundColour(wxColour(255, 255, 255));
	pGenreLabel->SetMinSize(szMinLabelSize);
	m_tcGenre = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_tcGenre->SetBackgroundColour(colTextCtrlBg);
	m_tcGenre->SetForegroundColour(colTextCtrlFg);

	wxBoxSizer* pGenreSizer = new wxBoxSizer(wxHORIZONTAL);
	pGenreSizer->Add(pGenreLabel, wxSizerFlags(0).CenterVertical());
	pGenreSizer->AddSpacer(5);
	pGenreSizer->Add(m_tcGenre, wxSizerFlags(2).CenterVertical());

	wxStaticText* pPublisherLabel = new wxStaticText(this, -1, _("Publisher:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	pPublisherLabel->SetForegroundColour(wxColour(255, 255, 255));
	pPublisherLabel->SetMinSize(szMinLabelSize);
	m_tcPublisher = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	m_tcPublisher->SetBackgroundColour(colTextCtrlBg);
	m_tcPublisher->SetForegroundColour(colTextCtrlFg);

	wxBoxSizer* pPubSizer = new wxBoxSizer(wxHORIZONTAL);
	pPubSizer->Add(pPublisherLabel, wxSizerFlags(0).CenterVertical());
	pPubSizer->AddSpacer(5);
	pPubSizer->Add(m_tcPublisher, wxSizerFlags(2).CenterVertical());

	wxStaticText* pDescriptionLabel = new wxStaticText(this, -1, _("Description:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	pDescriptionLabel->SetForegroundColour(wxColour(255, 255, 255));
	pDescriptionLabel->SetMinSize(szMinLabelSize);
	m_tcDescription = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxBORDER_SIMPLE);
	m_tcDescription->SetBackgroundColour(colTextCtrlBg);
	m_tcDescription->SetForegroundColour(colTextCtrlFg);

	wxBoxSizer* pDescSizer = new wxBoxSizer(wxHORIZONTAL);
	pDescSizer->Add(pDescriptionLabel, wxSizerFlags(0));
	pDescSizer->AddSpacer(5);
	pDescSizer->Add(m_tcDescription, wxSizerFlags(2).Expand());

	wxBoxSizer* pVerticalSizer = new wxBoxSizer(wxVERTICAL);
	pVerticalSizer->Add(pMainLabel, wxSizerFlags(0).Left());
	pVerticalSizer->AddSpacer(10);
	pVerticalSizer->Add(pTitleSizer, wxSizerFlags(0).Expand());
	pVerticalSizer->AddSpacer(10);
	pVerticalSizer->Add(pAuthorSizer, wxSizerFlags(0).Expand());
	pVerticalSizer->AddSpacer(10);
	pVerticalSizer->Add(pGenreSizer, wxSizerFlags(0).Expand());
	pVerticalSizer->AddSpacer(10);
	pVerticalSizer->Add(pPubSizer, wxSizerFlags(0).Expand());
	pVerticalSizer->AddSpacer(10);
	pVerticalSizer->Add(pDescSizer, wxSizerFlags(1).Expand());

	SetSizer(pVerticalSizer);
}

bool amFirstBookWizardPage::CanCreateBook()
{
	if ( m_tcTitle->IsEmpty() || m_tcAuthor->IsEmpty() )
		return false;

	return true;
}