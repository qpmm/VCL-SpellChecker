#include "CustomEditSpell.h"

#define ISALNUM(x) isalnum((x), std::locale("Russian_Russia.1251"))

/* class TextRange */

TextRange::TextRange()
{
  StartPos = -1;
  Length = -1;
}

TextRange::TextRange(int Pos, int Len)
{
  StartPos = Pos;
  Length = Len;
}

/* class CustomEditSpell */

CustomEditSpell::CustomEditSpell(TCustomEdit* Component)
{
  _object = Component;
  _misspell_pool = new std::map<int, int>();
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
  _object->SelStart = Range.StartPos;
  _object->SelLength = Range.Length;
  
  return std::wstring(_object->SelText.c_str());
}

TextRange CustomEditSpell::FindTextRange()
{
  int startPos = -1, length = -1;
  std::wstring buf = ToStdString();

  if (buf.size() != 0)
  {
    startPos = length = _object->SelStart;

    while (startPos - 1 >= 0 && ISALNUM(buf[startPos - 1])) --startPos;
    while (length < (int)buf.size() && ISALNUM(buf[length])) ++length;

    length -= startPos;
  }

  return TextRange(startPos, length);
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
  _current_pos = _object->SelStart;
}

void CustomEditSpell::CustomEndUpdate()
{
  _object->SelStart = _current_pos;
}

void CustomEditSpell::PerformSpell(TextRange Range)
{
  CustomBeginUpdate();
  
  _speller.CheckText(ToStdString(Range));
  for (unsigned i = 0; i < _speller.Result.size(); ++i)
  {
    Range.StartPos += _speller.Result[i].pos;
    Range.Length = _speller.Result[i].len;
    MarkAsMisspell(Range);
  }
  
  CustomEndUpdate();
  NotifyMisspell();
}

void CustomEditSpell::NotifyMisspell()
{
  UnicodeString descr;
  
  for (unsigned i = 0; i < _speller.Result.size(); ++i)
    descr.cat_sprintf(L"%s - %s\n", _speller.Result);
  
  _misspell_hint->Title = L"Вы допустили следующие ошибки";
  _misspell_hint->Description = descr;
  _misspell_hint->ShowHint(_object->BoundsRect.BottomRight());
}
