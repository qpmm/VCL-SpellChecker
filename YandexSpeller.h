#ifndef YandexSpellerH
#define YandexSpellerH

#include <IdHTTP.hpp>
#include <DBXJSON.hpp>
#include <string>
#include <vector>

struct ResponseObject
{
  int code;
  int pos;
  int row;
  int col;
  int len;
  std::wstring word;
  std::vector<std::wstring> s;
};

class YandexSpeller
{
  public:
    YandexSpeller();
    ~YandexSpeller();
  
    TJSONArray* MakeRequest(std::wstring Content);
    void ParseJSON(TJSONArray* Content);
    void CheckText(std::wstring Text);
    bool CheckWord(std::wstring Word);
    void Clear();

    std::vector<ResponseObject> Result;
    
  private:
    TIdHTTP*        _httpModule;
    TStringStream*  _buffer;
    std::wstring    _urlTemplate;
    int             _urlLen;
};

#endif