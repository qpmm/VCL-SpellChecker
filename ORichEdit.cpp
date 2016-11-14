#include "ORichEdit.h"

TextAttributes::TextAttributes(ITextRange** range)
{
  _range = range;
}

int TextAttributes::GetColor()
{
  long color;

  (*_range)->GetFont(&_style);
  _style->GetForeColor(&color);

  return color;
}

void TextAttributes::SetColor(int color)
{
  (*_range)->GetFont(&_style);
  _style->SetForeColor(color);
  (*_range)->SetFont(_style);
}

ORichEdit::ORichEdit(TRichEdit* Component)
{
  IUnknown* intf;
  
  Component->Perform(EM_GETOLEINTERFACE, 0, (int)&intf);
  intf->QueryInterface(__uuidof(ITextDocument), (void**)&_doc);

  SelAttributes = new TextAttributes(&_range);
  _doc->Range(0, 0, &_range);
  Object = Component;
}

ORichEdit::~ORichEdit()
{
  delete SelAttributes;
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

  _range->GetStart(&result);

  return result;
}

void ORichEdit::SetSelStart(int start)
{
  _range->SetStart(start);
}

int ORichEdit::GetSelLength()
{
  long start, end;

  _range->GetStart(&start);
  _range->GetEnd(&end);

  return (end - start);
}

void ORichEdit::SetSelLength(int length)
{
  long start;

  _range->GetStart(&start);
  _range->SetEnd(start + (long)length);
}

std::wstring ORichEdit::GetSelText()
{
  wchar_t* buf;

  _range->GetText(&buf);

  return std::wstring(buf);
}

void ORichEdit::SetSelText(std::wstring text)
{
  _range->SetText((wchar_t*)text.c_str());
}

void ORichEdit::SetSelText(wchar_t* text)
{
  _range->SetText(text);
}

std::wstring ORichEdit::GetText()
{
  ITextRange* range;
  wchar_t* buf;

  _doc->Range(0, GetTextLen(), &range);
  range->GetText(&buf);

  return std::wstring(buf);
}

void ORichEdit::SetText(std::wstring text)
{
  ITextRange* range;

  _doc->Range(0, GetTextLen(), &range);
  range->SetText((wchar_t*)text.c_str());
}

void ORichEdit::SetText(wchar_t* text)
{
  ITextRange* range;

  _doc->Range(0, GetTextLen(), &range);
  range->SetText(text);
}
