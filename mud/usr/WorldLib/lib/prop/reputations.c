# include <type.h>
/*
 * handles all of the statistics for this object, such as charisma.
 * There are two numbers: current value, and max value. Min value is always
 * zero.
 */

mapping reputations;

void create(varargs int clone) {
  if(clone) {
  }
}

mixed get_property(string *path) {
  object ur;
  int tmp;

  ur = this_object() -> get_template_object();
  if(!reputations) {
    if(ur) return ur -> get_property(({ "reputation" }) + path);
    if(!sizeof(path)) return ({ });
    return nil;
  }

  if(!sizeof(path)) {
    return map_indices(reputations) | (ur ? ur -> get_property(({ "reputation" }) + path) : ({ }));
  }
  if(sizeof(path) > 2) return nil;
  if(typeof(reputations[path[0]]) == T_INT) tmp = reputations[path[0]];
  else if(ur) tmp = ur -> get_property(({ "reputation" }) + path);
  else if(sizeof(path) == 2) return FALSE;
  else return nil;
  if(sizeof(path) == 1) return tmp;
/* TODO: flesh these out a bit more */
  switch(path[1]) {
    case "exalted": return tmp > 48000;
    case "loved": return 24000 < tmp && tmp <= 48000;
    case "liked": return 12000 < tmp && tmp <= 24000;
    case "neutral": return -12000 < tmp && tmp <= -12000;
    case "disliked": return -24000 < tmp && tmp <= -12000;
    case "hated": return -48000 < tmp && tmp <= -24000;
    case "loathed": return tmp <= -48000;
    default : return FALSE;
  }
}

int set_property(string *path, mixed value) {
  if(typeof(value) != T_INT) return FALSE;
  if(sizeof(path) == 0) return FALSE;
  if(sizeof(path) != 1) return FALSE;
  if(!reputations) reputations = ([ ]);
  reputations[path[0]] = value;
  return TRUE;
}
