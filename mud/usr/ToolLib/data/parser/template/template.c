# include <toollib.h>
# include <toollib/template.h>
# include <type.h>

mixed *bits;

void create(varargs int clone) {
  if(clone) {
    bits = ({ });
  }
}

void add_bit(mixed b) {
  bits += ({ b });
}

/*
 * When we encounter the pov object, we replace its name with 'you' or some
 * suitable form of 'you'.
 */
string render(mapping data, varargs object pov) {
  int i, n, j;
  string *ret, str;

  ret = ({ });
  for(i = 0, n = sizeof(bits); i < n; i++) {
    switch(typeof(bits[i])) {
      case T_STRING: ret += ({ bits[i] }); break;
      case T_OBJECT: ret += ({ bits[i] -> render(data, pov) }); break;
    }
  }

  ret -= ({ nil });
  str = implode(ret, "");

  /* remove multiple spaces in a row so we don't have to worry about empty
   * variables and substitutions elsewhere.
   */
  for(i = 0, n = strlen(str)-1; i < n; i++) {
    if(str[i] == ' ') {
      j = i;
      while(j < n && str[j] == ' ') j++;
      str = str[0..i] + str[j..];
      n -= (j - i - 1);
    }
  }
  if(str[0] >= 'a' && str[0] <= 'z') str[0] += 'A' - 'a';
  for(i = 1, n = strlen(str)-2; i < n; i++)
    if((str[i] == '.' || str[i] == '!' || str[i] == '?') && str[i+1] == ' ' && str[i+2] >= 'a' && str[i+2] <= 'z') str[i+2] += 'A' - 'a';
  for(i = 1, n = strlen(str)-3; i < n; i++)
    if((str[i] == '.' || str[i] == '!' || str[i] == '?') && (str[i+1] == '"' || str[i+1] == '\'') && str[i+2] == ' ' && str[i+3] >= 'a' && str[i+3] <= 'z') str[i+3] += 'A' - 'a';
  return str;
}
