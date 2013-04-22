# include <worldlib.h>
# include <toollib.h>
# include <type.h>

mapping details;

static void create(varargs int clone) {
  details = ([ ]);
}

mapping get_properties() {
  mapping info;
  string *idx;
  object ur;
  int i, n;

  ur = this_object() -> get_template_object();

  info = ([ ]);
  idx = map_indices(details) | (ur ? ur->get_property(({ "detail" })) : ({ }));
  for(i = 0, n = sizeof(idx); i < n; i++) {
    if(!details[idx[i]]) {
      info[idx[i]] = "*inherited*";
    }
    else if(details[idx[i]] == "*deleted*") {
      info[idx[i]] = "*deleted*";
    }
    else {
      info[idx[i]] = details[idx[i]] -> get_properties();
    }
  }
  return info;
}

mixed get_property(string *path) {
  string *idx;
  object ur;
  int i, n;

  ur = this_object() -> get_template_object();

  if(!sizeof(path)) {
    idx = map_indices(details) | (ur ? ur->get_property(({ "detail" })) : ({ }));
    for(i = 0, n = sizeof(idx); i < n; i++)
      if(details[idx[i]] && details[idx[i]] == "*deleted*")
        idx[i] = nil;
    return idx - ({ nil });
  }
  if(sizeof(path) == 1) {
    if(details[path[0]]) return details[path[0]] != "*deleted*";
    if(ur) return ur -> get_property(({ "detail" }) + path);
    return FALSE;
  }
  if(details[path[0]]) {
    if(details[path[0]] == "*deleted*") return nil;
    return details[path[0]] -> get_property(path[1..]);
  }
  if(ur) return ur -> get_property(({ "detail" }) + path);
  return nil;
}

int set_property(string *path, mixed value) {
  object ur;

  if(!sizeof(path)) return FALSE;

  ur = this_object() -> get_template_object();

  if(sizeof(path) == 1) {
    if(!value) {
      if(details[path[0]]) {
        if(ur->get_property(({ "detail", path[0] })))
          details[path[0]] = "*deleted*";
        else
          details[path[0]] = nil;
      }
      return TRUE;
    }
    return FALSE;
  }

  if(!details[path[0]]) {
    if(ur) details[path[0]] = ur->copy_details(path[0]);
  }
  if(details[path[0]] == "*deleted*" || !details[path[0]])
    details[path[0]] = new_object(EXIT_DATA);
  return details[path[0]] -> set_property(path[1..], value);
}

object copy_details(string detail) {
  object ur;

  if(details[detail]) {
    return new_object(details[detail]);
  }
  
  ur = this_object() -> get_template_object();
  if(ur) return ur -> copy_details(detail);
  return nil;
}

string *parse_command_id_list() {
  mixed ret;
  ret = get_property(({ "default", "noun" }));
  if(typeof(ret) == T_ARRAY) return ret;
  return ({ });
}

string *parse_command_adjective_id_list() {
  mixed ret;
  ret = get_property(({ "default", "adjective" }));
  if(typeof(ret) == T_ARRAY) return ret;
  return ({ });
}

string *parse_command_plural_id_list() {
  string *ids;
  mixed ret;
  int i, n;

  ids = parse_command_id_list();
  for(i = 0, n = sizeof(ids); i < n; i++)
    ids |= ({ ENGLISH_D -> pluralize(ids[i]) });
  ret = get_property(({ "default", "plural-noun" }));
  if(typeof(ret) == T_ARRAY) ids |= ret;
  return ids;
}

string *parse_command_plural_adjective_id_list() {
  mixed ret;
  ret = get_property(({ "default", "plural-adjective" }));
  if(typeof(ret) != T_ARRAY) ret = ({ });
  return parse_command_adjective_id_list() | ret;
}
