#ifndef AUTOWRAPTEXTSHAPE_H_
#define AUTOWRAPTEXTSHAPE_H_
#pragma once

#include "wx\wxsf\wxShapeFramework.h"

class AutoWrapTextShape : public wxSFEditTextShape {
private:
	wxColour bgColour{ 255, 255, 255 };
	wxArrayString m_lines{};

	static bool countLines;
	static bool clipRegion;

public:
	XS_DECLARE_CLONABLE_CLASS(AutoWrapTextShape);

	AutoWrapTextShape();
	AutoWrapTextShape(static AutoWrapTextShape& other);

	virtual ~AutoWrapTextShape() {}

	virtual void UpdateRectSize();
	virtual void DrawTextContent(wxDC& dc);

	virtual void OnLeftClick(wxPoint& pos);
	virtual void OnLeftDoubleClick(wxPoint& pos);

	static void willCountLines(bool will) { AutoWrapTextShape::countLines = will; }
	void willClip(bool will) { AutoWrapTextShape::clipRegion = will; }
	void calcWrappedText(int& length, int& numberOfLines);
};

#endif