#include "CustomMemoSpell.h"

CustomMemoSpell::CustomMemoSpell(TForm* Form, TCustomMemo* Component) : CustomEditSpell(Form, Component)
{
  _mainform = Form;
  _object = Component;
}

void CustomMemoSpell::CustomBeginUpdate()
{
  _current_sel.StartPos = _object->SelStart;
  //_current_sel.Length   = _object->SelLength;
  //_object->Lines->BeginUpdate();
}

void CustomMemoSpell::CustomEndUpdate()
{
  _object->SelStart  = _current_sel.StartPos;
  //_object->SelLength = _current_sel.Length;
  //_object->Lines->EndUpdate();
}

std::wstring CustomMemoSpell::ToStdString()
{
  CustomBeginUpdate();

  _object->SelectAll();
  std::wstring result = _object->SelText.c_str();

  CustomEndUpdate();

  return result;
}