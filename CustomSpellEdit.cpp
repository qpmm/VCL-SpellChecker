#include "CustomSpellEdit.h"

#define isalnum(x) isalnum((x), std::locale("Russian_Russia.1251"))

CustomSpellEdit::CustomSpellEdit(TCustomEdit* Sender)
{
  edit = Sender;
}

CustomSpellEdit::TextFragment CustomSpellEdit::WordBounds(int Pos)
{
  TextFragment result;
  std::wstring buf = this->GetText();

  if (buf.size() != 0)
  {
    result.StartPos = result.Length = Pos;

    while (result.StartPos - 1 >= 0 && isalnum(buf[result.StartPos - 1])) --result.StartPos;
    while (result.Length < (int)buf.size() && isalnum(buf[result.Length])) ++result.Length;

    result.Length -= result.StartPos;
  }

  return result;
}

std::wstring CustomSpellEdit::GetText()
{
  return std::wstring(edit->Text.c_str());
}

void CustomSpellEdit::PerformSpell(std::wstring SubString, int Start)
{
  speller.checkText(SubString);
  
  for (unsigned i = 0; i < speller.result.size(); ++i)
  {
    MarkAsMistake(Start + speller.result[i].pos, speller.result[i].len);
  }
}

