#include "WordProcessor.h"

#include <wx\dc.h>
#include <wx\wx.h>

#include "wxmemdbg.h"

IMPLEMENT_DYNAMIC_CLASS(amWPCommentTag, wxRichTextFieldType)

amWPCommentTag::amWPCommentTag(const wxString& name) : wxRichTextFieldType(name) {}

bool amWPCommentTag::GetRangeSize(wxRichTextField* obj, const wxRichTextRange& range, wxSize& size, int& descent, wxDC& dc,
	wxRichTextDrawingContext& context, int flags, const wxPoint& position, const wxSize& parentSize, wxArrayInt* partialExtents) const
{

	if ( !range.IsWithin(obj->GetRange()) )
		return false;

	int width = 50;

	size = wxSize(width, width);

	if ( partialExtents )
		partialExtents->Add(width);

	return true;
}

bool amWPCommentTag::Layout(wxRichTextField* obj, wxDC& dc, wxRichTextDrawingContext& context, const wxRect& rect,
	const wxRect& parentRect, int style)
{

	wxSize size(50, 50);

	obj->SetCachedSize(size);
	obj->SetMinSize(size);
	obj->SetMaxSize(size);
	return true;
}

bool amWPCommentTag::Draw(wxRichTextField* obj, wxDC& dc, wxRichTextDrawingContext& context, const wxRichTextRange& range,
	const wxRichTextSelection& selection, const wxRect& rect, int descent, int style)
{
	wxColour red(200, 0, 0);

	dc.SetPen(wxPen(red, 1));
	dc.SetBrush(wxBrush(red));
	dc.DrawCircle(rect.x + 25, rect.y + 25, 25);

	return true;
}


/////////////////////////////////////////////////////////////////////
/////////////////////////// WordProcessor ///////////////////////////
/////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(amWordProcessor, wxRichTextCtrl)
EVT_CHAR(amWordProcessor::OnChar)
END_EVENT_TABLE()


amWordProcessor::amWordProcessor(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos,
	const wxSize& size, long style, const wxValidator& validator, const wxString& name) :
	wxRichTextCtrl(parent, id, value, pos, size, style, validator, name)
{

}

void amWordProcessor::OnChar(wxKeyEvent& event)
{
	if ( event.CmdDown() || event.ControlDown() )
	{

		switch ( event.GetKeyCode() )
		{
		case 110:
		case 78:
		case 98:
		case 66:
		case 2:
		case 14:
			ApplyBoldToSelection();
			return;

		case 105:
		case 73:
		case 9:
			ApplyItalicToSelection();
			return;

		case 115:
		case 83:
		case 117:
		case 85:
		case 21:
		case 19:
			ApplyUnderlineToSelection();
			return;
		}
	}

	event.Skip();
}

void amWordProcessor::ApplyCommentToSelection() {}

void amWordProcessor::SetupScrollbars(bool atTop, bool fromOnPaint)
{

}
