#include "RichEditSpell.h"

RichEditSpell::RichEditSpell(TForm* Form, TRichEdit* Component) : CustomMemoSpell(Form, Component)
{
  _mainform = Form;
  _component = Component;

  _component->Perform(EM_GETOLEINTERFACE, 0, (int)&_ole);
  _ole->QueryInterface(__uuidof(ITextDocument), (void**)&_text);
  _ole->QueryInterface(__uuidof(ITextFont),     (void**)&_style);
}
    
bool RichEditSpell::IsMisspell(int Pos)
{
  bool result;

  CustomBeginUpdate();

  _component->SelStart += 1;
  result = (_component->SelAttributes->Color == clRed);

  CustomEndUpdate();

  return result;
}

void RichEditSpell::MarkAsMisspell(TextRange Range)
{
  _component->SelStart  = Range.StartPos;
  _component->SelLength = Range.Length;
  _component->Tag = 1;
  _component->SelAttributes->Color = clRed;
  _component->Tag = 0;
}

void RichEditSpell::UnmarkAsMisspell(TextRange Range)
{
  CustomBeginUpdate();

  _component->SelStart  = Range.StartPos;
  _component->SelLength = Range.Length;
  _component->Tag = 1;
  _component->SelAttributes->Color = clBlack;
  _component->Tag = 0;

  CustomEndUpdate();
}

void RichEditSpell::CustomEndUpdate()
{
  _component->SelStart = _current_sel.StartPos;
  //_component->SelLength = _current_sel.Length;
  _component->Tag = 1;
  _component->SelAttributes->Color = clBlack;
  _component->Tag = 0;
  _component->Lines->EndUpdate();
}

void RichEditSpell::NotifyMisspell()
{
  return;
}
