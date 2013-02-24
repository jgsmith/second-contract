# include <system.h>

string lower_case(string str) {
  int i, n;

  for(i = 0, n = strlen(str); i < n; i++)
    if(str[i] >= 'A' && str[i] <= 'Z') str[i] -= 'A'-'a';

  return str;
}
