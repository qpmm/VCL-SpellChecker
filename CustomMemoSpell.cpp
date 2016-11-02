#include "CustomMemoSpell.h"

CustomMemoSpell::CustomMemoSpell(TForm* Form, TCustomMemo* Component) : CustomEditSpell(Form, Component)
{
  _mainform = Form;
  _component = Component;
}

void CustomMemoSpell::CustomBeginUpdate()
{
  _current_sel.StartPos = _component->SelStart;
  _current_sel.Length   = _component->SelLength;
  _component->Lines->BeginUpdate();
}

void CustomMemoSpell::CustomEndUpdate()
{
  _component->SelStart  = _current_sel.StartPos;
  _component->SelLength = _current_sel.Length;
  _component->Lines->EndUpdate();
}

std::wstring CustomMemoSpell::ToStdString()
{
  CustomBeginUpdate();

  _component->SelectAll();
  std::wstring result = _component->SelText.c_str();

  CustomEndUpdate();

  return result;
}

std::wstring CustomMemoSpell::ToStdString(TextRange Range)
{
  CustomBeginUpdate();

  _component->SelStart = Range.StartPos;
  _component->SelLength = Range.Length;
  std::wstring result = _component->SelText.c_str();

  CustomEndUpdate();

  return result;
}