/*
 * This handles the coordination of URI paths to resource data objects.
 */

# include <kernel/kernel.h>
# include <kernel/user.h>
# include <config.h>
# include <system.h>
# include <system/http.h>

mapping resources;
string *paths;

static void create(varargs int clone) {
  if(!find_object(HTTP_USER)) {
    compile_object(HTTP_USER);
  }
  if(!find_object(HTTP_REQUEST)) {
    compile_object(HTTP_REQUEST);
  }
  resources = ([ ]);
  paths = ({ });
}

object select(string str) { 
  object ob;
  ob = clone_object(HTTP_USER); 
  ob -> set_mode(MODE_RAW);
  ob -> receive_message(str);
  return ob;
}

int query_timeout(object connection) { 
  connection->set_mode(MODE_RAW);
  return DEFAULT_TIMEOUT; 
}

string query_banner(object connection) { 
  connection->set_mode(MODE_RAW);
  return nil; 
}

int add_resource_handler(string uri_prefix, string data_file) {
  int i, n;

  if(!SYSTEM()) {
    ERROR_D -> message("Only System may install new URI resource handlers.\n");
    return FALSE;
  }
  resources[uri_prefix] = data_file;
  paths -= ({ uri_prefix });
  for(i = 0, n = sizeof(paths); i < n; i++) {
    if(paths[i] < uri_prefix) {
      if(i > 0) {
        paths = paths[0..i-1] + ({ uri_prefix }) + paths[i..];
        return TRUE;
      }
      else {
        paths = ({ uri_prefix }) + paths;
        return TRUE;
      }
    }
  }
  paths = paths + ({ uri_prefix });
  return TRUE;
}

object create_resource_handler(object request) {
  object ob;
  string uri;
  int i, n;

  uri = request -> get_uri();
  for(i = 0, n = sizeof(paths); i < n; i++) {
    if(uri[0..strlen(paths[i])-1] == paths[i]) {
      ob = new_object(resources[paths[i]]);
      ob -> set_request(request);
      return ob;
    }
  }
  return nil;
}
