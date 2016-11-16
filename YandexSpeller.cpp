#include "YandexSpeller.h"

YandexSpeller::YandexSpeller()
{
  _http_module = new TIdHTTP(NULL);
  _url_template.reserve(4096);
  _url_template = L"http://speller.yandex.net/services/spellservice.json/checkText?text=";
  _url_len = _url_template.length();
  _buffer = new TStringStream;
}

YandexSpeller::~YandexSpeller()
{
  delete _http_module;
  delete _buffer;
}

TJSONArray* YandexSpeller::MakeRequest(std::wstring Content)
{
  _buffer->Clear();
  _url_template.append(Content);
  _http_module->Get(TIdURI::URLEncode(_url_template.c_str()), _buffer);
  _http_module->Disconnect();
  _url_template.resize(_url_len);

  return (TJSONArray*)(TJSONObject::ParseJSONValue(_buffer->DataString));
}

void YandexSpeller::CheckText(std::wstring Text)
{
  ParseJSON(MakeRequest(Text));
}

bool YandexSpeller::CheckWord(std::wstring Word)
{
  return (MakeRequest(Word)->Size() == 0);
}

void YandexSpeller::Clear()
{
  Result.clear();
}

void YandexSpeller::ParseJSON(TJSONArray* Content)
{
  if (Content->Size() == 0)
  {
    Result.clear();
    return;
  }

  Result.resize(Content->Size());
  
  for (unsigned i = 0; i < Result.size(); ++i)
  {
    TJSONObject* item = (TJSONObject*)(Content->Get(i));

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
