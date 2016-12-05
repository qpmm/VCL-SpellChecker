#include "RichEditSpell.h"

ContextMenu::ContextMenu(YandexSpeller* speller)
{
  _speller = speller;

  for (int i = 0; i < 6; ++i)
    _defaults[i] = new TMenuItem(_owner->_component->PopupMenu);

  _defaults[0]->Caption = "Отменить";
  _defaults[1]->Caption = "Вырезать";
  _defaults[2]->Caption = "Копировать";
  _defaults[3]->Caption = "Вставить";
  _defaults[4]->Caption = "Удалить";
  _defaults[5]->Caption = "Выделить все";

  _defaults[0]->OnClick = Undo;
  _defaults[1]->OnClick = Cut;
  _defaults[2]->OnClick = Copy;
  _defaults[3]->OnClick = Paste;
  _defaults[4]->OnClick = Delete;
  _defaults[5]->OnClick = SelectAll;
}

ContextMenu::~ContextMenu()
{
  for (int i = 0; i < 6; ++i)
    delete _defaults[i];
}

void ContextMenu::Create(Range& word)
{
  TMenuItem* item;

  _owner->_component->PopupMenu->Items->Clear();
  _owner->_component->PopupMenu->Tag = word.ToInt();

  if (_owner->_speller.Result.Length != 0)
  {
    DynamicArray<UnicodeString>& suggs = _owner->_speller.Result[0].s;

    if (suggs.Length == 0)
    {
      item = new TMenuItem(_owner->_component->PopupMenu);

      item->Caption = L"Варианты отсутствуют";
      item->Enabled = false;

      _owner->_component->PopupMenu->Items->Add(item);
    }

    for (int i = 0; i < suggs.Length; ++i)
    {
      item = new TMenuItem(_owner->_component->PopupMenu);

      item->Caption = suggs[i];
      item->OnClick = OnMenuItemClick;

      _owner->_component->PopupMenu->Items->Add(item);
    }

    _owner->_component->PopupMenu->Items->InsertNewLineAfter(item);
  }

  for (int i = 0; i < 6; ++i)
  {
    item = _defaults[i];
    _owner->_component->PopupMenu->Items->Add(item);

    if (i == 0 || i == 4)
      _owner->_component->PopupMenu->Items->InsertNewLineAfter(item);
  }
}

void __fastcall ContextMenu::OnMenuItemClick(TObject* Sender)
{
  TMenuItem* item = (TMenuItem*)Sender;
  Range range = Range::FromInt(item->Owner->Tag);

  item->Owner->Tag = 0;
  OnChangeBlocked = true;
  _owner->UnmarkAsMisspell(range);
  _owner->ole->SetTextInRange(range, item->Caption.c_str());
  OnChangeBlocked = false;
}

void __fastcall ContextMenu::Undo(TObject* Sender)
{
  _owner->_component->Undo();
}

void __fastcall ContextMenu::Cut(TObject* Sender)
{
  _owner->_component->CutToClipboard();
}

void __fastcall ContextMenu::Copy(TObject* Sender)
{
  _owner->_component->CopyToClipboard();
}

void __fastcall ContextMenu::Paste(TObject* Sender)
{
  _owner->_component->PasteFromClipboard();
}

void __fastcall ContextMenu::Delete(TObject* Sender)
{
  _owner->_component->ClearSelection();
}

void __fastcall ContextMenu::SelectAll(TObject* Sender)
{
  _owner->_component->SelectAll();
}

RichEditSpell::RichEditSpell(TRichEdit* edit)
{
  component = edit;
  ole = new ORichEdit(component);
}

RichEditSpell::~RichEditSpell()
{
  delete ole;
}

void RichEditSpell::FindTextRange(Range& range)
{
  range = FindTextRange(ole->SelRange.Start);
}

Range RichEditSpell::FindTextRange(int pos)
{
  int start, end;

  std::wstring buf = ole->GetFullText();

  for (start = pos; start - 1 > 0 && ISALNUM(buf[start - 1]); --start);
  for (end = pos; end < buf.size() && ISALNUM(buf[end]); ++end);

  return Range(start, end);
}

bool RichEditSpell::IsCorrect(int pos)
{
  if (pos != CURRENT_POS)
  {
    ole->SetTextRange(Range(pos, pos));
    return (ole->GetTextColor() != clRed);
  }

  return (ole->GetSelColor() != clRed);
}

void RichEditSpell::MarkAsMisspell(Range range)
{
  OnChangeBlocked = true;
  ole->TextRange = range;
  ole->SetTextColor(clRed);
  OnChangeBlocked = false;
}

void RichEditSpell::UnmarkAsMisspell(Range range)
{
  OnChangeBlocked = true;
  ole->TextRange = range;
  ole->SetTextColor(tomAutoColor);
  OnChangeBlocked = false;
}

void RichEditSpell::PerformSpell(Range range)
{
  if (range.Length() == 0)
    return;

  speller.CheckText(ole->GetTextFromRange(range));

  Range word;
  for (unsigned i = 0; i < speller.Result.size(); ++i)
  {
    word.Start = speller.Result[i].pos + range.Start;
    word.End   = speller.Result[i].len + word.Start;
    MarkAsMisspell(word);
  }
}

std::vector<std::wstring>& RichEditSpell::GetSuggestions(int pos)
{
  return speller.Result[0].s;
}
