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
mapping status_messages;

static void create(varargs int clone) {
  if(!find_object(HTTP_USER)) {
    compile_object(HTTP_USER);
  }
  if(!find_object(HTTP_REQUEST)) {
    compile_object(HTTP_REQUEST);
  }
  if(!find_object(HTTP_RESPONSE)) {
    compile_object(HTTP_RESPONSE);
  }
  /* if(!find_object(HTTP_RESOURCE_LIB)) {
    compile_object(HTTP_RESOURCE_LIB);
  } */
  if(!find_object(HTTP_FSM_D)) {
    compile_object(HTTP_FSM_D);
  }
  resources = ([ ]);
  paths = ({ });
  status_messages = ([
    100: "Continue",
    101: "Switching Protocols",

    200: "OK",
    201: "Created",
    202: "Accepted",
    203: "Non-Authoritative Information",
    204: "No Content",
    205: "Reset Content",
    206: "Partial Content",

    300: "Multiple Choices",
    301: "Moved Permanently",
    302: "Found",
    303: "See Other",
    304: "Not Modified",
    305: "Use Proxy",
    306: "(Unused)",
    307: "Temporary Redirect",

    400: "Bad Request",
    401: "Unauthorized",
    402: "Payment Required",
    403: "Forbidden",
    404: "Not Found",
    405: "Method Not Allowed",
    406: "Not Acceptable",
    407: "Proxy Authentication Required",
    408: "Request Timeout",
    409: "Conflict",
    410: "Gone",
    411: "Length Required",
    412: "Precondition Failed",
    413: "Request Entity Too Long",
    414: "Request-URI Too Long",
    415: "Unsupported Media Type",
    416: "Requested Range Not Satisfiable",
    417: "Expectation Failed",

    500: "Internal Server Error",
    501: "Not Implemented",
    502: "Bad Gateway",
    503: "Service Unavailable",
    504: "Gateway Timeout",
    505: "HTTP Version Not Supported",
  ]);
}

string status_message(int status) {
  if(status_messages[status]) return status_messages[status];
  return "Unknown";
}

object select(string str) { 
  object ob;
  ob = clone_object(HTTP_USER); 
  previous_object() -> set_mode(MODE_RAW);
  ob -> set_mode(MODE_RAW);
  /* ob -> receive_message(str); */
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
  string initd;
  object po;

  po = previous_object();
  if(!po) {
    error("Unable to determine the previous object.");
    return FALSE;
  }
  
  /* We want to make sure the previous object is one of the /usr/ * /initd.c
   * objects and that they are only installing resource handlers for things
   * under /usr/ * /data/resource/ *
   */
  if(!sscanf(object_name(po), "/usr/%s/initd", initd)) {
    error("Only user inits may install new URI resource handlers.\n");
    return FALSE;
  }
  data_file = find_object(DRIVER)->normalize_path(data_file, "/usr/" + initd + "/", initd);

  if(strlen(data_file) < strlen(initd)+19) {
    error("The datafile (" + data_file + ") is not a valid resource for " + initd + ".");
    return FALSE;
  }
  if(data_file[0..strlen(initd)+19] != "/usr/"+initd+"/data/resource/") {
    error("The datafile (" + data_file + ") is not a valid resource for " + initd + ".");
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
    if(strlen(uri) >= strlen(paths[i])) {
      if(uri[0..strlen(paths[i])-1] == paths[i]) {
        ob = new_object(resources[paths[i]]);
        ob -> set_request(request);
        ob -> set_base(paths[i]);
        return ob;
      }
    }
  }
  return nil;
}
