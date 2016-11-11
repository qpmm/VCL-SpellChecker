#ifndef ORichEditH
#define ORichEditH

#include <richole.h>
#include <tom.h>
#include <string>

class OTextAttributes
{
  
};

class ORichEdit
{
  public:
    ORichEdit(TRichEdit* Component);
    
    __property int          SelStart = {read = GetSelStart, write = SetSelStart};
    __property int          SelLength = {read = GetSelLength, write = SetSelLength};
    __property std::wstring SelText = {read = GetSelText, write = SetSelText};
    
    __property int          RangeStart = {read = GetRangeStart, write = SetRangeStart};
    __property int          RangeLength = {read = GetSelLength, write = SetSelLength};
    __property std::wstring SelText = {read = GetSelText, write = SetSelText};
    
    __property std::wstring Text = {read = GetText, write = SetText};
    
    int GetTextLen();
    
    int GetSelStart();
    void SetSelStart(int start);
    int GetSelLength();
    void SetSelLength(int length);
    std::wstring GetSelText();
    void SetSelText(std::wstring text);
    void SetSelText(wchar_t* text);
    std::wstring GetText();
    void SetText(std::wstring text);
    void SetText(wchar_t* text);
    
    void GetTextRange(TextRange range);
    void SetTextRange(TextRange range, std::wstring text);
    void SetTextRange(TextRange range, wchar_t* text);
    
    TextRange GetSelRange();
    void SetSelRange(TextRange range);
    
    
  
  //private:
    ITextDocument*   _doc;
    ITextSelection*  _sel;
    ITextRange*      _range;
    ITextFont*       _style;
};

#endif