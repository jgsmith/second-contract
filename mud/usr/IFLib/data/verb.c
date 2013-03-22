/*
 * This contains all of the data associated with a verb. Verbs are
 * persistant objects that are added to the /sys/verbs handler.
 *
 * Each verb has:
 *   verbs : a list of verbs which use this LWO
 *   help : a string with help to show the player
 *   see_alsos : other help documents that can give context
 *   syntaxes: a mapping of syntax to info about how to use that syntax
 */
# include <iflib.h>
# include <iflib/verb.h>
# include <worldlib/terrain.h>
# include <devlib.h>
# include <kernel/kernel.h>
# include <type.h>
# include <data.h>

string *verbs;
string brief;
string help;
string *see_alsos;
int args_used;
int direct_obj_flags, indirect_obj_flags, implement_obj_flags;
int actor_reqs;
string *climates;
int respirations;
int terrains;
int weathers;
int seasons;
int disabled;
string *action;

static void create(varargs int clone) {
  verbs = ({ });
  help = "";
  brief = "";
  see_alsos = ({ });
  actor_reqs = 0;
  climates = ({ });
  terrains = 0;
  weathers = 0;
  seasons = 0;
  respirations = 0;
  disabled = 1;
  args_used = 0;
}

static int can_modify() {
  if(SYSTEM()) return TRUE;
  if(explode(object_name(previous_object()),"#")[0] == HTTP_VERB_RESOURCE) return TRUE;
  error("Verbs may only be modified by System or through the web administrative interface.");
  return FALSE;
}

mapping get_properties() {
  mapping env, actor, args;
  env = ([
    "respiration": ({ }),
    "timing": ({ }),
    "terrain": ({ }),
    "climate": climates,
    "weather": ({ }),
  ]);

  if(respirations & RESP_AIR) env["respiration"] |= ({ "air" });
  if(respirations & RESP_WATER) env["respiration"] |= ({ "water" });
  if(respirations & RESP_VACUUM) env["respiration"] |= ({ "vacuum" });
  if(respirations & RESP_METHANE) env["respiration"] |= ({ "methane" });

  if(seasons & SEASON_SPRING) env["timing"] |= ({ "spring" });
  if(seasons & SEASON_SUMMER) env["timing"] |= ({ "summer" });
  if(seasons & SEASON_FALL) env["timing"] |= ({ "fall" });
  if(seasons & SEASON_WINTER) env["timing"] |= ({ "winter" });
  if(seasons & SEASON_DIURNAL) env["timing"] |= ({ "diurnal" });
  if(seasons & SEASON_NOCTURNAL) env["timing"] |= ({ "nocturnal" });

  if(terrains & TERRAIN_OUTDOORS) env["terrain"] |= ({ "outdoors" });
  if(terrains & TERRAIN_INDOORS) env["terrain"] |= ({ "indoors" });
  if(terrains & TERRAIN_ROAD) env["terrain"] |= ({ "road" });
  if(terrains & TERRAIN_UNDERWATER) env["terrain"] |= ({ "underwater" });
  if(terrains & TERRAIN_SURFACE) env["terrain"] |= ({ "surface" });
  if(terrains & TERRAIN_MIDAIR) env["terrain"] |= ({ "midair" });
  if(terrains & TERRAIN_SWAMP) env["terrain"] |= ({ "swamp" });
  if(terrains & TERRAIN_WOODS) env["terrain"] |= ({ "woods" });
  if(terrains & TERRAIN_JUNGLE) env["terrain"] |= ({ "jungle" });
  if(terrains & TERRAIN_ROUGH) env["terrain"] |= ({ "rough" });
  if(terrains & TERRAIN_UNDERGROUND) env["terrain"] |= ({ "underground" });
  if(terrains & TERRAIN_SPACE) env["terrain"] |= ({ "space" });
  if(terrains & TERRAIN_MAGMA) env["terrain"] |= ({ "magma" });
  if(terrains & TERRAIN_PLASMA) env["terrain"] |= ({ "plasma" });
  if(terrains & TERRAIN_PLANAR) env["terrain"] |= ({ "planar" });
  if(terrains & TERRAIN_SNOW) env["terrain"] |= ({ "snow" });
  if(terrains & TERRAIN_SAND) env["terrain"] |= ({ "sand" });
  if(terrains & TERRAIN_ICE) env["terrain"] |= ({ "ice" });
  if(terrains & TERRAIN_BIOLOGICAL) env["terrain"] |= ({ "biological" });
  if(terrains & TERRAIN_SEAFLOOR) env["terrain"] |= ({ "seafloor" });
  if(terrains & TERRAIN_WATERSHALLOW) env["terrain"] |= ({ "water-shallow" });
  if(terrains & TERRAIN_INDOORSWINDOW) env["terrain"] |= ({ "indoors-window" });
  if(terrains & TERRAIN_ROOFTOP) env["terrain"] |= ({ "rooftop" });
  if(terrains & TERRAIN_ALL_OUTDOORS) env["terrain"] |= ({ "any-outdoors" });
  if(terrains & TERRAIN_ALL_INDOORS) env["terrain"] |= ({ "any-indoors" });
  if(terrains & TERRAIN_ALL_WATER) env["terrain"] |= ({ "any-water" });
  if(terrains & TERRAIN_SEE_WEATHER) env["terrain"] |= ({ "sees-weather" });

  if(weathers & WEATHER_SNOWING) env["weather"] |= ({ "snowing" });
  if(weathers & WEATHER_RAINING) env["weather"] |= ({ "raining" });
  if(weathers & WEATHER_CLOUDY) env["weather"] |= ({ "cloudy" });

  actor = ([
    "act": ({ }),
    "sense": ({ }),
    "position": ({ }),
  ]);

  if(actor_reqs & ACTOR_MANIPULATE) actor["act"] |= ({ "manipulate" });
  if(actor_reqs & ACTOR_COMBAT) actor["act"] |= ({ "combat" });
  if(actor_reqs & ACTOR_MOVE) actor["act"] |= ({ "move" });

  if(actor_reqs & ACTOR_SEE) actor["sense"] |= ({ "see" });
  if(actor_reqs & ACTOR_SMELL) actor["sense"] |= ({ "smell" });
  if(actor_reqs & ACTOR_TOUCH) actor["sense"] |= ({ "touch" });
  if(actor_reqs & ACTOR_TASTE) actor["sense"] |= ({ "taste" });
  if(actor_reqs & ACTOR_HEAR) actor["sense"] |= ({ "hear" });

  if(actor_reqs & ACTOR_STANDING) actor["position"] |= ({ "standing" });
  if(actor_reqs & ACTOR_SITTING) actor["position"] |= ({ "sitting" });
  if(actor_reqs & ACTOR_LYING) actor["position"] |= ({ "lying" });
  if(actor_reqs & ACTOR_FLYING) actor["position"] |= ({ "flying" });
  if(actor_reqs & ACTOR_LEANING) actor["position"] |= ({ "leaning" });
  if(actor_reqs & ACTOR_KNEELING) actor["position"] |= ({ "kneeling" });
  if(actor_reqs & ACTOR_FLOATING) actor["position"] |= ({ "floating" });
  if(actor_reqs & ACTOR_SWIMMING) actor["position"] |= ({ "swimming" });

  args = ([
    "direct": ([
      "used": args_used & ARG_DIRECT_OBJ != 0,
      "requirements": ({ }),
    ]),
    "indirect": ([
      "used": args_used & ARG_INDIRECT_OBJ != 0,
      "requirements": ({ }),
    ]),
    "instrumental": ([
      "used": args_used & ARG_IMPLEMENT_OBJ != 0,
      "requirements": ({ }),
    ]),
    "communication": args_used & ARG_COMMUNICATION != 0,
  ]);

  if(direct_obj_flags & ARG_LIVING) args["direct"]["requirements"] |= ({ "living" });
  if(direct_obj_flags & ARG_PLAYER) args["direct"]["requirements"] |= ({ "player" });
  if(direct_obj_flags & ARG_DISTANT) args["direct"]["requirements"] |= ({ "distant" });
  if(direct_obj_flags & ARG_WIELDED) args["direct"]["requirements"] |= ({ "wielded" });
  if(direct_obj_flags & ARG_WORN) args["direct"]["requirements"] |= ({ "worn" });
  if(direct_obj_flags & ARG_IN_INVENTORY) args["direct"]["requirements"] |= ({ "in-inventory" });

  if(indirect_obj_flags & ARG_LIVING) args["indirect"]["requirements"] |= ({ "living" });
  if(indirect_obj_flags & ARG_PLAYER) args["indirect"]["requirements"] |= ({ "player" });
  if(indirect_obj_flags & ARG_DISTANT) args["indirect"]["requirements"] |= ({ "distant" });
  if(indirect_obj_flags & ARG_WIELDED) args["indirect"]["requirements"] |= ({ "wielded" });
  if(indirect_obj_flags & ARG_WORN) args["indirect"]["requirements"] |= ({ "worn" });
  if(indirect_obj_flags & ARG_IN_INVENTORY) args["indirect"]["requirements"] |= ({ "in-inventory" });
  if(indirect_obj_flags & ARG_IN_DIRECT_OBS) args["indirect"]["requirements"] |= ({ "in-direct-obs" });

  if(implement_obj_flags & ARG_LIVING) args["instrumental"]["requirements"] |= ({ "living" });
  if(implement_obj_flags & ARG_PLAYER) args["instrumental"]["requirements"] |= ({ "player" });
  if(implement_obj_flags & ARG_DISTANT) args["instrumental"]["requirements"] |= ({ "distant" });
  if(implement_obj_flags & ARG_WIELDED) args["instrumental"]["requirements"] |= ({ "wielded" });
  if(implement_obj_flags & ARG_WORN) args["instrumental"]["requirements"] |= ({ "worn" });
  if(implement_obj_flags & ARG_IN_INVENTORY) args["instrumental"]["requirements"] |= ({ "in-inventory" });
  return ([
    "see_also": see_alsos,
    "help": help,
    "verb": verbs,
    "arguments": args,
    "environment": env,
    "actor": actor,
    "disabled": disabled,
    "brief": brief,
    "action": action,
  ]);
}

mixed get_property(string *path) {
  string p;
  switch(path[0]) {
    case "see-also":
      return sizeof( ({ path[1] }) & see_alsos) != 0;
    case "help": 
      return help;
    case "verb":
      return sizeof( ({ path[1] }) & verbs) != 0;
    case "environment":
      switch(path[1]) {
        case "respiration":
          switch(path[2]) {
            case "air": return !respirations || respirations & RESP_AIR;
            case "water": return !respirations || respirations & RESP_WATER;
            case "vacuum": return !respirations || respirations & RESP_VACUUM;
            case "methane": return !respirations || respirations & RESP_METHANE;
          }
          break;
        case "timing":
          switch(path[2]) {
            case "spring": return !seasons || seasons & SEASON_SPRING;
            case "summer": return !seasons || seasons & SEASON_SUMMER;
            case "fall": return !seasons || seasons & SEASON_FALL;
            case "winter": return !seasons || seasons & SEASON_WINTER;
            case "diurnal": return !seasons || seasons & SEASON_DIURNAL;
            case "nocturnal": return !seasons || seasons & SEASON_NOCTURNAL;
          }
          break;
        case "terrain":
          switch(path[2]) {
            case "outdoors": return !terrains || terrains & TERRAIN_OUTDOORS;
            case "indoors": return !terrains || terrains & TERRAIN_INDOORS;
            case "road": return !terrains || terrains & TERRAIN_ROAD;
            case "underwater": return !terrains || terrains & TERRAIN_UNDERWATER;
            case "surface": return !terrains || terrains & TERRAIN_SURFACE;
            case "midair": return !terrains || terrains & TERRAIN_MIDAIR;
            case "swamp": return !terrains || terrains & TERRAIN_SWAMP;
            case "woods": return !terrains || terrains & TERRAIN_WOODS;
            case "jungle": return !terrains || terrains & TERRAIN_JUNGLE;
            case "rough": return !terrains || terrains & TERRAIN_ROUGH;
            case "underground": return !terrains || terrains & TERRAIN_UNDERGROUND;
            case "space": return !terrains || terrains & TERRAIN_SPACE;
            case "magma": return !terrains || terrains & TERRAIN_MAGMA;
            case "plasma": return !terrains || terrains & TERRAIN_PLASMA;
            case "planar": return !terrains || terrains & TERRAIN_PLANAR;
            case "snow": return !terrains || terrains & TERRAIN_SNOW;
            case "sand": return !terrains || terrains & TERRAIN_SAND;
            case "ice": return !terrains || terrains & TERRAIN_ICE;
            case "biological": return !terrains || terrains & TERRAIN_BIOLOGICAL;
            case "seafloor": return !terrains || terrains & TERRAIN_SEAFLOOR;
            case "water-shallow": return !terrains || terrains & TERRAIN_WATERSHALLOW;
            case "indoors-window": return !terrains || terrains & TERRAIN_INDOORSWINDOW;
            case "rooftop": return !terrains || terrains & TERRAIN_ROOFTOP;
            case "any-outdoors": return !terrains || terrains & (TERRAIN_ALL_OUTDOORS);
            case "any-indoors": return !terrains || terrains & (TERRAIN_ALL_INDOORS);
            case "any-water": return !terrains || terrains & (TERRAIN_ALL_WATER);
            case "sees-weather": return !terrains || terrains & (TERRAIN_SEE_WEATHER);
          }
          break;
        case "climate":
          return sizeof(climates) == 0 || sizeof( ({ path[2] }) & climates) != 0;
        case "weather":
          switch(path[2]) {
            case "snowing": return weathers & WEATHER_SNOWING;
            case "raining": return weathers & WEATHER_RAINING;
            case "cloudy": return weathers & WEATHER_CLOUDY;
          }
          break;
      }
      break;
    case "actor":
      switch(path[1]) {
        case "act":
          switch(path[2]) {
            case "manipulate": return actor_reqs & ACTOR_MANIPULATE;
            case "combat": return actor_reqs & ACTOR_COMBAT;
            case "move": return actor_reqs & ACTOR_MOVE;
          }
          break;
        case "sense":
          switch(path[2]) {
            case "see": return actor_reqs & ACTOR_SEE;
            case "smell": return actor_reqs & ACTOR_SMELL;
            case "touch": return actor_reqs & ACTOR_TOUCH;
            case "taste": return actor_reqs & ACTOR_TASTE;
            case "hear": return actor_reqs & ACTOR_HEAR;
          }
          break;
        case "position":
          switch(path[2]) {
            case "standing": return actor_reqs & ACTOR_STANDING;
            case "sitting": return actor_reqs & ACTOR_SITTING;
            case "lying": return actor_reqs & ACTOR_LYING;
            case "flying": return actor_reqs & ACTOR_FLYING;
            case "leaning": return actor_reqs & ACTOR_LEANING;
            case "kneeling": return actor_reqs & ACTOR_KNEELING;
            case "floating": return actor_reqs & ACTOR_FLOATING;
            case "swimming": return actor_reqs & ACTOR_SWIMMING;
          }
          break;
      }
      break;
  }
}

/* all properties are read-only for scripting purposes */
void set_property(string *path, mixed value) {
  if(!can_modify()) return;
}

void set_verbs(string *v) {
  /* TODO: notify VERB_D that the set of verbs has changed. - makes it
   * easier than relying on the rest resource object to do it through an
   * update call -- if we know our identifier...
   */
  if(can_modify()) verbs = v;
}
string *get_verbs() { return verbs; }

void set_help(string h) { 
  if(can_modify()) help = h; 
}
string get_help() { return help; }

void set_brief(string b) {
  if(can_modify()) brief = b;
}

string get_brief() { return brief; }

void set_see_also(string *v) {
  if(can_modify()) see_alsos = v;
}

void set_action(string *acts) {
  action = acts;
}

string *get_action() { return action; }

string *get_see_also() { return see_alsos; }

int get_actor_requirements() { return actor_reqs; }
void set_actor_requirements(int x) { if(can_modify()) actor_reqs = x; }

string *get_climates() { return climates; }
void set_climates(string *c) { if(can_modify()) climates = c; }

int get_terrains() { return terrains; }
void set_terrains(int x) { if(can_modify()) terrains = x; }

int get_weathers() { return weathers; }
void set_weathers(int x) { if(can_modify()) weathers = x; }

int get_seasons() { return seasons; }
void set_seasons(int x) { if(can_modify()) seasons = x; }

int get_disabled() { return disabled; }

void set_disabled(int x) {
  if(can_modify()) disabled = x;
}

int uses_args(int args) {
  if(typeof(args_used) == T_NIL) return TRUE;
  if(!args_used && !args) return TRUE;
  return (args_used & args) == args;
}

void set_args_used(int a) { 
  if(can_modify()) args_used = a; 
}

int get_args_used() { return args_used; }

int actor_allowed(object ob) {
  object EVENT_DATA e;
  int i, n;

  if(!action || !sizeof(action)) return TRUE;
  for(i = 0, n = sizeof(action); i < n; i++) {
    e = EVENTS_D -> create_event("pre-" + action[i]);
    e -> set_object(ob);
    if(!EVENTS_D -> call_event(e)) return FALSE;
  }
  return TRUE;
}
