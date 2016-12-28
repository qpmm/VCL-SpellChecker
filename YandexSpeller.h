#ifndef YandexSpellerH
#define YandexSpellerH

#include <IdHTTP.hpp>
#include <DBXJSON.hpp>

struct ResponseObject
{
  int code;
  int pos;
  int row;
  int col;
  int len;
  UnicodeString word;
  DynamicArray<UnicodeString> s;
};

class YandexSpeller
{
  public:
    YandexSpeller();
    ~YandexSpeller();
  
    TJSONValue* MakeRequest(UnicodeString& text);
    void ParseJSON(TJSONArray* json);
    void CheckText(UnicodeString text);

    DynamicArray<ResponseObject> Result;
    
  private:
    TIdHTTP*        httpModule;
    TStringStream*  buffer;
    UnicodeString   urlTemplate;
    int             urlLength;
};

#endif
