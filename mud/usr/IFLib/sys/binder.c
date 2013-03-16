# include <type.h>
# include <iflib/binder.h>
# include <worldlib/proximity.h>

/*
 * Given a set of phrases, find all matching objects in another object's
 * inventory or environment
 */

mapping ordinals;
mapping counters;
mapping fractions;

void create(varargs int clone) {
  ordinals = ([
    "any": 1,
    "a": 1,
    "an": 1,
    "the": 1,
    "1st" : 1, "first" : 1,
    "2nd" : 2, "second" : 2,
    "3rd" : 3, "third" : 3,
    "4th" : 4, "fourth" : 4,
    "5th" : 5, "fifth" : 5,
    "6th" : 6, "sixth" : 6,
    "7th" : 7, "seventh" : 7,
    "8th" : 8, "eighth" : 8,
    "9th" : 9, "ninth" : 9,
    "10th" : 10, "tenth" : 10,
    "last" : -1
  ]);

  counters = ([
    "1" : 1, "one" : 1,
    "2" : 2, "two" : 2,
    "3" : 3, "three" : 3,
    "4" : 4, "four" : 4,
    "5" : 5, "five" : 5,
    "6" : 6, "six" : 6,
    "7" : 7, "seven" : 7,
    "8" : 8, "eight" : 8,
    "9" : 9, "nine" : 9,
    "10" : 10, "ten" : 10,
    "11" : 11, "eleven" : 11,
    "12" : 12, "twelve" : 12,
    "13" : 13, "thirteen" : 13,
    "14" : 14, "fourteen" : 14,
    "15" : 15, "fifteen" : 15,
    "16" : 16, "sixteen" : 16,
    "17" : 17, "seventeen" : 17,
    "18" : 18, "eighteen" : 18,
    "19" : 19, "nineteen" : 19,
    "20" : 20, "twenty" : 20,
    "many" : 20,
    "every": EVERY_NUM,
  ]);

  fractions = ([
    "half" : ({ 1, 2 }),
    "quarter" : ({ 1, 4 }),
    "some" : ({ 1, 50 }),
  ]);

  if(!find_object(BINDER_MATCH_CONTEXT)) compile_object(BINDER_MATCH_CONTEXT);
  if(!find_object(BINDER_MATCH)) compile_object(BINDER_MATCH);
}

static void fixup_context(object actor, object *objects, object BINDER_MATCH_CONTEXT context) {
  if(!actor || !sizeof(objects)) return;

  if(sizeof(objects) > 1) {
    context -> set_plural(objects);
  }
  else if(objects[0] -> is_living() && objects[0] != actor) {
    switch(objects[0] -> get_gender()) {
      case 0: context->set_him(objects[0]); break;
      case 1: context->set_her(objects[0]); break;
      default : context->set_it(objects[0]); break;
    }
  }
  else {
    context -> set_it(objects[0]);
  }

  actor -> set_binder_context(context);
}

/*
 * Input: ({ ({ adjs, noun }) })
 * Prep: PREP_...
 *
 * This is used to walk our way from an initial set of objects to the ones
 * we're looking for. The prep indicates the expected relation to the
 * ob_list.
 */
static object BINDER_MATCH _match_object_in_array( int *prep, string *input, object *ob_list, int type, object actor ) {
  object BINDER_MATCH omatch;
  object BINDER_MATCH_CONTEXT context;
  object *obs;
  mixed  *result;
  int i, n, j, m;

  context = actor -> get_binder_context();

  omatch = new_object(BINDER_MATCH);

  /*
   * given the ob_list, prep, and input, we want all of the items
   * with the "prep" relationship to ob_list that satisfy the input
   */
  for(i = 0, n = sizeof(ob_list); i < n; i++) {
    obs = ob_list[i]->get_proxied_objects( prep );
    if(obs) {
      for(j = 0, m = sizeof(obs); j < m; j++) {
        result = obs[j]->parse_match_object( input, actor, context );
        if(result[BINDER_MATCH_TYPE] & BINDER_MATCH_PLURAL) {
          omatch->add_plural_objects(result[BINDER_MATCH_OBJECTS]);
        }
        if(result[BINDER_MATCH_TYPE] & BINDER_MATCH_SINGULAR) {
          omatch->add_singular_objects(result[BINDER_MATCH_OBJECTS]);
        }
      }
    }
  }

  obs = omatch -> get_plural_objects() | omatch -> get_singular_objects();

  for(i = 0, n = sizeof(obs); i < n; i++)
    if(!obs[i]->is_visible(actor))
      omatch -> remove_object(obs[i]);

  if(!sizeof(omatch -> get_objects())) {
    omatch -> set_result(BINDER_NO_MATCH);
  }
  else {
  }

  return omatch;
}

object BINDER_MATCH match_object_in_array( mixed *input, object *ob_list, int type, object actor ) {
  object BINDER_MATCH result;
  object BINDER_MATCH omatch;
  object BINDER_MATCH_CONTEXT context;
  object *sources;
  int i;

  context = actor -> get_binder_context();

  if(!context) {
    context = new_object(BINDER_MATCH_CONTEXT);
    actor -> set_binder_context(context);
  }

  omatch = new_object(BINDER_MATCH);
  if(sizeof(input) == 0) {
    omatch->set_result(BINDER_NO_MATCH);
    return omatch;
  }
  
  sources = ob_list + ({ });
  result = _match_object_in_array( ({ PROX_INSIDE, PROX_ON }), input[sizeof(input)-1], sources, type, actor );
  i = sizeof(input)-3;
  while(result -> get_result() == BINDER_SUCCESS && i > 0 && sizeof(sources)) {
    sources = result -> get_objects();
    result = _match_object_in_array( ({ input[i] }), input[i-1], sources, type, actor );
    i -= 2;
  }
  if(result -> get_result() != BINDER_SUCCESS) {
    omatch -> set_result(result->get_result());
    omatch -> replace_objects(result);
    return omatch;
  }
  return result;
}

/*
 * when binding a direct object, we're looking in the anchor's inventory
 * or in the anchor's environment's inventory.
 *
 * The parse context object tracks things like "my", "her", "she", "him",...
 */
object BINDER_MATCH bind_direct(object actor, int type, mixed *phrases) {
  int i, n;
  object BINDER_MATCH result;
  object BINDER_MATCH omatch;
  object *obs;
  object *unwanted;
  object *sources;
  object env;

  if(type & BINDER_MATCH_IN_INVENTORY) {
    sources = ({ actor });
  }
  else if(type & BINDER_MATCH_DISTANT) {
    env = actor;
    while(env && !sizeof(({ env }) & sources)) {
      sources |= ({ env });
      env = env -> get_environment();
    }
  }
  else {
    sources = ({ actor, actor -> get_environment() }) - ({ nil });
  }

  omatch = new_object(BINDER_MATCH);

  if(typeof(phrases) == T_ARRAY) {
    for(i = 0, n = sizeof(phrases); i < n; i++) {
      result = match_object_in_array(phrases[i], sources, type, actor);
      if(result -> get_result() == BINDER_SUCCESS) {
        omatch -> add_objects(result -> get_objects());
      }
    }
  }
  else {
    omatch = match_object_in_array(phrases, sources, type, actor);
  }
  if(!sizeof(omatch -> get_objects())) {
    omatch -> set_result(BINDER_NO_MATCH);
  }

  obs = omatch -> get_objects();
  unwanted = ({ });
  for(i = 0, n = sizeof(obs); i < n; i++) {
    if(type & BINDER_MATCH_LIVING && !obs[i]->is_living())
      unwanted += obs[i..i];
    else if(type & BINDER_MATCH_PLAYER && !obs[i]->is_player())
      unwanted += obs[i..i];
    else if(type & BINDER_MATCH_WIELDED && !obs[i]->is_wielded())
      unwanted += obs[i..i];
    else if(type & BINDER_MATCH_WORN && !obs[i]->is_worn())
      unwanted += obs[i..i];
  }
  omatch -> remove_objects(unwanted);
  if(type & BINDER_MATCH_SINGULAR) {
    switch(sizeof(omatch -> get_singular_objects())) {
      case 0: omatch -> set_result(BINDER_NO_MATCH); break;
      case 1: break;
      default : omatch -> set_result(BINDER_AMBIGUOUS); break;
    }
  }
  else {
    if(!sizeof(omatch -> get_plural_objects())) {
      omatch -> set_result(BINDER_NO_MATCH);
    }
  }
  
  return omatch;
}
