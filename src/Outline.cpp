#include "Outline.h"

#include <boost/filesystem.hpp>

#include "MainFrame.h"

namespace fs = boost::filesystem;

Outline::Outline(wxWindow* parent) : wxNotebook(parent, -1) {
    corkboard = new Corkboard(this);
    timeline = new wxPanel(this);

    AddPage(corkboard, "Corkboard");
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
