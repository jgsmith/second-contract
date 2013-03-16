# include <kernel/kernel.h>

object ur_object;
int is_ur_object;

atomic void set_ur_object(object ob) {
  if(is_ur_object && !SYSTEM()) {
    error("Only System may modify the ur-object of an ur-object.\n");
    return;
  }
  if(ur_object) {
    error("The ur-object of an ur-object can not be changed once it is assigned.\n");
    return;
  }
  ur_object = ob;
}

object get_ur_object() { return ur_object; }

int set_is_ur_object(int x) {
  if(is_ur_object) {
    if(!SYSTEM()) {
      error("Only System may modify the ur-object property of an ur-object.\n");
    }
    else {
      is_ur_object = x;
    }
  }
  else {
    is_ur_object = x;
  }
  return is_ur_object;
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
int is_valid_object() {
  return (!ur_object ||  ur_object->is_ur_object()) && !is_ur_object;
}
