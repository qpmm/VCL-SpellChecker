﻿#include "RichEditSpell.h"

#define ONCHANGE_BLOCK 1
#define ONCHANGE_ALLOW 0

RichEditSpell::RichEditSpell(TForm* form, TRichEdit* component)
{
  _mainform = form;
  _component = component;
  ole = new ORichEdit(component);
}

RichEditSpell::~RichEditSpell()
{
  delete ole;
}

void RichEditSpell::FindTextRange(Range& range)
{
  #define ISALNUM(x) isalnum((x), std::locale("Russian_Russia.1251"))
  #define END -1

  std::wstring buf = ole->GetTextFromRange(Range(0, END));

  range.Start = range.End = ole->SelRange.Start;

  while (ISALNUM(buf[range.Start - 1]) && range.Start - 1 >= 0)
    range.Start--;
  while (ISALNUM(buf[range.End]) && range.End < (long)buf.size())
    range.End++;
}

bool RichEditSpell::IsCorrect()
{
  return (ole->GetSelColor() != clRed);
}

void RichEditSpell::MarkAsMisspell(Range range)
{
  _component->Tag = ONCHANGE_BLOCK;
  ole->TextRange = range;
  ole->SetTextColor(clRed);
  _component->Tag = ONCHANGE_ALLOW;
}

void RichEditSpell::UnmarkAsMisspell(Range range)
{
  _component->Tag = ONCHANGE_BLOCK;
  ole->TextRange = range;
  ole->SetTextColor(tomAutoColor);
  _component->Tag = ONCHANGE_ALLOW;
}

void RichEditSpell::PerformSpell(Range range)
{
  if (range.Length() == 0)
    return;

  _speller.CheckText(ole->GetTextFromRange(range));

  Range word;
  for (unsigned i = 0; i < _speller.Result.size(); ++i)
  {
    word.Start = _speller.Result[i].pos + range.Start;
    word.End   = _speller.Result[i].len;
    MarkAsMisspell(word);
  }
}

std::vector<std::wstring>* RichEditSpell::GetSuggestions(int pos)
{
  for (unsigned i = 0; i < _speller.Result.size(); ++i)
  {
    if (_speller.Result[i].pos == pos)
      return &_speller.Result[i].s;
  }

  return NULL;
}
