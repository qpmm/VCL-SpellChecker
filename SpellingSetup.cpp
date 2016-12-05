#include "SpellingSetup.h"

SpellingSetup::SpellingSetup()
{
}

SpellingSetup::~SpellingSetup()
{
  for (int i = 0; i < setup_pool.size(); ++i)
  {
    setup_pool[i]->Disable();
    delete setup_pool[i];
  }
}

void SpellingSetup::Add(TRichEdit* component)
{
  RichEditSetup* setup = NULL;

  try
  {
    setup = new RichEditSetup(component);
    setup_pool.push_back(setup);
  }
  catch (...)
  {
    TBalloonHint* warning = new TBalloonHint(this);
    warning->Title = "Ошибка запуска проверки орфографии";
    warning->Description = "Щелкните для скрытия уведомления";
    warning->ShowHint(component->ClientRect);
    if (setup)
      setup->Disable();
  }
}
