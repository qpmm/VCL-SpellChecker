#include <vcl.h>
#include <locale>
#include "CustomEditSpell.h"
#include "CustomMemoSpell.h"
#include "RichEditSpell.h"

#define ONCHANGE_BLOCK 1
#define ONCHANGE_ALLOW 0

//#define ISMODIF(x) (isprint((x), std::locale("Russian_Russia.1251")) || (x) == VK_DELETE || (x) == VK_BACK)
#define ISDELIM(x) (isspace((x), std::locale("Russian_Russia.1251")) || ispunct((x), std::locale("Russian_Russia.1251")))

struct OnKeyDownValues
{
  int        TextPos;
  WORD       RawKey;
  wchar_t    CharKey;
};

struct CurrentWord
{
  TextRange  Bounds;
  bool       IsCorrect;
};

struct EventHandlers
{
  TKeyEvent         OnKeyDown;
  TKeyEvent         OnKeyUp;
  TKeyPressEvent    OnKeyPress;
  TNotifyEvent      OnChange;
  TNotifyEvent      OnClick;
  TNotifyEvent      OnExit;
};

template<typename CustomEditType>
class SpellingSetup
{
  public:
    SpellingSetup();
    ~SpellingSetup();

    void Init(TForm* Form, CustomEditType* Component);
    void Cleanup();

    void __fastcall OnKeyDownWrapper (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyUpWrapper   (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyPressWrapper(TObject* Sender, wchar_t& Key);
    void __fastcall OnChangeWrapper  (TObject* Sender);
    void __fastcall OnClickWrapper   (TObject* Sender);
    void __fastcall OnExitWrapper    (TObject* Sender);
    
  private:
    CustomEditType*       _component;
    CustomEditSpell*      _wrapper;
    EventHandlers         _handlers;
    OnKeyDownValues       _keydown;
    CurrentWord           _cw;
};

template<typename CustomEditType>
SpellingSetup<CustomEditType>::SpellingSetup()
{
  _component = NULL;
  _wrapper = NULL;
  memset(&_keydown, 0, sizeof(_keydown));
  memset(&_handlers, 0, sizeof(_handlers));
}

template<typename CustomEditType>
SpellingSetup<CustomEditType>::~SpellingSetup()
{
  Cleanup();
}

template<typename CustomEditType>
void SpellingSetup<CustomEditType>::Init(TForm* Form, CustomEditType* Component)
{
  _component = Component;

  _handlers.OnKeyDown  = _component->OnKeyDown;
  _handlers.OnKeyUp    = _component->OnKeyUp;
  _handlers.OnKeyPress = _component->OnKeyPress;
  _handlers.OnChange   = _component->OnChange;
  _handlers.OnClick    = _component->OnClick;
  _handlers.OnExit     = _component->OnExit;

  _component->OnKeyDown  = OnKeyDownWrapper;
  _component->OnKeyUp    = OnKeyUpWrapper;
  _component->OnKeyPress = OnKeyPressWrapper;
  _component->OnChange   = OnChangeWrapper;
  _component->OnClick    = OnClickWrapper;
  _component->OnExit     = OnExitWrapper;

  _wrapper = new RichEditSpell(Form, _component);

  _wrapper->FindTextRange(_cw.Bounds);
  _wrapper->PerformSpell(TextRange(0, _wrapper->GetLength()));
  
  /*if (__classid(CustomEditType) == __classid(TRichEdit))
    _wrapper = new RichEditSpell(_mainform, _component);
  else if (_component->InheritsFrom(__classid(TCustomMemo)))
    _wrapper = new CustomMemoSpell(_mainform, _component);
  else
    _wrapper = new CustomEditSpell(_mainform, _component);
  */
}

template<typename CustomEditType>
void SpellingSetup<CustomEditType>::Cleanup()
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
    _component->OnClick    = _handlers.OnClick;
    _component->OnExit     = _handlers.OnExit;
    _component = NULL;
  }

  memset(&_handlers, 0, sizeof(_handlers));
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnKeyDownWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  if (_handlers.OnKeyDown)
    _handlers.OnKeyDown(Sender, Key, Shift);

  if (_component->SelLength)
    _wrapper->FindTextRange(_cw.Bounds);

  _component->Tag = ONCHANGE_BLOCK;
  _component->SelAttributes->Color = (TColor)tomAutoColor;
  _component->Tag = ONCHANGE_ALLOW;

  _keydown.TextPos = _wrapper->component->SelStart;
	_keydown.CharKey = '\0';
	_keydown.RawKey = Key;
	_cw.IsCorrect = _wrapper->IsCorrect(_cw.Bounds.StartPos);
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnKeyUpWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
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
    OnClickWrapper(NULL);
  }
}


template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnKeyPressWrapper(TObject* Sender, wchar_t& Key)
{
  if (_handlers.OnKeyPress)
    _handlers.OnKeyPress(Sender, Key);

  _keydown.CharKey = Key;
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnChangeWrapper(TObject* Sender)
{
  if (_handlers.OnChange)
    _handlers.OnChange(Sender);

  // Если изменились только параметры шрифта
  if (_component->Tag == ONCHANGE_BLOCK)
		return;

  TextRange newWord;
  _wrapper->FindTextRange(newWord);
	int posDiff = _component->SelStart - _keydown.TextPos;

  if (!_cw.IsCorrect)
    _wrapper->UnmarkAsMisspell(_cw.Bounds);

  _wrapper->UnmarkAsMisspell(newWord);

	if (ISDELIM(_keydown.CharKey) || _keydown.RawKey == VK_RETURN || posDiff > 1)
	{
    _cw.Bounds.Length += posDiff;
    _wrapper->PerformSpell(_cw.Bounds);
	}

  _cw.Bounds = newWord;
	_cw.IsCorrect = _wrapper->IsCorrect(newWord.StartPos);
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnClickWrapper(TObject* Sender)
{
  if (_handlers.OnClick)
    _handlers.OnClick(Sender);

  int NewPos = _wrapper->component->SelStart;

	if (   _cw.IsCorrect
      && _cw.Bounds.Length
      && (NewPos < _cw.Bounds.StartPos || NewPos > _cw.Bounds.EndPos()))
	{
		_wrapper->PerformSpell(_cw.Bounds);
  }

  _wrapper->FindTextRange(_cw.Bounds);
  _keydown.TextPos = NewPos;
  _keydown.RawKey = 0;
  _keydown.CharKey = L'\0';
  _cw.IsCorrect = _wrapper->IsCorrect(_cw.Bounds.StartPos);
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnExitWrapper(TObject* Sender)
{
  if (_handlers.OnExit)
    _handlers.OnExit(Sender);

  _wrapper->PerformSpell(_cw.Bounds);
}
