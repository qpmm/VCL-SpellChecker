#include "SpellingSetup.h"

#include <Clipbrd.hpp>
#include <locale>

bool OnChangeBlocked;
bool PasteText;

SpellingSetup::SpellingSetup()
{
  component = NULL;
  richspell = NULL;
  memset(&handlers, 0, sizeof(handlers));
  memset(&values,   0, sizeof(values));
}

SpellingSetup::~SpellingSetup()
{
  Disable();
}

void SpellingSetup::Init(TRichEdit* Component)
{
  component = Component;
  richspell = new RichEditSpell(component);

  handlers.OnKeyDown    = component->OnKeyDown;
  handlers.OnKeyUp      = component->OnKeyUp;
  handlers.OnKeyPress   = component->OnKeyPress;
  handlers.OnChange     = component->OnChange;
  handlers.OnMouseDown  = component->OnMouseDown;
  handlers.OnExit       = component->OnExit;

  component->OnKeyDown    = OnKeyDownWrapper;
  component->OnKeyUp      = OnKeyUpWrapper;
  component->OnKeyPress   = OnKeyPressWrapper;
  component->OnChange     = OnChangeWrapper;
  component->OnMouseDown  = OnMouseDownWrapper;
  component->OnExit       = OnExitWrapper;

  richspell->PerformSpell(richspell->Ole->GetTextBounds());
  richspell->FindTextRange(values.Word.Bounds);
  values.Word.IsCorrect = richspell->IsCorrect();

  OnChangeBlocked = false;
}

void SpellingSetup::Disable()
{
  if (richspell)
  {
    delete richspell;
    richspell = NULL;
  }

  if (component)
  {
    component->OnKeyDown    = handlers.OnKeyDown;
    component->OnKeyUp      = handlers.OnKeyUp;
    component->OnKeyPress   = handlers.OnKeyPress;
    component->OnChange     = handlers.OnChange;
    component->OnMouseDown  = handlers.OnMouseDown;
    component->OnExit       = handlers.OnExit;

    component = NULL;
  }
}

void SpellingSetup::SafeActivate(TRichEdit* Component)
{
  if (richspell == NULL)
  {
    try
    {
      Init(Component);
    }
    catch (...)
    {
      TBalloonHint* warning = new TBalloonHint(Component);
      TPoint coord = Component->ClientRect.BottomRight();

      warning->Title = "Ошибка запуска проверки орфографии";
      warning->HideAfter = 5000;
      warning->ShowHint(Component->ClientToScreen(coord));

      Disable();
    }
  }
}

void __fastcall SpellingSetup::OnKeyDownWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  if (handlers.OnKeyDown)
    handlers.OnKeyDown(Sender, Key, Shift);

  if (   Key == VK_ESCAPE       || Key == VK_MENU           || Key == VK_APPS
      || Shift.Contains(ssCtrl) || Shift.Contains(ssShift))
    return;

  values.RawKey = Key;
  values.CharKey = L'\0';
  values.SelRange = richspell->Ole->SelRange;
  values.Word.IsCorrect = richspell->IsCorrect(values.SelRange.Start);

  if (values.SelRange.Length())
    richspell->FindTextRange(values.Word.Bounds);

  if (!values.Word.IsCorrect && !OnlySelectionChanged(Shift))
	  richspell->UnmarkAsMisspell(values.Word.Bounds);

  PasteText = Shift.Contains(ssCtrl) && Key == 'V' || Shift.Contains(ssShift) && Key == VK_INSERT;
  if (PasteText)
    Key = 0;

  OnChangeBlocked = true;
  component->SelAttributes->Color = (TColor)tomAutoColor;
  OnChangeBlocked = false;
}

void __fastcall SpellingSetup::OnKeyUpWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  if (handlers.OnKeyUp)
    handlers.OnKeyUp(Sender, Key, Shift);

  if (   Key == VK_ESCAPE       || Key == VK_MENU
      || Shift.Contains(ssCtrl) || Shift.Contains(ssShift))
    return;

  if (Key == VK_APPS)
  {
    int coord = component->Perform(EM_POSFROMCHAR, values.SelRange.Start, 0);
    OnMouseDownWrapper(Sender, mbRight, TShiftState(), LOWORD(coord), HIWORD(coord));
  }

  if (OnlySelectionChanged(Shift))
  {
    UpdateCurrentWord();
    return;
  }

  if (PasteText && Key != VK_TAB)
  {
    component->SelText = Clipboard()->AsText;
    Key = 0;
  }
}

void __fastcall SpellingSetup::OnKeyPressWrapper(TObject* Sender, wchar_t& Key)
{
  if (handlers.OnKeyPress)
    handlers.OnKeyPress(Sender, Key);

  values.CharKey = Key;
}

void __fastcall SpellingSetup::OnChangeWrapper(TObject* Sender)
{
  if (handlers.OnChange)
    handlers.OnChange(Sender);

  if (OnChangeBlocked || values.RawKey == 0)
  //if (OnChangeBlocked)
		return;

  int posDiff = richspell->Ole->SelRange.Start - values.SelRange.Start;

  NumRange newWord, spellRange;
  richspell->FindTextRange(newWord);

//  if (values.Word.Bounds.Length() == 0 && newWord.Length() == 0 && posDiff < 2)
//  {
//    values.Word.Bounds = newWord;
//    values.Word.IsCorrect = richspell->IsCorrect();
//    return;
//  }

  if (posDiff < 0)
  {
    spellRange = newWord;
  }
  else
  {
    spellRange.Start = values.Word.Bounds.Start;
    spellRange.End   = newWord.End;
  }

  if (values.Word.Bounds.Length() != 0)
    richspell->UnmarkAsMisspell(spellRange);

  if (ISDELIM(values.CharKey) || values.RawKey == VK_RETURN || posDiff > 1)
    richspell->PerformSpell(spellRange);

  values.Word.Bounds = newWord;
  values.Word.IsCorrect = richspell->IsCorrect();
  values.RawKey = 0;
  values.CharKey = L'\0';
}

void __fastcall SpellingSetup::OnMouseDownWrapper(TObject* Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (handlers.OnMouseDown)
    handlers.OnMouseDown(Sender, Button, Shift, X, Y);

  //UpdateCurrentWord();

  if (Button == mbLeft)
  {
    UpdateCurrentWord();
    return;
  }

  if (Button == mbRight)
  {
    int Pos = component->Perform(EM_CHARFROMPOS, 0, (int)&TPoint(X, Y));
    //TPoint Coord = (TPoint)component->Perform(EM_POSFROMCHAR, Pos, 0);

    NumRange word = richspell->FindTextRange(Pos);

    NumRange tmp = richspell->Ole->SelRange;
    if (tmp.Length() && (Pos < tmp.Start || Pos > tmp.End))
      richspell->Ole->SelRange = NumRange(Pos, Pos);

    bool hasSuggestions = !(word.Length() == 0
          || richspell->Ole->SelRange.Length() != 0
          || richspell->IsCorrect(word.Start + 1));

    if (hasSuggestions)
      richspell->PerformSpell(word);

    richspell->ContextMenu->Create(word, hasSuggestions, component->SelLength);
    richspell->ContextMenu->Show(X, Y);
  }
}

void __fastcall SpellingSetup::OnExitWrapper(TObject* Sender)
{
  if (handlers.OnExit)
    handlers.OnExit(Sender);

  richspell->PerformSpell(values.Word.Bounds);
}

bool SpellingSetup::OnlySelectionChanged(TShiftState& shift)
{
  return (
        values.RawKey == VK_END
    ||  values.RawKey == VK_HOME
    ||  values.RawKey == VK_LEFT
    ||  values.RawKey == VK_RIGHT
    ||  values.RawKey == VK_PRIOR
    ||  values.RawKey == VK_NEXT
    || (values.RawKey == VK_UP   && !shift.Contains(ssCtrl))
    || (values.RawKey == VK_DOWN && !shift.Contains(ssCtrl))
  );
}

void SpellingSetup::UpdateCurrentWord()
{
  NumRange newSel = richspell->Ole->SelRange;

  if (   richspell->IsCorrect(values.Word.Bounds.Start)
	    && values.Word.Bounds.Length()
      && (newSel.Start < values.Word.Bounds.Start || newSel.Start > values.Word.Bounds.End))
  {
	  richspell->PerformSpell(values.Word.Bounds);
  }

  richspell->FindTextRange(values.Word.Bounds);
  values.SelRange = newSel;
  values.Word.IsCorrect = richspell->IsCorrect();
}
