# include <type.h>

mapping counters;

void create(varargs int clone) {
  if(clone) {
    counters = ([ ]);
  }
}

mixed get_property(string *path) {
  object ur;
  int tmp;

  if(!sizeof(path)) {
    return map_indices(counters);
  }
  if(sizeof(path) == 1) {
    switch(typeof(counters[path[0]])) {
      case T_MAPPING: return map_indices(counters[path[0]]);
      default: return counters[path[0]];
    }
  }
  if(sizeof(path) == 2) {
    switch(typeof(counters[path[0]])) {
      case T_MAPPING: return counters[path[0]][path[1]];
      case T_ARRAY: return sizeof(({ path[1] }) & counters[path[0]]);
      default: return nil;
    }
  }
}

int set_property(string *path, mixed value) {
  if(typeof(value) != T_INT && typeof(value) != T_NIL) return FALSE;
  if(sizeof(path) == 1) {
    switch(typeof(counters[path[0]])) {
      case T_MAPPING: case T_ARRAY: return FALSE;
      default: counters[path[0]] = value;
    }
  }
  else if(sizeof(path) == 2) {
    switch(typeof(counters[path[0]])) {
      case T_MAPPING:
        counters[path[0]][path[1]] = value;
        break;
      case T_ARRAY:
        if(value) counters[path[0]] |= ({ value });
        else counters[path[0]] -= ({ value });
        break;
      case T_NIL:
        if(typeof(value) == T_INT && (value == 0 || value == 1)) {
          if(value) counters[path[0]] = ({ path[1] });
          else counters[path[0]] = ({ });
        }
        else {
          counters[path[0]] = ([ path[1] : value ]);
        }
        break;
      default: return FALSE;
    }
  }
  return TRUE;
}
