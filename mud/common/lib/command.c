# include <system.h>

static void create(varargs int clone) {
  if(clone) {
    
  }
}

int _cmd(object user, string args) {
  string path;
  
  sscanf(object_name(this_object()), "/usr/%*s/cmd/%s/%*s", path);
  switch(path) {
    case "admin": 
      if(!AUTH_D -> is_in_group("ADMIN", user -> get_ident())) return FALSE;
      break;
    case "creator":
      if(!AUTH_D -> is_in_group("CREATOR", user -> get_ident()) &&
         !AUTH_D -> is_in_group("ADMIN",   user -> get_ident())) return FALSE;
      break;
    case "builder":
      if(!AUTH_D -> is_in_group("BUILDER", user -> get_ident()) &&
         !AUTH_D -> is_in_group("CREATOR", user -> get_ident()) &&
         !AUTH_D -> is_in_group("ADMIN",   user -> get_ident())) return FALSE;
      break;
  }
  return this_object() -> cmd(user, args);
}