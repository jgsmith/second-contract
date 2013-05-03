# include <worldlib.h>
/*
 * data for a domain
 */

string *members;
string *administrators;
string plan;
string description;
mapping areas;

static void create(varargs int clone) {
  if(clone) {
    members = ({ });
    administrators = ({ });
    areas = ([ ]);
    plan = "";
    description = "";
  }
}

mapping get_properties() {
  return ([
    "areas": map_indices(areas),
    "administrators": administrators,
    "members": members,
    "plan": plan,
    "description": description,
  ]);
}

string get_plan() { return plan; }
string get_description() { return description; }

void set_plan(string p) { plan = p; }
void set_description(string d) { description = d; }

int is_member(string *char) {
  if(!char) return FALSE;
  return sizeof( char & members) || sizeof( char & administrators);
}

int is_admin(string *char) {
  if(!char) return FALSE;
  return sizeof( char & administrators);
}

void add_member(string char) {
  if(sizeof( ({ char }) & administrators) == 0)
    members |= ({ char });
}

void add_admin(string char) {
  members -= ({ char });
  administrators |= ({ char });
}

void remove_admin(string char) {
  administrators -= ({ char });
}

void remove_member(string char) {
  members -= ({ char });
}

string *get_areas() { return map_indices(areas); }

object get_area(string a) { return areas[a]; }

object create_area(string a) {
  if(areas[a]) return nil;
  areas[a] = new_object(AREA_DATA);
  return areas[a];
}

int rename_area(string old, string new) {
  if(areas[new]) return FALSE;
  if(!areas[old]) return FALSE;
  areas[new] = areas[old];
  areas[old] = nil;
  return TRUE;
}

object get_template_object(string *path) {
  if(areas[path[0]]) return areas[path[0]] -> get_object(path[1], path[2]);
  return nil;
}
