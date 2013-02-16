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
# include <verb.h>
# include <iflib.h>

string *verbs;
string help;
string *see_alsos;
mapping syntaxes;
int actor_reqs;
string *climates;
int respirations;
int terrains;
int weathers;
int seasons;

static void create(varargs int clone) {
  verbs = ({ });
  help = "";
  see_alsos = ({ });
  syntaxes = ([ ]);
  actor_reqs = 0;
  climates = ({ });
  terrains = 0;
  weathers = 0;
  seasons = 0;
  respirations = 0;
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
            case "snowing": return !weathers || weathers & WEATHER_SNOWING;
            case "raining": return !weathers || weathers & WEATHER_RAINING;
            case "cloudy": return !weathers || weathers & WEATHER_CLOUDY;
          }
          break;
      }
      break;
    case "actor":
      switch(path[1]) {
        case "act":
          switch(path[2]) {
            case "manipulate": return actor_reqs & ACTOR_MANIPULATE;
            case "fight": return actor_reqs & ACTOR_FIGHT;
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

// all properties are read-only for scripting purposes
void set_property(string *path, mixed value) {
}

void set_verbs(string *v) {
  verbs = v;
}
string *get_verbs() { return verbs; }

void set_help(string h) { help = h; }
string get_help() { return help; }

void set_see_also(string *v) {
  see_alsos = v;
}

string *get_see_alsos() { return see_alsos; }

/*
 * info:
 *   action: action this verb maps to (required)
 *   args: mapping of additional constant args to include in action call
 */
void add_syntax(string s, mapping info) {
  if(info["action"]) syntaxes[s] = info;
}

mapping get_syntax(string s) {
  return syntaxes[s];
}

string *get_syntaxes() { return map_indices(syntaxes); }

int get_actor_requirements() { return actor_reqs; }
void set_actor_requirements(int x) { actor_reqs = x; }

string *get_climates() { return climates; }
void set_climates(string *c) { climates = c; }

int get_terrains() { return terrains; }
void set_terrains(int x) { terrains = x; }

int get_weathers() { return weathers; }
void set_weathers(int x) { weathers = x; }

int get_seasons() { return seasons; }
void set_seasons(int x) { seasons = x; }
