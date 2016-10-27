#include <vcl.h>
#include <locale>
#include "CustomEditSpell.h"
#include "CustomMemoSpell.h"
#include "RichEditSpell.h"

//#define ISMODIF(x) (isprint((x), std::locale("Russian_Russia.1251")) || (x) == VK_DELETE || (x) == VK_BACK)
//#define ISALNUM(x) isalnum((x), std::locale("Russian_Russia.1251"))
#define ISDELIM(x) (isspace((x), std::locale("Russian_Russia.1251")) || ispunct((x), std::locale("Russian_Russia.1251")))

struct WordUnderCursor
{
  TextRange  Bounds;
  int        CursorPos;
  wchar_t    PressedKey;
  bool       Misspelled;
};

template<typename CustomEditType>
class SpellingSetup
{
  public:
    SpellingSetup()
    {
      _speller = _component = NULL;
    }
    ~SpellingSetup()
    {
      delete _speller;
    }
    
    void Init(TForm* Form, CustomEditType* Component);
    void UpdateCurrentWord();

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
  
  /*if (__classid(CustomEditType) == __classid(TRichEdit))
    _speller = new RichEditSpell(_mainform, _component);
  else if (_component->InheritsFrom(__classid(TCustomMemo)))
    _speller = new CustomMemoSpell(_mainform, _component);
  else
    _speller = new CustomEditSpell(_mainform, _component);
    */
}

template<typename CustomEditType>
void SpellingSetup<CustomEditType>::UpdateCurrentWord()
{
  _current_word.Bounds = _speller->FindTextRange();
  _current_word.CursorPos = cursorPos;
  _current_word.PressedKey = L'\0';
  _current_word.Misspelled = _speller->IsMisspell();
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnKeyDownWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  /*memset(&_current_word, 0, sizeof(_current_word));

  if (ISMODIF(Key))
  {
    _current_word.CursorPos = _component->SelStart;
    _current_word.Bounds = _speller->FindTextRange();
    _current_word.Missp
    elled = _speller->IsMisspell(_current_word.Bounds.StartPos);
    
    if (_current_word.Misspelled)
      _speller->UnmarkAsMisspell(_current_word.Bounds);
  }*/
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnKeyPressWrapper(TObject* Sender, wchar_t& Key)
{
  _current_word.PressedKey = Key;
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnChangeWrapper(TObject* Sender)
{
  if (ISDELIM(_current_word.PressedKey))
  {
    _current_word.Bounds.StartPos = 0;
    _current_word.Bounds.Length = _speller->ToStdString().size();
    _speller->PerformSpell(_current_word.Bounds);
  }

  /*int CurPosDiff = _component->SelStart - _current_word.CursorPos;
  
  if (CurPosDiff < -1)
  {
    MessageBox(NULL, L" ак ты это сделал?", L"Ёй.", MB_OK);
    return;
  }
  
  if (!_current_word.Misspelled && !ISDELIM(_current_word.PressedKey))
    return;
  
  _current_word.Bounds.Length += CurPosDiff - 1;
  _speller->PerformSpell(_current_word.Bounds);*/
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnClickWrapper(TObject* Sender)
{
  int cursorPos = _component->SelStart;
  
  if (!(   cursorPos > _current_word.Bounds.StartPos
        && cursorPos < _current_word.Bounds.EndPos()))
  {
    _speller->PerformSpell(_current_word.Bounds);
    UpdateCurrentWord();
  }
}
