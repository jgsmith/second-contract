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

int is_living() {
  object g;

  g = get_genetics();
  if(g) return g -> is_living();
  return FALSE;
}

mixed get_property(string *path) {
  string *tmp;
  object *inv;
  int i, n;
  object GENETICS_DATA g;

  if(!sizeof(path)) return nil;

  g = get_genetics();

  if(!g) return nil;

  switch(path[0]) {
    case "living": return g->is_living();
    case "medium": 
      if(sizeof(path) == 1) return WORDS_D -> medium_word(g->get_medium());
      if(sizeof(path) == 2) return path[1] == WORDS_D -> medium_word(g->get_medium());
      return nil;
    case "terrain": 
      if(sizeof(path) == 1) return WORDS_D -> terrain_word(g->get_terrain());
      if(sizeof(path) == 2) return sizeof( ({ path[1] }) & WORDS_D -> terrain_word(g->get_terrain()) );
      return nil;
    case "respiration": 
      /* we need to either allow this respiration ourselves or have an
       * item "on" us that provides this respiration
       */
      if(sizeof(path) == 1 || sizeof(path) == 2) {
        tmp = WORDS_D -> respiration_word(g->get_respiration());
        inv = this_object() -> get_all_inventory();
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

static void _ensure_genetics() {
  if(!genetics) genetics = get_genetics();
  if(!genetics) genetics = new_object(GENETICS_DATA);
}

int set_property(string *path, mixed value) {
  switch(path[0]) {
    case "living": 
      _ensure_genetics();
      genetics -> set_living(!!value);
      return TRUE;
  }
  return FALSE;
}
