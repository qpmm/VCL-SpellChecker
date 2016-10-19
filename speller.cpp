#include "speller.h"

YandexSpeller::YandexSpeller()
{
  HTTPModule = new TIdHTTP(NULL);
  UrlTemplate.reserve(4096);
  UrlTemplate = L"http://speller.yandex.net/services/spellservice.json/checkText?text=";
  UrlLen = UrlTemplate.length();
  Buffer = new TStringStream;
}

YandexSpeller::~YandexSpeller()
{
  delete HTTPModule;
  delete Buffer;
}

TJSONArray* YandexSpeller::makeRequest(std::wstring content)
{
  Buffer->Clear();
  UrlTemplate.append(content);
  HTTPModule->Get(TIdURI::URLEncode(UrlTemplate.c_str()), Buffer);
  HTTPModule->Disconnect();
  UrlTemplate.resize(UrlLen);

  return (TJSONArray*)(TJSONObject::ParseJSONValue(Buffer->DataString));
}

void YandexSpeller::checkText(std::wstring text)
{
  result = ParseJSON(makeRequest(text));
}

bool YandexSpeller::checkWord(std::wstring word)
{
  return (makeRequest(word)->Size() == 0);
}

void YandexSpeller::clear()
{
  result.clear();
}

std::vector<ResponseObject> YandexSpeller::ParseJSON(TJSONArray* content)
{
  std::vector<ResponseObject> result;

  result.resize(content->Size());
  
  for (unsigned i = 0; i < result.size(); ++i)
  {
    TJSONObject* item = (TJSONObject*)(content->Get(i));

    result[i].code = ((TJSONNumber*)(item->Get("code")->JsonValue))->AsInt;
    result[i].pos  = ((TJSONNumber*)(item->Get("pos")->JsonValue))->AsInt;
    result[i].row  = ((TJSONNumber*)(item->Get("row")->JsonValue))->AsInt;
    result[i].col  = ((TJSONNumber*)(item->Get("col")->JsonValue))->AsInt;
    result[i].len  = ((TJSONNumber*)(item->Get("len")->JsonValue))->AsInt;
    result[i].word = ((TJSONString*)(item->Get("word")->JsonValue))->ToString().c_str();
    
    TJSONArray* suggestions = (TJSONArray*)(item->Get("s")->JsonValue);

    result[i].s.resize(suggestions->Size());

    for (unsigned j = 0; j < result[i].s.size(); ++j)
    {
      result[i].s[j] = suggestions->Get(j)->ToString().c_str();
    }
  }
  
  return result;
}
