#include "speller.h"

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
  Result = ParseJSON(MakeRequest(Text));
}

bool YandexSpeller::CheckWord(std::wstring Word)
{
  return (MakeRequest(Word)->Size() == 0);
}

void YandexSpeller::clear()
{
  Result.clear();
}

void YandexSpeller::ParseJSON(TJSONArray* Content)
{
  Result.resize(Content->Size());
  
  for (unsigned i = 0; i < Result.size(); ++i)
  {
    TJSONObject* item = (TJSONObject*)(Content->Get(i));

    Result[i].code = ((TJSONNumber*)(item->Get("code")->JsonValue))->AsInt;
    Result[i].pos  = ((TJSONNumber*)(item->Get("pos") ->JsonValue))->AsInt;
    Result[i].row  = ((TJSONNumber*)(item->Get("row") ->JsonValue))->AsInt;
    Result[i].col  = ((TJSONNumber*)(item->Get("col") ->JsonValue))->AsInt;
    Result[i].len  = ((TJSONNumber*)(item->Get("len") ->JsonValue))->AsInt;
    Result[i].word = ((TJSONString*)(item->Get("word")->JsonValue))->ToString().c_str();
    Result[i].s    = ((TJSONArray* )(item->Get("s")   ->JsonValue))->ToString().c_str();
    
    /*TJSONArray* suggestions = (TJSONArray*)(item->Get("s")->JsonValue);

    Result[i].s.resize(suggestions->Size());

    for (unsigned j = 0; j < Result[i].s.size(); ++j)
    {
      Result[i].s[j] = suggestions->Get(j)->ToString().c_str();
    }*/
  }
}
