#include "CommonEditSpell.h"

void CommonEditSpell::MarkAsMistake(int Start, int Length)
{
  mistakes[Start] = Length;
}

void CommonEditSpell::UnmarkAsMistake(int Start, int Length)
{
  mistakes.erase(Start);
}

bool CommonEditSpell::IsMistakeUnderCursor()
{
  TextRange temp = WordBounds(Component->SelStart);

  return (mistakes.find(temp.StartPos) != mistakes.end());
}
