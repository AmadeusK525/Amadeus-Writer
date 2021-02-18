#ifndef CORKBOARDSHAPES_H_
#define CORKBOARDSHAPES_H_
#pragma once

#include <wx\wxsf\wxShapeFramework.h>

class AutoWrapTextShape : public wxSFEditTextShape {
private:
	wxColour m_bgColour{ 255, 255, 255 };
	wxArrayString m_lines{};
	wxString m_textToDraw{""};

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


////////////////////////////////////////////////////////////////////
//////////////////////////// NoteShape /////////////////////////////
////////////////////////////////////////////////////////////////////


class NoteShape : public wxSFRoundRectShape {
protected:
	AutoWrapTextShape* m_content = nullptr;

public:
	wxWindowID m_currentColour;

public:
	// Enable RTTI and cloneability
	XS_DECLARE_CLONABLE_CLASS(NoteShape);

	NoteShape();
	NoteShape(const NoteShape& other);

	virtual ~NoteShape() {}

	void ChangeColour(wxWindowID id);

	void ShouldCountLines(bool count) { m_content->ShouldCountLines(count); }
	void ShouldClip(bool will) { m_content->ShouldClip(will); }

	virtual void OnBeginHandle(wxSFShapeHandle& handle);
	virtual void OnHandle(wxSFShapeHandle& handle);
	virtual void OnEndHandle(wxSFShapeHandle& handle);

	virtual void OnBeginDrag(const wxPoint& pos);
	virtual void OnEndDrag(const wxPoint& pos);
};


////////////////////////////////////////////////////////////////////
//////////////////////////// ImageShape ////////////////////////////
////////////////////////////////////////////////////////////////////


class ImageShape : public wxSFBitmapShape {
private:
	double ratio = 99999.9;

public:
	ImageShape();
	ImageShape(const ImageShape& other);
	virtual ~ImageShape();

	void Create(const wxString& path, wxBitmapType type = wxBITMAP_TYPE_ANY);

	virtual void OnBeginHandle(wxSFShapeHandle& handle);
	virtual void OnHandle(wxSFShapeHandle& handle);
	virtual void OnEndHandle(wxSFShapeHandle& handle);

	virtual void OnEndDrag(const wxPoint& pos);

	XS_DECLARE_CLONABLE_CLASS(ImageShape)
};


////////////////////////////////////////////////////////////////////
///////////////////////////// TextShape ////////////////////////////
////////////////////////////////////////////////////////////////////


class TextShape : public wxSFEditTextShape {
public:
	TextShape();
	TextShape(const TextShape& other);

	virtual void OnEndHandle(wxSFShapeHandle& handle);
	virtual void OnEndDrag(const wxPoint& pos);
};


#endif