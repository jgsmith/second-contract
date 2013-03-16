# include <type.h>
# include <iflib.h>
# include <worldlib.h>

inherit detail DETAIL_DATA_LIB;

object dest_scene;
int    dest_distance;
int    dest_relation;

/* maximum distance from this detail to make use of the exit */
int source_distance;
/* the types of relationships between the actor and this object
   in order to use this exit */
int *source_relations;

void create(varargs int clone) {
  detail::create();
  source_relations = nil; /* no exit set up */
}

mixed get_property(string *path) {
  string *ret;
  int i, n, tmp;

  if(!sizeof(path)) return nil;
  switch(path[0]) {
    case "adjective":
    case "noun":
    case "plural-adjective":
    case "plural-noun":
    case "description":
    case "detail":
      return detail::get_property(path);
    case "exit":
      if(sizeof(path) == 1) return source_relations && sizeof(source_relations) > 0;
      if(!source_relations || !sizeof(source_relations)) return nil;
      switch(path[1]) {
        case "destination":
          if(sizeof(path) == 2) return dest_scene;
          switch(path[2]) {
            case "scene":
              if(sizeof(path) == 3) return dest_scene;
              return nil;
            case "distance":
              if(sizeof(path) == 3) return WORDS_D -> distance_word(dest_distance);
              if(sizeof(path) == 4) return path[3] == WORDS_D -> distance_word(dest_distance);
              return nil;
            case "relation":
              if(sizeof(path) == 3) return WORDS_D -> relation_word(dest_relation);
              if(sizeof(path) == 4) return path[3] == WORDS_D -> relation_word(dest_relation);
              return nil;
          }
          return nil;
        case "source":
          if(sizeof(path) == 2) return nil;
          switch(path[2]) {
            case "distance":
              if(sizeof(path) == 3) return WORDS_D -> distance_word(source_distance);
              if(sizeof(path) == 4) return path[3] == WORDS_D -> distance_word(source_distance);
              return nil;
            case "relation":
              if(sizeof(path) == 3) {
                ret = ({ });
                for(i = 0, n = sizeof(source_relations); i < n; i++) {
                  ret += ({ WORDS_D -> relation_word(source_relations[i]) });
                }
                return ret;
              }
              if(sizeof(path) == 4) {
                tmp = WORDS_D -> preposition_number(path[3]);
                if(tmp == -1) return FALSE;
                for(i = 0, n = sizeof(source_relations); i < n; i++) {
                  if(source_relations[i] == tmp) return TRUE;
                }
                return FALSE;
              }
              return nil;
          }
          return nil;
      }
      return nil;
  }
  return nil;
}

int set_property(string *path, mixed value) {
  int tmp;

  if(!sizeof(path)) return FALSE;

  switch(path[0]) {
    case "adjective":
    case "noun":
    case "plural-adjective":
    case "plural-noun":
    case "description":
    case "detail":
      return detail::set_property(path, value);
    case "exit":
      if(sizeof(path) == 1) {
        if(!value) { /* remove the exit */
          dest_scene = nil;
          dest_distance = 0;
          dest_relation = 0;
          source_distance = 0;
          source_relations = nil;
        }
        else return FALSE;
      }
      if(sizeof(path) < 3) return FALSE;
      switch(path[1]) {
        case "destination":
          switch(path[2]) {
            case "scene":
              if(sizeof(path) != 3) return FALSE;
              if(typeof(value) != T_OBJECT) return FALSE;
              dest_scene = value;
              return TRUE;
            case "distance":
              if(sizeof(path) != 3) return FALSE;
              tmp = WORDS_D -> distance_number(value);
              if(tmp == -1) return FALSE;
              dest_distance = tmp;
              return TRUE;
            case "relation":
              if(sizeof(path) != 3) return FALSE;
              tmp = WORDS_D -> preposition_number(value);
              if(tmp == -1) return FALSE;
              dest_relation = tmp;
              return TRUE;
          }
        case "source":
          switch(path[2]) {
            case "distance":
              if(sizeof(path) != 3) return FALSE;
              tmp = WORDS_D -> distance_number(value);
              if(tmp == -1) return FALSE;
              source_distance = tmp;
              return TRUE;
            case "relation":
              if(sizeof(path) != 4) return FALSE;
              tmp = WORDS_D -> preposition_number(path[3]);
              if(tmp == -1) return FALSE;
              if(value) source_relations |= ({ tmp });
              else      source_relations -= ({ tmp });
              return TRUE;
          }
      }
      return FALSE;
  }
  return FALSE;
}
