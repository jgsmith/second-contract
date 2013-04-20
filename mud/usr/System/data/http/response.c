# include <system.h>
# include <toollib.h>
# include <type.h>

/*
 * data associated with an HTTP response
 */

int status;
string *body;
mapping headers;
int spool_line;

void create(varargs int clone) {
  if(clone) {
    headers = ([ ]);
    body = nil;
    spool_line = 0;
  }
}

void set_status(int s) { status = s; }
int get_status() { return status; }

int get_content_length() {
  if(headers["Content-Length"]) {
    return headers["Content-Length"][0];
  }
  return 0;
}

int has_body() { return body ? TRUE : FALSE; }

void set_body(mixed s) { 
  if(typeof(s) == T_STRING) body = ({ s }); 
  else body = s;
}

void add_to_body(string s) {
  if(!body) body = ({ });
  body += ({ s });
}

void remove_header(string name) {
  headers[name] = nil;
  headers[STRING_D -> lower_case(name)] = nil;
}

void add_header(string name, string value) {
  if(!headers[name]) headers[name] = ({ value });
  else headers[name] |= ({ value });
}

void add_headers(mapping h) {
  string *hs;
  int i, n;

  hs = map_indices(h);
  for(i = 0, n = sizeof(hs); i < n; i++) {
    if(typeof(h[hs[i]]) == T_ARRAY) {
      if(!headers[hs[i]]) headers[hs[i]] = h[hs[i]];
      else headers[hs[i]] |= h[hs[i]];
    }
    else {
      if(!headers[hs[i]]) headers[hs[i]] = ({ h[hs[i]] });
      else headers[hs[i]] |= ({ h[hs[i]] });
    }
  }
}

void output(object target) {
  string *keys;
  int i, n, j, m;
  mixed length;

  if(!status) status = 500;
  target -> message("HTTP/1.0 " + status + " " +  HTTP_D -> status_message(status) + "\n");

  headers["Access-Control-Allow-Origin"] = ({ "*" });
  headers["Keep-Alive"] = ({ "timeout=0; max=0" });
  if(body != nil) {
    if(!headers["Content-Length"]) {
      length = 0;
      for(i = 0, n = sizeof(body); i < n; i++) length += strlen(body[i]);
      headers["Content-Length"] = ({ length });
    }
    if(!headers["Content-Type"]) headers["Content-Type"] = ({ "application/json" });
  }

  keys = map_indices(headers);
  for(i = 0, n = sizeof(keys); i < n; i++) {
    for(j = 0, m = sizeof(headers[keys[i]]); j < m; j++)
      target -> message(keys[i] + ": " + headers[keys[i]][j] + "\n");
  }
  target -> message("\n");
  if(body) {
    if(sizeof(body) > 0) {
      target -> message(body[0]);
      spool_line = 1;
    }
  }
}

void spool_body(object target) {
  /* on to the next line */
  if(spool_line < sizeof(body)) target -> message(body[spool_line]);
  spool_line ++;
}

int spool_finished() { return !body || spool_line >= sizeof(body); }
