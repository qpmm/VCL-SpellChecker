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
    bool operator==(TextRange rvl);

  int StartPos;
  int Length;
};

class CustomEditSpell
{
  public:
    CustomEditSpell(TForm* Form, TCustomEdit* Component);
    ~CustomEditSpell();

    TextRange FindTextRange(); // Попробовать сделать виртуальной (для RichEdit)
    virtual std::wstring ToStdString();
    virtual std::wstring ToStdString(TextRange Range);
    
    virtual bool IsMisspell(int Pos);
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