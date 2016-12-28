#include "YandexSpeller.h"

YandexSpeller::YandexSpeller()
{
  httpModule = new TIdHTTP(NULL);
  urlTemplate = L"http://speller.yandex.net/services/spellservice.json/checkText?text=";
  urlLength = urlTemplate.Length();
  buffer = new TStringStream;
}

YandexSpeller::~YandexSpeller()
{
  delete httpModule;
  delete buffer;
}

TJSONValue* YandexSpeller::MakeRequest(UnicodeString& text)
{
  UnicodeAppend(urlTemplate, text);
  httpModule->Get(TIdURI::URLEncode(urlTemplate), buffer);
  httpModule->Disconnect();
  urlTemplate.SetLength(urlLength);

  TJSONValue* response = TJSONObject::ParseJSONValue(buffer->DataString);
  buffer->Clear();

  return response;
}

void YandexSpeller::CheckText(UnicodeString text)
{
  TJSONValue* json = MakeRequest(text);
  ParseJSON((TJSONArray*)json);
  json->Free();
}

void YandexSpeller::ParseJSON(TJSONArray* json)
{
  if (json->Size() == 0)
  {
    Result.Length = 0;
    return;
  }

  Result.Length = json->Size();
  
  for (int i = 0; i < Result.Length; ++i)
  {
    TJSONObject* item = (TJSONObject*)(json->Get(i));

    Result[i].code = ((TJSONNumber*)item->Get(0)->JsonValue)->AsInt;      // code
    Result[i].pos  = ((TJSONNumber*)item->Get(1)->JsonValue)->AsInt;      // pos
    Result[i].row  = ((TJSONNumber*)item->Get(2)->JsonValue)->AsInt;      // row
    Result[i].col  = ((TJSONNumber*)item->Get(3)->JsonValue)->AsInt;      // col
    Result[i].len  = ((TJSONNumber*)item->Get(4)->JsonValue)->AsInt;      // len
    Result[i].word = ((TJSONString*)item->Get(5)->JsonValue)->ToString(); // word

    TJSONArray* suggestions = (TJSONArray*)item->Get(6)->JsonValue;       // s
    Result[i].s.Length = suggestions->Size();

    for (int j = 0; j < Result[i].s.Length; ++j)
      Result[i].s[j] = ((TJSONString*)suggestions->Get(j))->Value();
  }
}
