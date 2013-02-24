# include <devlib.h>

static int create(varargs int clone) {
  if(!find_object(EVENT_DATA)) compile_object(EVENT_DATA);

  if(!find_object(EVENTS_D)) compile_object(EVENTS_D);
  if(!find_object(EVENT_SCRIPT_D)) compile_object(EVENT_SCRIPT_D);
}
