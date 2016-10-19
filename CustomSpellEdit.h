#ifndef CUSTOMSPELLEDITH
#define CUSTOMSPELLEDITH

#include <vcl.h>
#include <locale>
#include <string>
#include "speller.h"

class TextRange
{
  TextRange();
  TextRange();
};

class CustomSpellEdit
{
  public:
    struct TextFragment
    {
      TextFragment() { StartPos = -1; Length = -1; IsMisspell = false; }

      int StartPos;
      int Length;
      bool IsMisspell;
    };

    virtual void MarkAsMistake(int Start, int Length) = 0;
    virtual void UnmarkAsMistake(int Start, int Length) = 0;
    virtual bool IsMistakeUnderCursor() = 0;
    virtual TextFragment WordBounds(int Pos);

    // virtual bool isDelim();
    // richedit -> EM_WORDBREAK

    std::wstring GetText();
    void PerformSpell(std::wstring SubString, int Start);
  
  protected:
    CustomSpellEdit(TCustomEdit* Sender);
    
    TCustomEdit* edit;
    YandexSpeller speller;
  
  private:
};

#endif