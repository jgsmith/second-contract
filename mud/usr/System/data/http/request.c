# include <system.h>

string *content;
string buffer;
mapping headers;
string request_method;
string request_uri;
string server_protocol;
string path_info;
string query_string;
string script_name;
int still_reading_headers;
mapping env;

void create(varargs int clone) {
  if(clone) {
    headers = ([ ]);
    env = ([ ]);
    buffer = "";
    still_reading_headers = TRUE;
  }
}

static string trim_front(string s) {
  while(strlen(s) && (s[0] == '\x0a' || strlen(s) > 1 && s[0..1] == "\x0d\0x0a")) {
    if(s[0] == '\x0d') s == s[2..];
    else s = s[1..];
  }
  return s;
}

int _parse_header(string chunk, int eoh) {
  string header;
  string *tmp, *out;
  string request;
  string k;
  int i, n, j, m, minor, ch;

  header = chunk[0..eoh-1];
  chunk = trim_front(chunk[eoh..]);

  tmp = explode(header, "\n");

  if(sizeof(tmp)) {
    request = tmp[0];
    tmp = tmp[1..];
  }
  else return -2;

  out = ({ });
  for(i = 0, n = sizeof(tmp); i < n; i++) {
    if(tmp[i][0] == ' ' || tmp[i][0] == '\t') {
      if(sizeof(out) == 0) out += ({ "" });
      out[sizeof(out)-1] += tmp[i];
    }
    else {
      out += ({ tmp[i] });
    }
  }

  tmp = explode(request, " ") - ({ "" });
  if(sizeof(tmp) != 3) return -3;
  if(tmp[2][0..6] != "HTTP/1.") return -4;
  if(!sscanf(tmp[2][7..], "%d", minor)) return -5;

  server_protocol = "HTTP/1." + minor;
  request_method = tmp[0];
  request_uri = tmp[1];

  i = 0, n = strlen(request_uri);
  while(i < n && request_uri[i] != '?' && request_uri[i] != '#') i++;

  path_info = request_uri[0..i-1];
  if(i > strlen(request_uri)) query_string = request_uri[i+1..];
  else query_string = "";
  tmp = explode(path_info, "%");
  if(path_info[0] == '%') tmp = ({ "" }) + tmp;
  for(i = 1, n = sizeof(tmp); i < n; i++) {
    if(strlen(tmp[i]) < 2) return -6;
    switch(tmp[i][0]) {
      case '0'..'9': ch = tmp[i][0]-'0'; break;
      case 'a'..'f': ch = tmp[i][0]-'a'+10; break;
      case 'A'..'F': ch = tmp[i][0]-'A'+10; break;
      default : return -7;
    }
    switch(tmp[i][1]) {
      case '0'..'9': ch = tmp[i][1]-'0'; break;
      case 'a'..'f': ch = tmp[i][1]-'a'+10; break;
      case 'A'..'F': ch = tmp[i][1]-'A'+10; break;
      default : return -8;
    }
    tmp[i] = tmp[i][1..]; tmp[i][0] = ch;
  }
  path_info = implode(tmp, "");

  for(i = 0, n = sizeof(out); i < n; i++) {
    for(j = 0, m = strlen(out[i]); j < m && out[i][j] != ':'; j++) {
      ch = out[i][j];
      if(ch >= 127) return -9;
      switch(ch) {
        case 0 .. 31:
        case '[': case ']': case '(': case ')': case '<' : case '>':
        case '@': case ',': case ';': case '"': case '\\': case '/':
        case '?': case '=': case '{': case '}':
          return -10;
      }
    }
    if(j == strlen(out[i])) return -11;
    k = STRING_D -> lower_case(out[i][0..j-1]);
    if(out[i][j+1] == ' ') j++;
    if(headers[k]) headers[k] += out[i][j+1..];
    else headers[k] = ({ out[i][j+1..] });
  }
  return eoh;
}

string get_buffer() { return buffer; }

int parse_http_request(string chunk) {
  int i, n, r;
  string *lines;

  /* this is due to chunk coming through twice at the beginning */
  if(chunk != buffer) buffer = buffer + chunk;

  if(still_reading_headers) {
    if(sizeof(explode(buffer+" ", "\n\n")) > 1) {
      still_reading_headers = FALSE;
    }
    else {
      return -2;
    }
  } 
  chunk = buffer;
  if(!strlen(chunk)) return -20;
  for(i = 0, n = strlen(chunk); i < n; i++) {
    if(i+1 < n && chunk[i..i+1] == "\x0a\x0a"
        || i+2 < n && chunk[i..i+2] == "\x0a\x0d\x0a" 
        || i+2 < n && chunk[i..i+2] == "\x0d\x0a\x0a"
        || i+3 < n && chunk[i..i+3] == "\x0d\x0a\x0d\x0a" 
    ) {
      r = _parse_header(chunk, i);
      if(r < 0) {
        buffer = chunk;
        return -100 + r;
      }
      buffer = r < strlen(chunk) ? chunk[r..] : "";
      return r;
    }
  }
  buffer = chunk;
  return -21;
}

mixed get_env(varargs string n) {
  if(n) return env[n];
  else return env;
}

string get_method() { return request_method; }

string get_uri() { return request_uri; }

string *get_header(string nom) {
  nom = STRING_D -> lower_case(nom);
  if(headers[nom]) return headers[nom];
  return ({ });
}

mapping get_headers() { return headers; }

int get_content_length() {
  int l;
  if(headers["content-length"] && sizeof(headers["content-length"]) == 1) {
    if(sscanf(headers["content-length"][0], "%d", l)) {
      return l;
    }
  }
  return -1;
}
