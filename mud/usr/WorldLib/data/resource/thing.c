# include <worldlib.h>
# include <toollib.h>
# include <system/http.h>
# include <type.h>

/*
 * Represents a "thing" to the world -- lots of data here
 */

inherit resource HTTP_RESOURCE_LIB;

object resource_obj;
object domain;
object area;

static void create(varargs int clone) {
  resource::create(clone);

  if(clone) {
  }
}

/* how do we want to address things via the rest api? 
 *
 * /worldlib/thing  /:domain_id/:area_id/:ward_id/:name
 * /worldlib/scene  /:domain_id/:area_id         /:name , :ward_id = scene
 * /worldlib/path   /:domain_id/:area_id         /:name , :ward_id = path
 * /worldlib/terrain/:domain_id/:area_id         /:name , :ward_id = terrain
 */
int resource_exists() {
  string area_id, ward_id;

  domain = DOMAINS_D -> get_domain(get_parameter("domain_id"));
  if(!domain) return FALSE;
  if(area_id = get_parameter("area_id")) {
    area = domain -> get_area(area_id);
    if(!area) return FALSE;
  }
  else return FALSE;
  /* we're looking for something in the area
   * unless ward_id is scene, path, or terrain
   */
  ward_id = get_parameter("ward_id");
  if(typeof(ward_id) == T_NIL || ward_id == "") return FALSE;
  switch(ward_id) {
    case "scene": 
      if(typeof(get_resource_id()) == T_NIL) return TRUE;
      resource_obj = area -> get_scene(get_resource_id());
      break;
    case "path":
      if(typeof(get_resource_id()) == T_NIL) return TRUE;
      resource_obj = area -> get_path(get_resource_id());
      break;
    case "terrain":
      if(typeof(get_resource_id()) == T_NIL) return TRUE;
      resource_obj = area -> get_terrain(get_resource_id());
      break;
    default:
      if(!area -> get_ward(ward_id)) return FALSE;
      if(typeof(get_resource_id()) == T_NIL) return TRUE;
      resource_obj = area -> get_object(ward_id, get_resource_id());
      break;
  }
  return resource_obj ? TRUE : FALSE;
}

mixed to_json(mapping metadata) {
  string *list;
  string ward_id;
  mapping items;
  string *keys;
  int i, n;

  if(resource_obj) {
    return JSON_P -> to_json(resource_obj->get_properties() + ([
      "id": get_resource_id(),
    ]));
  }
  else {
    list = ({ "[" });
    ward_id = get_parameter("ward_id");
    if(area) {
      switch(ward_id) {
        case "scene": items = area -> get_scene_mapping(); break;
        case "path":  items = area -> get_path_mapping(); break;
        case "terrain":  items = area -> get_terrain_mapping(); break;
        default: items = area -> get_objects(ward_id); break;
      }
    }
    if(items) {
      keys = map_indices(items);
      for(i = 0, n = sizeof(keys); i < n; i++) {
        list += ({ (i ? "," : "") +
          JSON_P -> to_json(items[keys[i]]->get_properties() + ([
            "id": keys[i],
          ]))
        });
      }
    }
    list += ({ "]" });
    return list;
  }
}

atomic void transfer_info(object obj, string *path, mapping info) {
  string *keys;
  int i, n;

  keys = map_indices(info);
  for(i = 0, n = sizeof(keys); i < n; i++) {
    if(typeof(info[keys[i]]) == T_MAPPING) {
      transfer_info(obj, path+({ keys[i] }), info[keys[i]]);
    }
    else {
      obj -> set_property(path + ({ keys[i] }), info[keys[i]]);
    }
  }

  if(info["ident"]) {
    if(info["ident"]["name"]) obj -> set_name(info["ident"]["name"]);
    if(info["ident"]["cap_name"]) obj -> set_cap_name(info["ident"]["cap_name"]);
  }
  if(info["ur"]) {
  }
}

atomic mixed from_json(mapping metadata) {
  mixed info;
  string json;
  string id;
  int stat;
  string ward_id;

  json = implode(get_request()->get_body(), "");
  info = JSON_P -> from_json(json);
  if(typeof(info) == T_NIL) return 400;
  id = get_resource_id();

  if(!id) return 400;

  if(!resource_obj) {
    /* we're creating a resource */
    ward_id = get_parameter("ward_id");
    if(!area || !ward_id) return 400;

    resource_obj = clone_object(THING_OBJ);
    switch(ward_id) {
      case "scene": stat = area -> add_scene(id, resource_obj); break;
      case "path": stat = area -> add_path(id, resource_obj); break;
      case "terrain": stat = area -> add_terrain(id, resource_obj); break;
      default:
        stat = area -> add_object(ward_id, resource_obj);
        break;
    }
    if(!stat) {
      destruct_object(resource_obj);
      return 400;
    }
  }

  if(!resource_obj) return 404;

  /* this gets the simple stuff */
  transfer_info(resource_obj, ({ }), info);

  /* we still need to handle things like exits and events */
  resource_obj -> set_event_handler_source(info["events"]);
  
  return 200;
}
