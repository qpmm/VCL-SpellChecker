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
}

void RichEditSpell::UnmarkAsMisspell(TextRange Range)

void RichEditSpell::CustomBeginUpdate()
{
  _object->Lines->BeginUpdate();
}

void RichEditSpell::CustomEndUpdate()
{
  _object->SelStart = _current_pos;
  // _object->SelAttributes->Color = clBlack;
  _object->Lines->EndUpdate();
}

void RichEditSpell::PerformSpell(TextRange Range)

void RichEditSpell::NotifyMisspell()
