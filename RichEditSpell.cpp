#include "RichEditSpell.h"

TRichEdit* RichEditSpell::richedit()
{
  return static_cast<TRichEdit*>(Component);
}

/*void RichEditSpell::WordBounds(int Pos, int& Start, int& Length)
{
  Start = richedit()->Perform(EM_FINDWORDBREAK, WB_MOVEWORDLEFT, Pos);
  Length = richedit()->Perform(EM_FINDWORDBREAK, WB_LEFT, richedit()->Perform(EM_FINDWORDBREAK, WB_MOVEWORDRIGHT, Pos));
  //Length = richedit()->Perform(EM_FINDWORDBREAK, WB_MOVEWORDRIGHT, Pos);
  //int a = richedit()->Perform(EM_FINDWORDBREAK, WB_LEFT, Length);
  //int b = richedit()->Perform(EM_FINDWORDBREAK, WB_LEFTBREAK, Length);
  //int c = richedit()->Perform(EM_FINDWORDBREAK, WB_MOVEWORDLEFT, Length);
  Length -= Start;
}*/

void RichEditSpell::PerformSpell(std::wstring SubString, int Start)
{
  speller.checkText(SubString);

  richedit()->Lines->BeginUpdate();

  for (unsigned i = 0; i < speller.result.size(); ++i)
    MarkAsMistake(Start + speller.result[i].pos, speller.result[i].len);

  richedit()->Modified = false;
  richedit()->Lines->EndUpdate();
}

void RichEditSpell::MarkAsMistake(int Start, int Length)
{
  int CurPos = richedit()->SelStart;
  
  richedit()->SelStart = Start;
  richedit()->SelLength = Length;
  richedit()->SelAttributes->Color = clRed;

  richedit()->SelStart = CurPos;
  richedit()->SelAttributes->Color = clBlack;
}

void RichEditSpell::UnmarkAsMistake(int Start, int Length)
{
  int CurPos = richedit()->SelStart;

  richedit()->Lines->BeginUpdate();
  
  richedit()->SelStart = Start;
  richedit()->SelLength = Length;
  richedit()->SelAttributes->Color = clBlack;
  
  richedit()->SelStart = CurPos;
  richedit()->Modified = false;
  richedit()->Lines->EndUpdate();
}

bool RichEditSpell::IsMistakeUnderCursor()
{
  return (richedit()->SelAttributes->Color == clRed);
}

