# include <iflib.h>
# include <worldlib.h>
# include <type.h>
# include <worldlib/details.h>

/*
 * Holds information about an observable aspect of something. Details can not
 * be manipulated (e.g., taken or dropped).
 */

inherit id ID_DATA_LIB;

mapping details;

/* These are types of things that can be worn/wielded here - opaque strings
 * as far as this part is concerned. Allows us to hold or wear things.
 * Numbers indicate how many we can have attached (if mass allows).
 *
 * Worn items might cover details.
 */
mapping worn_types;
mapping held_types;

/* The following two arrays are to make it easier to manage what is worn
 * or held by this object
 */
object *worn_items;
object *held_items;

/* Allows us to restrict which kinds of relations objects can have with this
 * detail.
 */
int *proximities;

string ur_ward, ur_item;
int detail_flags;
int damage; /* too much damage and a detachable detail might detach */

void create(varargs int clone) {
  id::create(clone);
  if(clone) {
    details = ([ ]);
    worn_types = ([ ]);
    held_types = ([ ]);
    worn_items = ({ });
    held_items = ({ });
    detail_flags = DETAIL_WORN_ITEMS_HIDE_DETAILS;
  }
}

object *get_worn_items() { return worn_items + ({ }); }
object *get_held_items() { return held_items + ({ }); }

object *get_all_worn_items() {
  object *obs;
  string *keys;
  int i, n;

  obs = ({ }) + worn_items;
  keys = map_indices(details);
  for(i = 0, n = sizeof(keys); i < n; i++)
    if(details[keys[i]]) obs |= details[keys[i]] -> get_all_worn_items();
  return obs;
}

object *get_all_held_items() {
  object *obs;
  string *keys;
  int i, n;

  obs = ({ }) + held_items;
  keys = map_indices(details);
  for(i = 0, n = sizeof(keys); i < n; i++)
    if(details[keys[i]]) obs |= details[keys[i]] -> get_all_held_items();
  return obs;
}

int is_detachable() {
  return ur_ward && ur_item;
}

mapping get_properties() {
  mapping info;
  string *keys;
  int i, n;

  info = id::get_properties();
  info["wear"] = worn_types;
  info["hold"] = held_types;

  /* We don't necessarily need these unless we track some other stats 
   * for the details as well - we're getting dangerously close to 
   * having the detail be a full item -- instead, we should probably 
   * allow details to be detached and instantiated with an ur object.
   *
   * When the detail is detached, any worn or held items need to go with it.
   * (e.g., when an arm gets lopped off in combat)
   *
   * A detail is detachable *only* if the ur info is defined.
   */
  if(ur_ward && ur_item) {
    info["ur"] = ([ 
      "hospital": "global",
      "ward": ur_ward,
      "item": ur_item,
    ]);
  }
  info["worn_items_hide_details"] = !!(detail_flags & DETAIL_WORN_ITEMS_HIDE_DETAILS);
  info["targetable"] = !!(detail_flags & DETAIL_TARGETABLE);

  info["details"] = ([ ]);
  keys = map_indices(details);
  for(i = 0, n = sizeof(keys); i < n; i++)
    info["details"][keys[i]] = details[keys[i]]->get_properties();

  return info;
}

mixed get_property(string *path) {
  object *obs;
  int i, n;
  string keys;

  switch(path[0]) {
    case "adjective":
    case "noun":
    case "plural-adjective":
    case "plural-noun":
    case "description":
      return id::get_property(path);
    case "wear":
      if(sizeof(path) == 1) return map_indices(worn_types);
      if(sizeof(path) > 2) return nil;
      if(worn_types[path[1]]) return worn_types[path[1]];
      return 0;
    case "hold":
      if(sizeof(path) == 1) return map_indices(held_types);
      if(sizeof(path) > 2) return nil;
      if(held_types[path[1]]) return held_types[path[1]];
      return 0;
    case "worn":
      if(sizeof(path) == 1) return worn_items + ({ });
      return nil;
    case "held":
      if(sizeof(path) == 1) return held_items + ({ });
      return nil;
    case "all-worn": /* for this detail and any sub-details */
      if(sizeof(path) != 1) return nil;
      return get_all_worn_items();
    case "all-held": /* for this detail and any sub-details */
      if(sizeof(path) != 1) return nil;
      return get_all_held_items();
    case "targetable":
      if(sizeof(path) != 1) return nil;
      return !!(detail_flags & DETAIL_TARGETABLE);
    case "worn-items-hide-details":
      if(sizeof(path) != 1) return nil;
      return !!(detail_flags & DETAIL_WORN_ITEMS_HIDE_DETAILS);
    case "detail":
      if(sizeof(path) > 1) {
        if(sizeof(path) > 2) {
          if(!details[path[1]]) return nil;
          switch(path[2]) {
            case "preposition": /* detail:name:proposition:... */
              if(sizeof(path) == 3) {
                return WORDS_D -> preposition_name(details[path[1]][0]);
              }
              if(sizeof(path) == 4) {
                return details[path[1]][0] == WORDS_D -> preposition_number(path[3]);
              }
              return nil;
            default:
              return details[path[1]][1] -> get_property(path[2..]);
          }
        }
        else {
          return details[path[1]] ? 1 : 0;
        }
      }
      else {
        return map_indices(details);
      }
  }
  return nil;
}

int set_property(string *path, mixed value) {
  int del_if_false;

  if(!sizeof(path)) return FALSE;

  switch(path[0]) {
    case "noun":
    case "adjective":
    case "plural-noun":
    case "plural-adjective":
    case "description":
      return id::set_property(path, value);
    case "wear":
      if(sizeof(path) == 2) {
        if(!value) {
          worn_types[path[1]] = nil;
        }
        else {
          if(typeof(value) != T_INT) return FALSE;
          worn_types[path[1]] = value;
        }
        return TRUE;
      }
      return FALSE;
    case "hold":
      if(sizeof(path) == 2) {
        if(!value) {
          worn_types[path[1]] = nil;
        }
        else {
          if(typeof(value) != T_INT) return FALSE;
          worn_types[path[1]] = value;
        }
        return TRUE;
      }
      return FALSE;
    case "detail":
      if(sizeof(path) == 2) {
        if(!value) {
          details[path[1]] = nil;
          return TRUE;
        }
        return FALSE;
      }
      if(sizeof(path) > 2) {
        del_if_false = FALSE;
        if(!details[path[1]]) {
          del_if_false = TRUE;
          details[path[1]] = clone_object(DETAIL_DATA);
        }
        if(details[path[1]] -> set_property(path[2..], value)) return TRUE;
        if(del_if_false) details[path[1]] = nil;
        return FALSE;
      }
    default: return FALSE;
  }
}
