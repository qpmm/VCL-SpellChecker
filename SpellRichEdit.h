#ifndef SPELLRICHEDITH
#define SPELLRICHEDITH

#include "CustomSpellEdit.h"

class SpellRichEdit : public CustomSpellEdit
{
  public:
    SpellRichEdit(TCustomEdit* Sender) : CustomSpellEdit(Sender) {};

    //void WordBounds(int Pos, int& Start, int& Length);
    void MarkAsMistake(int Start, int Length);
    void UnmarkAsMistake(int Start, int Length);
    bool IsMistakeUnderCursor();

  private:
    TRichEdit* richedit();
};

#endif