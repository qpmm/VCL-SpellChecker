#ifndef CustomMemoSpellH
#define CustomMemoSpellH

#include "CustomEditSpell.h"

class CustomMemoSpell : protected CustomEditSpell
{
  public:
    CustomMemoSpell(TCustomMemo* Component);
    
    virtual void CustomBeginUpdate();
    virtual void CustomEndUpdate();
    
  private:
    TCustomMemo* _object;
};

#endif