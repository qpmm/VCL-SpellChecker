#ifndef SpellingSetupH
#define SpellingSetupH

#include <vcl.h>
#include <locale>
#include "RichEditSpell.h"
#include "ORichEdit.h"
#include "defines.h"

struct CurrentWord
{
  Range  Bounds;
  bool   IsCorrect;
};

struct PreOnChangeValues
{
  int         CursorPos;
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

    void Init(TForm* form, TRichEdit* component);
    void Disable();

    void __fastcall OnKeyDownWrapper  (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyUpWrapper    (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyPressWrapper (TObject* Sender, wchar_t& Key);
    void __fastcall OnChangeWrapper   (TObject* Sender);
    void __fastcall OnMouseDownWrapper(TObject* Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OnExitWrapper     (TObject* Sender);

    void __fastcall OnMenuItemClick   (TObject* Sender);

    bool OnlySelectionChanged(TShiftState& shift);
    void UpdateCurrentWord();
    
  private:
    TForm*             _mainform;
    TRichEdit*         _component;
    RichEditSpell*     _richspell;
    EventHandlers      _handlers;
    PreOnChangeValues  _values;
};

#endif
