# include <worldlib.h>
# include <kernel/kernel.h>

object ur_object;
string ur_object_path; /* domain:ward:item */
string this_object_path; /* set if we're an ur_object */

/* if max_population > 0, then there can't be more objects using this as an
 * ur object than max_population
 */
int max_population;
object *children;

static void create(varargs int clone) {
  if(clone) {
  }
}

void destructed(int clone) {
  int i, n;
  if(SYSTEM() && clone) {
    if(children) {
      for(i = 0, n = sizeof(children); i < n; i++)
        destruct_object(children[i]);
    }
  }
}

object create_clone() {
  object ob;

  if(!this_object_path) return nil;
  children -= ({ nil });
  if(max_population > 0 && sizeof(children) >= max_population) return nil;

  ob = clone_object(THING_OBJ);
  ob -> set_ur_object();
  children += ({ ob });
  return ob;
}

/* this can only be called from this library */
void remove_child(object ob) {
  if(previous_program() == "/usr/WorldLib/lib/comp/ur") {
    children -= ({ ob });
  }
  children -= ({ nil });
}

atomic void set_ur_object_path(string path) {
  object ob;
  if(this_object_path && !SYSTEM()) {
    error("Only System may modify the ur-object of an ur-object.\n");
    return;
  }
  if(this_object_path && ur_object_path) {
    error("The ur-object of an ur-object can not be changed once it is assigned.\n");
    return;
  }
  /* get the domain/ward/item */
  if(!path) path = previous_object() -> get_object_path();
  if(!path) return;
  ob = DOMAINS_D -> get_ur_object(path);
  if(ob) {
    if(ur_object) {
      if(ur_object_path == path) return;
      ur_object = nil;
    }
    ur_object_path = path;
    ur_object = ob;
    if(this_object_path) ur_object -> remove_child(this_object());
  }
}

object get_ur_object() { return ur_object; }

int set_object_path(string x) {
  if(this_object_path) {
    if(!SYSTEM()) {
      error("Only System may modify the object path property of an ur-object.\n");
    }
    else {
      this_object_path = x;
    }
  }
  else {
    this_object_path = x;
  }
  if(this_object_path) {
    if(!children) children = ({ });
    if(ur_object) ur_object -> remove_child(this_object());
  }
  return this_object_path != nil;
}

/*
 * Valid objects are using ur objects that are marked as ur objects in the
 * system. Valid objects are not themselves ur objects.
 *
 * Only valid objects can appear to players in the "real" game. Otherwise,
 * these objects can only be used in test areas by creators.
 *
 * Invalid objects are eliminated from direct/indirect object lists. They
 * are essentially invisible to non-creators and don't receive any events
 * from actions initated by players.
 */
int is_ur_object() { return this_object_path != nil; }

int is_valid_object() {
  return (!ur_object ||  ur_object->is_ur_object()) && !this_object_path;
}
