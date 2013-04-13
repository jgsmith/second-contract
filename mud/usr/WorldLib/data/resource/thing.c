# include <worldlib.h>
# include <toollib.h>
# include <system/http.h>
# include <type.h>

/*
 * Represents a "thing" to the world -- lots of data here
 */

inherit resource HTTP_RESOURCE_LIB;

object HOSPITAL_OBJ hospital;
object resource_obj;

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
  object domain;
  object area;

  domain = DOMAINS_D -> get_domain(get_parameter("domain_id"));
  if(!domain) return FALSE;
  if(area_id = get_parameter("area_id")) {
    area = domain -> get_area(area_id);
    if(!area) return FALSE;
    hospital = area -> get_hospital();
  }
  else return FALSE;
  /* we're looking for something in the area *or* domain hospital 
   * unless ward_id is scene, path, or terrain
   */
  ward_id = get_parameter("ward_id");
  (users())[0]->message("ward: " + (ward_id ? ward_id : "<none>") + "\n");
  if(!ward_id) return FALSE;
  if(area) {
    switch(ward_id) {
      case "scene": 
        resource_obj = area -> get_scene(get_resource_id());
        return resource_obj ? TRUE : FALSE;
      case "path":
        resource_obj = area -> get_path(get_resource_id());
        return resource_obj ? TRUE : FALSE;
      case "terrain":
        resource_obj = area -> get_terrain(get_resource_id());
        return resource_obj ? TRUE : FALSE;
    }
  }
  resource_obj = hospital -> get_object(ward_id, get_resource_id());
  return resource_obj ? TRUE : FALSE;
}

mapping data_for_thing(string d) {
  object thing;

  if(thing) return thing -> get_properties();
  else return ([ ]);
}

/* we don't handle collections at the moment - that's for a different
 * resource handler
 */
mixed to_json(mapping metadata) {
  return JSON_P -> to_json(resource_obj->get_properties()) + ([
    "id": get_resource_id(),
  ]);
}

void transfer_info(object obj, string *path, mapping info) {
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
}

mixed from_json(mapping metadata) {
  mixed info;
  string json;

  json = implode(get_request()->get_body(), "");
  info = JSON_P -> from_json(json);
  if(typeof(info) == T_NIL) return 400;

  if(!resource_obj) {
    /* we're creating a resource */
    resource_obj = clone_object(THING_OBJ);
  }

  if(!resource_obj) return 404;

  /* this gets the simple stuff */
  transfer_info(resource_obj, ({ }), info);

  /* we still need to handle things like exits and events */
  resource_obj -> set_event_handler_source(info["events"]);
  
  return 200;
}
