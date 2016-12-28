#ifndef SpellingSetupH
#define SpellingSetupH

#include <vcl.h>
#include "RichEditSpell.h"
#include "ORichEdit.h"
#include "defines.h"

struct CurrentWord
{
  NumRange  Bounds;
  bool   IsCorrect;
};

struct PreOnChangeValues
{
  NumRange    SelRange;
  WORD        RawKey;
  wchar_t     CharKey;
  CurrentWord Word;
};

struct EventHandlers
{
  TKeyEvent         OnKeyDown;
  TKeyEvent         OnKeyUp;
  TKeyPressEvent    OnKeyPress;
  TNotifyEvent      OnChange;
  TMouseEvent       OnMouseDown;
  TNotifyEvent      OnExit;
};

class SpellingSetup
{
  public:
    SpellingSetup();
    ~SpellingSetup();

    void Init(TRichEdit* Component);
    void Disable();

    void SafeActivate(TRichEdit* Component);

    void __fastcall OnKeyDownWrapper  (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyUpWrapper    (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyPressWrapper (TObject* Sender, wchar_t& Key);
    void __fastcall OnChangeWrapper   (TObject* Sender);
    void __fastcall OnMouseDownWrapper(TObject* Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OnExitWrapper     (TObject* Sender);

    bool OnlySelectionChanged(TShiftState& shift);
    void UpdateCurrentWord();
    
  private:
    TRichEdit*         component;
    RichEditSpell*     richspell;
    EventHandlers      handlers;
    PreOnChangeValues  values;
};

#endif
