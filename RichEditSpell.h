#ifndef RichEditSpellH
#define RichEditSpellH

#include <vcl.h>
#include <locale>

#include "YandexSpeller.h"
#include "ORichEdit.h"
#include "defines.h"

class ContextMenu
{
  public:
    ContextMenu();
    ~ContextMenu();

    void Create(TPopupMenu* menu, DynamicArray<UnicodeString>& suggs);
    void __fastcall OnMenuItemClick(TObject* Sender);

    void __fastcall Undo(TObject* Sender);
    void __fastcall Cut(TObject* Sender);
    void __fastcall Copy(TObject* Sender);
    void __fastcall Paste(TObject* Sender);
    void __fastcall Delete(TObject* Sender);
    void __fastcall SelectAll(TObject* Sender);

  private:
    TMenuItem* _defaults[6];
};

class RichEditSpell
{
  public:
    RichEditSpell(TRichEdit* edit);
    ~RichEditSpell();

    void  FindTextRange(Range& range);
    Range FindTextRange(int pos);

    bool IsCorrect(int pos = CURRENT_POS);
    void MarkAsMisspell(Range range);
    void UnmarkAsMisspell(Range range);
    void PerformSpell(Range range);

    std::vector<std::wstring>& GetSuggestions(int pos);

    ORichEdit* ole;

  private:
    TRichEdit*     component;
	  YandexSpeller  speller;
};

#endif
