#include "CustomEditSpell.h"

#define isalnum(x) isalnum((x), std::locale("Russian_Russia.1251"))

TextRange::TextRange()
{
  StartPos = -1;
  Length = -1;
  IsMisspell = false;
}

TextRange::TextRange(int pos, int len)
{
  StartPos = pos;
  Length = len;
  IsMisspell = false;
}

CustomEditSpell::CustomEditSpell(TCustomEdit* Sender)
{
  Component = Sender;
}

TextRange CustomEditSpell::WordBounds(int Pos)
{
  TextRange result;
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

std::wstring CustomEditSpell::GetText()
{
  return std::wstring(Component->Text.c_str());
}

void CustomEditSpell::PerformSpell(std::wstring SubString, int Start)
{
  speller.checkText(SubString);
  
  for (unsigned i = 0; i < speller.result.size(); ++i)
  {
    MarkAsMistake(Start + speller.result[i].pos, speller.result[i].len);
  }
}

