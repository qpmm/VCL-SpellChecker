#include "SpellingSetup.h"

#define ISPRINT(x) isprint((x), std::locale("Russian_Russia.1251"))
#define ISALNUM(x) isalnum((x), std::locale("Russian_Russia.1251"))
#define ISDELIM(x) (isspace((x), std::locale("Russian_Russia.1251")) && ispunct((x), std::locale("Russian_Russia.1251")))

SpellingSetup::SpellingSetup()
{
  _object = NULL;
  _object_speller = NULL;
}

SpellingSetup::~SpellingSetup()
{
  delete _object_speller;
}

void SpellingSetup::Init(TCustomEdit* Component)
{
  _object = Component;
  _object->OnKeyDown = OnKeyDownWrapper;
  _object->OnKeyPress = OnKeyPressWrapper;
  _object->OnChange = OnChangeWrapper;
  
  if (_object->InheritsFrom(__classid(TRichEdit)))
    _object_speller = new RichEditSpell(_object);
  else if (_object->InheritsFrom(_classid(TCustomMemo)))
    _object_speller = new CustomMemoSpell(_object);
  else
    _object_speller = new CustomEditSpell(_object);
}

void __fastcall SpellingSetup::OnKeyDownWrapper(TObject* Sender, WORD &Key, TShiftState Shift)
{
  if (ISPRINT(Key))
  {
    _word_state.CursorPos = _object->SelStart;
    _word_state.WordRange = _object->FindTextRange();
    _word_state.PressedKey = '\0';
    _word_state.Misspelled = _object->IsMisspell(_word_state.Range.StartPos);
    
    if (_word_state.Misspelled)
      _object->UnmarkAsMisspell(_word_state.Range);
  }
}

void __fastcall SpellingSetup::OnKeyPressWrapper(TObject *Sender, wchar_t &Key)
{
  _word_state.PressedKey = Key;
}

void __fastcall SpellingSetup::OnChangeWrapper(TObject* Sender)
{
  int CurPosDiff = _object->SelStart - _word_state.CursorPos;
  
  if (CurPosDiff < -1)
  {
    MessageBox(NULL, " ак ты это сделал?", "Ёй.", MB_OK);
    return;
  }
  
  if (!_word_state.Misspelled && !ISDELIM(_word_state.PressedKey) || !_object->Modified)
    return;
  
  _word_state.Range.Length += CurPosDiff;
  _object_speller->PerformSpell(_word_state.Range);
}