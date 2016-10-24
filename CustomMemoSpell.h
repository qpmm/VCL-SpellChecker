#include "CustomEditSpell.h"

class CustomMemoSpell : CustomEditSpell
{
  public:
    CustomMemoSpell(TCustomMemo* Component);
    
    /*virtual bool IsMisspell();
    virtual void MarkAsMisspell(TextRange Range);
    virtual void UnmarkAsMisspell(TextRange Range);*/
    virtual void PerformSpell(TextRange Range);
    //virtual void NotifyMisspell();
    
  private:
    TCustomMemo* _object;
};