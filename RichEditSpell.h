#ifndef RichEditSpellH
#define RichEditSpellH

#include <vcl.h>
#include <locale>

#include "YandexSpeller.h"
#include "ORichEdit.h"

class RichEditSpell
{
  public:
    RichEditSpell(TForm* Form, TRichEdit* Component);
    ~RichEditSpell();

    void FindTextRange(Range& range);

    int FindWordStart();
    int FindWordEnd();

    bool IsCorrect();
    void MarkAsMisspell(Range range);
    void UnmarkAsMisspell(Range range);
    void PerformSpell(Range range);

    std::vector<std::wstring>& GetSuggestions(int pos);

    ORichEdit* ole;

  private:
    TForm*         _mainform;
    TRichEdit*     _component;
	  YandexSpeller  _speller;
};

#endif