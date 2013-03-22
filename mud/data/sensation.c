# include <worldlib.h>
# include <devlib.h>
# include <data.h>

/*
 * Holds data associated with a sense-based event (something seen, heard, etc.)
 */

string *narrative;
string type;
string sense;
int intensity;
int duration; /* 0 = acute, >0 = duration */
mapping args; /* used for sending messages - such as who's doing what */
object LOCATION_DATA epicenter;

/* We will probably have some effects going with this eventually...
 *
 * Automatic effects: loud sound can cause sonic damage
 *                    high heat can cause thermal damage
 *                    low freezing can cause frost/ice damage
 *
 */

void create(varargs int clone) {
  if(clone) {
    args = ([ ]);
    narrative = allocate(3);
    sense = "light";
    type = "env";
  }
}

void set_start(string s) { narrative[0] = s; }
void set_narration(string s) { narrative[1] = s; }
void set_end(string s) { narrative[2] = s; }

void set_epicenter(object LOCATION_DATA e) { epicenter = e; }
object get_epicenter() { return epicenter; }

void set_duration(int c) { duration = c; }
int get_duration() { return duration; }

void set_type(string t) { type = t; }

void set_sense(string s) { sense = s; }

void set_intensity(int i) { intensity = i; }

void set_arg(string k, mixed v) { args[k] = v; }

void set_args(mapping a) { args += a; }

static void _send_messages(int idx, object *observers) {
  /* we check to see how intense the sense is for the observer based on their
   * distance from the epicenter and the intensity
   */
  int i, n;
  int rel;
  int *d;
  mapping distances;

  distances = ([ ]);
  for(i = 0, n = sizeof(observers); i < n; i++) {
    rel = PROXIMITY_D -> get_son_distance(observers[i], epicenter);
    if(sense == "sight") rel = rel / 2;
    else if(sense == "smell") rel = rel * 2;
    if(intensity - rel >= -8) {
      if(!distances[intensity-rel]) distances[intensity-rel] = ({ observers[i] });
      else distances[intensity-rel] |= ({ observers[i] });
    }
  }
  d = map_indices(distances);
  /* for(i = 0, n = sizeof(observers); i < n; i++) {
    MESSAGING_D -> send_messages(sense + ":" + type, narrative[idx], args + ([ "intensity": d[i] ]), distances[d[i]]);*/
    MESSAGING_D -> send_messages(sense + ":" + type, narrative[idx], args + ([ "intensity": 4 ]), observers);
  /* } */
}

/*
 * Eventually, we want something like the following:
 *
 * Up close:  Bob whispers to Alice: "..."
 * A bit away: Bob whispers to Alice.
 *
 * We need support for {{#if intensity > ...}}msg{{#else}}.{{/if}} in the narrative.
 */

static object *default_observers() {
  object env;

  if(epicenter && (env = epicenter->get_object())) {
    if(env = env -> get_environment()) return env -> get_all_inventory();
    return epicenter->get_object() -> get_all_inventory();
  }
  return ({ });
}

void start_sensation(varargs object *observers) { /* set the timer running...
  /* build up observers from current environment */
  if(duration) {
    if(!observers) observers = default_observers();
    _send_messages(0, observers);
  }
}

void observe_sensation(varargs object *observers) {
  if(!observers) observers = default_observers();
  if(epicenter->get_object()) {
    (users())[0]->message("We have an epicenter object\n");
  }
  (users())[0] -> message("We have " + sizeof(observers) + " observers\n");
  _send_messages(1, observers);
}

void end_sensation(varargs object *observers) {
  if(duration) {
    if(!observers) observers = default_observers();
    _send_messages(2, observers);
  }
}
