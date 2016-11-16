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
    void         SetSelRange(Range range);

    Range        GetTextRange();
    void         SetTextRange(Range range);

    std::wstring GetSelText();
    //void         SetSelText(std::wstring text);
    void         SetSelText(wchar_t* text);

    std::wstring GetText();
    //void         SetText(std::wstring text);
    void         SetText(wchar_t* text);

    std::wstring GetTextFromRange(Range range);
    //void         SetTextInRange(Range range, std::wstring text);
    void         SetTextInRange(Range range, wchar_t* text);

    int          GetSelColor();
    void         SetTextColor(int color);

    int          GetLength();

  private:
    ITextDocument*   _doc;
    ITextSelection*  _sel;
    ITextRange*      _range;
    ITextFont*       _style;
};

#endif