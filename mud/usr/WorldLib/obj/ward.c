# include <worldlib.h>
# include <worldlib/hospital.h>

inherit ward WARD_LIB;

static void create(varargs int clone) {
  ward::create();
}
