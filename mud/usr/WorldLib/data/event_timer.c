# include <worldlib.h>
# include <devlib.h>
# include <data.h>

/*
 * Manages periodic (or not) events for things
 */

object target;
string event;
int frequency;
int periodic;

void create(varargs int clone) {
  if(clone) {
  }
}

void set_target(object t) { target = t; }
void set_event(string e) { event = e; }
void set_frequency(int i) { frequency = i; }
void set_periodic(int p) { periodic = !!p; }

object get_target() { return target; }
string get_event() { return event; }
int get_frequency() { return frequency; }
int get_periodic() { return periodic; }

/*
 * Creates a DevLib event for this timer
 */
void trigger() {
  object EVENT_DATA e;

  e = EVENTS_D -> create_event(event);
  e -> set_object(target);
  e -> set_args(([ ]));
  EVENTS_D -> queue_event(e);
}
