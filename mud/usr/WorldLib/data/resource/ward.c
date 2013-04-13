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

  if(!get_parameter("domain_id")) return FALSE;
  if(!get_parameter("area_id")) return FALSE;
  domain = DOMAINS_D -> get_domain(get_parameter("domain_id"));
  if(!domain) return FALSE;
  area = domain -> get_area(get_parameter("area_id"));
  if(!area) return FALSE;
  if(typeof(get_resource_id()) == T_NIL ||
     area -> get_ward(get_resource_id())) return TRUE;
  return FALSE;
}

mixed to_json(mapping metadata) {
  string *list;
  string *wards;
  object ward;
  int i, n;

  if(get_resource_id()) {
    ward = area -> get_ward(get_resource_id());
    return JSON_P -> to_json(ward->get_properties() + ([
      "id": get_resource_id(),
      "area": get_parameter("area_id"),
      "domain": get_parameter("domain_id"),
    ]));
  }
  else {
    wards = area -> get_wards();
    list = ({ "[" });
    for(i = 0, n = sizeof(wards); i < n; i++) {
      list += ({ 
        (i ? "," : "") +
        JSON_P -> to_json(area->get_ward(wards[i])->get_properties() + ([
          "id": get_resource_id(),
           "area": get_parameter("area_id"),
           "domain": get_parameter("domain_id"),
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
  object ward;
  string id;

  json = implode(get_request()->get_body(), "");
  info = JSON_P -> from_json(json);
  if(typeof(info) == T_NIL) return 400;

  id = get_resource_id();
  if(!id) id = info["name"];
  if(!id) return 400;
  ward = area -> get_ward(id);
  if(!ward) {
    ward = area -> create_ward(id);
  }
  if(!ward) return 404;

  if(id != info["name"]) area -> rename_ward(id, info["name"]);

  return 200;
}
