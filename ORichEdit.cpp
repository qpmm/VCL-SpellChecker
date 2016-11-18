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

long Range::Length()
{
  return (End - Start);
}

ORichEdit::ORichEdit(TRichEdit* component)
{
  IUnknown* intf;

  component->Perform(EM_GETOLEINTERFACE, 0, (int)&intf);
  intf->QueryInterface(__uuidof(ITextDocument), (void**)&_doc);
  _doc->Range(0, 0, &_range);
}

Range ORichEdit::GetSelRange()
{
  Range range;

  _doc->GetSelection(&_sel);
  _sel->GetStart(&range.Start);
  _sel->GetEnd(&range.End);

  return range;
}

void ORichEdit::SetSelRange(Range range)
{
  _doc->GetSelection(&_sel);
  _sel->SetRange(range.Start, range.End);
}

Range ORichEdit::GetTextRange()
{
  Range range;

  _range->GetStart(&range.Start);
  _range->GetEnd(&range.End);

  return range;
}

void ORichEdit::SetTextRange(Range range)
{
  if (range.Start == 0 && range.End == -1)
    range.End = GetLength();

  _doc->Range(range.Start, range.End, &_range);
}

std::wstring ORichEdit::GetSelText()
{
  _doc->GetSelection((ITextSelection**)&_range);
  return GetText();
}

//void ORichEdit::SetSelText(std::wstring text)
//{
//  SetSelText(text.c_str());
//}

void ORichEdit::SetSelText(wchar_t* text)
{
  _doc->GetSelection((ITextSelection**)&_range);
  SetText(text);
}

std::wstring ORichEdit::GetText()
{
  wchar_t* buf;
  _range->GetText(&buf);
  return std::wstring(buf);
}

//void ORichEdit::SetText(std::wstring text)
//{
//  SetText(text.c_str());
//}
void ORichEdit::SetText(wchar_t* text)
{
  std::wstring stroka = text;
  _range->SetText((wchar_t*)stroka.c_str());
}

std::wstring ORichEdit::GetTextFromRange(Range range)
{
  SetTextRange(range);
  return GetText();
}

//void ORichEdit::SetTextInRange(Range range, std::wstring text)
//{
//  SetTextInRange(range, text.c_str());
//}
void ORichEdit::SetTextInRange(Range range, wchar_t* text)
{
  SetTextRange(range);
  SetText(text);
}

int ORichEdit::GetSelColor()
{
  Range selection;
  long color;

  _doc->GetSelection(&_sel);
  //_sel->
  //selection = GetSelRange();

//  if (selection.Length() == 0)
//    _sel->SetEnd(selection.Start + 1);

  _sel->GetFont(&_style);
  _style->GetForeColor(&color);

  return color;
}

int ORichEdit::GetTextColor()
{
  long color;

  _range->GetFont(&_style);
  _style->GetForeColor(&color);

  return color;
}

void ORichEdit::SetTextColor(int color)
{
  _range->GetFont(&_style);
  _style->SetForeColor(color);
  _range->SetFont(_style);
}

int ORichEdit::GetLength()
{
  long length;

  _doc->GetSelection(&_sel);
  _sel->GetStoryLength(&length);

  return length;
}
