# include <worldlib.h>
# include <kernel/kernel.h>

inherit thing THING_LIB;

/*
 * Not much else needed.
 */

object iflib_driver;

static void create(varargs int clone) {
  if(clone) {
    thing::create(clone);
  }
}

void set_iflib_driver(object x) {
  if(!SYSTEM()) error("Illegal setting of IFLib driver.");
  iflib_driver = x;
}

void message(string x) {
  if(iflib_driver) iflib_driver -> message(x);
}
