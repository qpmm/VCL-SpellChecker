#ifndef CustomMemoSpellH
#define CustomMemoSpellH

#include "CustomEditSpell.h"

class CustomMemoSpell : public CustomEditSpell
{
  public:
    CustomMemoSpell(TForm* Form, TCustomMemo* Component);

    virtual void CustomBeginUpdate();
    virtual void CustomEndUpdate();

    virtual std::wstring ToStdString();
    
  private:
    TCustomMemo* _object;
};

#endif