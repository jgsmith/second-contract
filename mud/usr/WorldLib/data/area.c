# include <type.h>
# include <worldlib.h>
# include <worldlib/proximity.h>
# include <data.h>

/*
 * An area is a package of rooms, NPCs, etc., that together make a playable
 * bit of the game. They can be put into play after being constructed.
 *
 * Builders work with areas instead of doing in-game/game-wide construction.
 *
 */

inherit hospital HOSPITAL_LIB;

/* primary builder for this area - 
 *   if set, then this area is not in play, but is being built/tested 
 *   if not set, then this area can be in play and only domain admins can
 *     make changes
 */
object owner; /* the character - not the email address */

/* actual scenes, etc., in the area with ids to allow management via web */
mapping scenes; 
mapping paths;
mapping terrains;

static void create(varargs int clone) {
  hospital::create(clone);
  if(clone) {
    scenes = ([ ]);
    paths = ([ ]);
    terrains = ([ ]);
  }
}

/* If not playable, then only characters marked as testers and creators can
 * be placed in a room in the deep inventory of top_room
 */
int is_playable() { return typeof(owner) == T_NIL; }

object get_scene(string nom) { return scenes[nom]; }
object get_path(string nom) { return paths[nom]; }
object get_terrain(string nom) { return terrains[nom]; }

int add_scene(string id, object THING_OBJ s) {
  if(scenes[id] || !s) return FALSE;
  scenes[id] = s;
  return TRUE;
}
int add_path(string id, object THING_OBJ s) {
  if(paths[id] || !s) return FALSE;
  paths[id] = s;
  return TRUE;
}
int add_terrain(string id, object THING_OBJ s) {
  if(terrains[id] || !s) return FALSE;
  terrains[id] = s;
  return TRUE;
}

mapping get_scene_mapping() {
  if(explode(object_name(previous_object()),"#")[0] == HTTP_THING_RESOURCE)
    return scenes;
  return ([ ]);
}
mapping get_path_mapping() {
  if(explode(object_name(previous_object()),"#")[0] == HTTP_THING_RESOURCE)
    return paths;
  return ([ ]);
}
mapping get_terrain_mapping() {
  if(explode(object_name(previous_object()),"#")[0] == HTTP_THING_RESOURCE)
    return terrains;
  return ([ ]);
}

mapping get_properties() {
  mapping info;

  info = hospital::get_properties();

  info["scenes"] = scenes ? map_indices(scenes) : ({ });
  info["paths"] = paths ? map_indices(paths) : ({ });
  info["terrains"] = terrains ? map_indices(terrains) : ({ });
  info["owner"] = owner ? owner -> get_name() : "";

  return info;
}

/* called when we want to remove the area - we destroy every object so that
 * we don't leave a lot of objects lying around. Characters get moved elsewhere.
 */

static destroy_inventory(object ob, object LOCATION_DATA dest) {
  object *obs;
  int i, n;

  obs = ob -> get_all_inventory();
  for(i = 0, n = sizeof(obs); i < n; i++) {
    if(obs[i] -> is_player()) {
      /* move them elsewhere */
      /* obs[i] -> move_to(PROX_INSIDE, ); */
    }
    else {
      destroy_inventory(obs[i], dest);
      obs[i] -> move_to(0, nil);
      destruct_object(obs[i]);
    }
  }
}

atomic void empty_area() {
  object LOCATION_DATA dest;
  object *items;
  int i, n;

  dest = new_object(LOCATION_DATA);
  dest -> set_object(/*HOSPITAL_D -> get_world_room()*/);
  /* now go through scenes, etc., and do the same for any that are left */
  items = map_values(scenes) - ({ nil });
  for(i = 0, n = sizeof(items); i < n; i++) {
    if(items[i]) {
      destroy_inventory(items[i], dest);
      destruct_object(items[i]);
    }
  }
  items = map_values(paths) - ({ nil });
  for(i = 0, n = sizeof(items); i < n; i++) {
    if(items[i]) {
      destroy_inventory(items[i], dest);
      destruct_object(items[i]);
    }
  }
  items = map_values(terrains) - ({ nil });
  for(i = 0, n = sizeof(items); i < n; i++) {
    if(items[i]) {
      destroy_inventory(items[i], dest);
      destruct_object(items[i]);
    }
  }

  /* now empty out the wards */
  
}
