#include "RichEditSpell.h"

RichEditSpell::RichEditSpell(TRichEdit* Component)
{
  _object = Component;
}
    
bool RichEditSpell::IsMisspell()
{
  return (_object->SelAttributes->Color == clRed);
}

void RichEditSpell::MarkAsMisspell(TextRange Range)
{
  _object->SelStart = Range.StartPos;
  _object->SelLength = Range.Length;
  _object->SelAttributes->Color = clRed;

  _object->SelStart = CurPos;
  _object->SelAttributes->Color = clBlack;
}

void RichEditSpell::UnmarkAsMisspell(TextRange Range)

void RichEditSpell::PerformSpell(TextRange Range)

void RichEditSpell::NotifyMisspell()
