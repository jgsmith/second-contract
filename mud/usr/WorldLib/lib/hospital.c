# include <kernel/kernel.h>
# include <devlib.h>
# include <worldlib.h>
# include <worldlib/hospital.h>
# include <worldlib/proximity.h>
# include <system.h>
# include <data.h>

# define TAKE_ACTION  "action:take"
# define WIELD_ACTION "action:equip"
# define WEAR_ACTION  "action:equip"

/*
 * Handles creating new objects from ur-objects, and knows about the available
 * ur-objects.
 *
 * Hospitals are tied to domains and areas. Once an area is put into 
 * play, only admin may modify information in the hospital. Until then, 
 * an area is managed by a group of builders.
 */

mapping wards;
int max_cache;
int generate_npcs;
object *transient_npcs;

/*
 * these are basic configurations about how we populate the domain/area
 * with clones of UR objects held in the wards.
 */
mapping npcs;
mapping zones;
mapping groups;
mapping treasure_sets;
mapping armor_sets;
mapping weapon_sets;
mapping armors;
mapping treasures;
mapping weapons;
mapping gatherables;
mapping trash_items;

mapping treasure_stats;

static void create(varargs int clone) {
  if(clone) {
    wards = ([ ]);
    npcs = ([ ]);
    zones = ([ ]);
    groups = ([ ]);
    treasure_sets = ([ ]);
    armor_sets = ([ ]);
    weapon_sets = ([ ]);
    armors = ([ ]);
    treasures = ([ ]);
    weapons = ([ ]);
    gatherables = ([ ]);
    trash_items = ([ ]);

    treasure_stats = ([ ]);

    transient_npcs = ({ });
  }
}

int can_modify() { return 1; }

mapping get_properties() {
  mapping info;

  info = ([
    "wards": map_indices(wards),
    "zones": zones,
    "groups": groups,
    "npcs": npcs,
    "treasure_sets": treasure_sets,
    "armor_sets": armor_sets,
    "weapon_sets": weapon_sets,
    "treasures": treasures,
    "armors": armors,
    "weapons": weapons,
    "gatherables": gatherables,
    "items": trash_items,
  ]);

  return info;
}

/* wards contain the UR objects that we use to create new NPCs, etc. */

int add_ward(string name) {
  if(wards[name]) return FALSE;
  wards[name] = clone_object(WARD_OBJ);
  return TRUE;
}

string *get_wards() {
  return map_indices(wards);
}

int query_ward_exists(string w) {
  return wards[w] ? TRUE : FALSE;
}

string *get_objects_in_ward(string ward) {
  if(!wards[ward]) return ({ });
  return wards[ward] -> get_object_names();
}

int query_object_exists(string w, string o) {
  if(!wards[w]) return FALSE;
  return wards[w] -> query_object_exists(o);
}

mapping get_objects(string ward) {
  if(!wards[ward]) return ([ ]);
  return wards[ward] -> get_objects();
}

object get_object(string ward, string nom) {
  if(!wards[ward]) return nil;
  return wards[ward] -> get_object(nom);
}

/* creates a new thing with the ur_object set to the given item */
atomic object create_object(string ward, string ur_name) {
  object ur;
  object thing;

  if(!wards[ward]) return nil;

  ur = wards[ward] -> get_object(ur_name);
  if(!ur) return nil;

  thing = clone_object(THING_OBJ);
  thing -> set_ur_object(ur);
  return thing;
}

/*
 * Creates an object and places it somewhere.
 */
atomic object create_placed_object(string ward, string ur_name, object dest, int relation) {
  object thing;
  object loc;

  thing = create_object(ward, ur_name);
  if(!thing) return nil;
  /* now we can add things like clothing or other inventory */

  /* finally, we place the item in the environment */
  loc = new_object(LOCATION_DATA);
  loc -> set_object(dest);
  if(thing -> move_to(relation, loc)) {
    return thing;
  }
  destruct_object(thing);
  /* also destruct any other created items */
  return nil;
}

int add_object(string ward, object ur) {
  if(!wards[ward]) return 0;
  return wards[ward] -> add_object(ur);
}

/*
 * The rest of this deals with populating zones based on zone/group/npc data
 */


object *create_random_npc(string zone, int wealth, int busy) {

}

/* This function provides an interface to the wards, selecting
 * an item at random from the 'items' array and attempting to create a
 * clone placed in the prox relationship to dest.
 */
int create_item(int prox, object LOCATION_DATA dest, string ward, string *items) {
  object item;
  int i;

  if(!dest || !items || !ward) return FALSE;
  if(!sizeof(items)) return FALSE;
  while(!item) {
    i = random(sizeof(items));
    item = create_object(ward, items[i]);
    if(!item) {
      items = items[0..i-1] + items[i+1..];
      continue;
    }
    if(!sizeof(items)) {
      break;
    }
  }
  if(item) {
    i = item -> move_to(prox, dest);
    if(i == 1) return TRUE;
    destruct_object(item);
  }
  return FALSE;
}

void set_max_cache(int num) {
  if(can_modify()) max_cache = num;
}

void disable_npc_generation() {
  if(can_modify()) generate_npcs = 0;
}

void enable_npc_generation() {
  if(can_modify()) generate_npcs = 1;
}

int make_unique(string ward, string name) {
  int delay;

} 

int reset_unique(string ward, string name) {
}

/*
 * die dsl:
 * die: _int
 * die: _int 'd' _int
 * die: 'd' _int
 * dies: die
 * dies: dies '+' die
 */

int get_max_population(string ward, string npc) {
  if(!wards[ward]) return 0;
  return wards[ward] -> get_max_population(npc);
}

object build_treasure(string nom) {
  object item;
  object info;

  if(!treasures || !(info = treasures[nom])) return nil;
  if(!info -> in_season(CALENDAR_D -> get_season())) return nil;
  item = create_object(info->get_ward(), info->get_name());
  if(!item) return nil;
  item -> set_quality(info->get_quality());
  if(treasure_stats[nom]) treasure_stats[nom]++;
  else treasure_stats[nom] = 1;
  return item;
}

object build_armor(string nom) {
  object item;
  object info;

  if(!armors || !(info = armors[nom])) return nil;
  if(!info->in_season(CALENDAR_D -> get_season())) return nil;
  item = create_object(info->get_ward(), info->get_name());
  if(!item) return nil;
  item -> set_quality(info->get_quality());
  return item;
}

object build_weapon(string nom) {
  object item;
  object info;

  if(!weapons || !(info = weapons[nom])) return nil;
  if(!info->in_season(CALENDAR_D -> get_season())) return nil;
  item = create_object(info->get_ward(), info->get_name());
  if(!item) return nil;
  item -> set_quality(info->get_quality());
  return item;
}

int get_weapon(object destination, string *items) {
  int i, n;
  object item;
  object loc;
  string *limbs;
  int hands;

  if(!destination) return FALSE;

  limbs = destination -> get_limbs();
  if(!limbs) return FALSE;
  for(i = 0, n = sizeof(limbs); i < n; i++)
    if(destination->get_wielded(limbs[i])) limbs[i] = nil;
  limbs -= ({ nil });
  
  items = items + ({ });
  while(!item && sizeof(items)) {
    i = random(sizeof(items));
    item = build_weapon(items[i]);
    if(!item) items -= ({ items[i] });
  }
  if(!item) return FALSE;

  loc = new_object(LOCATION_DATA);
  loc -> set_object(destination);
  item -> move_to(PROX_CLOSE, loc);
  hands = item -> get_hands();

  if(item -> get_equipped()) return TRUE;

  if(hands < sizeof(limbs)) limbs = limbs[0..hands-1];
  else if( hands > sizeof(limbs) ) return TRUE;

  /* force destination to hold/wield item */
  /* action:equip direct: item */
  if(EVENTS_D -> run_event_set(
    destination -> build_event_sequence(({ TAKE_ACTION, WIELD_ACTION }), ([
      "direct": ({ item }),
      "indirect": ({ }),
      "instrument": ({ }),
      "verb": "wield",
      "adverb": ({ }),
      "mods": nil,
    ]))
  )) return TRUE;

  destruct_object(item);

  return FALSE;
}

int get_armor(object destination, string *items) {
  int i, n;
  object item;
  object loc;

  if(!destination) return FALSE;

  items = items + ({ });
  while(!item && sizeof(items)) {
    i = random(sizeof(items));
    item = build_armor(items[i]);
    if(!item) items -= ({ items[i] });
  }
  if(!item) return FALSE;

  loc = new_object(LOCATION_DATA);
  loc -> set_object(destination);
  item -> move_to(PROX_CLOSE, loc);

  if(EVENTS_D -> run_event_set(
    destination -> build_event_sequence(({ TAKE_ACTION, WEAR_ACTION }), ([
      "direct": ({ item }),
      "indirect": ({ }),
      "instrument": ({ }),
      "verb": "wear",
      "adverb": ({ }),
      "mods": nil,
    ]))
  )) return TRUE;

  destruct_object(item);

  return FALSE;
}

int get_treasure(object destination, string *items) {
  int i;
  object item;
  object loc;

  if(!destination) return FALSE;
  items = items + ({ });
  while(!item && sizeof(items)) {
    i = random(sizeof(items));
    item = build_treasure(items[i]);
    if(!item) items -= ({ items[i] });
  }
  if(!item) return FALSE;

  loc = new_object(LOCATION_DATA);
  loc -> set_object(destination);
  item -> move_to(PROX_CLOSE, loc);

  if(EVENTS_D -> run_event_set(
    destination -> build_event_sequence(({ TAKE_ACTION }), ([
      "direct": ({ item }),
      "indirect": ({ }),
      "instrument": ({ }),
      "verb": "take",
      "adverb": ({ }),
      "mods": nil,
    ]))
  )) return TRUE;
  /* It wasn't taken, so we remove the object and return FALSE */

  destruct_object(item);

  return FALSE;
}

void add_items(string adder, mapping item_sets, object npc, mapping sets) {
  int total;
  string *keys, *keys2;
  int i, n, j, m;

  total = 0;
  keys = map_indices(sets);
  for(i = 0, n = sizeof(keys); i < n; i++)
    total += sets[keys[i]];
  total = random(total);
  for(i = 0, n = sizeof(keys); i < n; i++) {
    if(total < sets[keys[i]] && armor_sets[keys[i]]) {
      keys2 = map_indices(item_sets[keys[i]]);
      for(j = 0, m = sizeof(keys2); j < m; j++)
        call_other(this_object(), adder, npc, item_sets[keys[i]][keys2[j]]);
      return;
    }
    total -= sets[keys[i]];
  }
}

void add_armor(object npc, mapping sets) {
  add_items("get_armor", armor_sets, npc, sets);
}

void add_treasure(object npc, mapping sets) {
  add_items("get_treasure", treasure_sets, npc, sets);
}

void add_weapon(object npc, mapping sets) {
  add_items("get_weapon", weapon_sets, npc, sets);
}

/* This is where we manage transient NPCs and such instead of relying on
 * scenes/rooms managing them.
 */
atomic void heart_beat() {
  int i, n;
  object *envs;
  object *chars;

  /* do this every five seconds - not a critical thing overall */
  transient_npcs -= ({ nil });
  envs = ({ });
  chars = CHARACTER_D -> active_characters();
  for(i = 0, n = sizeof(chars); i < n; i++)
    envs |= ({ chars[i] -> get_environment() });
  for(i = 0, n = sizeof(transient_npcs); i < n; i++) {
    if(!sizeof(({ transient_npcs[i]->get_environment() }) & envs))
      destruct_object(transient_npcs[i]);
  }
  transient_npcs -= ({ nil });
}

void destructed(int clone) {
  int i, n;
  object *w;

  if(clone) {
    w = map_values(wards);
    for(i = 0, n = sizeof(w); i < n; i++) destruct_object(w[i]);
    transient_npcs -= ({ nil });
    for(i = 0, n = sizeof(transient_npcs); i < n; i++)
      destruct_object(transient_npcs[i]);
  }
}
