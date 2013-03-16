mapping traits;
mapping deleted_traits;
/*
 * consider allowing timed traits - traits that go away after a certain
 * amount of time. This could be a set expiration time that we check on
 * getting (instead of relying on a timer to go off).
 */

void create(varargs int clone) {
  traits = ([ ]);
}

mixed get_property(string *path) {
  string p;
  object ur;
  if(!sizeof(path)) return nil;

  p = implode(path, ":");
  if(deleted_traits[p]) return nil;

  if(traits[p]) return traits[p];
  ur = this_object() -> get_ur_object();
  if(ur) return ur -> get_property(({ "trait" }) + path);
  return nil;
}

int set_property(string *path, mixed value) {
  string p;
  if(!sizeof(path)) return FALSE;
  p = implode(path, ":");
  if(value == nil) deleted_traits[p] = 1;
  else deleted_traits[p] = nil;
  traits[p] = value;
  return TRUE;
}
