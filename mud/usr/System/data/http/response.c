# include <system.h>
# include <type.h>

/*
 * data associated with an HTTP response
 */

int status;
string *body;
mapping headers;

void create(varargs int clone) {
  if(clone) {
    headers = ([ ]);
    body = nil;
  }
}

void set_status(int s) { status = s; }

int has_body() { return body ? TRUE : FALSE; }

void set_body(mixed s) { 
  if(typeof(s) == T_STRING) body = ({ s }); 
  else body = s;
}

void add_to_body(string s) {
  if(!body) body = ({ });
  body += ({ s });
}

void output(object target) {
  string *keys;
  int i, n;
  int length;

  if(status) {
    target -> message("HTTP/1.0 " + status + " " +  HTTP_D -> status_message(status) + "\n");

    headers["Access-Control-Allow-Origin"] = "*";
    if(body != nil) {
      if(!headers["content-length"]) {
        length = 0;
        for(i = 0, n = sizeof(body); i < n; i++) length += strlen(body[i]);
        headers["content-length"] = length;
      }
      if(!headers["content-type"]) headers["content-type"] = "application/json";
    }

    keys = map_indices(headers);
    for(i = 0, n = sizeof(keys); i < n; i++) {
      target -> message(keys[i] + ": " + headers[keys[i]] + "\n");
    }
    target -> message("\n");
    if(body) {
      for(i = 0, n = sizeof(body); i < n; i++)
        target -> message(body[i]);
    }
  }
}
