#ifndef RICHEDITSPELLH
#define RICHEDITSPELLH

#include "CustomEditSpell.h"

class RichEditSpell : public CustomEditSpell
{
  public:
    RichEditSpell(TCustomEdit* Sender) : CustomEditSpell(Sender) {};

    //void WordBounds(int Pos, int& Start, int& Length);
    void PerformSpell(std::wstring SubString, int Start);
    void MarkAsMistake(int Start, int Length);
    void UnmarkAsMistake(int Start, int Length);
    bool IsMistakeUnderCursor();

  private:
    TRichEdit* richedit();
};

#endif