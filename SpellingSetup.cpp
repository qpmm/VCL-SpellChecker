#include <vcl.h>
#include <locale>
#include "CustomEditSpell.h"
#include "CustomMemoSpell.h"
#include "RichEditSpell.h"

#define ONCHANGE_BLOCK 1

#define ISMODIF(x) (isprint((x), std::locale("Russian_Russia.1251")) || (x) == VK_DELETE || (x) == VK_BACK)
//#define ISALNUM(x) isalnum((x), std::locale("Russian_Russia.1251"))
#define ISDELIM(x) (isspace((x), std::locale("Russian_Russia.1251")) || ispunct((x), std::locale("Russian_Russia.1251")))

struct WordUnderCursor
{
  TextRange  Bounds;
  int        CursorPos;
  WORD       RawKey;
  wchar_t    PressedKey;
  bool       Misspelled;
};

struct ExistingEventHandlers
{
  TKeyEvent      KeyDown;
  TKeyEvent      KeyUp;
  TKeyPressEvent KeyPress;
  TNotifyEvent   Change;
  TNotifyEvent   Click;
};

template<typename CustomEditType>
class SpellingSetup
{
  public:
    SpellingSetup()
    {
      _speller = NULL;
      _component = NULL;
    }
    ~SpellingSetup()
    {
      delete _speller;
    }
    
    void Init(TForm* Form, CustomEditType* Component);

    void __fastcall OnKeyDownWrapper (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyUpWrapper   (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyPressWrapper(TObject* Sender, wchar_t& Key);
    void __fastcall OnChangeWrapper  (TObject* Sender);
    void __fastcall OnClickWrapper   (TObject* Sender);
    
  private:
    TForm*                _mainform;
    CustomEditType*       _component;
    CustomEditSpell*      _speller;
    WordUnderCursor       _cw;
    ExistingEventHandlers _eeh;
};

template<typename CustomEditType>
void SpellingSetup<CustomEditType>::Init(TForm* Form, CustomEditType* Component)
{
  _mainform = Form;
  _component = Component;

  _eeh.KeyDown  = _component->OnKeyDown;
  _eeh.KeyUp    = _component->OnKeyUp;
  _eeh.KeyPress = _component->OnKeyPress;
  _eeh.Change   = _component->OnChange;
  _eeh.Click    = _component->OnClick;

  _component->OnKeyDown  = OnKeyDownWrapper;
  _component->OnKeyUp    = OnKeyUpWrapper;
  _component->OnKeyPress = OnKeyPressWrapper;
  _component->OnChange   = OnChangeWrapper;
  _component->OnClick    = OnClickWrapper;

  _speller = new RichEditSpell(_mainform, _component);

  _cw.Bounds = _speller->FindTextRange();
  
  /*if (__classid(CustomEditType) == __classid(TRichEdit))
    _speller = new RichEditSpell(_mainform, _component);
  else if (_component->InheritsFrom(__classid(TCustomMemo)))
    _speller = new CustomMemoSpell(_mainform, _component);
  else
    _speller = new CustomEditSpell(_mainform, _component);
  */
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnKeyDownWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  if (_eeh.KeyDown)
    _eeh.KeyDown(Sender, Key, Shift);

  if (_component->SelLength)
    _cw.Bounds = _speller->FindTextRange();

  _component->SelAttributes->Color = (TColor)tomAutoColor;

  _cw.CursorPos = _component->SelStart;
	_cw.PressedKey = '\0';
	_cw.RawKey = Key;
	_cw.Misspelled = _speller->IsMisspell(_cw.Bounds.StartPos);
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnKeyUpWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  if (_eeh.KeyUp)
    _eeh.KeyUp(Sender, Key, Shift);

  // Если просто изменилось положение курсора
	if (   _cw.RawKey  == VK_END
      || _cw.RawKey  == VK_HOME
      || _cw.RawKey  == VK_LEFT
      || _cw.RawKey  == VK_RIGHT
      || (_cw.RawKey == VK_UP   && !Shift.Contains(ssCtrl))
      || (_cw.RawKey == VK_DOWN && !Shift.Contains(ssCtrl)))
  {
    OnClickWrapper(NULL);
  }
}


template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnKeyPressWrapper(TObject* Sender, wchar_t& Key)
{
  if (_eeh.KeyPress)
    _eeh.KeyPress(Sender, Key);

  _cw.PressedKey = Key;
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnChangeWrapper(TObject* Sender)
{
  if (_eeh.Change)
    _eeh.Change(Sender);

  // Если изменились только параметры шрифта
  if (_component->Tag == ONCHANGE_BLOCK)
		return;

  int CursorPos = _component->SelStart;
	int PosDiff = CursorPos - _cw.CursorPos;

	TextRange newWord = _speller->FindTextRange();

  if (_cw.Misspelled)
    _speller->UnmarkAsMisspell(_cw.Bounds);

  _speller->UnmarkAsMisspell(newWord);

	if (ISDELIM(_cw.PressedKey) || PosDiff > 1)
	{
    _cw.Bounds.Length += PosDiff;
    _speller->PerformSpell(_cw.Bounds);
	}

	_cw.Misspelled = _speller->IsMisspell(newWord.StartPos);
	_cw.Bounds = newWord;
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnClickWrapper(TObject* Sender)
{
  if (_eeh.Click)
    _eeh.Click(Sender);

  int CursorPos = _component->SelStart;

	if (   !_cw.Misspelled
      && _cw.Bounds.Length
      && !(CursorPos >= _cw.Bounds.StartPos && CursorPos <= _cw.Bounds.EndPos()))
	{
		_speller->PerformSpell(_cw.Bounds);
  }

  _cw.Bounds = _speller->FindTextRange();
  _cw.CursorPos = CursorPos;
  _cw.RawKey = 0;
  _cw.PressedKey = L'\0';
  _cw.Misspelled = _speller->IsMisspell(_cw.Bounds.StartPos);
}
