#ifndef DefinesH
#define DefinesH

#define CURRENT_POS -1

#define ISMODIF(x) (isprint((x), std::locale("Russian_Russia.1251")) || (x) == VK_DELETE || (x) == VK_BACK)
#define ISDELIM(x) (isspace((x), std::locale("Russian_Russia.1251")) || ispunct((x), std::locale("Russian_Russia.1251")))
#define ISALNUM(x) (isalnum((x), std::locale("Russian_Russia.1251")))
#define ISSPACE(x) (isspace((x), std::locale("Russian_Russia.1251")))

extern bool OnChangeBlocked;

#endif