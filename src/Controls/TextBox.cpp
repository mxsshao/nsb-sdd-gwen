/*
 *  GWEN
 *  Copyright (c) 2010 Facepunch Studios
 *  See license in Gwen.h
 */


#include "Gwen/Gwen.h"
#include "Gwen/Controls/TextBox.h"
#include "Gwen/Skin.h"
#include "Gwen/Anim.h"
#include "Gwen/Utility.h"
#include "Gwen/Platform.h"

#include <math.h>

using namespace Gwen;
using namespace Gwen::Controls;

#ifndef GWEN_NO_ANIMATION
class ChangeCaretColor : public Gwen::Anim::Animation
{
public:

    virtual void Think()
    {
        gwen_cast<TextBox>(m_Control)->UpdateCaretColor();
    }

};


#endif // ifndef GWEN_NO_ANIMATION


GWEN_CONTROL_CONSTRUCTOR(TextBox)
{
    SetSize(200, 20);
    SetMouseInputEnabled(true);
    SetKeyboardInputEnabled(true);
    SetAlignment(Pos::Left|Pos::CenterV);
    SetPadding(Padding(4, 2, 4, 2));
    m_iCursorPos = 0;
    m_iCursorEnd = 0;
    m_iCursorLine = 0;
    m_bSelectAll = false;
    SetTextColor(Gwen::Color(50, 50, 50, 255));         // TODO: From Skin
    SetSelectAllOnFocus(true);
	SetTabable(true);
    AddAccelerator(L"Ctrl + C", &TextBox::OnCopy);
    AddAccelerator(L"Ctrl + X", &TextBox::OnCut);
    AddAccelerator(L"Ctrl + V", &TextBox::OnPaste);
    AddAccelerator(L"Ctrl + A", &TextBox::OnSelectAll);
    Gwen::Anim::Add(this, new ChangeCaretColor());
}

bool TextBox::OnChar(Gwen::UnicodeChar c)
{
    if (c == '\t')
        return false;

    Gwen::UnicodeString str;
    str += c;
    InsertText(str);
    return true;
}

void TextBox::InsertText(const Gwen::UnicodeString& strInsert)
{
    // TODO: Make sure fits (implement maxlength)
    if (HasSelection())
        EraseSelection();

    if (m_iCursorPos > TextLength())
        m_iCursorPos = TextLength();

    if (!IsTextAllowed(strInsert, m_iCursorPos))
        return;

    UnicodeString str = GetText().GetUnicode();
    str.insert(m_iCursorPos, strInsert);
    SetText(str);
    m_iCursorPos += (int)strInsert.size();
    m_iCursorEnd = m_iCursorPos;
    m_iCursorLine = 0;
    RefreshCursorBounds();
}

#ifndef GWEN_NO_ANIMATION
void TextBox::UpdateCaretColor()
{
    if (m_fNextCaretColorChange > Gwen::Platform::GetTimeInSeconds())
        return;

    if (!HasFocus())
    {
        m_fNextCaretColorChange = Gwen::Platform::GetTimeInSeconds()+0.5f; return;
    }

    Gwen::Color targetcolor = Gwen::Color(230, 230, 230, 255);

    if (m_CaretColor == targetcolor)
        targetcolor = Gwen::Color(20, 20, 20, 255);

    m_fNextCaretColorChange = Gwen::Platform::GetTimeInSeconds()+0.5;
    m_CaretColor = targetcolor;
    Redraw();
}

#endif // ifndef GWEN_NO_ANIMATION

void TextBox::Render(Skin::Base* skin)
{
    if (ShouldDrawBackground())
        skin->DrawTextBox(this);

    if (!HasFocus())
        return;

    // Draw selection.. if selected..
    if (m_iCursorPos != m_iCursorEnd)
    {
        skin->GetRender()->SetDrawColor(Gwen::Color(50, 170, 255, 200));
        skin->GetRender()->DrawFilledRect(m_rectSelectionBounds);
    }

    // Draw caret
    skin->GetRender()->SetDrawColor(m_CaretColor);
    skin->GetRender()->DrawFilledRect(m_rectCaretBounds);
}

void TextBox::RefreshCursorBounds()
{
    m_fNextCaretColorChange = Gwen::Platform::GetTimeInSeconds()+1.5f;
    m_CaretColor = Gwen::Color(30, 30, 30, 255);
    MakeCaratVisible();
    Gwen::Rect pA = GetCharacterPosition(m_iCursorPos);
    Gwen::Rect pB = GetCharacterPosition(m_iCursorEnd);
    m_rectSelectionBounds.x = Gwen::Min(pA.x, pB.x);
    m_rectSelectionBounds.y = m_Text->Y()-1;
    m_rectSelectionBounds.w = Gwen::Max(pA.x, pB.x)-m_rectSelectionBounds.x;
    m_rectSelectionBounds.h = m_Text->Height()+2;
    m_rectCaretBounds.x = pA.x;
    m_rectCaretBounds.y = pA.y;
    m_rectCaretBounds.w = 1;
    m_rectCaretBounds.h = pA.h;
    Redraw();
}

void TextBox::OnPaste(Gwen::Controls::Base* /*pCtrl*/)
{
    InsertText(Platform::GetClipboardText());
}

void TextBox::OnCopy(Gwen::Controls::Base* /*pCtrl*/)
{
    if (!HasSelection())
        return;

    Platform::SetClipboardText(GetSelection());
}

void TextBox::OnCut(Gwen::Controls::Base* /*pCtrl*/)
{
    if (!HasSelection())
        return;

    Platform::SetClipboardText(GetSelection());
    EraseSelection();
}

void TextBox::OnSelectAll(Gwen::Controls::Base* /*pCtrl*/)
{
    m_iCursorEnd = 0;
    m_iCursorPos = TextLength();
    m_iCursorLine = 0;
    RefreshCursorBounds();
}

void TextBox::OnMouseDoubleClickLeft(int /*x*/, int /*y*/)
{
    OnSelectAll(this);
}

UnicodeString TextBox::GetSelection()
{
    if (!HasSelection())
        return L"";

    int iStart = Gwen::Min(m_iCursorPos, m_iCursorEnd);
    int iEnd = Gwen::Max(m_iCursorPos, m_iCursorEnd);
    const UnicodeString& str = GetText().GetUnicode();
    return str.substr(iStart, iEnd-iStart);
}

bool TextBox::OnKeyReturn(bool bDown)
{
    if (bDown)
        return true;

    // Try to move to the next control, as if tab had been pressed
    OnKeyTab(true);

    // If we still have focus, blur it.
    if (HasFocus())
        Blur();

    // This is called AFTER the blurring so you can
    // refocus in your onReturnPressed hook.
    OnEnter();
    return true;
}

bool TextBox::OnKeyBackspace(bool bDown)
{
    if (!bDown)
        return true;

    if (HasSelection())
    {
        EraseSelection();
        return true;
    }

    if (m_iCursorPos == 0)
        return true;

    DeleteText(m_iCursorPos-1, 1);
    return true;
}

bool TextBox::OnKeyDelete(bool bDown)
{
    if (!bDown)
        return true;

    if (HasSelection())
    {
        EraseSelection();
        return true;
    }

    if (m_iCursorPos >= TextLength())
        return true;

    DeleteText(m_iCursorPos, 1);
    return true;
}

bool TextBox::OnKeyLeft(bool bDown)
{
    if (!bDown)
        return true;

    if (m_iCursorPos > 0)
        m_iCursorPos--;

    if (!Gwen::Input::IsShiftDown())
        m_iCursorEnd = m_iCursorPos;

    RefreshCursorBounds();
    return true;
}

bool TextBox::OnKeyRight(bool bDown)
{
    if (!bDown)
        return true;

    if (m_iCursorPos < TextLength())
        m_iCursorPos++;

    if (!Gwen::Input::IsShiftDown())
        m_iCursorEnd = m_iCursorPos;

    RefreshCursorBounds();
    return true;
}

bool TextBox::OnKeyHome(bool bDown)
{
    if (!bDown)
        return true;

    m_iCursorPos = 0;

    if (!Gwen::Input::IsShiftDown())
        m_iCursorEnd = m_iCursorPos;

    RefreshCursorBounds();
    return true;
}

bool TextBox::OnKeyEnd(bool /*bDown*/)
{
    m_iCursorPos = TextLength();

    if (!Gwen::Input::IsShiftDown())
        m_iCursorEnd = m_iCursorPos;

    RefreshCursorBounds();
    return true;
}

void TextBox::SetCursorPos(int i)
{
    if (m_iCursorPos == i)
        return;

    m_iCursorPos = i;
    m_iCursorLine = 0;
    RefreshCursorBounds();
}

void TextBox::SetCursorEnd(int i)
{
    if (m_iCursorEnd == i)
        return;

    m_iCursorEnd = i;
    RefreshCursorBounds();
}

void TextBox::DeleteText(int iStartPos, int iLength)
{
    UnicodeString str = GetText().GetUnicode();
    str.erase(iStartPos, iLength);
    SetText(str);

    if (m_iCursorPos > iStartPos)
        SetCursorPos(m_iCursorPos-iLength);

    SetCursorEnd(m_iCursorPos);
}

bool TextBox::HasSelection()
{
    return m_iCursorPos != m_iCursorEnd;
}

void TextBox::EraseSelection()
{
    int iStart = Gwen::Min(m_iCursorPos, m_iCursorEnd);
    int iEnd = Gwen::Max(m_iCursorPos, m_iCursorEnd);
    DeleteText(iStart, iEnd-iStart);
    // Move the cursor to the start of the selection,
    // since the end is probably outside of the string now.
    m_iCursorPos = iStart;
    m_iCursorEnd = iStart;
}

void TextBox::OnMouseClickLeft(int x, int y, bool bDown)
{
    if (m_bSelectAll)
    {
        OnSelectAll(this);
        m_bSelectAll = false;
        return;
    }

    int iChar = m_Text->GetClosestCharacter(m_Text->CanvasPosToLocal(Gwen::Point(x, y)));

    if (bDown)
    {
        SetCursorPos(iChar);

        if (!Gwen::Input::IsShiftDown())
            SetCursorEnd(iChar);

        Gwen::MouseFocus = this;
    }
    else
    {
        if (Gwen::MouseFocus == this)
        {
            SetCursorPos(iChar);
            Gwen::MouseFocus = NULL;
        }
    }
}

void TextBox::OnMouseMoved(int x, int y, int /*deltaX*/, int /*deltaY*/)
{
    if (Gwen::MouseFocus != this)
        return;

    int iChar = m_Text->GetClosestCharacter(m_Text->CanvasPosToLocal(Gwen::Point(x, y)));
    SetCursorPos(iChar);
}

void TextBox::MakeCaratVisible()
{
    if (m_Text->Width() < Width())
    {
        m_Text->Position(m_iAlign);
    }
    else
    {
        int iCaratPos = m_Text->GetCharacterPosition(m_iCursorPos).x;
        int iRealCaratPos = iCaratPos+m_Text->X();
        int iSlidingZone = m_Text->GetFont()->size+1;   // Width()*0.1f

        // If the carat is already in a semi-good position, leave it.
        if (iRealCaratPos >= iSlidingZone && iRealCaratPos <= Width()-iSlidingZone)
            return;

        int x = 0;

        if (iRealCaratPos > Width()-iSlidingZone)
            x = Width()-iCaratPos-iSlidingZone;

        if (iRealCaratPos < iSlidingZone)
            x = -iCaratPos+iSlidingZone;

        // Don't show too much whitespace to the right
        if (x+m_Text->Width() < Width()-GetPadding().right)
            x = -m_Text->Width()+(Width()-GetPadding().right);

        // Or the left
        if (x > GetPadding().left)
            x = GetPadding().left;

        int y = 0;

        if (m_iAlign&Pos::Top)
            y = GetPadding().top;

        if (m_iAlign&Pos::Bottom)
            y = Height()-m_Text->Height()-GetPadding().bottom;

        if (m_iAlign&Pos::CenterV)
            y = (Height()-m_Text->Height()) / 2;

        m_Text->SetPos(x, y);
    }
}

void TextBox::Layout(Skin::Base* skin)
{
    BaseClass::Layout(skin);
    RefreshCursorBounds();
}

void TextBox::PostLayout(Skin::Base* skin)
{
}

void TextBox::OnTextChanged()
{
    if (m_iCursorPos > TextLength())
        m_iCursorPos = TextLength();

    if (m_iCursorEnd > TextLength())
        m_iCursorEnd = TextLength();

    onTextChanged.Call(this);
}

void TextBox::OnEnter()
{
    onReturnPressed.Call(this);
}

void TextBox::MoveCaretToEnd()
{
    m_iCursorPos = TextLength();
    m_iCursorEnd = TextLength();
    RefreshCursorBounds();
}

void TextBox::MoveCaretToStart()
{
    m_iCursorPos = 0;
    m_iCursorEnd = 0;
    RefreshCursorBounds();
}

GWEN_CONTROL_CONSTRUCTOR(TextBoxMultiline)
{
    SetWrap(true);
    SetAlignment(Pos::Left|Pos::Top);
}

bool TextBoxMultiline::OnKeyReturn(bool bDown)
{
    if (bDown)
        InsertText(L"\n");

    return true;
}

void TextBoxMultiline::MakeCaratVisible()
{
    // TODO. scroll vertically
}

int TextBoxMultiline::GetCurrentLine()
{
    return m_Text->GetLineFromChar(m_iCursorPos);
}

bool TextBoxMultiline::OnKeyHome(bool bDown)
{
    if (!bDown)
        return true;

    int iCurrentLine = GetCurrentLine();
    int iChar = m_Text->GetStartCharFromLine(iCurrentLine);
    m_iCursorLine = 0;
    m_iCursorPos = iChar;

    if (!Gwen::Input::IsShiftDown())
        m_iCursorEnd = m_iCursorPos;

    RefreshCursorBounds();
    return true;
}

bool TextBoxMultiline::OnKeyEnd(bool bDown)
{
    if (!bDown)
        return true;

    int iCurrentLine = GetCurrentLine();
    int iChar = m_Text->GetEndCharFromLine(iCurrentLine);
    m_iCursorLine = 0;
    m_iCursorPos = iChar-1;   // NAUGHTY

    if (!Gwen::Input::IsShiftDown())
        m_iCursorEnd = m_iCursorPos;

    RefreshCursorBounds();
    return true;
}

bool TextBoxMultiline::OnKeyUp(bool bDown)
{
    if (!bDown)
        return true;

    if (m_iCursorLine == 0)
        m_iCursorLine = m_Text->GetCharPosOnLine(m_iCursorPos);

    int iLine = m_Text->GetLineFromChar(m_iCursorPos);

    if (iLine == 0)
        return true;

    m_iCursorPos = m_Text->GetStartCharFromLine(iLine-1);
    m_iCursorPos += Clamp(m_iCursorLine, 0, m_Text->GetLine(iLine-1)->Length());
    m_iCursorPos = Clamp(m_iCursorPos, 0, m_Text->Length());

    if (!Gwen::Input::IsShiftDown())
        m_iCursorEnd = m_iCursorPos;

    RefreshCursorBounds();
    return true;
}

bool TextBoxMultiline::OnKeyDown(bool bDown)
{
    if (!bDown)
        return true;

    if (m_iCursorLine == 0)
        m_iCursorLine = m_Text->GetCharPosOnLine(m_iCursorPos);

    int iLine = m_Text->GetLineFromChar(m_iCursorPos);

    if (iLine >= m_Text->NumLines()-1)
        return true;

    m_iCursorPos = m_Text->GetStartCharFromLine(iLine+1);
    m_iCursorPos += Clamp(m_iCursorLine, 0, m_Text->GetLine(iLine+1)->Length());
    m_iCursorPos = Clamp(m_iCursorPos, 0, m_Text->Length());

    if (!Gwen::Input::IsShiftDown())
        m_iCursorEnd = m_iCursorPos;

    RefreshCursorBounds();
    return true;
}
