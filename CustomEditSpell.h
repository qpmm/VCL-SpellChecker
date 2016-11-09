#ifndef CustomEditSpellH
#define CustomEditSpellH

#include <vcl.h>
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

class CustomEditSpell
{
  public:
    CustomEditSpell(TForm* Form, TCustomEdit* Component);
    ~CustomEditSpell();

    bool CheckRange(TextRange& Range);
    void FindTextRange(TextRange& Range);
    virtual std::wstring ToStdString(TextRange Range = TextRange(0, -1));
    
    virtual bool IsCorrect(int Pos);
    virtual void MarkAsMisspell(TextRange Range);
    virtual void UnmarkAsMisspell(TextRange Range);
    virtual void CustomBeginUpdate();
    virtual void CustomEndUpdate();
    virtual void PerformSpell(TextRange Range);
    virtual void NotifyMisspell();

  protected:
    TForm*               _mainform;
    YandexSpeller        _speller;
    TextRange            _current_sel;
    
  private:
    TCustomEdit*         _component;
    std::map<int, int>*  _misspell_pool;
    TBalloonHint*        _misspell_hint;
};

#endif