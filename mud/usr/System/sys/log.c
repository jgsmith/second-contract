# include <kernel/kernel.h>
# include <trace.h>

/*
 * Log handler
 */

/* we store log data in the mapping and write it out on heart beats */
mapping logs;
mapping ensured_dirs;

int log_level;

void create(varargs int clone) {
  log_level = 0;
  logs = ([ ]);
  ensured_dirs = ([ "/": 1, "/log/": 1 ]);
  call_out("do_log", 1);
}

void set_level(int x) {
  if(x >= 0) log_level = x;
}

static ensure_dir(string dir) {
  string *bits;
  int i, n;
  if(ensured_dirs[dir]) return;
  dir = find_object(DRIVER) -> normalize_path(dir, "/log/");
  bits = explode(dir, "/") - ({ "" });
  if(bits[0] != "log") error("Illegal log location");
  for(i = 1, n = sizeof(bits); i < n; i++) {
    dir = "/" + implode(bits[0..i], "/") + "/";
    if(!ensured_dirs[dir]) {
      if(!sizeof(get_dir(dir)[0])) {
        if(!make_dir(dir)) error("Unable to create log directory \"" + dir + "\".");
        ensured_dirs[dir] = 1;
      }
    }
  }
}

void do_log() {
  int i, n;
  string *files;
  string *bits;

  call_out("do_log", 1);
  files = map_indices(logs);
  for(i = 0, n = sizeof(files); i < n; i++) {
    if(sizeof(logs[files[i]])) {
      bits = explode(files[i], "/");
      ensure_dir("/" + implode(bits[0..sizeof(bits)-2], "/"));
      write_file(files[i], implode(logs[files[i]], "\n")+"\n");
      logs[files[i]] = nil;
    }
  }
}


void log(string type, string content, varargs int level) {
  string basedir, user;
  string *bits;

  if(level > log_level) return;

  user = previous_object() ? previous_object() -> query_owner() : "Unknown";
  if(sscanf(type, "%*s:..%*s") != 0)
    error("Illegal log name");

  switch(user) {
    case "System":
    case "Kernel":
      basedir = "/log/";
      break;
    default:
      basedir = "/log/" + user + "/";
      break;
  }

  bits = explode(type, ":");
  if(sizeof(bits) > 2) error("Illegal error type \"" + type + "\".");
  if(sizeof(bits) == 2) {
    basedir = basedir + bits[0] + "/";
    bits = bits[1..1];
  }
  basedir = find_object(DRIVER) -> normalize_path(basedir + bits[0], "/log/");
  if(basedir[0..4] != "/log/") error("Illegal log location");
  if(!logs[basedir]) logs[basedir] = ({ });
  if(type == "http:access")
    logs[basedir] += ({ content });
  else
    logs[basedir] += ({ "--- " + ctime(time()), content });
}

static build_error(string type, string str, mixed **trace) {
  int i, sz;
  string progname, objname, line, function;
  int len;
  object obj;

  sz = sizeof(trace);
  str += "\n";
  for(i = 0; i < sz; i ++) {
    progname = trace[i][TRACE_PROGNAME];
    len = trace[i][TRACE_LINE];
    if(!len) {
      line = "    ";
    } else {
      line = "    " + len;
      line = line[strlen(line) - 4 ..];
    }
    function = trace[i][TRACE_FUNCTION];
    len = strlen(function);
    if(progname == AUTO && i != sz - 1 && len > 3) {
      switch(function[..2]) {
        case "bad": case "_F_": case "_Q_": continue;
      }
    }
    if(len < 17) {
      function += "                 "[len ..];
    }
    objname = trace[i][TRACE_OBJNAME];
    if (progname != objname) {
      len = strlen(progname);
      if (len < strlen(objname) && progname == objname[.. len - 1] &&
        objname[len] == '#') {
        objname = objname[len ..];
      }
      str += line + " " + function + " " + progname + " (" + objname + ")\n";
    } 
    else {
      str += line + " " + function + " " + progname + "\n";
    }
  }
  log(type, str, 0);
  if(this_user() && (obj = this_user() -> query_user())) {
    obj -> message("--- " + type + " ---\n" + str);
  }
  else if(this_user()) {
    this_user() -> message("--- " + type + " ---\n" + str);
  }
}

void runtime_error(string str, int caught, mixed **trace) {
  string type;

  if(caught) type = "caught";
  else type = "runtime";

  build_error(type, str, trace);
}
  
void atomic_error(string str, int atom, mixed **trace) {
  build_error("atomic", str, trace[atom..]);
}

void compile_error(string file, int line, string err) {
  object obj;

  log("compile", file += ", " + line + ": " + err + "\n", 0);
  if(this_user() && (obj = this_user() -> query_user())) {
    obj -> message(file);
  }
  else if(this_user()) {
    this_user() -> message(file);
  }
}
