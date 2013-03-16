# include <iflib/binder.h>

/*
 * Data associated with an object match in the binder
 */

int result;
object *singular_objects;
object *plural_objects;

void create(varargs int clone) {
  if(clone) {
    singular_objects = ({ });
    plural_objects = ({ });
  }
}

void remove_object(object o) {
  singular_objects -= ({ o });
  plural_objects -= ({ o });
}
void remove_objects(object *o) {
  singular_objects -= o;
  plural_objects -= o;
}

object *get_objects() { return singular_objects | plural_objects; }

void add_singular_object(object o) { singular_objects |= ({ o }); }
void add_singular_objects(object *o) { singular_objects |= o; }
void remove_singular_objects(object *o) { singular_objects -= o; }
void remove_singular_object(object o) { singular_objects -= ({ o }); }
object *get_singular_objects() { return singular_objects; }

void add_plural_object(object o) { plural_objects |= ({ o }); }
void add_plural_objects(object *o) { plural_objects |= o; }
void remove_plural_objects(object *o) { plural_objects -= o; }
void remove_plural_object(object o) { plural_objects -= ({ o }); }
object *get_plural_objects() { return plural_objects; }

void set_result(int r) { result = r; }
int get_result() { return result; }

void merge_objects(object BINDER_MATCH src) {
  singular_objects |= src -> get_singular_objects();
  plural_objects |= src -> get_plural_objects();
}

void replace_objects(object BINDER_MATCH src) {
  singular_objects = src -> get_singular_objects();
  plural_objects = src -> get_plural_objects();
}
