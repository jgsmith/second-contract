# include <system.h>
# include <system/http.h>
# include <toollib.h>
# include <status.h>

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

mixed to_json(mapping metadata) {
  mixed *info;

  info = status();

  return JSON_P -> to_json(([
    "name": "Second Contract",
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
  ]));
}
