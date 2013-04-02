# include <devlib.h>

/*
 * We want to manage action event handling here
 */

mapping event_handler_source;

static void create(varargs int clone) {
  if(clone) {
    event_handler_source = ([ ]);
  }
}

/* returns the list of event handlers defined with this object */
string *get_event_handlers() {
  return EVENT_SCRIPT_D -> get_event_handlers();
}

mapping get_event_handler_source() {
  return event_handler_source;
}

atomic mixed set_event_handler_source(mapping s) {
  mixed ret;
  s = event_handler_source + s;
  if((ret = EVENT_SCRIPT_D -> set_event_handlers(s)) == 1) {
    event_handler_source = s;
    return TRUE;
  }
  return ret;
}

mixed call_ur_event_handler(string event, mapping args) {
  object ur;
  if(ur = this_object()->get_ur_object()) return ur -> call_event_handler(event,args);
}

mixed call_event_handler(string event, mapping args) {
  return EVENT_SCRIPT_D -> run_event_handler(event, args);
}
