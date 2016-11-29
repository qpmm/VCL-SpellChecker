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
  OnChangeBlocked = true;
  ole->TextRange = range;
  ole->SetTextColor(clRed);
  OnChangeBlocked = false;
}

void RichEditSpell::UnmarkAsMisspell(Range range)
{
  OnChangeBlocked = true;
  ole->TextRange = range;
  ole->SetTextColor(tomAutoColor);
  OnChangeBlocked = false;
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
