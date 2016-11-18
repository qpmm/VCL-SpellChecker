#ifndef RichEditSpellH
#define RichEditSpellH

#include <vcl.h>
#include <locale>

#include "YandexSpeller.h"
#include "ORichEdit.h"

class RichEditSpell
{
  public:
    RichEditSpell(TRichEdit* Component);
    ~RichEditSpell();

    void  FindTextRange(Range& range);
    Range FindTextRange(int pos);

    int FindWordStart();
    int FindWordEnd();

    bool IsCorrect(int pos = -1);
    void MarkAsMisspell(Range range);
    void UnmarkAsMisspell(Range range);
    void PerformSpell(Range range);

    std::vector<std::wstring>& GetSuggestions(int pos);

    ORichEdit* ole;

  private:
    TRichEdit*     _component;
	  YandexSpeller  _speller;
};

#endif