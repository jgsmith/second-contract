# include <worldlib.h>
# include <iflib.h>
# include <type.h>

int level;
int position;
int thing_type;

int get_type() { return thing_type; }

mixed get_property(string *path) {
  if(!sizeof(path)) return nil;

  switch(path[0]) {
    case "type":
      if(sizeof(path) == 1)
        return WORDS_D -> thing_type_name(thing_type);
      else if(sizeof(path) == 2)
        return path[1] = WORDS_D -> thing_type_name(thing_type);
      else return nil;
    case "environment": return this_object() -> get_environment();
    case "position":
      if(sizeof(path) == 1) {
        return WORDS_D -> position_name(position);
      }
      else if(sizeof(path) == 2) {
        return path[1] == WORDS_D -> position_name(position);
      }
      else return nil;
    case "level": return level;
  }
}

int set_property(string *path, mixed value) {
  int tmp;
  object env;

  if(!sizeof(path)) return FALSE;

  switch(path[0]) {
    case "environment": return FALSE; /* we don't change environment through this interface */
    case "level":
      if(typeof(value) == T_INT && value > 0) {
        level = value;
        return TRUE;
      }
      return FALSE;
    case "position":
      tmp = -1;
      if(sizeof(path) == 1) {
        tmp = WORDS_D -> position_number(value);
      }
      else if(sizeof(path) == 2) {
        tmp = WORDS_D -> position_number(path[1]);
        if(!value) tmp = -1;
      }
      if(tmp < 1) return FALSE;
      env = this_object() -> get_environment();
      if(env) {
        /* we need to check on a few things to ensure that this position
         * is allowed.
         */
        position = tmp;
      }
      return TRUE;
  }
}
