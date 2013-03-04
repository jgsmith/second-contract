# include <system.h>
# include <system/http.h>
# include <toollib.h>
# include <iflib.h>

inherit resource HTTP_RESOURCE_LIB;

void create(varargs int clone) {
  resource::create(clone);

  set_authentication_flags(HTTP_ALLOWS_PUBLIC_READ);

  if(clone) {
  }
}

mixed *content_types_provided() {
  return ({ ({ "application/json", "to_json" }) });
}

mapping data_for_verb(string verb) {
  object verb_data;

  verb_data = VERB_D -> get_verb(verb);
  if(verb_data) return verb_data -> get_properties();
  else return ([ ]);
}

mixed to_json(mapping metadata) {
  string *list;
  string *verbs;
  int i, n;

  if(get_resource_id()) {
    return JSON_P -> to_json(data_for_verb(get_resource_id()) + ([
      "id": get_resource_id(),
    ]));
  }
  else {
    /* we want to list all of the available verbs */
    list = ({ "[" });
    verbs = VERB_D -> get_verb_ids();
    for(i = 0, n = sizeof(verbs); i < n; i++) {
      list += ({ 
        JSON_P -> to_json(data_for_verb(verbs[i]) + ([
          "id": verbs[i],
        ]))
      });
    }
    list += ({ "]" });
    return list;
  }
}
