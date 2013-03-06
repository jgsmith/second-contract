# include <system.h>
# include <system/http.h>
# include <toollib.h>
# include <status.h>
# include <type.h>
# include <gamelib.h>

inherit resource HTTP_RESOURCE_LIB;

void create(varargs int clone) {
  resource::create(clone);

  set_authentication_flags(HTTP_ALLOWS_PUBLIC_READ);

  if(clone) {
  }
}

int is_authorized(string auth) { return 1; }

int forbidden() { return 0; }

/* this isn't a collection of resources */
int resource_exists() {
  return typeof(get_resource_id()) == T_NIL;
}

mixed *content_types_provided() {
  return ({ ({ "application/json", "to_json" }) });
}

mixed *content_types_accepted() {
  return ({ ({ "application/json", "from_json" }) });
}

int valid_entity_length(int length) {
  return length <= status(ST_STRSIZE);
}

mixed *allowed_methods() {
  return resource::allowed_methods() - ({ "DELETE" }) + ({ "POST", "PUT" });
}

mixed from_json(mapping metadata) {
  mixed info;
  string json;

  json = implode(get_request() -> get_body(), "");
  info = JSON_P -> from_json(json);
  if(typeof(info) == T_NIL) return 400;
  if(typeof(info["name"]) != T_NIL) GAME_D -> set_name(info["name"]);
  return 200;
}

mixed to_json(mapping metadata) {
  mixed *info;

  info = status();

  return JSON_P -> to_json(([
    "name": GAME_D -> get_name(),
    "version": info[ST_VERSION],
    "starttime": info[ST_STARTTIME],
    "boottime": info[ST_BOOTTIME],
    "uptime": info[ST_UPTIME],
    "swap_size": info[ST_SWAPSIZE],
    "swap_used": info[ST_SWAPUSED],
    "sectorsize": info[ST_SECTORSIZE],
    "swaprate_1min": info[ST_SWAPRATE1],
    "swaprate_5min": info[ST_SWAPRATE5],
    "static_memory_size": info[ST_SMEMSIZE],
    "static_memory_used": info[ST_SMEMUSED],
    "dynamic_memory_size": info[ST_DMEMSIZE],
    "dynamic_memory_used": info[ST_DMEMUSED],
    "object_table_size": info[ST_OTABSIZE],
    "objects": info[ST_NOBJECTS],
    "call_out_size": info[ST_COTABSIZE],
    "call_out_short": info[ST_NCOSHORT],
    "call_out_long": info[ST_NCOLONG],
    "user_size": info[ST_UTABSIZE],
    "user_used": sizeof(users()),
    "telnet_ports": info[ST_TELNETPORTS],
    "binary_ports": info[ST_BINARYPORTS],
    "string_size": info[ST_STRSIZE],
    "array_size": info[ST_ARRAYSIZE],
    "precompiled_objects": info[ST_PRECOMPILED],
  ]));
}
