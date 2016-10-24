#include "CustomMemoSpell.h"

CustomMemoSpell::CustomMemoSpell(TCustomMemo* Component)
{
  _object = Component;
}

/*bool CustomMemoSpell::IsMisspell()

void CustomMemoSpell::MarkAsMisspell(TextRange Range)

void CustomMemoSpell::UnmarkAsMisspell(TextRange Range)*/

void CustomMemoSpell::PerformSpell(TextRange Range)
{
  int CurPos = _object->SelStart;
  
  _object->Lines->BeginUpdate();
  
  _speller->checkText(_object->ToStdString(Range));
  for (unsigned i = 0; i < _speller->Result.size(); ++i)
    MarkAsMisspell(Range.StartPos + _speller->Result[i].pos, _speller->Result[i].len);
  
  
  _object->Lines->EndUpdate();
}

//void CustomMemoSpell::NotifyMisspell()
