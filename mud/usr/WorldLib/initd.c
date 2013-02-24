# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <config.h>
# include <system.h>
# include <worldlib.h>
# include <toollib.h>

static int create(varargs int clone) {

  if(!find_object(PLACEMENT_D)) compile_object(PLACEMENT_D);
  if(!find_object(EXITS_D))     compile_object(EXITS_D);
  if(!find_object(HOSPITAL_D))  compile_object(HOSPITAL_D);
  if(!find_object(CHARACTER_D))  compile_object(CHARACTER_D);

  if(!find_object(THING_OBJ))   compile_object(THING_OBJ);
  if(!find_object(WARD_OBJ))    compile_object(WARD_OBJ);

  if(!find_object(DETAIL_DATA)) compile_object(DETAIL_DATA);
  if(!find_object(EXIT_DATA))   compile_object(EXIT_DATA);
}

private int configure_object(object ob, mixed json) {
  int i, n;
  string *props;
  if(json["properties"]) {
    /* read through each property and call set_property... */
    props = map_indices(json["properties"]);
    for(i = 0, n = sizeof(props); i < n; i++) {
      ob -> set_property(explode(props[i], ":"), json["properties"][props[i]]);
    }
  }
  return TRUE;
}

void initialize_data() {
  object ob;
  string *fnames, *wards;
  mixed *dir;
  mixed json;
  mapping delayed; /* keyed off of "ward" */
  mixed *tmp_delayed;
  int i, n, j, m;
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
          if(ob) {
            if(configure_object(ob, json)) {
              HOSPITAL_D -> add_object(wards[i], ob);
            }
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
}
