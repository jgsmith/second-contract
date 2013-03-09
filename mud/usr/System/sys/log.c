# include <kernel/kernel.h>
# include <trace.h>

/*
 * Log handler
 */

int log_level;

void create(varargs int clone) {
  log_level = 0;
}

void set_level(int x) {
  if(x >= 0) log_level = x;
}

static do_log(string file, string content) {
  if(content[strlen(content)-1] != '\n')
    content += "\n";
  write_file(file, content);
}

static string ensure_dir(string dir) {
  dir = find_object(DRIVER) -> normalize_path(dir, "/log/");
  if(dir[0..4] != "/log/") error("Illegal log location");
  if(sizeof(get_dir(dir)[0])) return dir;
  if(!make_dir(dir)) error("Unable to create log directory \"" + dir + "\".");
  return dir;
}

void log(string type, string content, varargs int level) {
  string basedir, user;
  string *bits;

  if(level > log_level) return;

  user = previous_object() -> query_owner();
  if(sscanf(type, "%*s:..%*s") != 0)
    error("Illegal log name");

  switch(user) {
    case "system":
    case "kernel":
      basedir = "/log/";
      break;
    default:
      basedir = "/log/" + user + "/";
      break;
  }

  basedir = ensure_dir(basedir);
  bits = explode(type, ":");
  if(sizeof(bits) > 2) error("Illegal error type \"" + type + "\".");
  if(sizeof(bits) == 2) {
    basedir = ensure_dir(basedir + bits[0] + "/");
    bits = bits[1..1];
  }
  basedir = find_object(DRIVER) -> normalize_path(basedir + bits[0], "/log/");
  if(basedir[0..4] != "/log/") error("Illegal log location");
  call_out("do_log", 0, basedir, content);
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
  send_message(file);
  if(this_user() && (obj = this_user() -> query_user())) {
    obj -> message(file);
  }
}
