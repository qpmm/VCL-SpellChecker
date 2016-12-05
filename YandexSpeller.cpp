#include "YandexSpeller.h"

YandexSpeller::YandexSpeller()
{
  httpModule = new TIdHTTP(NULL);
  urlTemplate.reserve(4096);
  urlTemplate = L"http://speller.yandex.net/services/spellservice.json/checkText?text=";
  urlLength = urlTemplate.length();
  buffer = new TStringStream;
}

YandexSpeller::~YandexSpeller()
{
  delete httpModule;
  delete buffer;
}

TJSONArray* YandexSpeller::MakeRequest(std::wstring content)
{
  buffer->Clear();
  urlTemplate.append(content);
  httpModule->Get(TIdURI::URLEncode(urlTemplate.c_str()), buffer);
  httpModule->Disconnect();
  urlTemplate.resize(urlLength);

  return (TJSONArray*)(TJSONObject::ParseJSONValue(buffer->DataString));
}

void YandexSpeller::CheckText(std::wstring text)
{
  ParseJSON(MakeRequest(text));
}

void YandexSpeller::ParseJSON(TJSONArray* content)
{
  if (content->Size() == 0)
  {
    Result.clear();
    return;
  }

  Result.resize(content->Size());
  
  for (unsigned i = 0; i < Result.size(); ++i)
  {
    TJSONObject* item = (TJSONObject*)(content->Get(i));

    Result[i].code = ((TJSONNumber*)(item->Get(0)->JsonValue))->AsInt; // code
    Result[i].pos  = ((TJSONNumber*)(item->Get(1)->JsonValue))->AsInt; // pos
    Result[i].row  = ((TJSONNumber*)(item->Get(2)->JsonValue))->AsInt; // row
    Result[i].col  = ((TJSONNumber*)(item->Get(3)->JsonValue))->AsInt; // col
    Result[i].len  = ((TJSONNumber*)(item->Get(4)->JsonValue))->AsInt; // len
    Result[i].word = ((TJSONString*)(item->Get(5)->JsonValue))->ToString().c_str(); // word

    TJSONArray* suggestions = (TJSONArray*)(item->Get(6)->JsonValue); // s
    Result[i].s.resize(suggestions->Size());

    for (unsigned j = 0; j < Result[i].s.size(); ++j)
    {
      Result[i].s[j] = ((TJSONString*)suggestions->Get(j))->Value().c_str();
    }
  }
}
