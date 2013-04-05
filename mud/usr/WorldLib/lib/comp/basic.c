# include <worldlib.h>
# include <iflib.h>
# include <type.h>

int level;
int position;
int gait; /* default movement speed */
mapping senses; /* senses available */

mapping get_properties() {
  return ([
    "gait": WORDS_D -> gait_word(gait),
    "position": WORDS_D -> position_word(position),
    "level": level,
  ]);
}

mixed get_property(string *path) {
  object ur;
  ur = this_object() -> get_ur_object();

  if(!sizeof(path)) return nil;

  switch(path[0]) {
    case "environment": return this_object() -> get_environment();
    case "gait":
      if(sizeof(path) == 1) {
        return WORDS_D -> gait_word(gait);
      }
      else if(sizeof(path) == 2) {
        return path[1] == WORDS_D -> gait_word(gait);
      }
      else return nil;
    case "position":
      if(sizeof(path) == 1) {
        return WORDS_D -> position_word(position);
      }
      else if(sizeof(path) == 2) {
        return path[1] == WORDS_D -> position_word(position);
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
    case "gait":
      tmp = -1;
      if(sizeof(path) == 1) {
        tmp = WORDS_D -> gait_number(value);
      }
      else if(sizeof(path) == 2) {
        tmp = WORDS_D -> gait_number(path[1]);
        if(!value) tmp = -1;
      }
      if(tmp < 1) return FALSE;
      gait = tmp;
      return TRUE;
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
