#ifndef CUSTOMEDITSPELLH
#define CUSTOMEDITSPELLH

#include <vcl.h>
#include <locale>
#include <string>
#include "speller.h"

class TextRange
{
  public:
    TextRange();
    TextRange(int pos, int len);

  int StartPos;
  int Length;
  bool IsMisspell;
};

class CustomEditSpell
{
  public:
    virtual void MarkAsMistake(int Start, int Length) = 0;
    virtual void UnmarkAsMistake(int Start, int Length) = 0;
    virtual bool IsMistakeUnderCursor() = 0;

    virtual void PerformSpell(std::wstring SubString, int Start);
    virtual TextRange WordBounds(int Pos);

    std::wstring GetText();

    TCustomEdit* Component;
  
  protected:
    CustomEditSpell(TCustomEdit* Sender);
    
    //TCustomEdit* Component;
    YandexSpeller speller;
  
  private:
};

#endif