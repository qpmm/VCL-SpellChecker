#ifndef RichEditSpellH
#define RichEditSpellH

#include "CustomMemoSpell.h"
#include <richole.h>
#include <tom.h>

struct RichEditOLE
{
  IUnknown*      intf;
  ITextDocument* text;
  ITextRange*    range;
  ITextFont*     style;
};

class RichEditSpell : public CustomMemoSpell
{
  public:
    RichEditSpell(TForm* Form, TRichEdit* Component);

    int GetLength();
    std::wstring ToStdString();
    void SetStyle(TextRange& Range, long Color);
    
    bool IsMisspell(int Pos);
    void MarkAsMisspell(TextRange Range);
    void UnmarkAsMisspell(TextRange Range);
    void NotifyMisspell();
  
  private:
    TRichEdit*   _component;
    RichEditOLE  _ole;
};

#endif