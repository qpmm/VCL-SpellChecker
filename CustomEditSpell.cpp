#include "CustomEditSpell.h"

#define ISALNUM(x) isalnum((x), std::locale("Russian_Russia.1251"))

/* class TextRange */

TextRange::TextRange()
{
  StartPos = -1;
  Length   = -1;
}

TextRange::TextRange(int Pos, int Len)
{
  StartPos = Pos;
  Length   = Len;
}

bool TextRange::operator==(TextRange rvl)
{
  return (StartPos == rvl.StartPos && Length == rvl.Length);
}

/* class CustomEditSpell */

CustomEditSpell::CustomEditSpell(TForm* Form, TCustomEdit* Component)
{
  _mainform = Form;
  _object = Component;
  _misspell_pool = new std::map<int, int>;
  _misspell_hint = new TBalloonHint(Component);
}

CustomEditSpell::~CustomEditSpell()
{
  delete _misspell_pool;
  delete _misspell_hint;
}

// Может отдельная функция не нужна?
std::wstring CustomEditSpell::ToStdString()
{
  return std::wstring(_object->Text.c_str());
}

std::wstring CustomEditSpell::ToStdString(TextRange Range)
{
  _object->SelStart  = Range.StartPos;
  _object->SelLength = Range.Length;
  
  return std::wstring(_object->SelText.c_str());
}

TextRange CustomEditSpell::FindTextRange()
{
  int start = -1, len = -1;
  std::wstring buf = ToStdString();

  if (buf.size() != 0)
  {
    start = len = _object->SelStart;

    while (start - 1 >= 0 && ISALNUM(buf[start - 1])) --start;
    while (len < (int)buf.size() && ISALNUM(buf[len])) ++len;

    len -= start;

    if (len == 0)
    {
      start = -1;
      len = -1;
    }
  }

  return TextRange(start, len);
}

bool CustomEditSpell::IsMisspell(int Pos)
{
  return (_misspell_pool->find(Pos) != _misspell_pool->end());
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
  _current_sel.StartPos = _object->SelStart;
  //_current_sel.Length   = _object->SelLength;
}

void CustomEditSpell::CustomEndUpdate()
{
  _object->SelStart  = _current_sel.StartPos;
  //_object->SelLength = _current_sel.Length;
}

void CustomEditSpell::PerformSpell(TextRange Range)
{
  _speller.CheckText(ToStdString(Range));

  if (_speller.Result.size())
  {
    CustomBeginUpdate();

    for (unsigned i = 0; i < _speller.Result.size(); ++i)
      MarkAsMisspell(TextRange(Range.StartPos + _speller.Result[i].pos, _speller.Result[i].len));

    CustomEndUpdate();
    NotifyMisspell();
  }
}

void CustomEditSpell::NotifyMisspell()
{
  UnicodeString descr;
  
  for (unsigned i = 0; i < _speller.Result.size(); ++i)
    descr.cat_sprintf(L"%s - %s\n", _speller.Result[i].word.c_str(), _speller.Result[i].s.c_str());
  
  _misspell_hint->Title = L"Вы допустили следующие ошибки";
  _misspell_hint->Description = descr;
  _misspell_hint->ShowHint(_mainform->ClientToScreen(_object->BoundsRect.BottomRight()));
}
