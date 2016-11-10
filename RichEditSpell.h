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

    std::wstring ToStdString();
    std::wstring ToSubString(TextRange Range);
    void SetStyle(TextRange& Range, long Color);
    
    bool IsCorrect(int Pos);
    void MarkAsMisspell(TextRange Range);
    void UnmarkAsMisspell(TextRange Range);
    void NotifyMisspell();
  
  private:
    TRichEdit*   _component;
    RichEditOLE  _ole;
};

#endif