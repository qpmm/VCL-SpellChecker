#include <vcl.h>
#include <map>
#include "YandexSpeller.h"

class TextRange
{
  public:
    TextRange();
    TextRange(int Pos, int Len);

  int StartPos;
  int Length;
  bool IsMisspell;
};

class CustomEditSpell
{
  public:
    CustomEditSpell(TCustomEdit* Component);
    ~CustomEditSpell();
    
    // Попробовать сделать виртуальной (для RichEdit)
    TextRange FindTextRange();
    std::wstring ToStdString();
    std::wstring ToStdString(TextRange Range);
    
    virtual bool IsMisspell();
    virtual void MarkAsMisspell(TextRange Range);
    virtual void UnmarkAsMisspell(TextRange Range);
    virtual void CustomBeginUpdate();
    virtual void CustomEndUpdate();
    virtual void PerformSpell(TextRange Range);
    virtual void NotifyMisspell();
  
  protected:
    YandexSpeller        _speller;
    int                  _current_pos;
    
  private:
    TCustomEdit*         _object;
    std::map<int, int>*  _misspell_pool;
    TBalloonHint*        _misspell_hint;
};