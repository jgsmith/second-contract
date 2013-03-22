# include <devlib.h>
# include <data.h>

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

atomic object EVENT_SET create_event_set() {
  return new_object(EVENT_SET);
}

atomic object EVENT_DATA create_event(string type) {
  object e;

  e = new_object(EVENT_DATA);
  e -> set_event_type(type);
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
mixed call_event(object e) {
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

atomic int run_event_set(object EVENT_SET set) {
  /* we want to run forward until no get_next - then run backward until no
   * get_prev
   */
  int guard_result;
  object EVENT_SET next;
  object EVENT_DATA *es;
  int i, n;

  next = set;

  while(next) {
    set = next;
    guard_result = set -> run_guards(1);
    if(guard_result == 0) return FALSE;
    if(guard_result < 0) {
      while(guard_result < 0 && (next = set -> get_previous())) {
        guard_result++;
        set = next;
      }
      next = set;
    }
    else {
      es = set -> get_consequents(1);
      for(i = 0, n = sizeof(es); i < n; i++) call_event(es[i]);
      es = set -> get_reactions(1);
      for(i = 0, n = sizeof(es); i < n; i++) call_event(es[i]);
      next = set -> get_next();
    }
  }
  next = set;
  while(next) {
    set = next;
    guard_result = set->run_guards(0);
    /* we're unwinding the events, so we accomplished the primary goal 
     * of the event set 
     */
    if(guard_result != 1) return TRUE; 
    es = set -> get_consequents(0);
    for(i = 0, n = sizeof(es); i < n; i++) call_event(es[i]);
    es = set -> get_reactions(0);
    for(i = 0, n = sizeof(es); i < n; i++) queue_event(es[i]);
    next = set -> get_previous();
  }
  return TRUE;
}
