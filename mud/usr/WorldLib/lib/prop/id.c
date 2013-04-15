/*
 * This manages all of the methods we need to handle matching the
 * adjectives + noun coming from the parser.
 */

string name;
string cap_name;

void create(varargs int clone) {
}

mixed get_property(string *path) {
  if(sizeof(path) > 1) return nil;

  switch(path[0]) {
    case "name": return name;
    case "cap-name": return cap_name;
    default : return nil;
  }
}

mapping get_properties() {
  return ([
    "name": name,
    "cap_name": cap_name,
  ]);
}

/* We can't use set_property for this - only set_name */
mixed set_property(string *path, mixed value) { return FALSE; }

void set_name(string str) {
  name = str;
}

void set_cap_name(string str) {
  cap_name = str;
}

string get_name() { return name; }

string get_cap_name() { return cap_name; }

string *parse_command_id_list() {
  return ({ name, cap_name }) - ({ nil, "" });
}

string *parse_command_plural_id_list() {
  return ({ name, cap_name }) - ({ nil, "" });
}

string *parse_command_adjective_id_list() {
  return ({ });
}

string *parse_command_plural_adjective_id_list() {
  return ({ });
}
