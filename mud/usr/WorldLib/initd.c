# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <config.h>
# include <system.h>
# include <worldlib.h>
# include <toollib.h>
# include <worldlib/proximity.h>
# include <data.h>

static int create(varargs int clone) {
  string *files;
  int i, n;

  files = ({
    DETAIL_DATA, EXIT_DATA, LOCATION_DATA, SENSATION_DATA,
    SENSATION_SET, SENSATION_END_DATA, EVENT_TIMER_DATA,
    GENETICS_DATA, DOMAIN_DATA,
    HTTP_DOMAIN_RESOURCE,
    THING_OBJ, WARD_OBJ, HOSPITAL_OBJ,
    PRIORITY_QUEUE, PRIORITY_QUEUE_ITEM,
    EXITS_D, HOSPITAL_D, CHARACTER_D, PROXIMITY_D,
    WORLD_EVENTS_D, DOMAINS_D,
  });

  for(i = 0, n = sizeof(files); i < n; i++)
    if(!find_object(files[i])) compile_object(files[i]);
}

private int configure_object(object ob, mixed json) {
  int i, n;
  string *props;
  string *events;
  if(json["properties"]) {
    /* read through each property and call set_property... */
    props = map_indices(json["properties"]);
    for(i = 0, n = sizeof(props); i < n; i++) {
      ob -> set_property(explode(props[i], ":"), json["properties"][props[i]]);
    }
    if(json["events"]) {
      ob -> add_event_handlers(json["events"]);
    }
  }
  return TRUE;
}

void debug(string msg) {
  find_object(DRIVER) -> message(msg);
}

void initialize_data() {
  object ob;
  object *obs;
  mapping ward;
  string *fnames, *wards;
  mixed *dir;
  mixed json;
  mapping delayed; /* keyed off of "ward" */
  mixed *tmp_delayed;
  int i, n, j, m;

  HTTP_D -> add_resource_handler("/api/worldlib/domain", HTTP_DOMAIN_RESOURCE);
  HTTP_D -> add_resource_handler("/api/worldlib/domain/:id", HTTP_DOMAIN_RESOURCE);
/*
  HTTP_D -> add_resource_handler("/api/worldlib/area/:domain_id/:id", HTTP_AREA_RESOURCE);
  HTTP_D -> add_resource_handler("/api/worldlib/ward/:domain_id/:area_id", HTTP_WARD_RESOURCE);
  HTTP_D -> add_resource_handler("/api/worldlib/ward/:domain_id/:area_id/:id", HTTP_WARD_RESOURCE);
  HTTP_D -> add_resource_handler("/api/worldlib/thing/:domain_id/:area_id/:ward_id/:id", HTTP_THING_RESOURCE);
*/

  /*
   * We want to initialize various things with data from the json/
   * directory.
   */
  /* Note: if an object doesn't specify an ur object, then it
   * gets created without an ur object */
  
  /* /usr/WorldLib/json/hospital/$ward/$object.json */
  dir = get_dir("/usr/WorldLib/json/hospital/*");
  wards = ({ });
  delayed = ([ ]);
  for(i = 0, n = sizeof(dir[0]); i < n; i++) {
    if(dir[1][i] == -2 && dir[0][i][0] != '.') {
      wards += ({ dir[0][i] });
      HOSPITAL_D -> add_ward(dir[0][i]);
    }
  }
  for(i = 0, n = sizeof(wards); i < n; i++) {
    dir = get_dir("/usr/WorldLib/json/hospital/" + wards[i] + "/*.json");
    for(j = 0, m = sizeof(dir[0]); j < m; j++) {
      if(dir[1][j] > 0) {
        json = JSON_P -> from_json(read_file("/usr/WorldLib/json/hospital/" + wards[i] + "/" + dir[0][j]));
        if(json) {
          if(json["ur"]) {
            if(json["ur"]["ward"]) {
              ob = HOSPITAL_D -> create_object(json["ur"]["ward"], json["ur"]["object"]);
              if(!ob) {
                if(!delayed[json["ur"]["ward"]]) delayed[json["ur"]["ward"]] = ({ });
                delayed[json["ur"]["ward"]] += ({ json });
              }
            }
            else {
              ob = HOSPITAL_D -> create_object(wards[i], json["ur"]["object"]);
              if(!ob) {
                if(!delayed[wards[i]]) delayed[wards[i]] = ({ });
                delayed[wards[i]] += ({ json });
              }
            }
          }
          else {
            ob = clone_object(THING_OBJ);
          }
          debug("We have an object to configure for " + dir[0][j]);
          if(ob) {
            if(configure_object(ob, json)) {
              HOSPITAL_D -> add_object(wards[i], ob);
            }
            else { destruct_object(ob); }
          }
        }
      }
    }
  }

  wards = map_indices(delayed);
  while(sizeof(wards)) {
    for(i = 0, n = sizeof(wards); i < n; i++) {
      tmp_delayed = ({ });
      for(j = 0, m = sizeof(delayed[wards[i]]); j < m; j++) {
        json = delayed[wards[i]][j];
        ob = HOSPITAL_D -> create_object(wards[i], json["ur"]["object"]);
        if(!ob) tmp_delayed += ({ json });
        else {
          if(configure_object(ob, json)) {
            HOSPITAL_D -> add_object(wards[i], ob);
          }
          else { destruct_object(ob); }
        }
      }
      if(sizeof(tmp_delayed)) {
        delayed[wards[i]] = tmp_delayed;
      }
      else {
        delayed[wards[i]] = nil;
      }
    }
  }

  /* now see if we have a "universe" room 
   * this room is the container into which the entire game fits
   */
  ward = HOSPITAL_D -> get_objects("rooms");
  if(!ward) error("No 'rooms' ward from which to get the universe");
  if(!ward["universe"]) error("No 'universe' room with which to build the universe. Available rooms: " + implode(map_indices(ward), ", "));
  HOSPITAL_D -> set_world_object(ward["universe"][0]);
}
