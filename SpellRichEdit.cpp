#include "SpellRichEdit.h"

TRichEdit* SpellRichEdit::richedit()
{
  return static_cast<TRichEdit*>(edit);
}

/*void SpellRichEdit::WordBounds(int Pos, int& Start, int& Length)
{
  Start = richedit()->Perform(EM_FINDWORDBREAK, WB_MOVEWORDLEFT, Pos);
  Length = richedit()->Perform(EM_FINDWORDBREAK, WB_LEFT, richedit()->Perform(EM_FINDWORDBREAK, WB_MOVEWORDRIGHT, Pos));
  //Length = richedit()->Perform(EM_FINDWORDBREAK, WB_MOVEWORDRIGHT, Pos);
  //int a = richedit()->Perform(EM_FINDWORDBREAK, WB_LEFT, Length);
  //int b = richedit()->Perform(EM_FINDWORDBREAK, WB_LEFTBREAK, Length);
  //int c = richedit()->Perform(EM_FINDWORDBREAK, WB_MOVEWORDLEFT, Length);
  Length -= Start;
}*/

void SpellRichEdit::MarkAsMistake(int Start, int Length)
{
  int CurPos = richedit()->SelStart;

  richedit()->Lines->BeginUpdate();
  
  richedit()->SelStart = Start;
  richedit()->SelLength = Length;
  richedit()->SelAttributes->Color = clRed;
  
  richedit()->SelStart = CurPos;
  richedit()->SelAttributes->Color = clBlack;

  richedit()->Lines->EndUpdate();
}

void SpellRichEdit::UnmarkAsMistake(int Start, int Length)
{
  int CurPos = richedit()->SelStart;

  richedit()->Lines->BeginUpdate();
  
  richedit()->SelStart = Start;
  richedit()->SelLength = Length;
  richedit()->SelAttributes->Color = clBlack;
  
  richedit()->SelStart = CurPos;
  richedit()->Lines->EndUpdate();
}

bool SpellRichEdit::IsMistakeUnderCursor()
{
  return (richedit()->SelAttributes->Color == clRed);
}

