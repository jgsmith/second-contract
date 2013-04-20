/*
 * Handles static content in /www/
 */

# include <system.h>
# include <system/http.h>
# include <status.h>
# include <type.h>
# include <kernel/kernel.h>

inherit resource HTTP_RESOURCE_LIB;

string filename;

void create(varargs int clone) {
  resource::create(clone);

  set_authentication_flags(HTTP_ALLOWS_PUBLIC_READ);

  if(clone) {
  }
}

int resource_exists() {
  string path;

  path = get_parameter("directory") + "/" + get_resource_id();

  filename = find_object(DRIVER)->normalize_path("/www/" + path, "/www/");

  /* does file exist under /www/ ? */
  if(filename[0..4] != "/www/") return FALSE;
  if(sizeof(get_dir(filename)[0]) != 1) return FALSE;
  return TRUE;
}

int is_authorized(string auth) { return TRUE; }

string *allowed_methods() {
  return ({ "OPTIONS", "GET", "HEAD" });
}

int forbidden() { return FALSE; }

mixed *content_types_provided() {
  return ({ 
    ({ "text/javascript", "get_content" }),
    ({ "application/javascript", "get_content" }),
    ({ "text/ecmascript", "get_content" }),
    ({ "application/ecmascript", "get_content" }),
    ({ "text/css", "get_content" }),
    ({ "text/html", "get_content" }),
  });
}

mixed *content_types_accepted() { return ({ }); }

mixed get_content(mapping metadata) {
  string *content;
  int size;
  int i, n, strsize;
  string *bits;
  string ext, type;

  size = get_dir(filename)[1][0];
  strsize = status(ST_STRSIZE);
  if(size < strsize) { /* we can read it all at once */
    content = ({ read_file(filename) });
  }
  else { /* we just have to make sure we don't have any files greater than
          * STRSIZE * ARRAYSZ
          */
    n = size/strsize+1;
    content = allocate(n);
    for(i = 0, n = size/strsize+1; i < n; i++)
      content[i] = read_file(filename, strsize*i, strsize);
  }
  bits = explode(filename, "/");
  ext = bits[sizeof(bits)-1];
  bits = explode(ext, ".");
  ext = bits[sizeof(bits)-1];
  switch(ext) {
    case "html": type = "text/html"; break;
    case "css":  type = "text/css"; break;
    case "min.css":  type = "text/css"; break;
    case "js":   type = "application/javascript"; break;
    default: type = "text/plain"; break;
  }
  get_response() -> add_header("Content-Type", type);
  return content;
}
