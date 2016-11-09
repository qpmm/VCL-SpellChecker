#include "CustomEditSpell.h"

#define ISALNUM(x) isalnum((x), std::locale("Russian_Russia.1251"))

/* class TextRange */

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

/* class CustomEditSpell */

CustomEditSpell::CustomEditSpell(TForm* Form, TCustomEdit* Component)
{
  _mainform = Form;
  _component = Component;

  _misspell_pool = new std::map<int, int>;
  _misspell_hint = new TBalloonHint(Component);
}

CustomEditSpell::~CustomEditSpell()
{
  delete _misspell_pool;
  delete _misspell_hint;
}

/*int CustomEditSpell::GetLength()
{
  GETTEXTLENGTHEX params;
  params.flags = GT_DEFAULT;
  params.codepage = 1200; // Unicode

  return _component->Perform(EM_GETTEXTLENGTHEX, (int)&params, 0);
}*/

bool CustomEditSpell::CheckRange(TextRange& Range)
{
  return (Range.StartPos > 0 && Range.EndPos() <= _component->GetTextLen());
}

std::wstring CustomEditSpell::ToStdString(TextRange Range)
{
  std::wstring result;

  if (Range.StartPos == 0 && Range.Length == -1)
  {
    result = _component->Text.c_str();
    return;
  }

  CustomBeginUpdate();

  _component->SelStart  = Range.StartPos;
  _component->SelLength = Range.Length;
  result = _component->SelText.c_str();

  CustomBeginUpdate();
  
  return result;
}

void CustomEditSpell::FindTextRange(TextRange& Range)
{
  std::wstring buf = ToStdString();

  Range.StartPos = Range.Length = _component->SelStart;

  while (ISALNUM(buf[Range.StartPos - 1]) && Range.StartPos - 1 >= 0) --Range.StartPos;
  while (ISALNUM(buf[Range.Length]) && Range.Length < (int)buf.size()) ++Range.Length;

  Range.Length -= Range.StartPos;
}

bool CustomEditSpell::IsCorrect(int Pos)
{
  return (_misspell_pool->find(Pos) == _misspell_pool->end());
}

void CustomEditSpell::MarkAsMisspell(TextRange Range)
{
  (*_misspell_pool)[Range.StartPos] = Range.Length;
}

void CustomEditSpell::UnmarkAsMisspell(TextRange Range)
{
  _misspell_pool->erase(Range.StartPos);
}

void CustomEditSpell::CustomBeginUpdate()
{
  _current_sel.StartPos = _component->SelStart;
  _current_sel.Length   = _component->SelLength;
}

void CustomEditSpell::CustomEndUpdate()
{
  _component->SelStart  = _current_sel.StartPos;
  _component->SelLength = _current_sel.Length;
}

void CustomEditSpell::PerformSpell(TextRange Range)
{
  if (Range.Length == 0)
    return;

  _speller.CheckText(ToStdString(Range));

  CustomBeginUpdate();
  if (_speller.Result.size())
  {
    for (unsigned i = 0; i < _speller.Result.size(); ++i)
      MarkAsMisspell(TextRange(Range.StartPos + _speller.Result[i].pos, _speller.Result[i].len));

    NotifyMisspell();
  }

  CustomEndUpdate();
}

void CustomEditSpell::NotifyMisspell()
{
  UnicodeString descr;
  
  for (unsigned i = 0; i < _speller.Result.size(); ++i)
    descr.cat_sprintf(L"%s - %s\n", _speller.Result[i].word.c_str(), _speller.Result[i].s.c_str());
  
  _misspell_hint->Title = L"Вы допустили следующие ошибки";
  _misspell_hint->Description = descr;
  _misspell_hint->ShowHint(_mainform->ClientToScreen(_component->BoundsRect.BottomRight()));
}
