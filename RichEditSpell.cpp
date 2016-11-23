#include "RichEditSpell.h"

RichEditSpell::RichEditSpell(TRichEdit* component)
{
  _component = component;
  ole = new ORichEdit(component);
}

RichEditSpell::~RichEditSpell()
{
  delete ole;
}

void RichEditSpell::FindTextRange(Range& range)
{
  range = FindTextRange(ole->SelRange.Start);

//  int s = range.Start = range.End = ole->SelRange.Start;
//
//  bool delimToLeft  = _component->Perform(EM_FINDWORDBREAK, WB_ISDELIMITER, s - 1);
//  bool delimToRight = _component->Perform(EM_FINDWORDBREAK, WB_ISDELIMITER, s);
//
//  if (!delimToLeft)
//  {
//    range.Start = _component->Perform(EM_FINDWORDBREAK, WB_LEFT, s);
//  }
//
//  if (!delimToRight)
//  {
//    s = _component->Perform(EM_FINDWORDBREAK, WB_RIGHT, s);
//    range.End = _component->Perform(EM_FINDWORDBREAK, WB_LEFT, s);
//  }
}

Range RichEditSpell::FindTextRange(int pos)
{
  int start = pos, end = pos;

  std::wstring buf = ole->GetTextFromRange(ole->GetTextBounds());

  while (ISALNUM(buf[start - 1]) && start - 1 >= 0)
    start--;
  while (ISALNUM(buf[end]) && end < (long)buf.size())
    end++;

  return Range(start, end);
}

int RichEditSpell::FindWordStart()
{
  //_component->Perform(EM_FINDWORDBREAK, WB_LEFT, _component->SelStart);
  return 0;
}

int RichEditSpell::FindWordEnd()
{
  return 0;
}

bool RichEditSpell::IsCorrect(int pos)
{
  if (pos != CURRENT_POS)
  {
    ole->SetTextRange(Range(pos, pos));
    return (ole->GetTextColor() != clRed);
  }

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
    word.End   = _speller.Result[i].len + word.Start;
    MarkAsMisspell(word);
  }
}

std::vector<std::wstring>& RichEditSpell::GetSuggestions(int pos)
{
  return _speller.Result[0].s;
}
