# include <devlib.h>

/*
 * Handles ordering/tracking events.
 */

object *events;
object current_event;

void heart_beat();

void create() {
  events = ({ });
  heart_beat();
}

atomic object EVENT_DATA create_event(string type) {
  object e;

  e = new_object(EVENT_DATA);
  e -> set_type(type);
  e -> set_object(previous_object());
  return e;
}

/* Use queue_event when you don't care about the return value. This lets
 * the system run the event later when the current thread is finished.
 */
atomic int queue_event(object e) {
  if(current_event && e->get_event_type() == current_event->get_event_type() && e->get_object() == current_event->get_object()) return 0;
  events += ({ e });
  return 1;
}

/* Use call_event when you care about the return value or you can't wait
 * until later. This is not accessible from within event scripts.
 * This is mainly useful in verb processing when we may want to call an
 * event such as pre-action:eat-dob to see if the direct object will (or
 * should) respond to an action:eat event.
 */
atomic mixed call_event(object e) {
  object ce;
  mixed ret;
  if(current_event && e->get_event_type() == current_event->get_event_type() && e->get_object() == current_event->get_object()) return FALSE;
  ce = current_event;
  current_event = e;
  ret = e -> get_object() -> call_event_handler(e->get_event_type(), e->get_args() + ([ "this": e->get_object() ]));
  current_event = ce;
  return ret;
}

atomic void heart_beat() {
  call_out("heart_beat", 0.5);
  while(sizeof(events)) {
    current_event = events[0];
    events = events[1..];
    current_event -> get_object() -> call_event_handler(current_event->get_event_type(), current_event->get_args() + ([ "this": current_event->get_object() ]));
    current_event = nil;
  }
}
