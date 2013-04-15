# include <worldlib.h>
# include <toollib.h>
# include <system/http.h>
# include <type.h>

inherit resource HTTP_RESOURCE_LIB;

object area;

static void create(varargs int clone) {
  resource::create(clone);

  if(clone) {
  }
}

/* URLs:
 * /api/worldlib/ward/:domain_id/:area_id
 * /api/worldlib/ward/:domain_id/:area_id/:id
 */
int resource_exists() {
  object domain;
  string domain_id, area_id;

  if(!(domain_id = get_parameter("domain_id"))) return FALSE;
  if(!(area_id = get_parameter("area_id"))) return FALSE;
  domain = DOMAINS_D -> get_domain(domain_id);
  if(!domain) return FALSE;
  area = domain -> get_area(area_id);
  if(!area) return FALSE;
  if(typeof(get_resource_id()) == T_NIL ||
     area -> get_ward(get_resource_id())) return TRUE;
  return FALSE;
}

mixed to_json(mapping metadata) {
  string *list;
  string *wards;
  int i, n;

  if(get_resource_id()) {
    return JSON_P -> to_json(([
      "id": get_resource_id(),
      "name": get_resource_id(),
      "area": get_parameter("area_id"),
      "domain": get_parameter("domain_id"),
      "objects": map_indices(area->get_objects(get_resource_id())),
    ]));
  }
  else {
    wards = area -> get_wards();
    list = ({ "[" });
    for(i = 0, n = sizeof(wards); i < n; i++) {
      list += ({ 
        (i ? "," : "") +
        JSON_P -> to_json(([
          "id": wards[i],
          "name": wards[i],
          "area": get_parameter("area_id"),
          "domain": get_parameter("domain_id"),
          "objects": map_indices(area->get_objects(wards[i])),
        ])) 
      });
    }
    list += ({ "]" });
    return list;
  }
}

mixed from_json(mapping metadata) {
  mixed info;
  string json;
  string id;
  int has_ward;

  json = implode(get_request()->get_body(), "");
  info = JSON_P -> from_json(json);
  if(typeof(info) == T_NIL) return 400;

  id = get_resource_id();
  if(!id) id = info["name"];
  if(!id) return 400;
  if(!area -> query_ward_exists(id)) {
    if(!area -> add_ward(id)) return 409;
  }

  if(id != info["name"]) 
    if(!area -> rename_ward(id, info["name"])) return 409;

  return 200;
}
