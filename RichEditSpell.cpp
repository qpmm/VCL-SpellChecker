#include "RichEditSpell.h"

#define ONCHANGE_BLOCK 1
#define ONCHANGE_ALLOW 0

RichEditSpell::RichEditSpell(TForm* Form, TRichEdit* Component) : CustomMemoSpell(Form, Component)
{
  _mainform = Form;
  _component = Component;

  _component->Perform(EM_GETOLEINTERFACE, 0, (int)&_ole.intf);
  _ole.intf->QueryInterface(__uuidof(ITextDocument), (void**)&_ole.text);
}

int RichEditSpell::GetLength()
{
  GETTEXTLENGTHEX params;
  params.flags = GT_DEFAULT;
  params.codepage = 1200; // Unicode

  return _component->Perform(EM_GETTEXTLENGTHEX, (int)&params, 0);
}

std::wstring RichEditSpell::ToStdString()
{
  wchar_t* buf;
  int Length = GetLength();

  if (Length)
  {
    _ole.text->Range(0, GetLength(), &_ole.range);
    _ole.range->GetText(&buf);
    return std::wstring(buf);
  }

  return std::wstring();
}

std::wstring RichEditSpell::ToStdString(TextRange Range)
{
	wchar_t* buf;

  if (Range.Length)
  {
    _ole.text->Range(Range.StartPos, Range.EndPos(), &_ole.range);
    _ole.range->GetText(&buf);
    return std::wstring(buf);
  }

	return std::wstring();
}

void RichEditSpell::SetStyle(TextRange& Range, long Color)
{
  _component->Tag = ONCHANGE_BLOCK;
  _ole.text->Range(Range.StartPos, Range.EndPos(), &_ole.range);
  _ole.range->GetFont(&_ole.style);
  _ole.style->SetForeColor(Color);
  _ole.range->SetFont(_ole.style);
  _component->Tag = ONCHANGE_ALLOW;
}
    
bool RichEditSpell::IsMisspell(int Pos)
{
  long color;

  _ole.text->Range(Pos, Pos + 1, &_ole.range);
  _ole.range->GetFont(&_ole.style);
  _ole.style->GetForeColor(&color);

  return (color == clRed);
}

void RichEditSpell::MarkAsMisspell(TextRange Range)
{
  SetStyle(Range, clRed);
}

void RichEditSpell::UnmarkAsMisspell(TextRange Range)
{
  SetStyle(Range, tomAutoColor);
}

void RichEditSpell::NotifyMisspell()
{
}
