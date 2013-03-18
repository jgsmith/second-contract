# include <kernel/kernel.h>
# include <devlib.h>
# include <worldlib.h>
# include <worldlib/proximity.h>
# include <worldlib/hospital.h>
# include <system.h>

/*
 * Handles creating new objects from ur-objects, and knows about the available
 * ur-objects.
 *
 * Hospitals are tied to regions. Once a region is put into play, only admin
 * may modify information in the hospital. Until then, a region is managed
 * by a group of builders.
 */

mapping wards;

static void create(varargs int clone) {
  if(clone) {
    wards = ([ ]);
  }
}

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
object create_placed_object(string ward, string ur_name, object dest, int relation) {
  object thing;
  object loc;

  thing = create_object(ward, ur_name);
  if(!thing) return nil;
  /* now we can add things like clothing or other inventory */

  (users())[0]->message("We have a thing");
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
