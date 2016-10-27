#include "RichEditSpell.h"

RichEditSpell::RichEditSpell(TForm* Form, TRichEdit* Component) : CustomMemoSpell(Form, Component)
{
  _mainform = Form;
  _component = Component;
}
    
bool RichEditSpell::IsMisspell(int Pos)
{
  return (_component->SelAttributes->Color == clRed);
}

void RichEditSpell::MarkAsMisspell(TextRange Range)
{
  _component->SelStart  = Range.StartPos;
  _component->SelLength = Range.Length;
  _component->SelAttributes->Color = clRed;
}

void RichEditSpell::UnmarkAsMisspell(TextRange Range)
{
  _component->SelStart  = Range.StartPos;
  _component->SelLength = Range.Length;
  _component->SelAttributes->Color = clBlack;
}

void RichEditSpell::CustomEndUpdate()
{
  _component->SelStart  = _current_sel.StartPos;
  //_component->SelLength = _current_sel.Length;
  _component->SelAttributes->Color = clBlack;
  _component->Modified = false;
  //_component->Lines->EndUpdate();
}

void RichEditSpell::NotifyMisspell()
{
  return;
}
