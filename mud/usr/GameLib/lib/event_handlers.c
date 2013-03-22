# include <devlib.h>
# include <worldlib.h>
# include <data.h>

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

string source;

void set_source(string s) { source = s; }
string get_source() { return source; }

mixed _C_event(object ob, string type, mapping args) {
  object EVENT_DATA e;

  e = new_object(EVENT_DATA);
  e -> set_event_type(type);
  e -> set_object(ob);
  e -> set_args(args);
  return EVENTS_D -> call_event(e);
}

void _C_event_async(object ob, string type, mapping args) {
  object EVENT_DATA e;

  e = new_object(EVENT_DATA);
  e -> set_event_type(type);
  e -> set_object(ob);
  e -> set_args(args);
  EVENTS_D -> queue_event(e);
}

object SENSATION_SET _C_build_sensation_set(object owner, object SENSATION_DATA *sensations, varargs int delay) {
  object SENSATION_SET s;
  s = new_object(SENSATION_SET);
  s -> add_sensations(sensations);
  if(delay) s -> set_delay(delay);
  return s;
}

/* TODO: add type and intensity */
object SENSATION_DATA _C_build_sensation(object owner, string sense, mixed *msg, mapping args, varargs int duration) {
  object SENSATION_DATA s;
  object LOCATION_DATA l;
  s = new_object(SENSATION_DATA);
  l = new_object(LOCATION_DATA);
  l -> set_object(owner);
  s -> set_epicenter(l);
  s -> set_sense(sense);
  if(sizeof(msg) > 0 && msg[0]) s -> set_start(msg[0][2]);
  if(sizeof(msg) > 1 && msg[1]) s -> set_narration(msg[1][2]);
  if(sizeof(msg) > 2 && msg[2]) s -> set_end(msg[2][2]);
  if(args) s -> set_args(args);
  if(duration) s -> set_duration(duration);
  return s;
}

/*
 * Takes the list of sets and links them together before handing them off
 * to WorldLib's event manager.
 */
void _C_sensation_chain(object owner, object *sets) {
  int i, n;
  for(i = 1, n = sizeof(sets); i < n; i++)
    sets[i-1] -> set_next_set(sets[i]);
  WORLD_EVENTS_D -> add_sensation_set(sets[0]);
}

void _C_sensation_set(object owner, object SENSATION_DATA *sensations, int delay) {
  _C_sensation_chain(owner, ({
    _C_build_sensation_set(owner, sensations, delay)
  }));
}

void _C_sensation(object owner, string sense, mixed *msg, mapping args, varargs int duration) {
  /* we construct a single set and consider it a chain */
  _C_sensation_chain(owner, ({
    _C_build_sensation_set(owner, ({
      _C_build_sensation(owner, sense, msg, args, duration)
    }))
  }));
}

/* For "Every(timer, frequency)"
int _C_create_timer(object owner, string timer, int frequency) {
  return WORLD_EVENTS_D -> add_timer(owner, timer, frequency, 1);
}

/* For "Once(timer, delay)" */
void _C_create_delay(object owner, string timer, int delay) {
  WORLD_EVENTS_D -> add_timer(owner, timer, delay, 0);
}

/* for "Stop(id)" */
void _C_remove_timer(object owner, int id) {
  WORLD_EVENTS_D -> remove_timer(owner, id);
}

/* for "Emit(type, msg)" */
/* eventually, we want to have a transaction id for event-based messages */
void _C_queue_message(object owner, string type, string msg) {
  owner -> queue_message(0, type, msg);
}

mixed _C_get_property(object owner, object src, string prop) {
  if(!src) return nil;
  return src -> get_property(explode(prop, ":"));
}

mixed _C_set_property(object owner, object src, string prop, mixed value) {
  string *bits;
  if(!src) return nil;
  if(src -> set_property(bits = explode(prop, ":"), value)) return value;
  return src -> get_property(bits);
}
