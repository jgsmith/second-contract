# include <iflib/binder.h>

int ordinal;
int number_included;
int *fraction;
int ignore_rest;
int no_ambiguous;

object him;
object her;
object it;
object *plural;

void create(varargs int clone) {
  if(clone) {
    plural = ({ });
  }
}

object get_him() { return him; }
object get_her() { return her; }
object get_it() { return it; }

int update_number(int num, int singular) {
  if(ordinal) {
    if(ordinal == -1) return 1;
    if(ordinal > num) {
      ordinal -= num;
      return 0;
    }
    ignore_rest = 1;
    return 1;
  }

  if(number_included) {
    if(number_included <= num) {
      ignore_rest = 1;
      num = number_included;
      number_included = 0;
      return num;
    }
    number_included -= num;
    return num;
  }

  if(num > 0 && (singular & BINDER_MATCH_SINGULAR)) return 1;

  return num;
}
