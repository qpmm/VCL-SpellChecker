#include "RichEditSpell.h"

#include <locale>
//#include <Clipbrd.hpp>

CustomContextMenu::CustomContextMenu(RichEditSpell* Owner)
{
  owner = Owner;
  owner->component->PopupMenu = new TPopupMenu(owner->component);
  popupmenu = owner->component->PopupMenu;
  popupmenu->AutoPopup = false;

  for (int i = 0; i < 5; ++i)
    defaults[i] = new TMenuItem(popupmenu);

  defaults[0]->Caption = "Вырезать";
  defaults[1]->Caption = "Копировать";
  defaults[2]->Caption = "Вставить";
  defaults[3]->Caption = "Удалить";
  defaults[4]->Caption = "Выделить все";

  defaults[0]->OnClick = Cut;
  defaults[1]->OnClick = Copy;
  defaults[2]->OnClick = Paste;
  defaults[3]->OnClick = Delete;
  defaults[4]->OnClick = SelectAll;
}

CustomContextMenu::~CustomContextMenu()
{
  // Удалит все элементы и освободит их память
  // Т.е. освобождать defaults не требуется
  popupmenu->Items->Clear();

  delete popupmenu;
  owner->component->PopupMenu = NULL;
}

void CustomContextMenu::Create(NumRange Word, bool hasSuggestions, bool selLen)
{
  TMenuItem* item;

  popupmenu->Tag = Word.ToInt();
  RemoveAllItems();

  if (hasSuggestions)
  {
    DynamicArray<UnicodeString>& sugs = owner->speller.Result[0].s;

    if (sugs.Length)
    {
      for (int i = 0; i < sugs.Length; ++i)
      {
        item = new TMenuItem(popupmenu);

        item->Caption = sugs[i];
        item->OnClick = OnMenuItemClick;

        popupmenu->Items->Add(item);
      }
    }
    else
    {
      item = new TMenuItem(popupmenu);

      item->Caption = L"Варианты отсутствуют";
      item->Enabled = false;

      popupmenu->Items->Add(item);
    }

    popupmenu->Items->InsertNewLineAfter(item);
  }

  for (int i = 0; i < 5; ++i)
  {
    item = defaults[i];

    if (i == 0 || i == 1 || i == 3)
    {
      if (selLen == true)
        item->Enabled = true;
      else
        item->Enabled = false;
    }

    popupmenu->Items->Add(item);

    if (i == 3)
      popupmenu->Items->InsertNewLineAfter(item);
  }
}

void CustomContextMenu::Show(int X, int Y)
{
  TPoint coord = owner->component->ClientToScreen(TPoint(X, Y));
  popupmenu->Popup(coord.X, coord.Y);
}

void CustomContextMenu::RemoveAllItems()
{
  for (TMenuItem* item; popupmenu->Items->Count != 0; )
  {
    item = popupmenu->Items->Items[0];

    popupmenu->Items->Remove(item);
    if (item->OnClick == OnMenuItemClick)
      delete item;
  }
}

void __fastcall CustomContextMenu::OnMenuItemClick(TObject* Sender)
{
  TMenuItem* item = (TMenuItem*)Sender;
  NumRange word = NumRange::FromInt(item->Owner->Tag);

  OnChangeBlocked = true;
  owner->UnmarkAsMisspell(word);
  owner->Ole->SetTextInRange(word, item->Caption.c_str());
  OnChangeBlocked = false;
}

void __fastcall CustomContextMenu::Cut(TObject* Sender)
{
  owner->component->CutToClipboard();
}

void __fastcall CustomContextMenu::Copy(TObject* Sender)
{
  owner->component->CopyToClipboard();
}

void __fastcall CustomContextMenu::Paste(TObject* Sender)
{
  owner->component->PasteFromClipboard();
  //owner->component->SelText = Clipboard()->AsText;
}

void __fastcall CustomContextMenu::Delete(TObject* Sender)
{
  owner->component->ClearSelection();
}

void __fastcall CustomContextMenu::SelectAll(TObject* Sender)
{
  owner->component->SelectAll();
}

RichEditSpell::RichEditSpell(TRichEdit* Component)
{
  component = Component;
  ContextMenu = new CustomContextMenu(this);
  Ole = new ORichEdit(component);
}

RichEditSpell::~RichEditSpell()
{
  delete ContextMenu;
  delete Ole;
}

void RichEditSpell::FindTextRange(NumRange& Range)
{
  Range = FindTextRange(Ole->SelRange.Start);
}

NumRange RichEditSpell::FindTextRange(int Pos)
{
  int start, end;
  UnicodeString text;

  Ole->GetFullText(text);
  wchar_t* buf = text.c_str();

  for (start = Pos; start - 1 >= 0 && ISALNUM(buf[start - 1]); --start);
  for (end = Pos; end < text.Length() && ISALNUM(buf[end]); ++end);

  return NumRange(start, end);
}

bool RichEditSpell::IsCorrect(int Pos)
{
  if (Pos != CURRENT_POS)
  {
    Ole->SetTextRange(NumRange(Pos, Pos));
    return (Ole->GetTextColor() != clRed);
  }

  return (Ole->GetSelColor() != clRed);
}

void RichEditSpell::MarkAsMisspell(NumRange Range)
{
  OnChangeBlocked = true;
  Ole->TextRange = Range;
  Ole->SetTextColor(clRed);
  OnChangeBlocked = false;
}

void RichEditSpell::UnmarkAsMisspell(NumRange Range)
{
  OnChangeBlocked = true;
  Ole->TextRange = Range;
  Ole->SetTextColor(tomAutoColor);
  OnChangeBlocked = false;
}

void RichEditSpell::PerformSpell(NumRange Range)
{
  if (Range.Length() == 0)
    return;

  UnicodeString text;
  speller.CheckText(Ole->GetTextFromRange(Range, text));

  NumRange word;
  for (int i = 0; i < speller.Result.Length; ++i)
  {
    word.Start = speller.Result[i].pos + Range.Start;
    word.End   = speller.Result[i].len + word.Start;
    MarkAsMisspell(word);
  }
}

