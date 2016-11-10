#include <vcl.h>
#include <locale>
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
    void __fastcall OnClickWrapper   (TObject* Sender);
    void __fastcall OnExitWrapper    (TObject* Sender);
    
  private:
    TRichEdit*       _component;
    RichEditSpell*   _wrapper;
    EventHandlers    _handlers;
    OnKeyDownValues  _keydown;
    CurrentWord      _cw;
};

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

void SpellingSetup::Init(TForm* Form, TRichEdit* Component)
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
  _cw.IsCorrect = _wrapper->IsCorrect();
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
    _component->OnClick    = _handlers.OnClick;
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
    _wrapper->FindTextRange(_cw.Bounds);

  _component->Tag = ONCHANGE_BLOCK;
  _component->SelAttributes->Color = (TColor)tomAutoColor;
  _component->Tag = ONCHANGE_ALLOW;

  _keydown.TextPos = _component->SelStart;
  _keydown.CharKey = '\0';
  _keydown.RawKey = Key;
  _cw.IsCorrect = _wrapper->IsCorrect();
}

void __fastcall SpellingSetup::OnKeyUpWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  if (_handlers.OnKeyUp)
    _handlers.OnKeyUp(Sender, Key, Shift);

  // ���� ������ ���������� ��������� �������
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

  // ���� ���������� ������ ��������� ������
  if (_component->Tag == ONCHANGE_BLOCK)
		return;

  int posDiff = _component->SelStart - _keydown.TextPos;
  TextRange newWord;
  _wrapper->FindTextRange(newWord);

  if (!_cw.IsCorrect)
	_wrapper->UnmarkAsMisspell(_cw.Bounds);

  _wrapper->UnmarkAsMisspell(newWord);

  if (ISDELIM(_keydown.CharKey) || _keydown.RawKey == VK_RETURN || posDiff > 1)
  {
    _cw.Bounds.Length += posDiff;
	_wrapper->PerformSpell(_cw.Bounds);
  }

  _cw.Bounds = newWord;
  _cw.IsCorrect = _wrapper->IsCorrect();
}

void __fastcall SpellingSetup::OnClickWrapper(TObject* Sender)
{
  if (_handlers.OnClick)
    _handlers.OnClick(Sender);

  int NewPos = _component->SelStart;

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
  _cw.IsCorrect = _wrapper->IsCorrect();
}


void __fastcall SpellingSetup::OnExitWrapper(TObject* Sender)
{
  if (_handlers.OnExit)
    _handlers.OnExit(Sender);

  _wrapper->PerformSpell(_cw.Bounds);
}
