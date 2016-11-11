#include "ORichEdit.h"

ORichEdit::ORichEdit(TRichEdit* Component)
{
  IUnknown* intf;
  
  Component->Perform(EM_GETOLEINTERFACE, 0, (int)&intf);
  intf->QueryInterface(__uuidof(ITextDocument), (void**)&_doc);
}

int ORichEdit::GetTextLen()
{
  long result;

  _doc->GetSelection(&_sel);
  _sel->GetStoryLength(&result);

  return result;
}

int ORichEdit::GetSelStart()
{
  long result;

  _doc->GetSelection(&_sel);
  _sel->GetStart(&result);

  return result;
}

void ORichEdit::SetSelStart(int start)
{
  _doc->GetSelection(&_sel);
  _sel->SetStart(start);
}

int ORichEdit::GetSelLength()
{
  long start, end;

  _doc->GetSelection(&_sel);
  _sel->GetEnd(&end);
  _sel->GetStart(&start);

  return (end - start);
}

void ORichEdit::SetSelLength(int length)
{
  long start;

  _doc->GetSelection(&_sel);
  _sel->GetStart(&start);
  _sel->SetEnd(start + (long)length);
}

std::wstring ORichEdit::GetSelText()
{
  long start, end;
  wchar_t* buf;

  _doc->GetSelection(&_sel);
  _sel->GetStart(&start);
  _sel->GetEnd(&end);
  _doc->Range(start, end, &_range);
  _range->GetText(&buf);

  return std::wstring(buf);
}

void ORichEdit::SetSelText(std::wstring text)
{
  long start, end;

  _doc->GetSelection(&_sel);
  _sel->GetStart(&start);
  _sel->GetEnd(&end);

  _doc->Range(start, end, &_range);
  _range->SetText((wchar_t*)text.c_str());
}

void ORichEdit::SetSelText(wchar_t* text)
{
  long start, end;

  _doc->GetSelection(&_sel);
  _sel->GetStart(&start);
  _sel->GetEnd(&end);

  _doc->Range(start, end, &_range);
  _range->SetText(text);
}

std::wstring ORichEdit::GetText()
{
  wchar_t* buf;

  _doc->Range(0, GetTextLen(), &_range);
  _range->GetText(&buf);

  return std::wstring(buf);
}

void ORichEdit::SetText(std::wstring text)
{
  _doc->Range(0, GetTextLen(), &_range);
  _range->SetText((wchar_t*)text.c_str());
}

void ORichEdit::SetText(wchar_t* text)
{
  _doc->Range(0, GetTextLen(), &_range);
  _range->SetText(text);
}