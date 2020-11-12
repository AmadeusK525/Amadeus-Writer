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

void Outline::saveQuantities(std::ofstream& out) {
    if (out.is_open()) {
        out.write((char*)&non, sizeof(int));

        out.write((char*)&noi, sizeof(int));

        out.write((char*)&nob, sizeof(int));
    }
}

void Outline::loadQuantities(std::ifstream& in) {
    if (in.is_open()) {
        in.read((char*)&non, sizeof(int));

        in.read((char*)&noi, sizeof(int));

        in.read((char*)&nob, sizeof(int));
    }
}
