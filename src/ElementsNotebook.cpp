#include "ElementsNotebook.h"

#include "CharacterCreator.h"
#include "LocationCreator.h"
#include "CharacterShowcase.h"
#include "LocationShowcase.h"

#include "Outline.h"
#include "OutlineFiles.h"
#include "MyApp.h"

#include "wxmemdbg.h"

BEGIN_EVENT_TABLE(ElementsNotebook, wxNotebook)

EVT_LIST_ITEM_FOCUSED(LIST_CharList, ElementsNotebook::charSelected)
EVT_LIST_ITEM_SELECTED(LIST_LocList, ElementsNotebook::locSelected)

EVT_LIST_ITEM_RIGHT_CLICK(LIST_CharList, ElementsNotebook::onCharRightClick)
EVT_LIST_ITEM_ACTIVATED(LIST_CharList, ElementsNotebook::openChar)
EVT_MENU(LISTMENU_EditChar, ElementsNotebook::editChar)
EVT_LIST_END_LABEL_EDIT(LIST_CharList, ElementsNotebook::editCharName)
EVT_MENU(LISTMENU_DeleteChar, ElementsNotebook::deleteChar)

EVT_LIST_ITEM_RIGHT_CLICK(LIST_LocList, ElementsNotebook::onLocRightClick)
EVT_LIST_ITEM_ACTIVATED(LIST_LocList, ElementsNotebook::openLoc)
EVT_MENU(LISTMENU_EditLoc, ElementsNotebook::editLoc)
EVT_MENU(LISTMENU_DeleteLoc, ElementsNotebook::deleteLoc)

EVT_NOTEBOOK_PAGE_CHANGED(NOTEBOOK_THIS, ElementsNotebook::setSearchAC)

END_EVENT_TABLE()

wxListView* ElementsNotebook::charList;
wxListView* ElementsNotebook::locList;
wxListView* ElementsNotebook::itemsList;

ElementsNotebook::ElementsNotebook(wxWindow* parent) : wxNotebook(parent, NOTEBOOK_THIS) {
    this->mainFrame = (MainFrame*)wxGetApp().GetTopWindow();

    //Setting up first notebook tab with a characters list
    wxPanel* charFrame = new wxPanel(this, PANEL_Char);
    charFrame->SetBackgroundColour(wxColour(20, 20, 20));

    charList = new wxListView(charFrame, LIST_CharList, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE);
    charList->InsertColumn(0, "Name", wxLIST_FORMAT_CENTER, 190);
    charList->InsertColumn(1, "Age", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
    charList->InsertColumn(2, "Sex", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
    charList->InsertColumn(3, "Role", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
    charList->InsertColumn(4, "First Appearance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    charList->InsertColumn(5, "Chapters", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);

    charList->SetBackgroundColour(wxColour(45, 45, 45));
    charList->SetForegroundColour(wxColour(245, 245, 245));

    charShow = new CharacterShowcase(charFrame);

    wxBoxSizer* charSizer = new wxBoxSizer(wxHORIZONTAL);
    charSizer->Add(charList, wxSizerFlags(4).Expand().Border(wxLEFT | wxTOP | wxBOTTOM, 10));
    //charSizer->SetItemMinSize(size_t(0), wxSize());
    charSizer->Add(charShow, 3, wxGROW | wxBOTTOM |wxTOP, 10);
    charFrame->SetSizer(charSizer);

    this->AddPage(charFrame, "Characters");

    //Setting up second notebook tab with a locations list
    wxPanel* locFrame = new wxPanel(this, wxID_ANY);
    locFrame->SetBackgroundColour(wxColour(20, 20, 20));
    locList = new wxListView(locFrame, LIST_LocList, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxLC_HRULES | wxBORDER_NONE);
    locList->InsertColumn(0, "Name of location", wxLIST_FORMAT_CENTER, 150);
    locList->InsertColumn(1, "Space type", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
    locList->InsertColumn(2, "Importance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);
    locList->InsertColumn(3, "First Appearance", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE_USEHEADER);
    locList->InsertColumn(4, "Chapters", wxLIST_FORMAT_CENTER, wxLIST_AUTOSIZE);

    locList->SetBackgroundColour(wxColour(45, 45, 45));
    locList->SetForegroundColour(wxColour(245, 245, 245));
    locList->SetMinSize(wxSize(300, 400));

    locShow = new LocationShowcase(locFrame);

    wxBoxSizer* locSizer = new wxBoxSizer(wxHORIZONTAL);
    locSizer->Add(locList, 1, wxGROW | wxLEFT | wxTOP | wxBOTTOM, 10);
    locSizer->Add(locShow, 1, wxGROW | wxTOP | wxBOTTOM, 10);
    locFrame->SetSizer(locSizer);
    this->AddPage(locFrame, "Locations");

    //Setting up third notebook tab
    wxPanel* itemsFrame = new wxPanel(this, wxID_ANY);
    itemsFrame->SetBackgroundColour(wxColour(20, 20, 20));
    itemsList = new wxListView(itemsFrame, LIST_ItemsList, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxBORDER_NONE);
    itemsList->SetBackgroundColour(wxColour(45, 45, 45));
    itemsList->SetForegroundColour(wxColour(245, 245, 245));
    itemsList->InsertColumn(0, "Name of item", wxLIST_FORMAT_CENTER, 120);

    itemsList->SetMinSize(wxSize(300, 400));

    wxBoxSizer* itemSizer = new wxBoxSizer(wxHORIZONTAL);
    itemSizer->Add(itemsList, 1, wxGROW | wxALL, 10);
    itemsFrame->SetSizer(itemSizer);
    this->AddPage(itemsFrame, "Items");
}

void ElementsNotebook::onCharRightClick(wxListEvent& WXUNUSED(event)) {
    wxMenu menu;
    menu.Append(LISTMENU_EditChar, "&Edit");
    menu.Append(LISTMENU_DeleteChar, "&Delete");
    PopupMenu(&menu, wxDefaultPosition);
}

void ElementsNotebook::editChar(wxCommandEvent& WXUNUSED(event)) {
    auto it = MainFrame::characters.begin();
    for (int i = 0; i < charList->GetFirstSelected(); i++) {
        it++;
    }

    CharacterCreator* edit = new CharacterCreator(mainFrame, this);
    edit->SetTitle("Edit character - " + it->second.name);
    edit->CenterOnParent();

    edit->setEdit(&it->second);

    mainFrame->Enable(false);
}

void ElementsNotebook::editCharName(wxListEvent& event) {
    auto it = MainFrame::characters.begin();
    for (int i = 0; i < charList->GetFirstSelected(); i++) {
        it++;
    }

    it->second.name = event.GetLabel();
}

void ElementsNotebook::deleteChar(wxCommandEvent& WXUNUSED(event)) {
    long sel = charList->GetFirstSelected();

    wxMessageDialog deleteCheck(mainFrame, "Are you sure you want to delete '" + charList->GetItemText(sel) + "'?",
        "Delete character", wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
 
    if (deleteCheck.ShowModal() == wxID_YES) {
        charList->DeleteItem(sel);

        auto it = MainFrame::characters.begin();
        for (int i = 0; i < sel; i++) {
            it++;
        }

        charNames.Remove(it->second.name);

        mainFrame->getOutline()->getOutlineFiles()->deleteCharacter(it->second);
        MainFrame::characters.erase(it);
        MainFrame::saved[0] = MainFrame::characters.size();

        MainFrame::isSaved = false;
    }
}

void ElementsNotebook::openChar(wxListEvent& WXUNUSED(event)) {
    editChar(wxCommandEvent());
}

void ElementsNotebook::onLocRightClick(wxListEvent& WXUNUSED(event)) {
    wxMenu menu;
    menu.Append(LISTMENU_EditLoc, "&Edit");
    menu.Append(LISTMENU_DeleteLoc, "Delete");
    PopupMenu(&menu, wxDefaultPosition);
}

void ElementsNotebook::editLoc(wxCommandEvent& WXUNUSED(event)) {

    auto it = MainFrame::locations.begin();
    for (int i = 0; i < locList->GetFirstSelected(); i++) {
        it++;
    }

    LocationCreator* edit = new LocationCreator(mainFrame, this);
    edit->SetTitle("Edit location - " + it->second.name);
    edit->CenterOnParent();

    edit->setEdit(&it->second);

    mainFrame->Enable(false);
}

void ElementsNotebook::deleteLoc(wxCommandEvent& WXUNUSED(event)) {
    long sel = locList->GetFirstSelected();

    wxMessageDialog* deleteCheck = new wxMessageDialog(mainFrame, "Are you sure you want to delete '" + locList->GetItemText(sel) + "'?",
        "Delete character", wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);

    if (deleteCheck->ShowModal() == wxID_YES) {
        locList->DeleteItem(sel);

        auto it = MainFrame::locations.begin();
        for (int i = 0; i < sel; i++) {
            it++;
        }

        locNames.Remove(it->second.name);

        mainFrame->getOutline()->getOutlineFiles()->deleteLocation(it->second);
        MainFrame::locations.erase(it);
        MainFrame::saved[1] = MainFrame::locations.size();

        MainFrame::isSaved = false;
    }
}

void ElementsNotebook::openLoc(wxListEvent& event) {
    editLoc(wxCommandEvent());
}

void ElementsNotebook::charSelected(wxListEvent& event) {
    long sel = charList->GetFirstSelected();

    if (sel == -1) {
        charShow->setData(wxImage(), vector<string>(10, ""));
        return;
    }

    auto it = MainFrame::characters.begin();
    for (int i = 0; i < sel; i++) {
        it++;
    }
    vector<string> charData;
    charData.push_back(it->second.name);
    charData.push_back(it->second.sex);
    charData.push_back(it->second.age);
    charData.push_back(it->second.nat);
    charData.push_back(it->second.height);
    charData.push_back(it->second.nick);
    charData.push_back(it->second.role);
    charData.push_back(it->second.appearance);
    charData.push_back(it->second.personality);
    charData.push_back(it->second.backstory);

    charShow->setData(it->second.image, charData);
}

void ElementsNotebook::charDeselected(wxListEvent& WXUNUSED(event)) {
    //if (charList->GetSelectedItemCount() == 0)
        charShow->setData(wxImage(), vector<string>(10, ""));
}

void ElementsNotebook::locSelected(wxListEvent& WXUNUSED(event)) {
    long sel = locList->GetFirstSelected();

    auto it = MainFrame::locations.begin();
    for (int i = 0; i < sel; i++) {
        it++;
    }

    vector<string> locData;
    locData.push_back(it->second.name);
    locData.push_back(it->second.background);
    locData.push_back(it->second.natural);
    locData.push_back(it->second.architecture);
    locData.push_back(it->second.type);
    locData.push_back(it->second.economy);
    locData.push_back(it->second.culture);
    locData.push_back(it->second.importance);

    locShow->setData(it->second.image, locData);
}

void ElementsNotebook::locDeselected(wxListEvent& WXUNUSED(event)) {
    locShow->setData(wxImage(), vector<string>(5, ""));
}

void ElementsNotebook::updateLB() {
    charList->DeleteAllItems();

    int i = 0;

    for (auto it = MainFrame::characters.begin(); it != MainFrame::characters.end(); it++) {
        charList->InsertItem(i, it->second.name);
        charList->SetItem(i, 1, it->second.age);
        charList->SetItem(i, 2, it->second.sex);
        charList->SetItem(i, 3, it->second.role);

        if (it->second.hasAppeared) {
            charList->SetItem(i, 4, "Chapter " + std::to_string(it->second.firstChap));
        } else {
            charList->SetItem(i, 4, "-");
        }

        charList->SetItem(i, 5, std::to_string(it->second.chapters));

        i++;
    }

    i = 0;

    locList->DeleteAllItems();

    for (auto it = MainFrame::locations.begin(); it != MainFrame::locations.end(); it++) {
        locList->InsertItem(i, it->second.name);
        locList->SetItem(i, 1, it->second.type);
        locList->SetItem(i, 2, it->second.importance);
        locList->SetItem(i, 4, std::to_string(it->second.chapters));

        if (it->second.hasAppeared) {
            locList->SetItem(i, 3, "Chapter " + std::to_string(it->second.firstChap));
        } else {
            locList->SetItem(i, 3, "-");
        }

        i++;
    }
    
    i = 0;
}

void ElementsNotebook::setSearchAC(wxBookCtrlEvent& WXUNUSED(event)) {
    int sel = this->GetSelection();

    switch (sel) {
    case 0:
        searchBar->AutoComplete(charNames);
        searchBar->SetDescriptiveText("Search character");
        break;

    case 1:
        searchBar->AutoComplete(locNames);
        searchBar->SetDescriptiveText("Search location");
        break;

    case 2:
        searchBar->AutoComplete(wxArrayString());
        searchBar->SetDescriptiveText("Search item");
        break;

    case 3:
        searchBar->AutoComplete(wxArrayString());
        searchBar->SetDescriptiveText("Search scene");
        break;
    }

    searchBar->Clear();
}

void ElementsNotebook::addCharName(string& name) {
    charNames.Add(name);
}

void ElementsNotebook::addLocName(string& name) {
    locNames.Add(name);
}

void ElementsNotebook::removeCharName(string& name) {
    charNames.Remove(name);
}

void ElementsNotebook::removeLocName(string& name) {
    locNames.Remove(name);
}
