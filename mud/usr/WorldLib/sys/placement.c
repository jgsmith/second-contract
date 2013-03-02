# include <worldlib.h>
# include <worldlib/proximity.h>
# include <system.h>

/*
 * This daemon manages how objects are related to each other.
 *
 * rug is-on floor
 * rug is-over hole
 * hole is-in floor
 *
 * In this case, the hole in the floor is concealed by the rug until the
 *  rug is moved, revealing the hole (since the movement breaks the is-beneath
 *  connection for the hole).
 * "move rug over hole" will result in the hole being covered by the rug again
 */

/*
 * we want objects to contain objects in a scene sense.
 * It would be good to have an object that indicated a region in the game
 * and have objects associated with it, but not have it do anything in the
 * sense of affecting the local environment description.
 *
 */

int *opposite_relations;
mapping forward;
mapping reverse;

void create() {
  forward = ([ ]);
  reverse = ([ ]);

  opposite_relations = ({
    PROX_CONTAINS, 
    PROX_CLOSE, 
    PROX_AGAINST, 
    PROX_OVER, 
    PROX_UNDER,
    PROX_NEAR,
    PROX_UNDER,
    PROX_BEFORE,
    PROX_BEHIND,
    PROX_BESIDE,
    PROX_INSIDE,
    PROX_GUARD,
    PROX_BLOCK,
  });
}

/* returns the closest relationship between the two */
int get_relation(object contained, object container) {
  mapping forward_m, reverse_m;
  string key, r_key;
  int i, j, n;

  if(container -> is_detail() || contained -> is_detail()) {
    ERROR_D -> message("Relationships between details are not supported in the global position system.");
    return -1;
  }

  forward_m = MAPPING_D -> specific_mapping(forward, key = object_name(container));
  reverse_m = MAPPING_D -> specific_mapping(reverse, key);

  if(!forward_m[key] && !reverse_m[key]) return -1;

  for(i = 0; i <= DISTANCE_MAX; i++) {
    if(forward_m[key] && forward_m[key][i]) {
      for(j = 0, n = sizeof(forward_m[key][i]); j < n; j++) {
        if(forward_m[key][i][j][1] == contained) return forward_m[key][i][j][0];
      }
    }
    if(reverse_m[key] && reverse_m[key][i]) {
      for(j = 0, n = sizeof(reverse_m[key][i]); j < n; j++) {
        /* TODO: Make sure we really need the opposite_relations[...] here */
        if(reverse_m[key][i][j][1] == contained) return reverse_m[key][i][j][0];
      }
    }
  }
  return -1;
}

/* returns all items that are in this container - directly related */
object *get_all_inventory(object container, int distance) {
  object *items;
  mapping forward_m;
  string f_key;
  int i, j, n;

  forward_m = MAPPING_D -> specific_mapping(forward, f_key = object_name(container));

  if(!forward_m[f_key]) return ({ });

  items = ({ });
  if(distance < 0) distance = 0;
  if(distance > DISTANCE_MAX) distance = DISTANCE_MAX;

  for(i = 0; i <= distance; i++) {
    if(forward_m[f_key][i]) {
      for(j = 0, n = sizeof(forward_m[f_key][i]); j < n; j++) {
        items += forward_m[f_key][i][j][1..1];
      }
    }
  }

  return items;
}

object *get_deep_inventory(object container, int distance) {
  object *items;
  int i, j, n;

  if(distance < 0) distance = 0;
  if(distance > DISTANCE_MAX) distance = DISTANCE_MAX;

  items = get_all_inventory(container, distance);
  if(distance) {
    for(i = 0, n = sizeof(items); i < n; i++) {
      items |= get_deep_inventory(items[i], distance-1);
    }
  }
  return items;
}

int remove_relation(object contained, object container, int relation) {
  mapping forward_m, reverse_m;
  string f_key, r_key;
  mixed *info;
  int opposite_rel;
  int i, j, n;

  if(container -> is_detail() || contained -> is_detail()) {
    ERROR_D -> message("Relationships between details are not supported in the global position system.");
    return 0;
  }
  if(!relation) relation = PROX_INSIDE;
  if(relation != -1)
    opposite_rel = opposite_relations[relation];
  else
    opposite_rel = -1;

  forward_m = MAPPING_D -> specific_mapping(forward, f_key = object_name(container));
  reverse_m = MAPPING_D -> specific_mapping(reverse, r_key = object_name(contained));

  for(i = 0; i <= DISTANCE_MAX; i++) {
    if(forward_m[f_key] && forward_m[f_key][i]) {
      info = forward_m[f_key][i];
      for(j = 0, n = sizeof(info); j < n; j++) {
        if((relation == -1 || info[j][0] == relation) && info[j][1] == contained) 
          info[j] = nil;
      }
      forward_m[f_key][i] -= ({ nil });
    }
    if(reverse_m[r_key] && reverse_m[r_key][i]) {
      info = reverse_m[r_key][i];
      for(j = 0, n = sizeof(info); j < n; j++) {
        if((opposite_rel == -1 || info[j][0] == opposite_rel) && info[j][1] == container) 
          info[j] = nil;
      }
      reverse_m[r_key][i] -= ({ nil });
    }
  }
  return 1;
}

int add_relation(object contained, object container, int distance, varargs int relation) {
  mapping forward_m, reverse_m;
  string f_key, r_key;
  int opposite_rel;
  int i, j, n;

  /* for now, we don't handle the relationships involving details */
  if(container -> is_detail() || contained -> is_detail()) {
    ERROR_D -> message("Relationships between details are not supported in the global position system.");
    return 0;
  }

  if(!relation) relation = PROX_INSIDE;
  opposite_rel = opposite_relations[relation];

  forward_m = MAPPING_D -> specific_mapping(forward, f_key = object_name(container));
  reverse_m = MAPPING_D -> specific_mapping(reverse, r_key = object_name(contained));

  if(!forward_m[f_key]) forward_m[f_key] = allocate(DISTANCE_MAX+1);
  if(!reverse_m[r_key]) reverse_m[r_key] = allocate(DISTANCE_MAX+1);

  /* first, we remove any previous equivalent relation so we only have a
   * single relation/distance pair per object pair
   */
  remove_relation(contained, container, relation);

  if(!forward_m[f_key][distance]) forward_m[f_key][distance] = ({ });
  if(!reverse_m[r_key][distance]) reverse_m[r_key][distance] = ({ });

  forward_m[f_key][distance] += ({ ({ relation, contained }) });
  reverse_m[r_key][distance] += ({ ({ opposite_rel, container }) });

  return 1;
  /* each of these arrays is a list of arrays:
   * ({ relation, object })
   */
}

object environment(object thing) {
  mapping graph;
  object env;
  string key;
  int i, j, n;

  /* for now, details don't have environments :-/ */
  /* if(thing -> is_detail()) return nil; */
  if(thing -> is_detail()) {
    ERROR_D -> message("Relationships between details are not supported in the global position system.");
    return nil;
  }

  graph = MAPPING_D -> specific_mapping(reverse, key = object_name(thing));
  if(!graph[key]) return nil;

  /* we want the closest item that we're inside or on */
  for(i = 0; i <= DISTANCE_MAX; i++) {
    if(graph[key][i]) {
      /* we look for INSIDE first, and then ON
       * but a closer ON overrides a more distance INSIDE
       */
      for(j = 0, n = sizeof(graph[key][i]); j < n; j++)
        if(graph[key][i][j][0] == PROX_CONTAINS) return graph[key][i][j][1];
      for(j = 0, n = sizeof(graph[key][i]); j < n; j++) {
        if(graph[key][i][j][0] == PROX_UNDER) {
          env = environment(graph[key][i][j][1]);
          if(env) return env;
        }
      }
      for(j = 0, n = sizeof(graph[key][i]); j < n; j++) {
        if(graph[i][j][0] != PROX_UNDER && graph[i][j][0] != PROX_CONTAINS && graph[i][j][0] != PROX_INSIDE) {
          env = environment(graph[key][i][j][1]);
          if(env) return env;
        }
      }
    }
  }
  return nil;
}

/*
 * When an object moves between "rooms", anything that isn't within
 * their body distance (INSIDE/ON) has their relationship removed. Then new
 * relationships are added for the new "room".
 * that is, anything that is in the same place they are at the moment has its
 * relationship with the object removed unless it's INSIDE/ON the object.
 */

int move_object(object mover, object destination, int relationship, varargs int distance) {
  object env, *items;
  int tmp;
  int i, n;

  /* can't move details and can't move into a detail */
  if(mover -> is_detail() || destination -> is_detail()) {
    ERROR_D -> message("Relationships between details are not supported in the global position system.");
    return 0;
  }

  env = environment(mover);
  if(env) {
    /* sever links */
    items = get_deep_inventory(env, DISTANCE_MAX) - ({ mover });
    for(i = 0, n = sizeof(items); i < n; i++) {
      /* if this item is not on/in the mover, then sever links */
      tmp = get_relation(items[i], mover);
      if(tmp != PROX_ON && tmp != PROX_INSIDE) {
        if(tmp != -1) remove_relation(items[i], mover, -1);
        tmp = get_relation(mover, items[i]);
        if(tmp != -1) remove_relation(mover, items[i], -1);
      }
    }
  }

  /* relationship should be PROX_INSIDE or PROX_ON */
  add_relation(mover, destination, distance ? distance : relationship == PROX_INSIDE ? DISTANCE_IN : DISTANCE_CLOSE, relationship || PROX_INSIDE);
  return 1;
}

/* given a set of details or objects, get all of the details or objects
 * within a given distance and an optional given relation
 */
object *get_within_proximate_distance(object *origins, int distance, varargs int relation) {
  object *ret;
  int i, n;

  if(sizeof(origins) == 0) return ({ });
  if(sizeof(origins) > 1) {
    ret = ({ });
    for(i = 0, n = sizeof(origins); i < n; i++)
      ret |= get_within_proximate_distance(origins[i..i], distance, relation);
    return ret;
  }

  /* we have a single item */
  if(origins[0] -> is_detail()) {
    /* details handle their own relationships, so we ask them. Details only
     * relate to details they contain, so we can't climb out of the object
     * in which the detail is located.
     */
    return origins[0] -> get_within_proximate_distance(distance, relation);
  }
  else {
    ret = ({ });
    /* now we want to return any objects that we know have the relationship
     * requested (or its reciprocal), and any details of those objects that
     * are within the distance requested
     */
    

    /* we add details last since the object will give us all of them */
    /* ret |= (origins[0] -> get_details_within_proximate_distance(distance, relation) || ({ })); */
    
  }

  return ret;
}
