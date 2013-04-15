# include <devlib.h>
# include <data.h>
# include <kernel/kernel.h>

/*
 * We want to manage action event handling here
 */

mapping event_handler_source;

static void create(varargs int clone) {
  if(clone) {
    event_handler_source = ([ ]);
  }
}

void destructed(int clone) {
  if(SYSTEM() && clone) {
    EVENT_SCRIPT_D -> remove_handlers();
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
  if(!s) return TRUE;
  if(event_handler_source) s = event_handler_source + s;
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

object build_event_set(string action, mapping args) {
  object estmp, e;

  estmp = new_object(EVENT_SET);
  e = new_object(EVENT_DATA);
  e -> set_event_type("pre-"+action);
  e -> set_object(this_object());
  e -> set_args(args + ([ "actor": this_object() ]));
  estmp -> add_guard(e);
  e = new_object(e);
  e -> set_event_type(action);
  estmp -> add_consequent(e);
  e = new_object(e);
  e -> set_event_type("post-" + action);
  estmp -> add_reaction(e);
  return estmp;
}

object build_event_sequence(string *actions, mapping args) {
  object es, e;
  int i, n;

  for(i = 0, n = sizeof(actions); i < n; i++) {
    e = build_event_set(actions[i], args);
    if(!es) es = e;
    else es -> add_next(e);
  }
  return es;
}
