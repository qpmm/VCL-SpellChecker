//#ifndef VCL_WRAPPERH
//#define VCL_WRAPPERH

#include <vcl.h>
#include <locale>
#include "CustomEditSpell.h"
#include "CommonEditSpell.h"
#include "RichEditSpell.h"

//#define isalnum(x) isalnum((x), std::locale("Russian_Russia.1251"))

template <typename ComponentType>
class SpellingSetup
{
  public:
    SpellingSetup(ComponentType* Component);
    ~SpellingSetup();

    void __fastcall OnKeyDownWrapper(TObject* Sender, WORD &Key, TShiftState Shift);
    void __fastcall OnKeyPressWrapper(TObject *Sender, wchar_t &Key);
    void __fastcall OnChangeWrapper(TObject* Sender);

  private:
    CustomEditSpell* ComponentSpeller;
    //TShiftState ShiftState;
    TextRange TextState;
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
    ComponentSpeller = new RichEditSpell(Component);
  else
    ComponentSpeller = new CommonEditSpell(Component);
}

template <typename ComponentType>
SpellingSetup<ComponentType>::~SpellingSetup()
{
  delete ComponentSpeller;
}

template <typename ComponentType>
void __fastcall SpellingSetup<ComponentType>::OnKeyDownWrapper(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if (isprint(Key, std::locale("Russian_Russia.1251")))
  {
    TextState = ComponentSpeller->WordBounds(ComponentSpeller->Component->SelStart);
    TextState.IsMisspell = ComponentSpeller->IsMistakeUnderCursor();
    //ShiftState = Shift;

    CursorPos = ComponentSpeller->Component->SelStart;

    if (TextState.IsMisspell)
      ComponentSpeller->UnmarkAsMistake(TextState.StartPos, TextState.Length);
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
  int NewCursorPos = ComponentSpeller->Component->SelStart;

  if (!ComponentSpeller->Component->Modified)
    return;

  if (TextState.IsMisspell || (NewCursorPos - CursorPos == 1 && !isalnum(PressedKey, std::locale("Russian_Russia.1251"))))
  {
    // Длина нового фрагмента + длина слова, в которое он вошёл
    int Length = NewCursorPos - CursorPos + TextState.Length;
    ComponentSpeller->PerformSpell(ComponentSpeller->GetText().substr(TextState.StartPos, Length), TextState.StartPos);
  }
}

//#endif