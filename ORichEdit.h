#ifndef ORichEditH
#define ORichEditH

#include <vcl.h>
#include <richole.h>
#include <tom.h>
#include <string>

class Range
{
  public:
    Range();
    Range(long start, long end);
    
    int  ToInt();
    static Range FromInt(int src);

    long Length();

    long Start;
    long End;
};

class ORichEdit
{
  public:
    ORichEdit(TRichEdit* component);

    __property Range SelRange = {read = GetSelRange, write = SetSelRange};
    __property Range TextRange = {read = GetTextRange, write = SetTextRange};
    __property Range SelText = {read = GetSelText, write = SetSelText};
    __property Range Text = {read = GetText, write = SetText};

    Range        GetSelRange();
    void         SetSelRange(Range r);

    Range        GetTextRange();
    void         SetTextRange(Range r);

    std::wstring GetSelText();
    void         SetSelText(wchar_t* text);

    std::wstring GetText();
    void         SetText(wchar_t* text);

    std::wstring GetTextFromRange(Range r);
    void         SetTextInRange(Range r, wchar_t* text);

    int          GetSelColor();
    int          GetTextColor();
    void         SetTextColor(int color);

    Range        GetTextBounds();
    std::wstring GetFullText();

  private:
    ITextDocument*   doc;
    ITextSelection*  sel;
    ITextRange*      range;
    ITextFont*       style;
};

#endif
