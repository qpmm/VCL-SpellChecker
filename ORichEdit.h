#ifndef ORichEditH
#define ORichEditH

#include <vcl.h>
#include <richole.h>
#include <tom.h>

class NumRange
{
  public:
    NumRange();
    NumRange(long start, long end);
    
    int ToInt();
    static NumRange FromInt(int src);

    long Length();

    long Start;
    long End;
};

class ORichEdit
{
  public:
    ORichEdit(TRichEdit* Component);
    ~ORichEdit();

    __property NumRange SelRange = {read = GetSelRange, write = SetSelRange};
    __property NumRange TextRange = {read = GetTextRange, write = SetTextRange};

    NumRange        GetSelRange();
    NumRange        GetTextRange();
    UnicodeString&  GetText(UnicodeString& dest);
    UnicodeString&  GetTextFromRange(NumRange Range, UnicodeString& dest);
    int             GetSelColor();
    int             GetTextColor();

    void            SetSelRange(NumRange Range);
    void            SetTextRange(NumRange Range);
    void            SetText(wchar_t* str);
    void            SetTextInRange(NumRange Range, wchar_t* str);
    void            SetTextColor(int color);

    NumRange        GetTextBounds();
    UnicodeString&  GetFullText(UnicodeString& dest);

  private:
    ITextDocument*  doc;
    ITextRange*     txt;
    ITextFont*      style;
};

#endif
