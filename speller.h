#ifndef spellerH
#define spellerH

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
  
    TJSONArray* makeRequest(std::wstring content);
    void checkText(std::wstring text);
    bool checkWord(std::wstring word);
    void clear();
    
    std::vector<ResponseObject> ParseJSON(TJSONArray* content);
    std::vector<ResponseObject> result;
    
  private:
  
    TIdHTTP* HTTPModule;
    TStringStream* Buffer;
    std::wstring UrlTemplate;
    int UrlLen;
};

#endif