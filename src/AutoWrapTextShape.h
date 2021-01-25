#ifndef AUTOWRAPTEXTSHAPE_H_
#define AUTOWRAPTEXTSHAPE_H_
#pragma once

#include <wx\wxsf\wxShapeFramework.h>

class AutoWrapTextShape : public wxSFEditTextShape {
private:
	wxColour m_bgColour{ 255, 255, 255 };
	wxArrayString m_lines{};

	static bool m_countLines;
	bool m_clipRegion = false;

public:
	XS_DECLARE_CLONABLE_CLASS(AutoWrapTextShape);

	AutoWrapTextShape();
	AutoWrapTextShape(const AutoWrapTextShape& other);

	virtual ~AutoWrapTextShape() {}

	virtual void UpdateRectSize();
	virtual void DrawTextContent(wxDC& dc);

	virtual void OnLeftDoubleClick(wxPoint& pos);

	static void ShouldCountLines(bool will) { m_countLines = will; }
	void ShouldClip(bool will) { AutoWrapTextShape::m_clipRegion = will; }
	void CalcWrappedText(int& length, int& numberOfLines);
};

#endif