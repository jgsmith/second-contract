# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <config.h>
# include <system.h>
# include <gamelib.h>

static int create(varargs int clone) {
  string *files;
  int i, n;

  if(!find_object(EVENT_CODE_D)) compile_object(EVENT_CODE_D);
}
