#include <wx/wx.h>
#include <wx/stdpaths.h>

#include "Icon.hpp"

wxString GetPathFromIconName(IconName iconName)
{
    wxString resourcesDir = wxStandardPaths::Get().GetResourcesDir();

    switch (iconName)
    {
    case IconName::AddCharacter:
        return resourcesDir + "/AddCharacter.svg";
        break;
    case IconName::AddDocument:
        return resourcesDir + "/AddDocument.svg";
        break;
    case IconName::AddFile:
        return resourcesDir + "/AddFile.svg";
        break;
    case IconName::AddFileLight:
        return resourcesDir + "/AddFile-light.svg";
        break;
    case IconName::AddFolder:
        return resourcesDir + "/AddFolder.svg";
        break;
    case IconName::AddFolderLight:
        return resourcesDir + "/AddFolder-light.svg";
        break;
    case IconName::AddImage:
        return resourcesDir + "/AddImage.svg";
        break;
    case IconName::AddItem:
        return resourcesDir + "/AddItem.svg";
        break;
    case IconName::AddLocation:
        return resourcesDir + "/AddLocation.svg";
        break;
    case IconName::AddNote:
        return resourcesDir + "/AddNote.svg";
        break;
    case IconName::AddText:
        return resourcesDir + "/AddText.svg";
        break;
    case IconName::AlignCenter:
        return resourcesDir + "/AlignCenter.svg";
        break;
    case IconName::AlignCenterLight:
        return resourcesDir + "/AlignCenter-light.svg";
        break;
    case IconName::AlignCenterJust:
        return resourcesDir + "/AlignCenterJust.svg";
        break;
    case IconName::AlignCenterJustLight:
        return resourcesDir + "/AlignCenterJust-light.svg";
        break;
    case IconName::AlignLeft:
        return resourcesDir + "/AlignLeft.svg";
        break;
    case IconName::AlignLeftLight:
        return resourcesDir + "/AlignLeft-light.svg";
        break;
    case IconName::AlignRight:
        return resourcesDir + "/AlignRight.svg";
        break;
    case IconName::AlignRightLight:
        return resourcesDir + "/AlignRight-light.svg";
        break;
    case IconName::ArrowDown:
        return resourcesDir + "/ArrowDown.svg";
        break;
    case IconName::ArrowLeft:
        return resourcesDir + "/ArrowLeft.svg";
        break;
    case IconName::ArrowRight:
        return resourcesDir + "/ArrowRight.svg";
        break;
    case IconName::ArrowUp:
        return resourcesDir + "/ArrowUp.svg";
        break;
    case IconName::Bold:
        return resourcesDir + "/Bold.svg";
        break;
    case IconName::BoldLight:
        return resourcesDir + "/BoldLight.svg";
        break;
    case IconName::Book:
        return resourcesDir + "/Book.svg";
        break;
    case IconName::Connection:
        return resourcesDir + "/Connection.svg";
        break;
    case IconName::Cursor:
        return resourcesDir + "/Cursor.svg";
        break;
    case IconName::FullScreen:
        return resourcesDir + "/FullScreen.svg";
        break;
    case IconName::FullScreenLight:
        return resourcesDir + "/FullScreen-light.svg";
        break;
    case IconName::Italic:
        return resourcesDir + "/Italic.svg";
        break;
    case IconName::ItalicLight:
        return resourcesDir + "/Italic-light.svg";
        break;
    case IconName::Minus:
        return resourcesDir + "/Minus.svg";
        break;
    case IconName::NoteView:
        return resourcesDir + "/NoteView.svg";
        break;
    case IconName::NoteViewLight:
        return resourcesDir + "/NoteView-light.svg";
        break;
    case IconName::Plus:
        return resourcesDir + "/Plus.svg";
        break;
    case IconName::Research:
        return resourcesDir + "/Research.svg";
        break;
    case IconName::Save:
        return resourcesDir + "/Save.svg";
        break;
    case IconName::Trash:
        return resourcesDir + "/Trash.svg";
        break;
    case IconName::Underline:
        return resourcesDir + "/Underline.svg";
        break;
    case IconName::UnderlineLight:
        return resourcesDir + "/Underline-light.svg";
        break;
    }
}

wxBitmapBundle Style::Icon::GetBmp(IconName iconName, const wxSize& size)
{
    wxString iconPath = GetPathFromIconName(iconName);
    return wxBitmapBundle::FromSVGFile(iconPath, size);
}