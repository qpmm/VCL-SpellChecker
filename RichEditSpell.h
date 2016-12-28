#ifndef RichEditSpellH
#define RichEditSpellH

#include <vcl.h>

#include "YandexSpeller.h"
#include "ORichEdit.h"
#include "defines.h"

class RichEditSpell;

class CustomContextMenu
{
  public:
    CustomContextMenu(RichEditSpell* Owner);
    ~CustomContextMenu();

    void Create(NumRange Word, bool hasSuggestions, bool selLen);
    void Show(int X, int Y);

    void Create(DynamicArray<UnicodeString>* Suggestions);
    void RemoveAllItems();

    void __fastcall OnMenuItemClick(TObject* Sender);

    void __fastcall Cut(TObject* Sender);
    void __fastcall Copy(TObject* Sender);
    void __fastcall Paste(TObject* Sender);
    void __fastcall Delete(TObject* Sender);
    void __fastcall SelectAll(TObject* Sender);

  private:
    RichEditSpell*  owner;
    TPopupMenu*     popupmenu;
    TMenuItem*      defaults[5];
};

class RichEditSpell
{
  friend class CustomContextMenu;

  public:
    RichEditSpell(TRichEdit* Component);
    ~RichEditSpell();

    void FindTextRange(NumRange& Range);
    NumRange FindTextRange(int Pos);

    bool IsCorrect(int Pos = CURRENT_POS);
    void MarkAsMisspell(NumRange Range);
    void UnmarkAsMisspell(NumRange Range);
    void PerformSpell(NumRange Range);

    ORichEdit* Ole;
    CustomContextMenu* ContextMenu;

  private:
    TRichEdit*     component;
	  YandexSpeller  speller;
};

#endif
