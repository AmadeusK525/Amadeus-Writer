#include "AutoWrapTextShape.h"

XS_IMPLEMENT_CLONABLE_CLASS(AutoWrapTextShape,
	wxSFEditTextShape);

bool AutoWrapTextShape::countLines = true;
//bool AutoWrapTextShape::clipRegion = false;

AutoWrapTextShape::AutoWrapTextShape() : wxSFEditTextShape() {
	SetFill(bgColour);
}

AutoWrapTextShape::AutoWrapTextShape(const AutoWrapTextShape& other): wxSFEditTextShape(other) {
	SetFill(bgColour);
	m_sText = other.m_sText;
	bgColour = other.bgColour;
}

void AutoWrapTextShape::UpdateRectSize() {
	wxSize tsize = ((wxSFShapeBase*)(m_pParentItem))->GetBoundingBox().GetSize();
	tsize.y -= 30;

	if (tsize.IsFullySpecified()) {
		if (tsize.x <= 0) tsize.x = 1;
		if (tsize.y <= 0) tsize.y = 1;

		m_nRectSize.x = (double)tsize.x;
		m_nRectSize.y = (double)tsize.y;
	}
}

void AutoWrapTextShape::DrawTextContent(wxDC& dc) {
	dc.SetBrush(m_Fill);
	//dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

	dc.SetTextForeground(m_TextColor);
	dc.SetFont(m_Font);

	wxRect rect(GetBoundingBox());

	if (clipRegion)
		m_lines.Clear();

	// Get wrapped text and draw all text lines
	if (countLines && rect.width > 30) {
		int numberOfLines = 1;
		
		if (m_nLineHeight > 0)
			numberOfLines = rect.height / m_nLineHeight;

		calcWrappedText(rect.width, numberOfLines);
	}

	for (int i = 0; i < m_lines.GetCount(); i++) {
		dc.DrawText(m_lines[i], rect.x, rect.y + i * m_nLineHeight);
	}

	//if (clipRegion)
		//dc.DestroyClippingRegion();
}

void AutoWrapTextShape::OnLeftDoubleClick(wxPoint& pos) {
	if (GetParentCanvas()) {
		int dx, dy;
		wxRealPoint shpPos = GetAbsolutePosition();
		double scale = GetParentCanvas()->GetScale();
		GetParentCanvas()->CalcUnscrolledPosition(0, 0, &dx, &dy);

		wxRect shpBB = ((wxSFShapeBase*)(m_pParentItem))->GetBoundingBox();
		shpBB.x -= 30;
		shpBB.y -= 30;

		m_nCurrentState = GetStyle();
		RemoveStyle(sfsSIZE_CHANGE);

		m_pTextCtrl = new wxSFContentCtrl(GetParentCanvas(), wxID_ANY,
			this, m_sText, wxPoint(int((shpPos.x * scale) - dx),
			int((shpPos.y * scale) - dy)),
			wxSize(int(shpBB.GetWidth() * scale), int(shpBB.GetHeight() * scale)), wxTE_MULTILINE);

		m_pTextCtrl->SetBackgroundColour(wxColour(255, 255, 255));
	}
}

void AutoWrapTextShape::calcWrappedText(int& lenght, int& numberOfLines) {
	m_lines.Clear();

	wxSize textSize;
	wxString fullString = m_sText;
	wxString intactString = fullString;

	int index = 0;
	bool hasSpace;
	bool hasNewLine;
	bool isEqual;

	while (numberOfLines > 0) {
		textSize = GetTextExtent();

		if (textSize.x >= lenght - 2) {
			index = 0;
			do {
				m_sText = fullString.Left(++index);
				textSize = GetTextExtent();

				isEqual = fullString.Length() == m_sText.Length();
				if (textSize.x >= lenght - 2 || isEqual) {
					hasSpace = m_sText.Contains(" ");
					/*hasNewLine = m_sText.Contains("\n");

					if (hasNewLine) {
						while (true) {
							int where = m_sText.find_first_of("\n", 0);
							m_sText.Remove(where, m_sText.Length() - where);
							fullString.Remove(where);

							if (m_sText.IsEmpty())
								m_sText = " ";
						}

						hasSpace = false;
					}*/

					if (hasSpace) {
						while (true) {
							char s = m_sText.Last();
							if (s == ' ')
								break;

							m_sText.Remove(m_sText.Length() - 1, 1);
							index--;
						}
					} else if (!isEqual) {
						m_sText << "-";
					}

					break;
				}

			} while (true);
			m_lines.Add(m_sText);
			fullString.Remove(0, m_sText.Length());
			m_sText = fullString;
		} else {
			m_lines.Add(m_sText);
			break;
		}
		numberOfLines--;
	}

	m_sText = intactString;
}