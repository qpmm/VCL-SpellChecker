#ifndef RichEditSpellH
#define RichEditSpellH

#include <vcl.h>
#include <richole.h>
#include <tom.h>
#include <map>
#include <locale>
#include "YandexSpeller.h"

class TextRange
{
  public:
    TextRange();
    TextRange(int Pos, int Len);

    int EndPos();

    int StartPos;
    int Length;
};

struct RichEditOLE
{
  IUnknown*      intf;
  ITextDocument* text;
  ITextRange*    range;
  ITextFont*     style;
};

class RichEditSpell
{
  public:
    RichEditSpell(TForm* Form, TRichEdit* Component);
    ~RichEditSpell();

    bool CheckRange(TextRange& Range);
	void FindTextRange(TextRange& Range);
	std::wstring ToStdStr();
	std::wstring SubStr(TextRange Range);

	bool IsCorrect();
	void SetStyle(TextRange& Range, long Color);
    void MarkAsMisspell(TextRange Range);
	void UnmarkAsMisspell(TextRange Range);
    void PerformSpell(TextRange Range);

  private:
    TForm*         _mainform;
    TRichEdit*     _component;
    RichEditOLE    _ole;
	YandexSpeller  _speller;
};

#endif