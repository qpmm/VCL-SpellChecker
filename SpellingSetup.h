#ifndef SpellingSetupH
#define SpellingSetupH

#include <vcl.h>
#include <locale>
#include "RichEditSpell.h"
#include "ORichEdit.h"

struct CurrentWord
{
  Range  Bounds;
  bool   IsCorrect;
};

struct OnKeyDownValues
{
  CurrentWord Word;
  int         TextPos;
  WORD        RawKey;
  wchar_t     CharKey;
};

struct EventHandlers
{
  TKeyEvent         OnKeyDown;
  TKeyEvent         OnKeyUp;
  TKeyPressEvent    OnKeyPress;
  TNotifyEvent      OnChange;
  TMouseEvent       OnMouseUp;
  TNotifyEvent      OnExit;
};

class SpellingSetup
{
  public:
    SpellingSetup();
    ~SpellingSetup();

    void Init(TForm* form, TRichEdit* component);
    void Cleanup();

    void __fastcall OnKeyDownWrapper (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyUpWrapper   (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyPressWrapper(TObject* Sender, wchar_t& Key);
    void __fastcall OnChangeWrapper  (TObject* Sender);
    void __fastcall OnMouseUpWrapper (TObject* Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OnExitWrapper    (TObject* Sender);
    void __fastcall OnMenuItemClick  (TObject* Sender);

    void UpdateCurrentWord();
    
  private:
    TRichEdit*       _component;
    RichEditSpell*   _wrapper;
    EventHandlers    _handlers;
    OnKeyDownValues  _keydown;
};

#endif
