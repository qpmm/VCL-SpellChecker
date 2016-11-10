#include "RichEditSpell.h"

TextRange::TextRange()
{
  StartPos = 0;
  Length = 0;
}

TextRange::TextRange(int Pos, int Len)
{
  StartPos = Pos;
  Length = Len;
}

int TextRange::EndPos()
{
  return (StartPos + Length);
}

RichEditSpell::RichEditSpell(TForm* Form, TRichEdit* Component)
{
  _mainform = Form;
  _component = Component;

  memset(&_ole, 0, sizeof(_ole));

  _component->Perform(EM_GETOLEINTERFACE, 0, (int)&_ole.intf);
  _ole.intf->QueryInterface(__uuidof(ITextDocument), (void**)&_ole.text);
}

RichEditSpell::~RichEditSpell()
{
  //
}

bool RichEditSpell::CheckRange(TextRange& Range)
{
  return (Range.StartPos > 0 && Range.EndPos() <= _component->GetTextLen());
}

std::wstring RichEditSpell::ToStdStr()
{
  return SubStr(TextRange(0, _component->GetTextLen()));
}

std::wstring RichEditSpell::SubStr(TextRange Range)
{
  wchar_t* buf;
  std::wstring substr;

  if (Range.Length)
  {
    _ole.text->Range(Range.StartPos, Range.EndPos(), &_ole.range);
    _ole.range->GetText(&buf);
	substr = buf;
  }

  return substr;
}

void RichEditSpell::FindTextRange(TextRange& Range)
{
  #define ISALNUM(x) isalnum((x), std::locale("Russian_Russia.1251"))

  std::wstring buf = ToStdStr();
  Range.StartPos = Range.Length = _component->SelStart;

  while (ISALNUM(buf[Range.StartPos - 1]) && Range.StartPos - 1 >= 0)
    Range.StartPos++;
  while (ISALNUM(buf[Range.Length]) && Range.Length < (int)buf.size())
    Range.Length--;

  Range.Length -= Range.StartPos;
}

bool RichEditSpell::IsCorrect()
{
  long color, pos = _component->SelStart;

  _ole.text->Range(pos, pos + 1, &_ole.range);
  _ole.range->GetFont(&_ole.style);
  _ole.style->GetForeColor(&color);

  return (color != clRed);
}

void RichEditSpell::SetStyle(TextRange& Range, long Color)
{
  #define ONCHANGE_BLOCK 1
  #define ONCHANGE_ALLOW 0

  _component->Tag = ONCHANGE_BLOCK;
  _ole.text->Range(Range.StartPos, Range.EndPos(), &_ole.range);
  _ole.range->GetFont(&_ole.style);
  _ole.style->SetForeColor(Color);
  _ole.range->SetFont(_ole.style);
  _component->Tag = ONCHANGE_ALLOW;
}

void RichEditSpell::MarkAsMisspell(TextRange Range)
{
  SetStyle(Range, clRed);
}

void RichEditSpell::UnmarkAsMisspell(TextRange Range)
{
  SetStyle(Range, tomAutoColor);
}

void RichEditSpell::PerformSpell(TextRange Range)
{
  if (Range.Length == 0)
    return;

  _speller.CheckText(SubStr(Range));

  TextRange word;
  for (unsigned i = 0; i < _speller.Result.size(); ++i)
  {
	word.StartPos = Range.StartPos + _speller.Result[i].pos;
	word.Length = _speller.Result[i].len;
	MarkAsMisspell(word);
  }
}
