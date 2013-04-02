# include <worldlib.h>
# include <toollib.h>
# include <system/http.h>
# include <type.h>
# include <status.h>

inherit resource HTTP_RESOURCE_LIB;

static void create(varargs int clone) {
  resource::create(clone);

  set_authentication_flags(HTTP_ALLOWS_PUBLIC_READ);

  if(clone) {
  }
}

int resource_exists() {
  return typeof(get_resource_id()) == T_NIL || 
         typeof(DOMAINS_D -> get_domain(get_resource_id())) != T_NIL;
}

mixed *content_types_provided() {
  return ({ ({ "application/json", "to_json" }) });
}

mixed *content_types_accepted() {
  return ({ ({ "application/json", "from_json" }) });
}

mapping data_for_domain(string d) {
  object domain_data;

  domain_data = DOMAINS_D -> get_domain(d);
  if(domain_data) return domain_data -> get_properties();
  else return ([ ]);
}

mixed to_json(mapping metadata) {
  string *list;
  string *domains;
  int i, n;

  if(get_resource_id()) {
    return JSON_P -> to_json(data_for_domain(get_resource_id()) + ([
      "id": get_resource_id(),
    ]));
  }
  else {
    /* we want to list all of the available verbs */
    list = ({ "[" });
    domains = DOMAINS_D -> get_domains();
    for(i = 0, n = sizeof(domains); i < n; i++) {
      list += ({
        (i ? "," : "") +
        JSON_P -> to_json(data_for_domain(domains[i]) + ([
          "id": domains[i],
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
  object domain;
  string id;

  json = implode(get_request()->get_body(), "");
  info = JSON_P -> from_json(json);
  if(typeof(info) == T_NIL) return 400;

  id = get_resource_id();
  if(!id) {
    id = info["name"];
    if(!id || DOMAINS_D -> get_domain(id)) return 400;
    domain = DOMAINS_D -> create_domain(id);
  }
  else {
    domain = DOMAINS_D -> get_domain(id);
  }
  if(!domain) return 404;

  /* we don't allow management of areas here */
  /* only members/admins for now */
  /* eventually, we need to check administrative rights */
  if(info["description"]) domain -> set_description(info["description"]);
  if(info["plan"]) domain -> set_plan(info["plan"]);
  if(info["members"]) domain -> set_members(info["members"]);
  if(info["admin"]) domain -> set_admin(info["admin"]);
  return 200;
}
