# include <kernel/kernel.h>
# include <devlib.h>
# include <worldlib.h>
# include <worldlib/hospital.h>
# include <system.h>
# include <data.h>

/*
 * Handles creating new objects from ur-objects, and knows about the available
 * ur-objects.
 *
 * Hospitals are tied to regions. Once a region is put into play, only admin
 * may modify information in the hospital. Until then, a region is managed
 * by a group of builders.
 */

mapping wards;
int max_cache;
int generate_npcs;

static void create(varargs int clone) {
  if(clone) {
    wards = ([ ]);
  }
}

int can_modify() { return 1; }

int add_ward(string name) {
  if(wards[name]) return FALSE;
  wards[name] = clone_object(WARD_OBJ);
  return TRUE;
}

string *get_wards() {
  return map_indices(wards);
}

string *get_objects_in_ward(string ward) {
  if(!wards[ward]) return ({ });
  return wards[ward] -> get_object_names();
}

mapping get_objects(string ward) {
  if(!wards[ward]) return ([ ]);
  return wards[ward] -> get_objects();
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

/* This function provides an interface to the wards/armory, selecting
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

