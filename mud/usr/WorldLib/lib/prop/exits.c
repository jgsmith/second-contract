/*
 * Exits are not inherited from ur-objects.
 *
 * These are exits for cardinal directions from a scene, not for 
 * details that can be climbed, jumped, etc., through, over, etc.
 */

mapping exits;

static void create(varargs int clone) {
  if(clone) {
    exits = ([ ]);
  }
}

mapping get_properties() {
  mapping info;

  info = ([ ]);
}

mixed get_property(string *path) {
  string *results;
  int i, n;

  if(!sizeof(path)) return map_indices(exits);
  if(sizeof(path) == 1) {
    if(exits[path[0]]) return TRUE;
    return FALSE;
  }

  if(!exits[path[0]]) return nil;
  return nil;
}

mixed set_property(string *path) {
}
