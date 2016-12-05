#include "RichEditSetup.h"

bool OnChangeBlocked;

RichEditSetup::RichEditSetup()
{
  component = NULL;
  richspell = NULL;
  memset(&handlers, 0, sizeof(handlers));
  memset(&values,   0, sizeof(values));
}

RichEditSetup::~RichEditSetup()
{
  Disable();
}

void RichEditSetup::Init(TRichEdit* edit)
{
  component = edit;
  component->PopupMenu = new TPopupMenu(component);
  component->PopupMenu->AutoPopup = false;

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

  richspell = new RichEditSpell(component);
  richspell->PerformSpell(richspell->ole->GetTextBounds());
  richspell->FindTextRange(values.Word.Bounds);
  values.Word.IsCorrect = richspell->IsCorrect();

  OnChangeBlocked = false;
}

void RichEditSetup::Disable()
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

    delete component->PopupMenu;
    component->PopupMenu = NULL;
    component = NULL;
  }
}

void __fastcall RichEditSetup::OnKeyDownWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  if (handlers.OnKeyDown)
    handlers.OnKeyDown(Sender, Key, Shift);

  if (Key == VK_CONTROL || Key == VK_MENU || Key == VK_SHIFT || Key == VK_ESCAPE)
    return;

  Range selection = richspell->ole->SelRange;

  values.RawKey = Key;
  values.CharKey = L'\0';
  values.CursorPos = selection.Start;
  values.Word.IsCorrect = richspell->IsCorrect(selection.Start);

  if (selection.Length())
    richspell->FindTextRange(values.Word.Bounds);

  if (!values.Word.IsCorrect && !OnlySelectionChanged(Shift))
	  richspell->UnmarkAsMisspell(values.Word.Bounds);

  OnChangeBlocked = true;
  component->SelAttributes->Color = (TColor)tomAutoColor;
  OnChangeBlocked = false;
}

void __fastcall RichEditSetup::OnKeyUpWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  if (handlers.OnKeyUp)
    handlers.OnKeyUp(Sender, Key, Shift);

  if (Key == VK_CONTROL || Key == VK_MENU || Key == VK_SHIFT)
    return;

	if (OnlySelectionChanged(Shift))
    UpdateCurrentWord();
}

void __fastcall RichEditSetup::OnKeyPressWrapper(TObject* Sender, wchar_t& Key)
{
  if (handlers.OnKeyPress)
    handlers.OnKeyPress(Sender, Key);

  values.CharKey = Key;
}

void __fastcall RichEditSetup::OnChangeWrapper(TObject* Sender)
{
  if (handlers.OnChange)
    handlers.OnChange(Sender);

  if (OnChangeBlocked || values.RawKey == 0)
		return;

  int posDiff = richspell->ole->SelRange.Start - values.CursorPos;

  Range newWord, spellRange;
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

void __fastcall RichEditSetup::OnMouseDownWrapper(TObject* Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (handlers.OnMouseDown)
    handlers.OnMouseDown(Sender, Button, Shift, X, Y);

  if (Button == mbLeft)
  {
    UpdateCurrentWord();
    return;
  }

  if (Button == mbRight)
  {
    int cursorPos = component->Perform(EM_CHARFROMPOS, 0, (int)&TPoint(X, Y));
    Range selWord = richspell->FindTextRange(cursorPos);

    if (selWord.Length() == 0 || richspell->IsCorrect(selWord.Start + 1))
      return;

    richspell->PerformSpell(selWord);

    std::vector<std::wstring>& suggs = richspell->GetSuggestions(selWord.Start);
    CreateContextMenu(selWord, suggs);

    TPoint point = component->ClientToScreen(TPoint(X, Y));
    component->PopupMenu->Popup(point.X, point.Y);
  }
}

void RichEditSetup::CreateContextMenu(Range& word, std::vector<std::wstring>& suggs)
{
  TMenuItem* item;

  component->PopupMenu->Items->Clear();

  if (suggs.size() != 0)
  {
    for (unsigned i = 0; i < suggs.size(); ++i)
    {
      item = new TMenuItem(component->PopupMenu);

      item->Caption = suggs[i].c_str();
      item->Tag = (int)(new Range(word.Start, word.End));
      item->OnClick = OnMenuItemClick;

      component->PopupMenu->Items->Add(item);
    }
  }
  else
  {
    item = new TMenuItem(component->PopupMenu);

    item->Caption = L"Варианты отсутствуют";
    item->Enabled = false;

    component->PopupMenu->Items->Add(item);
  }
}

void __fastcall RichEditSetup::OnExitWrapper(TObject* Sender)
{
  if (handlers.OnExit)
    handlers.OnExit(Sender);

  richspell->PerformSpell(values.Word.Bounds);
}

void __fastcall RichEditSetup::OnMenuItemClick(TObject* Sender)
{
  TMenuItem* item = (TMenuItem*)Sender;
  Range* range = (Range*)item->Tag;

  OnChangeBlocked = true;
  richspell->UnmarkAsMisspell(*range);
  richspell->ole->SetTextInRange(*range, item->Caption.c_str());
  OnChangeBlocked = false;

  delete range;
}

bool RichEditSetup::OnlySelectionChanged(TShiftState& shift)
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

void RichEditSetup::UpdateCurrentWord()
{
  int newPos = richspell->ole->SelRange.Start;

  if (   richspell->IsCorrect()
	    && values.Word.Bounds.Length()
      && (newPos < values.Word.Bounds.Start || newPos > values.Word.Bounds.End))
  {
	  richspell->PerformSpell(values.Word.Bounds);
  }

  richspell->FindTextRange(values.Word.Bounds);
  values.CursorPos = newPos;
  values.Word.IsCorrect = richspell->IsCorrect();
}
