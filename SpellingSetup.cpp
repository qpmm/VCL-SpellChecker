#include "SpellingSetup.h"

#define ONCHANGE_BLOCK 1
#define ONCHANGE_ALLOW 0

//#define ISMODIF(x) (isprint((x), std::locale("Russian_Russia.1251")) || (x) == VK_DELETE || (x) == VK_BACK)
#define ISDELIM(x) (isspace((x), std::locale("Russian_Russia.1251")) || ispunct((x), std::locale("Russian_Russia.1251")))
#define END -1

SpellingSetup::SpellingSetup()
{
  _component = NULL;
  _wrapper = NULL;
  memset(&_keydown, 0, sizeof(_keydown));
  memset(&_handlers, 0, sizeof(_handlers));
}

SpellingSetup::~SpellingSetup()
{
  Cleanup();
}

void SpellingSetup::Init(TForm* form, TRichEdit* component)
{
  _component = component;
  //_component->PopupMenu = new TPopupMenu(_component);

  _handlers.OnKeyDown  = _component->OnKeyDown;
  _handlers.OnKeyUp    = _component->OnKeyUp;
  _handlers.OnKeyPress = _component->OnKeyPress;
  _handlers.OnChange   = _component->OnChange;
  _handlers.OnMouseUp  = _component->OnMouseUp;
  _handlers.OnExit     = _component->OnExit;

  _component->OnKeyDown   = OnKeyDownWrapper;
  _component->OnKeyUp     = OnKeyUpWrapper;
  _component->OnKeyPress  = OnKeyPressWrapper;
  _component->OnChange    = OnChangeWrapper;
  _component->OnMouseUp   = OnMouseUpWrapper;
  _component->OnExit      = OnExitWrapper;

  _wrapper = new RichEditSpell(form, _component);
  _wrapper->FindTextRange(_keydown.Word.Bounds);
  _wrapper->PerformSpell(Range(0, END));
  _keydown.Word.IsCorrect = _wrapper->IsCorrect();
}

void SpellingSetup::Cleanup()
{
  if (_wrapper)
  {
    delete _wrapper;
    _wrapper = NULL;
  }

  if (_component)
  {
    _component->OnKeyDown  = _handlers.OnKeyDown;
    _component->OnKeyUp    = _handlers.OnKeyUp;
    _component->OnKeyPress = _handlers.OnKeyPress;
    _component->OnChange   = _handlers.OnChange;
    _component->OnMouseUp  = _handlers.OnMouseUp;
    _component->OnExit     = _handlers.OnExit;
    _component = NULL;
  }

  memset(&_handlers, 0, sizeof(_handlers));
}

void __fastcall SpellingSetup::OnKeyDownWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  if (_handlers.OnKeyDown)
    _handlers.OnKeyDown(Sender, Key, Shift);

  if (_wrapper->ole->SelRange.Length())
    _wrapper->FindTextRange(_keydown.Word.Bounds);

  _component->SelAttributes->Color = (TColor)tomAutoColor;

  _keydown.TextPos = _wrapper->ole->SelRange.Start;
  _keydown.CharKey = '\0';
  _keydown.RawKey = Key;
  _keydown.Word.IsCorrect = _wrapper->IsCorrect();
}

void __fastcall SpellingSetup::OnKeyUpWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  if (_handlers.OnKeyUp)
    _handlers.OnKeyUp(Sender, Key, Shift);

  // Если просто изменилось положение курсора
	if (   _keydown.RawKey  == VK_END
      || _keydown.RawKey  == VK_HOME
      || _keydown.RawKey  == VK_LEFT
      || _keydown.RawKey  == VK_RIGHT
      || (_keydown.RawKey == VK_UP   && !Shift.Contains(ssCtrl))
      || (_keydown.RawKey == VK_DOWN && !Shift.Contains(ssCtrl)))
  {
    UpdateCurrentWord();
  }
}

void __fastcall SpellingSetup::OnKeyPressWrapper(TObject* Sender, wchar_t& Key)
{
  if (_handlers.OnKeyPress)
    _handlers.OnKeyPress(Sender, Key);

  _keydown.CharKey = Key;
}

void __fastcall SpellingSetup::OnChangeWrapper(TObject* Sender)
{
  if (_handlers.OnChange)
    _handlers.OnChange(Sender);

  // Если изменились только параметры шрифта
  if (_component->Tag == ONCHANGE_BLOCK)
		return;

  Range newWord;
  int posDiff = _wrapper->ole->SelRange.Start - _keydown.TextPos;
  _wrapper->FindTextRange(newWord);

  if (!_keydown.Word.IsCorrect)
	  _wrapper->UnmarkAsMisspell(_keydown.Word.Bounds);

  _wrapper->UnmarkAsMisspell(newWord);

  if (ISDELIM(_keydown.CharKey) || _keydown.RawKey == VK_RETURN || posDiff > 1)
  {
    if (posDiff > 1)
      _keydown.Word.Bounds.End += posDiff;
	  _wrapper->PerformSpell(_keydown.Word.Bounds);
  }

  _keydown.Word.Bounds = newWord;
  _keydown.Word.IsCorrect = _wrapper->IsCorrect();
}

void __fastcall SpellingSetup::OnMouseUpWrapper(TObject* Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (_handlers.OnMouseUp)
    _handlers.OnMouseUp(Sender, Button, Shift, X, Y);

  if (Button == mbRight)
  {
    int start = _component->Perform(EM_CHARFROMPOS, 0, (int)&TPoint(X, Y));
    _wrapper->ole->SelRange = Range(start, start);
  }

  UpdateCurrentWord();

  if (_keydown.Word.Bounds.Length() == 0 || _keydown.Word.IsCorrect)
    return;

  if (Button == mbRight)
  {
    _component->PopupMenu->Items->Clear();
    _wrapper->PerformSpell(_keydown.Word.Bounds);
    std::vector<std::wstring>& suggs = _wrapper->GetSuggestions(_keydown.Word.Bounds.Start);

    if (suggs.size())
    {
      for (unsigned i = 0; i < suggs.size(); ++i)
      {
        TMenuItem* item = new TMenuItem(_component->PopupMenu);

        item->Caption = suggs[i].c_str();
        item->Tag = (int)&_keydown.Word.Bounds;
        item->OnClick = OnMenuItemClick;

        _component->PopupMenu->Items->Add(item);
      }
    }
    else
    {
      TMenuItem* item = new TMenuItem(_component->PopupMenu);

      item->Caption = L"Варианты отсутствуют";
      item->Enabled = false;

      _component->PopupMenu->Items->Add(item);
    }

    TPoint kokord = _component->ClientToScreen(TPoint(X, Y));
    _component->PopupMenu->Popup(kokord.X, kokord.Y);
  }
}

void __fastcall SpellingSetup::OnExitWrapper(TObject* Sender)
{
  if (_handlers.OnExit)
    _handlers.OnExit(Sender);

  _wrapper->PerformSpell(_keydown.Word.Bounds);
}

void __fastcall SpellingSetup::OnMenuItemClick(TObject* Sender)
{
  TMenuItem* item = (TMenuItem*)Sender;
  Range* range = (Range*)(item->Tag);

  _component->Tag = ONCHANGE_BLOCK;
  _wrapper->UnmarkAsMisspell(*range);
  _wrapper->ole->SetTextInRange(*range, item->Caption.c_str());
  _component->Tag = ONCHANGE_ALLOW;
}

void SpellingSetup::UpdateCurrentWord()
{
  int NewPos = _wrapper->ole->SelRange.Start;

  if (   _keydown.Word.IsCorrect
	    && _keydown.Word.Bounds.Length()
      && (NewPos < _keydown.Word.Bounds.Start || NewPos > _keydown.Word.Bounds.End))
  {
	  _wrapper->PerformSpell(_keydown.Word.Bounds);
  }

  _wrapper->FindTextRange(_keydown.Word.Bounds);
  _keydown.TextPos = NewPos;
  _keydown.RawKey = 0;
  _keydown.CharKey = L'\0';
  _keydown.Word.IsCorrect = _wrapper->IsCorrect();
}
