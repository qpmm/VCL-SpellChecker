#ifndef ORichEditH
#define ORichEditH

#include <vcl.h>
#include <richole.h>
#include <tom.h>
#include <string>

class TextAttributes
{
  public:
    TextAttributes(ITextRange** range);

    __property int Color = {read = GetColor, write = SetColor};
    //__property int Style = {read = GetStyle, write = SetStyle};

    int GetColor();
    void SetColor(int color);

    //int GetStyle();
    //void SetStyle(int style);

  private:
    ITextRange**  _range;
    ITextFont*    _style;
};

class ORichEdit
{
  public:
    ORichEdit(TRichEdit* Component);
    ~ORichEdit();
    
    int GetTextLen();

    __property int SelStart = {read = GetSelStart, write = SetSelStart};
    __property int SelLength = {read = GetSelLength, write = SetSelLength};
    __property std::wstring SelText = {read = GetSelText, write = SetSelText};
    __property std::wstring Text = {read = GetText, write = SetText};
    
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

    TextAttributes*  SelAttributes;
    TRichEdit*       Object;
  
  //private:
    ITextDocument*   _doc;
    ITextSelection*  _sel;
    ITextRange*      _range;
};

#endif