# include <type.h>

mapping moralities;

void create(varargs int clone) {
  if(clone) {
  }
}

mixed get_property(string *path) {
  object ur;
  int tmp;

  ur = this_object() -> get_ur_object();
  if(!moralities) {
    if(ur) return ur -> get_property(({ "morality" }) + path);
    if(!sizeof(path)) return ({ });
    return nil;
  }

  if(!sizeof(path)) {
    return map_indices(moralities) | (ur ? ur -> get_property(({ "morality" }) + path) : ({ }));
  }
  if(sizeof(path) > 1) return nil;
  if(typeof(moralities[path[0]]) == T_INT) tmp = moralities[path[0]];
  else if(ur) tmp = ur -> get_property(({ "morality" }) + path);
  else return nil;
  return tmp;
}

int set_property(string *path, mixed value) {
  if(typeof(value) != T_INT) return FALSE;
  if(sizeof(path) != 1) return FALSE;
  if(!moralities) moralities = ([ ]);
  moralities[path[0]] = value;
  return TRUE;
}
