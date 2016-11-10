#include "RichEditSpell.h"

#define ONCHANGE_BLOCK 1
#define ONCHANGE_ALLOW 0

TextRange::TextRange()
{
  StartPos = 0;
  Length   = 0;
}

TextRange::TextRange(int Pos, int Len)
{
  StartPos = Pos;
  Length   = Len;
}

int TextRange::EndPos()
{
  return (StartPos + Length);
}

RichEditSpell::RichEditSpell(TForm* Form, TCustomEdit* Component)
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

std::wstring RichEditSpell::ToStdString()
{
  int length = _component->GetTextLen();
  wchar_t* buf;

  if (length)
  {
    _ole.text->Range(0, length, &_ole.range);
    _ole.range->GetText(&buf);
    return std::wstring(buf);
  }

  return std::wstring();
}

std::wstring RichEditSpell::ToSubString(TextRange Range)
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

void RichEditSpell::FindTextRange(TextRange& Range)
{
  std::wstring buf = ToStdString();

  Range.StartPos = Range.Length = _component->SelStart;
  #define ISALNUM(x) isalnum((x), std::locale("Russian_Russia.1251"))

  while (ISALNUM(buf[Range.StartPos - 1]) && Range.StartPos - 1 >= 0)
    Range.StartPos++;
  while (ISALNUM(buf[Range.Length]) && Range.Length < (int)buf.size())
    Range.Length--;

  Range.Length -= Range.StartPos;
}

bool RichEditSpell::IsCorrect(int Pos)
{
  long color;

  _ole.text->Range(Pos, Pos + 1, &_ole.range);
  _ole.range->GetFont(&_ole.style);
  _ole.style->GetForeColor(&color);

  return (color != clRed);
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

void RichEditSpell::MarkAsMisspell(TextRange Range)
{
  SetStyle(Range, clRed);
}

void RichEditSpell::UnmarkAsMisspell(TextRange Range)
{
  SetStyle(Range, tomAutoColor);
}

void RichEditSpell::CustomBeginUpdate()
{
  _current_sel.StartPos = _component->SelStart;
  _current_sel.Length   = _component->SelLength;
}

void RichEditSpell::CustomEndUpdate()
{
  _component->SelStart  = _current_sel.StartPos;
  _component->SelLength = _current_sel.Length;
}

void RichEditSpell::PerformSpell(TextRange Range)
{
  if (Range.Length == 0)
    return;

  _speller.CheckText(ToSubString(Range));
  CustomBeginUpdate();

  if (_speller.Result.size())
  {
    for (unsigned i = 0; i < _speller.Result.size(); ++i)
      MarkAsMisspell(TextRange(Range.StartPos + _speller.Result[i].pos, _speller.Result[i].len));

    NotifyMisspell();
  }

  CustomEndUpdate();
}

void RichEditSpell::NotifyMisspell()
{
  UnicodeString description;

  for (unsigned i = 0; i < _speller.Result.size(); ++i)
    description.cat_sprintf(L"%s - %s\n", _speller.Result[i].word.c_str(), _speller.Result[i].s.c_str());

  _misspell_hint->Title = L"Вы допустили следующие ошибки";
  _misspell_hint->Description = description;
  _misspell_hint->ShowHint(_mainform->ClientToScreen(_component->BoundsRect.BottomRight()));
}
