#ifndef SpellingSetupH
#define SpellingSetupH

#include <vcl.h>
#include <locale>
#include "CustomEditSpell.h"
#include "CustomMemoSpell.h"
#include "RichEditSpell.h"

struct OnKeyDownInfo
{
  int CursorPos;
  TextRange Range;
  wchar_t PressedKey;
  bool Misspelled;
};

class SpellingSetup
{
  public:
    SpellingSetup();
    ~SpellingSetup();
    
    void Init(TCustomEdit* Component);
    void __fastcall OnKeyDownWrapper(TObject* Sender, WORD &Key, TShiftState Shift);
    void __fastcall OnKeyPressWrapper(TObject *Sender, wchar_t &Key);
    void __fastcall OnChangeWrapper(TObject* Sender);
    
  private:
    TCustomEdit*      _object;
    CustomEditSpell*  _object_speller;
    OnKeyDownInfo     _word_state;
};

#endif