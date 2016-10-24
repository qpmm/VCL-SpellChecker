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
  CustomBeginUpdate();
  
  _speller->checkText(_object->ToStdString(Range));
  for (unsigned i = 0; i < _speller->Result.size(); ++i)
    MarkAsMisspell(Range.StartPos + _speller->Result[i].pos, _speller->Result[i].len);
  
  CustomEndUpdate();
}

//void CustomMemoSpell::NotifyMisspell()
