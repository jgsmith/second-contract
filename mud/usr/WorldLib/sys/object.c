/*
 * Used to create new objects in the world.
 */
# include <worldlib.h>
# include <worldlib/proximity.h>
# include <data.h>

static void create(varargs int clone) {
}

object THING_OBJ create_object(string template, varargs object owner) {
  object ob, thing;
  object LOCATION_DATA loc;
  string *bits, *bits2;

  loc = new_object(LOCATION_DATA);
  loc -> set_object(owner);
  bits = explode(template, ":");
  /* if we don't have a full path, we base it on the owner's template's path */
  if(sizeof(bits) < 4) {
    if(owner) ob = owner -> get_template_object();
    else ob = previous_object();
    if(!ob) return nil;
    template = ob -> get_template_path();
    while(ob && !template) {
      ob = ob -> get_template_object();
      if(ob) template = ob -> get_template_path();
    }
    if(!template) return nil;
    bits2 = explode(template, ":");
    template = implode(bits2[0..(3-sizeof(bits))] + bits, ":");
  }
  thing = clone_object(THING_OBJ);
  thing -> set_template_path(template);
  if(!owner) return thing;
  if(thing->move_to(loc, PROX_INSIDE)) {
    return thing;
  }
  destruct_object(thing);
  return nil;
}
