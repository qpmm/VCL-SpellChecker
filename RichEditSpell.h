#ifndef RichEditSpellH
#define RichEditSpellH

#include "CustomMemoSpell.h"
#include <richole.h>
#include <tom.h>

class RichEditSpell : public CustomMemoSpell
{
  public:
    RichEditSpell(TForm* Form, TRichEdit* Component);
    
    bool IsMisspell(int Pos);
    void MarkAsMisspell(TextRange Range);
    void UnmarkAsMisspell(TextRange Range);
    void CustomEndUpdate();
    void NotifyMisspell();
  
  private:
    TRichEdit*     _component;
    ITextDocument* _text;
    ITextFont*     _style;
    ITextRange*    _seltext;
    IUnknown*      _ole;
};

#endif