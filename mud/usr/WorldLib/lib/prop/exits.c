mapping exits;

void create(varargs int clone) {
  exits = ([ ]);
}

mixed get_property(string *path) {
  object ur;
  string *results;
  int i, n;

  ur = this_object() -> get_ur_object();

  if(!sizeof(path)) {
    results = map_indices(exits);
    if(ur) results = ur -> get_property(({ "exit" }));
    for(i = 0, n = sizeof(results); i < n; i++)
      if(exits[results[i]] == "*deleted*") results[i] = nil;
    return results - ({ nil });
  }
  if(sizeof(path) == 1) {
    if(exits[path[0]]) return exits[path[0]] != "*deleted*";
    if(ur) return ur -> get_property(({ "exit", path[0] }));
    return FALSE;
  }

  if(!exits[path[0]]) {
    if(ur) return ur -> get_property(({ "exit" }) + path);
    return nil;
  }
  if(exits[path[0]] == "*deleted*") return nil;
  switch(path[1]) {
    case "destination": 
      if(sizeof(path) == 2) return exits[path[0]]["destination"];
      switch(path[2]) {
        case "scene": 
          if(sizeof(path) == 3) return exits[path[0]]["destination"];
          return nil;
        case "distance":
          return nil;
        case "relation":
          return nil;
      }
      return nil;
    case "source":
      if(sizeof(path) == 2) return this_object();
      switch(path[2]) {
        case "scene": 
          if(sizeof(path) == 3) return this_object();
          return nil;
        case "distance":
          return nil;
        case "relation":
          return nil;
      }
      return nil;
  }
  return nil;
}
