#ifndef UntitledH
#define UntitledH

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
  int          CursorPos;
  WORD         RawKey;
  wchar_t      CharKey;
  CurrentWord  Word;
};

struct EventHandlers
{
  TKeyEvent       OnKeyDown;
  TKeyEvent       OnKeyUp;
  TKeyPressEvent  OnKeyPress;
  TNotifyEvent    OnChange;
  TMouseEvent     OnMouseDown;
  TNotifyEvent    OnExit;
};

class RichEditSetup
{
  public:
    RichEditSetup();
    ~RichEditSetup();

    void Init(TRichEdit* edit);
    void Disable();

    void __fastcall OnKeyDownWrapper  (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyUpWrapper    (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyPressWrapper (TObject* Sender, wchar_t& Key);
    void __fastcall OnChangeWrapper   (TObject* Sender);
    void __fastcall OnMouseDownWrapper(TObject* Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OnExitWrapper     (TObject* Sender);

    void __fastcall OnMenuItemClick   (TObject* Sender);

    void CreateContextMenu(Range& word, std::vector<std::wstring>& suggs);
    bool OnlySelectionChanged(TShiftState& shift);
    void UpdateCurrentWord();

  private:
    TRichEdit*         component;
    RichEditSpell*     richspell;
    EventHandlers      handlers;
    PreOnChangeValues  values;
};

#endif
