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
  
    TJSONArray* MakeRequest(std::wstring content);
    void ParseJSON(TJSONArray* content);
    void CheckText(std::wstring text);

    std::vector<ResponseObject> Result;
    
  private:
    TIdHTTP*        httpModule;
    TStringStream*  buffer;
    std::wstring    urlTemplate;
    int             urlLength;
};

#endif
