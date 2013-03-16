# include <iflib.h>
# include <worldlib.h>
# include <worldlib/proximity.h>

/*
 * We get our non-transient genetics from a LWO. The rest are managed
 * here.
 */

object GENETICS_DATA genetics;

void set_genetics(object GENETICS_DATA g) {
  if(!genetics) genetics = g;
}

object GENETICS_DATA get_genetics() { 
  object ur;
  if(genetics) return genetics;
  ur = this_object() -> get_ur_object();
  if(ur) return ur -> get_genetics();
  return nil;
}

mixed get_property(string *path) {
  string *tmp;
  object *inv;
  int i, n;

  if(!sizeof(path)) return nil;

  switch(path[0]) {
    case "medium": 
      if(sizeof(path) == 1) return WORDS_D -> medium_word(genetics->get_medium());
      if(sizeof(path) == 2) return path[1] == WORDS_D -> medium_word(genetics->get_medium());
      return nil;
    case "terrain": 
      if(sizeof(path) == 1) return WORDS_D -> terrain_word(genetics->get_terrain());
      if(sizeof(path) == 2) return sizeof( ({ path[1] }) & WORDS_D -> terrain_word(genetics->get_terrain()) );
      return nil;
    case "respiration": 
      /* we need to either allow this respiration ourselves or have an
       * item "on" us that provides this respiration
       */
      if(sizeof(path) == 1 || sizeof(path) == 2) {
        tmp = WORDS_D -> respiration_word(genetics->get_respiration());
        inv = PLACEMENT_D -> get_all_inventory(this_object(), DISTANCE_ON);
        for(i = 0, n = sizeof(inv); i < n; i++) {
          tmp |= ({ inv[i] -> get_property(({ "traits", "provides", "respiration"})) });
        }
        tmp -= ({ nil });
      }
      if(sizeof(path) == 1) return tmp;
      if(sizeof(path) == 2) return sizeof(path[1..1] & tmp);
      return nil;
  }
  return nil;
}

/*
 * When we do allow editing of genetics, we'll need to make sure we copy
 * the ur genetics before making any changes.
 */
int set_property(string *path, mixed value) {
  return FALSE;
}
