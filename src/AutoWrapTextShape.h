#ifndef AUTOWRAPTEXTSHAPE_H_
#define AUTOWRAPTEXTSHAPE_H_
#pragma once

#include <wx\wxsf\wxShapeFramework.h>

class AutoWrapTextShape : public wxSFEditTextShape {
private:
	wxColour bgColour{ 255, 255, 255 };
	wxArrayString m_lines{};

	static bool countLines;
	bool clipRegion = false;

public:
	XS_DECLARE_CLONABLE_CLASS(AutoWrapTextShape);

	AutoWrapTextShape();
	AutoWrapTextShape(const AutoWrapTextShape& other);

	virtual ~AutoWrapTextShape() {}

	virtual void UpdateRectSize();
	virtual void DrawTextContent(wxDC& dc);

	virtual void OnLeftDoubleClick(wxPoint& pos);

	static void willCountLines(bool will) { AutoWrapTextShape::countLines = will; }
	void willClip(bool will) { AutoWrapTextShape::clipRegion = will; }
	void calcWrappedText(int& length, int& numberOfLines);
};

#endif