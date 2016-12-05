#include "ORichEdit.h"

Range::Range()
{
  Start = 0;
  End = 0;
}

Range::Range(long start, long end)
{
  Start = start;
  End = end;
}

int Range::ToInt()
{
  return (End | (Start << 16));
}

Range Range::FromInt(int src)
{
  return Range(src >> 16, (src << 16) >> 16);
}

long Range::Length()
{
  return (End - Start);
}

ORichEdit::ORichEdit(TRichEdit* component)
{
  IUnknown* intf;

  component->Perform(EM_GETOLEINTERFACE, 0, (int)&intf);
  intf->QueryInterface(__uuidof(ITextDocument), (void**)&doc);
  doc->Range(0, 0, &range);
}

Range ORichEdit::GetSelRange()
{
  Range r;

  doc->GetSelection(&sel);
  sel->GetStart(&r.Start);
  sel->GetEnd(&r.End);

  return r;
}

void ORichEdit::SetSelRange(Range r)
{
  doc->GetSelection(&sel);
  sel->SetRange(r.Start, r.End);
}

Range ORichEdit::GetTextRange()
{
  Range r;

  range->GetStart(&r.Start);
  range->GetEnd(&r.End);

  return r;
}

void ORichEdit::SetTextRange(Range r)
{
  doc->Range(r.Start, r.End, &range);
}

std::wstring ORichEdit::GetSelText()
{
  doc->GetSelection((ITextSelection**)&range);
  return GetText();
}

void ORichEdit::SetSelText(wchar_t* text)
{
  doc->GetSelection((ITextSelection**)&range);
  SetText(text);
}

std::wstring ORichEdit::GetText()
{
  wchar_t* buf;
  range->GetText(&buf);
  return std::wstring(buf);
}

void ORichEdit::SetText(wchar_t* text)
{
  // Создается копия, т.к. ПОЧЕМУ-ТО текст вставляется не целиком, если просто передать указатель
  std::wstring stroka = text;
  range->SetText((wchar_t*)stroka.c_str());
}

std::wstring ORichEdit::GetTextFromRange(Range r)
{
  SetTextRange(r);
  return GetText();
}

void ORichEdit::SetTextInRange(Range r, wchar_t* text)
{
  SetTextRange(r);
  SetText(text);
}

int ORichEdit::GetSelColor()
{
  //Range selection;
  long color;

  doc->GetSelection(&sel);
  //  selection = GetSelRange();

  //  if (selection.Length() == 0)
  //    sel->SetEnd(selection.Start + 1);
  sel->GetFont(&style);
  style->GetForeColor(&color);

  return color;
}

int ORichEdit::GetTextColor()
{
  long color;

  range->GetFont(&style);
  style->GetForeColor(&color);

  return color;
}

void ORichEdit::SetTextColor(int color)
{
  range->GetFont(&style);
  style->SetForeColor(color);
  range->SetFont(style);
}

Range ORichEdit::GetTextBounds()
{
  long length;

  doc->GetSelection(&sel);
  sel->GetStoryLength(&length);

  return Range(0, length);
}

std::wstring ORichEdit::GetFullText()
{
  return GetTextFromRange(GetTextBounds());
}
