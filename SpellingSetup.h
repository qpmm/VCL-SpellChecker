#ifndef SpellingSetupH
#define SpellingSetupH

#include <vcl.h>
#include "RichEditSpell.h"

class SpellingSetup
{
  public:
    SpellingSetup();
    ~SpellingSetup();

    void Add(TRichEdit* component);
    
  private:
    std::vector<RichEditSetup*> setup_pool;
};

#endif
