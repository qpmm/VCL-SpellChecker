#include <assert.h>
#include <cwchar>
#include <vcl.h>
#include "../CustomEditSpell.h"

class TestCustomEditSpell : public CustomEditSpell
{
  public:
    TestCustomEditSpell(TForm* Form, TEdit* Object);
    
    void DoTest();
  
    TEdit* _tedit;
    wchar_t* _text;
};

TestCustomEditSpell::TestCustomEditSpell(TForm* Form, TEdit* Object) : CustomEditSpell(Form, static_cast<TCustomEdit*>(Object))
{
  _tedit = Object;
  _text = L"Раз \n\r\n two , преверка!!!!, ,слово,,сливо,,, @СИМВОЛ@ \r\n $очепятка?";
  _tedit->Text = _text;
  
  DoTest();

  _tedit->Text = L"Все тесты успешно пройдены!";
}

void TestCustomEditSpell::DoTest()
{
  // TextRange::TextRange()
  // TextRange::TextRange(int, int)
  
  TextRange test1;
  TextRange test2(4, 11);
  
  assert(    test1.StartPos == -1
          && test1.Length   == -1);
  
  assert(    test2.StartPos == 4
          && test2.Length   == 11);
  
  // ToStdString()
  
  std::wstring test3 = ToStdString();
  
  assert(     wcslen(_text) == test3.length()
          && !wcscmp(_text, test3.c_str()));

  // ToStdString(TextRange)
  
  std::wstring test4 = ToStdString(test2);
  wchar_t* substr = L"\n\r\n two , п";
  
  assert(     wcslen(substr) == test4.length()
          && !wcscmp(substr, test4.c_str()));
  
  /* FindTextRange()
   *
   * Раз:      (0, 3)
   * two:      (8, 3)
   * преверка: (14, 8)
   * слово:    (29, 5)
   * сливо:    (36, 5)
   * СИМВОЛ:   (46, 6)
   * очепятка: (58, 8)
   */
  
  _tedit->SelStart = 14;
  assert(    FindTextRange() == TextRange(14, 8));
  
  _tedit->SelStart = 20;
  assert(    FindTextRange() == TextRange(14, 8));
  
  _tedit->SelStart = 22;
  assert(    FindTextRange() == TextRange(14, 8));
  
  _tedit->SelStart = 30;
  assert(    FindTextRange() == TextRange(29, 5));
  
  _tedit->SelStart = 52;
  assert(    FindTextRange() == TextRange(46, 6));
  
  _tedit->SelStart = 44;
  assert(    FindTextRange() == TextRange(-1, -1));
  
  // MarkAsMisspell
  // IsMisspell
  
  MarkAsMisspell(TextRange(8, 3));
  MarkAsMisspell(TextRange(36, 5));
  MarkAsMisspell(TextRange(58, 8));
  assert(    IsMisspell(36) == true);
  
  // UnmarkAsMisspell
  
  UnmarkAsMisspell(TextRange(58, 8));
  assert(    IsMisspell(58) == false);
  
  // CustomBeginUpdate
  
  CustomBeginUpdate();
  assert(    _current_sel.StartPos == _tedit->SelStart
        /*&& _current_sel.Length   == _tedit->SelLength*/);
  
  // PerformSpell
  
  PerformSpell(TextRange(14, 32));
  assert(    _speller.Result.size() == 2
          && IsMisspell(14) == true
          && IsMisspell(36) == true);
  
  // CustomEndUpdate
  
  CustomEndUpdate();
  assert(    _current_sel.StartPos == _tedit->SelStart
        /*&& _current_sel.Length   == _tedit->SelLength*/);
}
