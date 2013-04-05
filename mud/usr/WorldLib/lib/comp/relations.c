# include <worldlib.h>
# include <worldlib/proximity.h>
# include <devlib.h>
# include <type.h>
# include <data.h>

/*
 * This keeps track of forward and reverse relations between objects and
 * object details. This replaces the PLACEMENT_D service. By distributing
 * this, we can scale to larger worlds and not run into a concurrency
 * bottleneck.
 */

object *inventory; /* a list of things targeting us */
object LOCATION_DATA *targets; /* a list of things we're targeting */
object LOCATION_DATA current_location; /* where we are at the moment */

void create(varargs int clone) {
  if(clone) {
    inventory = ({ });
    targets = ({ });
    current_location = nil;
  }
}

/* each item in inventory is an object - no details target us */
/* each item in targets is a LOCATION_DATA object that captures the:
 *    * object
 *    * detail
 *    * relationship
 */

/*
 * get_relation returns the closest relationship between the objects.
 * If the relationship is with something with a detail, then the proper
 * relationship is the larger of the relationship with the detail, and the
 * detail's relationship with its object.
 */
int get_relation(object obj) {
  int i, n, relation;

  for(i = 0, n = sizeof(targets); i < n; i++) {
    if(targets[i]->get_object() == obj) {
      relation = targets[i] -> get_relation();
      if(!targets[i] -> is_detail()) return relation;
      /* return the greater of the detail's relation with obj and
       * the relation between this obj and the detail/location
       */
      /* detail paths: (details):__:(details):__ ... */
      return PROXIMITY_D -> get_composite_proximity(relation, obj, targets[i]->get_detail_path());
    }
  }

  if(sizeof(({ obj }) & inventory)) {
    relation = obj -> get_relation(this_object());
    if(relation >= 0)
      return PROXIMITY_D -> get_opposite_proximity(relation);
  }

  return -1;
}

/* Find the nearest item in the prox chain that contains us.
 * We prefer looking through things that we are on, then close to, etc.
 */
object get_environment() {
  if(current_location) return current_location->get_object();
  return nil;
}

object get_location() { return current_location; }

/*
 * Return a list of all items that are contained in this object -
 * anything with a "in" or "on" relationship.
 */
object *get_all_inventory() {
  object *in;
  int i, n;
  int rel;

  in = ({ });
  for(i = 0, n = sizeof(inventory); i < n; i++) {
    rel = inventory[i] -> get_relation(this_object());
    if(rel == PROX_INSIDE || rel == PROX_ON) {
      in += ({ inventory[i] });
    }
  }
  /* now add the things we contain */
  for(i = 0, n = sizeof(targets); i < n; i++) {
    if(!targets[i]->is_detail()) {
      rel = targets[i] -> get_relation();
      if(rel == PROX_CONTAINS) in |= ({ targets[i] -> get_object() });
    }
  }
  return in - ({ nil });
}

object *get_deep_inventory() {
  object *items;
  int i, n;

  items = get_all_inventory();
  for(i = 0, n = sizeof(items); i < n; i++)
    items |= items[i]->get_deep_inventory();
  return items;
}

/* Only called by other methods on this object, such as move_to() */
atomic static int add_relation(int prox, object LOCATION_DATA target) {
  int i, n;

  for(i = 0, n = sizeof(targets); i < n; i++) {
    if(targets[i]->get_object() == target -> get_object()) {
      targets[i] = new_object(target);
      targets[i] -> set_relation(prox);
      return TRUE;
    }
  }

  target = new_object(target);
  targets += ({ target });
  target -> set_proximity(prox);
  target -> get_object() -> register_proximity_source(this_object());
  return TRUE;
}

atomic int remove_relation(object target) {
  int i, n, rflag;

  if(sizeof(({ target }) & inventory)) {
    inventory -= ({ target });
    target -> remove_relation(this_object());
    return TRUE;
  }
  else {
    rflag = FALSE;
    for(i = 0, n = sizeof(targets); i < n; i++) {
      if(targets[i] -> get_object() == target) {
        targets[i] -> get_object() -> unregister_proximity_source(this_object());
        targets[i] = nil;
        rflag = TRUE;
      }
    }
    targets -= ({ nil });
  }
  return rflag;
}

atomic void register_proximity_source(object source) {
  int i, n;

  for(i = 0, n = sizeof(targets); i < n; i++) {
    if(targets[i]->get_object() == source) {
      targets[i] = nil;
    }
  }
  targets -= ({ nil });

  inventory -= ({ nil });
  inventory |= ({ source });
}

atomic void unregister_proximity_source(object source) {
  inventory -= ({ source });
}

/*
 * coords depend on the type of target:
 *   scene: detail in the scene
 *   path: miletone along the path
 *   terrain/surface: ({ x, y }) on surface
 */

static int _call_move_event(int async, string type, varargs object ob, mapping args) {
  object EVENT_DATA e;
  mixed ret;

  e = EVENTS_D -> create_event(type);
  if(ob) e -> set_object(ob);
  if(args) e -> set_args(args);
  if(async) {
    EVENTS_D -> queue_event(e);
    return TRUE;
  }
  ret = EVENTS_D -> call_event(e);
  if(typeof(ret) == T_INT && ret) return TRUE;
  return FALSE;
}

/*
 * By time this is called, we'll already have checked for guards and blocks.
 * Guards and blocks are assigned to exits, not destinations.
 *
 * Triggers the following events:
 *   pre-move:release
 *   pre-move:receive
 *   pre-move:accept
 *
 *   post-move:release
 *   post-move:receive
 *   post-move:accept
 *
 *   as appropriate (if not changing overall environment):
 *     motion:in-scene
 *     motion:on-path
 *     motion:on-terrain
 *
 * pre-* events may return FALSE to block the move.
 * post-* and motion:* events should provide appropriate narration.
 *
 * N.B.: These are only called when this object is changing environments,
 * not when moving within a scene, on the same path, or in the same terrain.
 *
 */
atomic int move_to(int prox, object LOCATION_DATA location) {
  object EVENT_DATA e;
  object env;
  object *items;
  int i, n;

  if(prox == 0 && location == nil) {
    /* we want to remove all relations */
    items = inventory;
    inventory = ({ });
    for(i = 0, n = sizeof(items); i < n; i++)
      items[i] -> remove_relation(this_object());

    items = ({ });
    for(i = 0, n = sizeof(targets); i < n; i++)
      items |= targets[i]->get_object();
    for(i = 0, n = sizeof(items); i < n; i++)
      remove_relation(items[i]);
    return TRUE;
  }

  if(prox < 0 || prox > PROX_MAX) return FALSE;
  if(!location) return FALSE;

  /* check that the destination is not in development if we're not a tester */
  if(!this_object() -> is_play_tester()) {
    
  }

  env = get_environment();
  if(env) { 
    /* if we're somewhere in the game... */

    if(location -> get_object() != env) { 

      /* and we're moving to a new scene, path, or terrain... */

      if(!_call_move_event( /* does our current environment let us go? */
        FALSE, 
        "pre-move:release", 
        env, 
        ([ 
          "direct": ({ this_object() }) 
        ])
      )) return FALSE;
      if(!_call_move_event( /* does our new environment let us enter? */
        FALSE, 
        "pre-move:receive", 
        location->get_object(), 
        ([ 
          "direct": ({ this_object() }) 
        ])
      )) return FALSE;
      /* now we know that the locations will accept the move */
      if(!_call_move_event( /* do we accept the change? */
        FALSE, 
        "pre-move:accept"
      )) return FALSE;

      /* now that everyone is fine with the change, we make the change and
       * then let everyone know that we've done the change
       */
      items = env -> get_deep_inventory() - ({ this_object() }) - this_object() -> get_deep_inventory();
      for(i = 0, n = sizeof(items); i < n; i++) remove_relation(items[i]);
      remove_relation(env);

      add_relation(prox, location);

      _call_move_event(TRUE, "post-move:release", env, ([
        "direct": ({ this_object() }),
      ]));
      _call_move_event(TRUE, "post-move:receive", location->get_object(), ([
        "direct": ({ this_object() }),
      ]));
      _call_move_event(TRUE, "post-move:accept");

      return TRUE;
    }
    else { /* we're just changing within our location, so we're okay */
      /* we want to send an event with current and new coordinates */
      /* since current_location->get_object() == location->get_object(),
       * add_relation will just replace the current relationship.
       */

      /* sever relations with nearby items in the scene, and move to new 
       * relationship/detail 
       */

      items = env -> get_deep_inventory() - ({ this_object() }) - this_object() -> get_deep_inventory();
      for(i = 0, n = sizeof(items); i < n; i++) remove_relation(items[i]);

      add_relation(prox, location);
      current_location = location;

      switch(location->get_type()) {
        /* this is really about the coordinate system we want to use */
        
        case SCENE_THING:
          _call_move_event(TRUE, "motion:in-scene", this_object(), ([
            "from": current_location -> get_detail_path(),
            "to": location -> get_detail_path(),
          ]));
          break;
        case PATH_THING:
          _call_move_event(TRUE, "motion:on-path", this_object(), ([
            "from": current_location -> get_milestone(),
            "to": location -> get_milestone(),
          ]));
          break;
        case SURFACE_THING:
          _call_move_event(TRUE, "motion:on-surface", this_object(), ([
            "from": current_location -> get_coordinates(),
            "to": location -> get_coordinates(),
          ]));
          break;
        default: return FALSE;
      }

      return TRUE;
    }
  }
  else { /* we don't have an environment yet, so we're getting one */
    if(!_call_move_event(
      FALSE,
      "pre-move:receive", 
      location->get_object(), 
      ([ 
        "direct": ({ this_object() }) 
      ])
    )) return FALSE;
    if(!_call_move_event(
      FALSE, 
      "pre-move:accept"
    )) return FALSE;
    _call_move_event(TRUE, "post-move:receive", location->get_object(), ([
      "direct": ({ this_object() }),
    ]));
    _call_move_event(TRUE, "post-move:accept");

    /* establish relationship with new location */
    add_relation(prox, location);
    current_location = location;
    return TRUE;
  }

  /* if we get here, then it didn't work */
  return FALSE;
}
