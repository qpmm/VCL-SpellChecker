#ifndef SPELLCOMMONEDITH
#define SPELLCOMMONEDITH

#include "CustomSpellEdit.h"
#include <map>

class SpellCommonEdit : public CustomSpellEdit
{
  public:
    SpellCommonEdit(TCustomEdit* Sender) : CustomSpellEdit(Sender) {};
    
    void MarkAsMistake(int Start, int Length);
    void UnmarkAsMistake(int Start, int Length);
    bool IsMistakeUnderCursor();
  
  private:
    std::map<int, int> mistakes;
};

#endif