# include <devlib.h>

/*
 * This is the base inheritable for event handler collections.
 *
 * functions named _C_... are referenced by the script compiler.
 *
 * functions named __... are available as function calls in the script.
 * 
 * foo(i,j) => this_object() -> __foo(i,j)
 *
 */

mixed _C_event(object ob, string type, mapping args) {
  object EVENT_DATA e;

  e = EVENTS_D -> create_event(type);
  e -> set_type(type);
  e -> set_object(ob);
  e -> set_args(args);
  return EVENTS_D -> call_event(e);
}

void _C_event_async(object ob, string type, mapping args) {
  object EVENT_DATA e;

  e = EVENTS_D -> create_event(type);
  e -> set_type(type);
  e -> set_object(ob);
  e -> set_args(args);
  EVENTS_D -> queue_event(e);
}

