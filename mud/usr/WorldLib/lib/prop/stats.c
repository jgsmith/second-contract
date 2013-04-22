# include <type.h>
/*
 * handles all of the statistics for this object, such as charisma.
 * There are two numbers: current value, and max value. Min value is always
 * zero.
 */

mapping stats;
mapping max_stats;

void create(varargs int clone) {
  stats = ([ ]);
}

mixed get_property(string *path) {
  object ur;

  ur = this_object() -> get_template_object();

  if(!sizeof(path) || sizeof(path) == 1 && path[0] == "max") {
    return map_indices(max_stats) | (ur ? ur -> get_property(({ "stat" }) + path) : ({ }));
  }
  if(path[0] == "max") {
    if(sizeof(path) > 2) return nil;
    if(max_stats[path[1]] != nil) return max_stats[path[1]];
    if(ur) return ur -> get_property(({ "stat" }) + path);
    return nil;
  }
  if(sizeof(path) > 1) return nil;
  if(stats[path[0]] != nil) return stats[path[0]];
  if(ur) return ur -> get_property(({ "stat" }) + path);
  return nil;
}

int set_property(string *path, mixed value) {
  if(typeof(value) != T_INT && typeof(value) != T_FLOAT) return FALSE;
  if(value < 0) return FALSE;
  if(sizeof(path) == 0) return FALSE;
  if(sizeof(path) != 1 && (path[0] != "max" || sizeof(path) != 2)) return FALSE;
  if(path[0] == "max") {
    max_stats[path[1]] = value;
    return TRUE;
  }
  stats[path[0]] = value;
  return TRUE;
}

