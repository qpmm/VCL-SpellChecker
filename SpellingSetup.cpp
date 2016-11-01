#include <vcl.h>
#include <locale>
#include "CustomEditSpell.h"
#include "CustomMemoSpell.h"
#include "RichEditSpell.h"

#define ONCHANGE_BLOCK 1

//#define ISMODIF(x) (isprint((x), std::locale("Russian_Russia.1251")) || (x) == VK_DELETE || (x) == VK_BACK)
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
    void __fastcall OnKeyPressWrapper(TObject* Sender, wchar_t& Key);
    void __fastcall OnChangeWrapper  (TObject* Sender);
    void __fastcall OnClickWrapper   (TObject* Sender);
    
  private:
    TForm*           _mainform;
    CustomEditType*  _component;
    CustomEditSpell* _speller;
    WordUnderCursor  _current_word;
};

template<typename CustomEditType>
void SpellingSetup<CustomEditType>::Init(TForm* Form, CustomEditType* Component)
{
  _mainform = Form;
  _component = Component;
  _component->OnKeyDown  = OnKeyDownWrapper;
  _component->OnKeyPress = OnKeyPressWrapper;
  _component->OnChange   = OnChangeWrapper;
  _component->OnClick    = OnClickWrapper;

  _speller = new RichEditSpell(_mainform, _component);

  memset(&_current_word, 0, sizeof(_current_word));
  _current_word.CursorPos = _component->SelStart;
  _current_word.Bounds    = _speller->FindTextRange();
  
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
  _current_word.CursorPos = _component->SelStart;
  _current_word.PressedKey = '\0';
	_current_word.RawKey = Key;

	if (_current_word.Misspelled)
	{
		_speller->UnmarkAsMisspell(_current_word.Bounds);
		_current_word.Misspelled = false;
	}
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnKeyPressWrapper(TObject* Sender, wchar_t& Key)
{
  _current_word.PressedKey = Key;
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnChangeWrapper(TObject* Sender)
{
  int CurrentPos = _component->SelStart;
	int CurPosDiff = CurrentPos - _current_word.CursorPos;

  if (_component->Tag == ONCHANGE_BLOCK)
    return;

	if (_current_word.RawKey == VK_BACK)
	{
		if (   _current_word.Bounds.StartPos == _current_word.CursorPos
			  || _current_word.Bounds.Length == 0)
		{
			_current_word.Bounds = _speller->FindTextRange();
      _current_word.Misspelled = _speller->IsMisspell(_current_word.Bounds.StartPos);
		}
		else
		{
			_current_word.Bounds.Length -= 1;
			//return;
		}
	}
	else if (_current_word.RawKey == VK_DELETE)
	{
		if (   _current_word.Bounds.EndPos() == _current_word.CursorPos
			  || _current_word.Bounds.Length == 0)
		{
			_current_word.Bounds = _speller->FindTextRange();
      _current_word.Misspelled = _speller->IsMisspell(_current_word.Bounds.StartPos);
		}
		else
		{
			_current_word.Bounds.Length -= 1;
			//return;
		}
	}

	if (_current_word.Misspelled || ISDELIM(_current_word.PressedKey) || CurPosDiff > 1)
	{
		_current_word.Bounds.Length += CurPosDiff; // ctrl-x ???
		_speller->PerformSpell(_current_word.Bounds);
		_current_word.Bounds = _speller->FindTextRange();
		_current_word.Misspelled = _speller->IsMisspell(_current_word.Bounds.StartPos);
	}
	else
	{
    _current_word.CursorPos = CurrentPos;
		if (_current_word.RawKey != VK_BACK && _current_word.RawKey != VK_DELETE)
		{
			_current_word.Bounds.Length += CurPosDiff;
		}
	}
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnClickWrapper(TObject* Sender)
{
  int cursorPos = _component->SelStart;

	if (   !_current_word.Misspelled
      && _current_word.Bounds.Length
      && !(cursorPos >= _current_word.Bounds.StartPos && cursorPos <= _current_word.Bounds.EndPos()))
	{
		_speller->PerformSpell(_current_word.Bounds);
  }

  _current_word.Bounds = _speller->FindTextRange();
  _current_word.CursorPos = cursorPos;
  _current_word.RawKey = 0;
  _current_word.PressedKey = L'\0';
  _current_word.Misspelled = _speller->IsMisspell(_current_word.Bounds.StartPos);
}
