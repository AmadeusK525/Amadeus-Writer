#include "Outline.h"

#include <boost/filesystem.hpp>

#include "MainFrame.h"

namespace fs = boost::filesystem;

Outline::Outline(wxWindow* parent) : wxNotebook(parent, -1) {
    corkHolder = new wxPanel(this);
    corkboard = new Corkboard(corkHolder);
    timeline = new wxPanel(this);

    // Hack for doing corkboard fullscreen
    corkHolderSizer = new wxBoxSizer(wxVERTICAL);
    corkHolderSizer->Add(corkboard, wxSizerFlags(1).Expand());
    corkHolder->SetSizer(corkHolderSizer);

    AddPage(corkHolder, "Corkboard");
    AddPage(timeline, "Timeline");
}

void Outline::saveOutline(std::ofstream& out, int& progress, wxProgressDialog* dialog) {
    corkboard->save(out);
    dialog->Update(progress++);
}

void Outline::loadOutline(std::ifstream& in, int& progress, wxProgressDialog* dialog) {
    corkboard->load(in);
    dialog->Update(progress++);
}
