# include <worldlib.h>
# include <toollib.h>
# include <system/http.h>
# include <type.h>
# include <status.h>

inherit resource HTTP_RESOURCE_LIB;

object domain;

static void create(varargs int clone) {
  resource::create(clone);

  if(clone) {
  }
}

int resource_exists() {
  domain = DOMAINS_D -> get_domain(get_parameter("domain_id"));
  if(!domain) return FALSE;
  if(typeof(get_resource_id()) == T_NIL ||
     domain -> get_area(get_resource_id())) return TRUE;
  return FALSE;
}

mapping data_for_area(string d) {
  object area;

  if(domain) area = domain -> get_area(d);
  
  if(area) return area -> get_properties();
  return ([ ]);
}

mixed to_json(mapping metadata) {
  string *list;
  string *areas;
  mapping extra;
  int i, n;

  /* domain = DOMAINS_D -> get_domain(get_parameter("domain_id")); */
  extra = ([ "domain": get_parameter("domain_id") ]);
  if(get_resource_id()) {
    return JSON_P -> to_json(data_for_area(get_resource_id()) + extra + ([
      "id": get_resource_id(),
      "name": get_resource_id(),
    ]));
  }
  else {
    list = ({ "[" });
    if(domain) areas = domain -> get_areas();
    else areas = ({ });

    for(i = 0, n = sizeof(areas); i < n; i++) {
      list += ({
        (i ? "," : "") +
        JSON_P -> to_json(data_for_area(areas[i]) + extra + ([
          "id": areas[i],
          "name": areas[i],
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
  object area;
  string id;
  int status;

  json = implode(get_request()->get_body(), "");
  info = JSON_P -> from_json(json);
  if(typeof(info) == T_NIL) return 400;

  status = 200;

  id = get_resource_id();
  if(!id) {
    id = info["name"];
    if(!id || !domain || domain->get_area(id)) return 400;
    area = domain -> create_area(id);
  }
  else {
    area = domain -> get_area(id);
    if(!area) {
      if(id == info["name"]) {
        area = domain -> create_area(id);
      }
    }
    else if(info["name"] != id) {
      /* we're changing the area's name */
      if(!domain -> rename_area(id, info["name"])) {
        return 400;
      }
      /* we really should set a new location header for this */
    }
  }

  if(!area) return 404;

  return status;
}
