#include <vcl.h>
#include <locale>
#include "RichEditSpell.h"

#define ONCHANGE_BLOCK 1
#define ONCHANGE_ALLOW 0

//#define ISMODIF(x) (isprint((x), std::locale("Russian_Russia.1251")) || (x) == VK_DELETE || (x) == VK_BACK)
#define ISDELIM(x) (isspace((x), std::locale("Russian_Russia.1251")) || ispunct((x), std::locale("Russian_Russia.1251")))

struct CurrentWord
{
  TextRange  Bounds;
  bool       IsCorrect;
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

    void Init(TForm* Form, TRichEdit* Component);
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
    TPopupMenu*      _menu;
    RichEditSpell*   _wrapper;
    EventHandlers    _handlers;
    OnKeyDownValues  _keydown;
};

SpellingSetup::SpellingSetup()
{
  _component = NULL;
  _wrapper = NULL;
  _menu = NULL;
  memset(&_keydown, 0, sizeof(_keydown));
  memset(&_handlers, 0, sizeof(_handlers));
}

SpellingSetup::~SpellingSetup()
{
  Cleanup();
}

void SpellingSetup::Init(TForm* Form, TRichEdit* Component)
{
  _component = Component;

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

  _wrapper = new RichEditSpell(Form, _component);
  _menu = new TPopupMenu(_component);

  _wrapper->FindTextRange(_keydown.Word.Bounds);
  _keydown.Word.IsCorrect = _wrapper->IsCorrect();
  _wrapper->PerformSpell(TextRange(0, _component->GetTextLen()));
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

  if (_component->SelLength)
    _wrapper->FindTextRange(_keydown.Word.Bounds);

  _component->SelAttributes->Color = (TColor)tomAutoColor;

  _keydown.TextPos = _component->SelStart;
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

  int posDiff = _component->SelStart - _keydown.TextPos;
  TextRange newWord;
  _wrapper->FindTextRange(newWord);

  if (!_keydown.Word.IsCorrect)
	  _wrapper->UnmarkAsMisspell(_keydown.Word.Bounds);

  _wrapper->UnmarkAsMisspell(newWord);

  if (ISDELIM(_keydown.CharKey) || _keydown.RawKey == VK_RETURN || posDiff > 1)
  {
    _keydown.Word.Bounds.Length += posDiff;
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
    _component->SelStart = _component->Perform(EM_CHARFROMPOS, 0, (int)&TPoint(X, Y));

  UpdateCurrentWord();

  if (Button == mbRight)
  {
    _menu->Items->Clear();
    std::vector<std::wstring>* suggs = _wrapper->GetSuggestions(_keydown.Word.Bounds.StartPos);

    for (int i = 0; i < suggs->size(); ++i)
    {
      TMenuItem* item = new TMenuItem;

      item->Caption = suggs->at(i).c_str();
      item->Tag = (int)&_keyword.word.Bounds;
      item->OnClick = ;

      _menu->Items->Add()
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
  TextRange* range = (TextRange*)((TMenuItem*)Sender)->Tag;

  _wrapper->_ole.text->Range(range->StartPos, range->EndPos(), &_wrapper->_ole.range);
}

void SpellingSetup::UpdateCurrentWord()
{
  int NewPos = _component->SelStart;

  if (   _keydown.Word.IsCorrect
	    && _keydown.Word.Bounds.Length
      && (NewPos < _keydown.Word.Bounds.StartPos || NewPos > _keydown.Word.Bounds.EndPos()))
  {
	  _wrapper->PerformSpell(_keydown.Word.Bounds);
  }

  _wrapper->FindTextRange(_keydown.Word.Bounds);
  _keydown.TextPos = NewPos;
  _keydown.RawKey = 0;
  _keydown.CharKey = L'\0';
  _keydown.Word.IsCorrect = _wrapper->IsCorrect();
}
