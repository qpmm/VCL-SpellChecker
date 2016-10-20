#ifndef COMMONEDITSPELLH
#define COMMONEDITSPELLH

#include "CustomEditSpell.h"
#include <map>

class CommonEditSpell : public CustomEditSpell
{
  public:
    CommonEditSpell(TCustomEdit* Sender) : CustomEditSpell(Sender) {};
    
    void MarkAsMistake(int Start, int Length);
    void UnmarkAsMistake(int Start, int Length);
    bool IsMistakeUnderCursor();
  
  private:
    std::map<int, int> mistakes;
};

#endif