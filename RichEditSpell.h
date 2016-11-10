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
    std::wstring ToStdString();
    std::wstring ToSubString(TextRange Range);

    void SetStyle(TextRange& Range, long Color);
    bool IsCorrect(int Pos);
    void MarkAsMisspell(TextRange Range);
    void UnmarkAsMisspell(TextRange Range);
    void CustomBeginUpdate();
    void CustomEndUpdate();
    void PerformSpell(TextRange Range);

  private:
    TForm*         _mainform;
    TRichEdit*     _component;
    RichEditOLE    _ole;

    YandexSpeller  _speller;
    TextRange      _current_sel;
};

#endif