#include "ORichEdit.h"

NumRange::NumRange()
{
  Start = 0;
  End = 0;
}

NumRange::NumRange(long start, long end)
{
  Start = start;
  End = end;
}

int NumRange::ToInt()
{
  return (End | (Start << 16));
}

NumRange NumRange::FromInt(int src)
{
  return NumRange(src >> 16, (src << 16) >> 16);
}

long NumRange::Length()
{
  return (End - Start);
}

ORichEdit::ORichEdit(TRichEdit* component)
{
  IUnknown* intf;

  component->Perform(EM_GETOLEINTERFACE, 0, (int)&intf);
  intf->QueryInterface(__uuidof(ITextDocument), (void**)&doc);
  doc->Range(0, 0, &txt);
  intf->Release();
}

ORichEdit::~ORichEdit()
{
  txt->Release();
  doc->Release();
}

NumRange ORichEdit::GetSelRange()
{
  ITextSelection* sel;
  NumRange Range;

  doc->GetSelection(&sel);
  sel->GetStart(&Range.Start);
  sel->GetEnd(&Range.End);
  sel->Release();

  return Range;
}

void ORichEdit::SetSelRange(NumRange Range)
{
  ITextSelection* sel;

  doc->GetSelection(&sel);
  sel->SetRange(Range.Start, Range.End);
  sel->Release();
}

NumRange ORichEdit::GetTextRange()
{
  NumRange Range;

  txt->GetStart(&Range.Start);
  txt->GetEnd(&Range.End);

  return Range;
}

void ORichEdit::SetTextRange(NumRange Range)
{
  txt->Release();
  doc->Range(Range.Start, Range.End, &txt);
}

UnicodeString& ORichEdit::GetText(UnicodeString& dest)
{
  wchar_t* buf;

  txt->GetText(&buf);
  dest = buf;

  return dest;
}

void ORichEdit::SetText(wchar_t* str)
{
  wchar_t *buf = SysAllocString(str);
  txt->SetText(buf);
  SysFreeString(buf);
}

UnicodeString& ORichEdit::GetTextFromRange(NumRange Range, UnicodeString& dest)
{
  SetTextRange(Range);
  return GetText(dest);
}

void ORichEdit::SetTextInRange(NumRange Range, wchar_t* str)
{
  SetTextRange(Range);
  SetText(str);
}

int ORichEdit::GetSelColor()
{
  ITextSelection* sel;
  long color;

  doc->GetSelection(&sel);
  sel->GetFont(&style);
  style->GetForeColor(&color);
  style->Release();
  sel->Release();

  return color;
}

int ORichEdit::GetTextColor()
{
  long color;

  txt->GetFont(&style);
  style->GetForeColor(&color);
  style->Release();

  return color;
}

void ORichEdit::SetTextColor(int color)
{
  txt->GetFont(&style);
  style->SetForeColor(color);
  txt->SetFont(style);
  style->Release();
}

NumRange ORichEdit::GetTextBounds()
{
  ITextSelection* sel;
  long length;

  doc->GetSelection(&sel);
  sel->GetStoryLength(&length);
  sel->Release();

  return NumRange(0, length);
}

UnicodeString& ORichEdit::GetFullText(UnicodeString& dest)
{
  return GetTextFromRange(GetTextBounds(), dest);
}
