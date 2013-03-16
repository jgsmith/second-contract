# include <iflib.h>
# include <worldlib.h>

/*
 * Holds information about an observable aspect of something. Details can not
 * be manipulated (e.g., taken or dropped).
 */

inherit detail DETAIL_DATA_LIB;

void create(varargs int clone) {
  detail::create();
}

mixed get_property(string *path) { return detail::get_property(path); }

int set_property(string *path, mixed value) {
  return detail::set_property(path, value);
}
