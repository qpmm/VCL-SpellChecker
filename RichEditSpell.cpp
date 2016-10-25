#include "RichEditSpell.h"

RichEditSpell::RichEditSpell(TRichEdit* Component) : CustomMemoSpell(Component)
{
  _object = Component;
}
    
bool RichEditSpell::IsMisspell(int Pos)
{
  return (_object->SelAttributes->Color == clRed);
}

void RichEditSpell::MarkAsMisspell(TextRange Range)
{
  _object->SelStart  = Range.StartPos;
  _object->SelLength = Range.Length;
  _object->SelAttributes->Color = clRed;
}

void RichEditSpell::UnmarkAsMisspell(TextRange Range)
{
  _object->SelStart  = Range.StartPos;
  _object->SelLength = Range.Length;
  _object->SelAttributes->Color = clBlack;
}

void RichEditSpell::CustomEndUpdate()
{
  _object->SelStart = _current_pos;
  // _object->SelAttributes->Color = clBlack;
  _object->Modified = false;
  //_object->Lines->EndUpdate();
}

void RichEditSpell::NotifyMisspell()
{
  return;
}
