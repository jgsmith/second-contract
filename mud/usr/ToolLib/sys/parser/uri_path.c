# include <toollib.h>
# include <type.h>

void create(varargs int clone) {
}

mixed parse(string uri) {
  string *bits;
  int i, n;
  int score;

  if(uri[0] != '/') return nil;

  bits = explode(uri, "/") - ({ "", nil });
  score = 0;

  for(i = 0, n = sizeof(bits); i < n; i++) {
    if(bits[i][0] == ':') score += 1;
    else score += 3;
  }
  return ({ score }) + bits;
}
