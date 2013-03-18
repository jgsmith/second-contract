# include <kernel/kernel.h>
# include <devlib.h>
# include <worldlib.h>
# include <worldlib/hospital.h>
# include <system.h>

inherit hospital HOSPITAL_LIB;

object top_of_the_world;

/*
 * Handles creating new objects from ur-objects, and knows about the available
 * ur-objects.
 *
 * This one doesn't handle adding clothing or other things yet. We'll worry
 * about that later.
 */

static void create(varargs int clone) {
  hospital::create(1);

  if(!find_object(THING_OBJ)) compile_object(THING_OBJ);
}

object get_world_object() { return top_of_the_world; }
void set_world_object(object THING_OBJ ob) {
  if(!top_of_the_world) top_of_the_world = ob;
}

int add_ward(string name) {
  if(!SYSTEM() && object_name(previous_object()) != "/usr/WorldLib/initd") {
    error("Only System may create new wards in the primary hospital.\n");
  }
  return hospital::add_ward(name);
}

int add_object(string ward, object ur) {
  if(!SYSTEM() && object_name(previous_object()) != "/usr/WorldLib/initd") {
    error("Only System may add new ur-objects to the core hospital.\n");
  }
  return hospital::add_object(ward, ur);
}
