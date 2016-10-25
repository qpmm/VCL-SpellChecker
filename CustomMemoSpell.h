#ifndef CustomMemoSpellH
#define CustomMemoSpellH

#include "CustomEditSpell.h"

class CustomMemoSpell : public CustomEditSpell
{
  public:
    CustomMemoSpell(TCustomMemo* Component);
    
    virtual void CustomBeginUpdate();
    virtual void CustomEndUpdate();
    
  private:
    TCustomMemo* _object;
};

#endif