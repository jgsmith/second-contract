# include <iflib.h>
# include <worldlib.h>

/*
 * Holds information about an observable aspect of something. Details can not
 * be manipulated (e.g., taken or dropped).
 */

inherit id ID_DATA_LIB;

mapping details;

void create(varargs int clone) {
  id::create();
  details = ([ ]);
}

mixed get_property(string *path) {
  switch(path[0]) {
    case "adjective":
    case "noun":
    case "plural-adjective":
    case "plural-noun":
    case "description":
      return id::get_property(path);
    case "detail":
      if(sizeof(path) > 1) {
        if(sizeof(path) > 2) {
          if(!details[path[1]]) return nil;
          switch(path[2]) {
            case "preposition": /* detail:name:proposition:... */
              if(sizeof(path) == 3) {
                return WORDS_D -> preposition_name(details[path[1]][0]);
              }
              if(sizeof(path) == 4) {
                return details[path[1]][0] == WORDS_D -> preposition_number(path[3]);
              }
              return nil;
            default:
              return details[path[1]][1] -> get_property(path[2..]);
          }
        }
        else {
          return details[path[1]] ? 1 : 0;
        }
      }
      else {
        return map_indices(details);
      }
  }
  return nil;
}

int set_property(string *path, mixed value) {
  int del_if_false;

  if(!sizeof(path)) return FALSE;

  switch(path[0]) {
    case "noun":
    case "adjective":
    case "plural-noun":
    case "plural-adjective":
    case "description":
      return id::set_property(path, value);
    case "detail":
      if(sizeof(path) == 2) {
        if(!value) {
          details[path[1]] = nil;
          return TRUE;
        }
        return FALSE;
      }
      if(sizeof(path) > 2) {
        del_if_false = FALSE;
        if(!details[path[1]]) {
          del_if_false = TRUE;
          details[path[1]] = clone_object(DETAIL_DATA);
        }
        if(details[path[1]] -> set_property(path[2..], value)) return TRUE;
        if(del_if_false) details[path[1]] = nil;
        return FALSE;
      }
    default: return FALSE;
  }
}
