#include "CustomMemoSpell.h"

CustomMemoSpell::CustomMemoSpell(TCustomMemo* Component) : CustomEditSpell(Component)
{
  _object = Component;
}

void CustomMemoSpell::CustomBeginUpdate()
{
  _current_pos = _object->SelStart;
  _object->Lines->BeginUpdate();
}

void CustomMemoSpell::CustomEndUpdate()
{
  _object->SelStart = _current_pos;
  _object->Lines->EndUpdate();
}