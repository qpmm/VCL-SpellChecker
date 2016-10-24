#include "CustomMemoSpell.h"

class RichEditSpell : CustomMemoSpell
{
  public:
    RichEditSpell(TRichEdit* Component);
    
    bool IsMisspell();
    void MarkAsMisspell(TextRange Range);
    void UnmarkAsMisspell(TextRange Range);
    void PerformSpell(TextRange Range);
    void NotifyMisspell();
  
  private:
    TRichEdit* _object;
};