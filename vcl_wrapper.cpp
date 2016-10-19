//#ifndef VCL_WRAPPERH
//#define VCL_WRAPPERH

#include <vcl.h>
#include <locale>
#include "CustomSpellEdit.h"
#include "SpellCommonEdit.h"
#include "SpellRichEdit.h"

//#define isalnum(x) isalnum((x), std::locale("Russian_Russia.1251"))

template <typename ComponentType>
class SpellingSetup
{
  public:
    SpellingSetup(ComponentType* Component);
    ~SpellingSetup();

    //ComponentType* CompSpeller;

    void __fastcall OnKeyDownWrapper(TObject* Sender, WORD &Key, TShiftState Shift);
    void __fastcall OnKeyPressWrapper(TObject *Sender, wchar_t &Key);
    void __fastcall OnChangeWrapper(TObject* Sender);

  private:
    CustomSpellEdit* CompSpeller;
    CustomSpellEdit::TextFragment TextState;
    TShiftState ShiftState;
    wchar_t PressedKey;
    int CursorPos;

};

/************** IMPLEMENTATION **************/

template <typename ComponentType>
SpellingSetup<ComponentType>::SpellingSetup(ComponentType* Component)
{
  Component->OnKeyDown = OnKeyDownWrapper;
  Component->OnKeyPress = OnKeyPressWrapper;
  Component->OnChange = OnChangeWrapper;

  if (__classid(ComponentType) == __classid(TRichEdit))
    CompSpeller = new SpellRichEdit(Component);
  else
    CompSpeller = new SpellCommonEdit(Component);
}

template <typename ComponentType>
SpellingSetup<ComponentType>::~SpellingSetup()
{
  delete CompSpeller;
}

template <typename ComponentType>
void __fastcall SpellingSetup<ComponentType>::OnKeyDownWrapper(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if (isprint(Key, std::locale("Russian_Russia.1251")))
  {
    TextState = CompSpeller->WordBounds(Object->SelStart);
    TextState.IsMisspell = CompSpeller->IsMistakeUnderCursor();
    ShiftState = Shift;

    CursorPos = Object->SelStart;

    if (TextState.IsMisspell)
      CompSpeller->UnmarkAsMistake(TextState.StartPos, TextState.Length);
  }
}

template <typename ComponentType>
void __fastcall SpellingSetup<ComponentType>::OnKeyPressWrapper(TObject *Sender, wchar_t &Key)
{
  PressedKey = Key;
}

template <typename ComponentType>
void __fastcall SpellingSetup<ComponentType>::OnChangeWrapper(TObject* Sender)
{
  int NewCursorPos = Object->SelStart;
  int Length;// = NewCursorPos - TextState.CursorPos + TextState.Length;

  if (TextState.IsMisspell || (NewCursorPos - CursorPos == 1 && !isalnum(PressedKey, std::locale("Russian_Russia.1251"))))
  {
    // Длина нового фрагмента + длина слова, в которое он вошёл
    Length = NewCursorPos - CursorPos + TextState.Length;
    //CompSpeller->PerformSpell(CompSpeller->GetText().substr(TextState.StartPos, Length), TextState.StartPos);
  }
}

//#endif