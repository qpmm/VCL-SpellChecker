#include "SpellCommonEdit.h"

void SpellCommonEdit::MarkAsMistake(int Start, int Length)
{
  mistakes[Start] = Length;
}

void SpellCommonEdit::UnmarkAsMistake(int Start, int Length)
{
  mistakes.erase(Start);
}

bool SpellCommonEdit::IsMistakeUnderCursor()
{
  CustomSpellEdit::TextFragment temp = WordBounds(edit->SelStart);

  return (mistakes.find(temp.StartPos) != mistakes.end());
}
