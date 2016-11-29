#include "SpellingSetup.h"

bool OnChangeBlocked;

SpellingSetup::SpellingSetup()
{
  _mainform  = NULL;
  _component = NULL;
  _richspell = NULL;
  memset(&_handlers, 0, sizeof(_handlers));
  memset(&_values,   0, sizeof(_values));
}

SpellingSetup::~SpellingSetup()
{
  Disable();
}

void SpellingSetup::Init(TForm* form, TRichEdit* component)
{
  _mainform = form;
  _component = component;
  _component->PopupMenu = new TPopupMenu(_component);
  _component->PopupMenu->AutoPopup = false;

  _handlers.OnKeyDown    = _component->OnKeyDown;
  _handlers.OnKeyUp      = _component->OnKeyUp;
  _handlers.OnKeyPress   = _component->OnKeyPress;
  _handlers.OnChange     = _component->OnChange;
  _handlers.OnMouseDown  = _component->OnMouseDown;
  _handlers.OnExit       = _component->OnExit;

  _component->OnKeyDown    = OnKeyDownWrapper;
  _component->OnKeyUp      = OnKeyUpWrapper;
  _component->OnKeyPress   = OnKeyPressWrapper;
  _component->OnChange     = OnChangeWrapper;
  _component->OnMouseDown  = OnMouseDownWrapper;
  _component->OnExit       = OnExitWrapper;

  _richspell = new RichEditSpell(_component);
  _richspell->PerformSpell(_richspell->ole->GetTextBounds());
  _richspell->FindTextRange(_values.Word.Bounds);
  _values.Word.IsCorrect = _richspell->IsCorrect();

  OnChangeBlocked = false;
}

void SpellingSetup::Disable()
{
  _mainform = NULL;

  if (_richspell)
  {
    delete _richspell;
    _richspell = NULL;
  }

  if (_component)
  {
    _component->OnKeyDown    = _handlers.OnKeyDown;
    _component->OnKeyUp      = _handlers.OnKeyUp;
    _component->OnKeyPress   = _handlers.OnKeyPress;
    _component->OnChange     = _handlers.OnChange;
    _component->OnMouseDown  = _handlers.OnMouseDown;
    _component->OnExit       = _handlers.OnExit;

    delete _component->PopupMenu;
    _component->PopupMenu = NULL;
    _component = NULL;
  }
}

void SpellingSetup::SafeActivate(TForm* form, TRichEdit* component)
{
  try
  {
    Init(form, component);
  }
  catch (...)
  {
    MessageBox(form->Handle, "Не удалось запустить модуль проверки орфографии.", "Уведомление", MB_OK | MB_ICONWARNING);
    Disable();
  }
}

void __fastcall SpellingSetup::OnKeyDownWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  if (_handlers.OnKeyDown)
    _handlers.OnKeyDown(Sender, Key, Shift);

  if (Key == VK_CONTROL || Key == VK_MENU || Key == VK_SHIFT)
    return;

  Range selection = _richspell->ole->SelRange;

  _values.RawKey = Key;
  _values.CharKey = L'\0';
  _values.CursorPos = selection.Start;
  _values.Word.IsCorrect = _richspell->IsCorrect(selection.Start);

  if (selection.Length())
    _richspell->FindTextRange(_values.Word.Bounds);

  if (!_values.Word.IsCorrect && !OnlySelectionChanged(Shift))
	  _richspell->UnmarkAsMisspell(_values.Word.Bounds);

  OnChangeBlocked = true;
  _component->SelAttributes->Color = (TColor)tomAutoColor;
  OnChangeBlocked = false;
}

void __fastcall SpellingSetup::OnKeyUpWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  if (_handlers.OnKeyUp)
    _handlers.OnKeyUp(Sender, Key, Shift);

  if (Key == VK_CONTROL || Key == VK_MENU || Key == VK_SHIFT)
    return;

	if (OnlySelectionChanged(Shift))
    UpdateCurrentWord();
}

void __fastcall SpellingSetup::OnKeyPressWrapper(TObject* Sender, wchar_t& Key)
{
  if (_handlers.OnKeyPress)
    _handlers.OnKeyPress(Sender, Key);

  _values.CharKey = Key;
}

void __fastcall SpellingSetup::OnChangeWrapper(TObject* Sender)
{
  if (_handlers.OnChange)
    _handlers.OnChange(Sender);

  if (OnChangeBlocked || _values.RawKey == 0)
		return;

  int posDiff = _richspell->ole->SelRange.Start - _values.CursorPos;

  Range newWord, spellRange;
  _richspell->FindTextRange(newWord);

  if (_values.Word.Bounds.Length() == 0 && newWord.Length() == 0)
  {
    _values.Word.Bounds = newWord;
    _values.Word.IsCorrect = _richspell->IsCorrect();
    return;
  }

  if (posDiff < 0)
  {
    spellRange = newWord;
  }
  else
  {
    spellRange.Start = _values.Word.Bounds.Start;
    spellRange.End   = newWord.End;
  }

  _richspell->UnmarkAsMisspell(spellRange);

  if (ISDELIM(_values.CharKey) || _values.RawKey == VK_RETURN || posDiff > 1)
    _richspell->PerformSpell(spellRange);

  _values.Word.Bounds = newWord;
  _values.Word.IsCorrect = _richspell->IsCorrect();
  _values.RawKey = 0;
  _values.CharKey = L'\0';
}

void __fastcall SpellingSetup::OnMouseDownWrapper(TObject* Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (_handlers.OnMouseDown)
    _handlers.OnMouseDown(Sender, Button, Shift, X, Y);

  if (Button == mbLeft)
  {
    UpdateCurrentWord();
    return;
  }

  if (Button == mbRight)
  {
    int cursorPos = _component->Perform(EM_CHARFROMPOS, 0, (int)&TPoint(X, Y));
    Range selWord = _richspell->FindTextRange(cursorPos);

    if (selWord.Length() == 0 || _richspell->IsCorrect(selWord.Start + 1))
      return;

    _richspell->PerformSpell(selWord);

    std::vector<std::wstring>& suggs = _richspell->GetSuggestions(selWord.Start);
    CreateContextMenu(selWord, suggs);

    TPoint point = _component->ClientToScreen(TPoint(X, Y));
    _component->PopupMenu->Popup(point.X, point.Y);
  }
}

void SpellingSetup::CreateContextMenu(Range& word, std::vector<std::wstring>& suggs)
{
  TMenuItem* item;

  _component->PopupMenu->Items->Clear();

  if (suggs.size() != 0)
  {
    for (unsigned i = 0; i < suggs.size(); ++i)
    {
      item = new TMenuItem(_component->PopupMenu);

      item->Caption = suggs[i].c_str();
      item->Tag = (int)(new Range(word.Start, word.End));
      item->OnClick = OnMenuItemClick;

      _component->PopupMenu->Items->Add(item);
    }
  }
  else
  {
    item = new TMenuItem(_component->PopupMenu);

    item->Caption = L"Варианты отсутствуют";
    item->Enabled = false;

    _component->PopupMenu->Items->Add(item);
  }
}

void __fastcall SpellingSetup::OnExitWrapper(TObject* Sender)
{
  if (_handlers.OnExit)
    _handlers.OnExit(Sender);

  _richspell->PerformSpell(_values.Word.Bounds);
}

void __fastcall SpellingSetup::OnMenuItemClick(TObject* Sender)
{
  TMenuItem* item = (TMenuItem*)Sender;
  Range* range = (Range*)item->Tag;

  OnChangeBlocked = true;
  _richspell->UnmarkAsMisspell(*range);
  _richspell->ole->SetTextInRange(*range, item->Caption.c_str());
  OnChangeBlocked = false;

  delete range;
}

bool SpellingSetup::OnlySelectionChanged(TShiftState& shift)
{
  return (
        _values.RawKey == VK_END
    ||  _values.RawKey == VK_HOME
    ||  _values.RawKey == VK_LEFT
    ||  _values.RawKey == VK_RIGHT
    ||  _values.RawKey == VK_PRIOR
    ||  _values.RawKey == VK_NEXT
    || (_values.RawKey == VK_UP   && !shift.Contains(ssCtrl))
    || (_values.RawKey == VK_DOWN && !shift.Contains(ssCtrl))
  );
}

void SpellingSetup::UpdateCurrentWord()
{
  int newPos = _richspell->ole->SelRange.Start;

  if (   _richspell->IsCorrect()
	    && _values.Word.Bounds.Length()
      && (newPos < _values.Word.Bounds.Start || newPos > _values.Word.Bounds.End))
  {
	  _richspell->PerformSpell(_values.Word.Bounds);
  }

  _richspell->FindTextRange(_values.Word.Bounds);
  _values.CursorPos = newPos;
  _values.Word.IsCorrect = _richspell->IsCorrect();
}

