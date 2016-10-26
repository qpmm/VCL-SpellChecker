#include <vcl.h>
#include <locale>
#include "CustomEditSpell.h"
#include "CustomMemoSpell.h"
#include "RichEditSpell.h"

//#define ISMODIF(x) (isprint((x), std::locale("Russian_Russia.1251")) || (x) == VK_DELETE || (x) == VK_BACK)
//#define ISALNUM(x) isalnum((x), std::locale("Russian_Russia.1251"))
#define ISDELIM(x) (isspace((x), std::locale("Russian_Russia.1251")) || ispunct((x), std::locale("Russian_Russia.1251")))

struct OnKeyDownInfo
{
  int CursorPos;
  TextRange Range;
  wchar_t PressedKey;
  bool Misspelled;
};

template<typename CustomEditType>
class SpellingSetup
{
  public:
    SpellingSetup()
    {
      _object         = NULL;
      _object_speller = NULL;
    }
    ~SpellingSetup()
    {
      delete _object_speller;
    }
    
    void Init(TForm* Form, CustomEditType* Component);
    void __fastcall OnKeyDownWrapper (TObject* Sender, WORD&    Key, TShiftState Shift);
    void __fastcall OnKeyPressWrapper(TObject* Sender, wchar_t& Key);
    void __fastcall OnChangeWrapper  (TObject* Sender);
    void shell();
    
  private:
    TForm*            _mainform;
    CustomEditType*   _object;
    CustomEditSpell*  _object_speller;
    OnKeyDownInfo     _word_state;
};

template<typename CustomEditType>
void SpellingSetup<CustomEditType>::Init(TForm* Form, CustomEditType* Component)
{
  _mainform = Form;
  _object = Component;
  _object->OnKeyDown  = OnKeyDownWrapper;
  _object->OnKeyPress = OnKeyPressWrapper;
  _object->OnChange   = OnChangeWrapper;
  
  /*if (__classid(CustomEditType) == __classid(TRichEdit))
    _object_speller = new RichEditSpell(_mainform, _object);
  else if (_object->InheritsFrom(__classid(TCustomMemo)))
    _object_speller = new CustomMemoSpell(_mainform, _object);
  else*/
    _object_speller = new CustomEditSpell(_mainform, _object);
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnKeyDownWrapper(TObject* Sender, WORD& Key, TShiftState Shift)
{
  /*memset(&_word_state, 0, sizeof(_word_state));

  if (ISMODIF(Key))
  {
    _word_state.CursorPos = _object->SelStart;
    _word_state.Range = _object_speller->FindTextRange();
    _word_state.Misspelled = _object_speller->IsMisspell(_word_state.Range.StartPos);
    
    if (_word_state.Misspelled)
      _object_speller->UnmarkAsMisspell(_word_state.Range);
  }*/
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnKeyPressWrapper(TObject* Sender, wchar_t& Key)
{
  _word_state.PressedKey = Key;
}

template<typename CustomEditType>
void __fastcall SpellingSetup<CustomEditType>::OnChangeWrapper(TObject* Sender)
{
  if (ISDELIM(_word_state.PressedKey))
  {
    _word_state.Range.StartPos = 0;
    _word_state.Range.Length = _object_speller->ToStdString().size();
    _object_speller->PerformSpell(_word_state.Range);
  }

  /*int CurPosDiff = _object->SelStart - _word_state.CursorPos;
  
  if (CurPosDiff < -1)
  {
    MessageBox(NULL, L" ак ты это сделал?", L"Ёй.", MB_OK);
    return;
  }
  
  if (!_word_state.Misspelled && !ISDELIM(_word_state.PressedKey))
    return;
  
  _word_state.Range.Length += CurPosDiff - 1;
  _object_speller->PerformSpell(_word_state.Range);*/
}

template<typename CustomEditType>
void SpellingSetup<CustomEditType>::shell()
{
  _object_speller->MarkAsMisspell(_object_speller->FindTextRange());
}
