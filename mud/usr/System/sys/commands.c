/*
 * This daemon tracks all of the current command objects. Commands are run with their
 * command name preceeded by a percent sign. For example, /usr/IFLib/cmd/player/who.c
 * is run with "%who".
 */
# include <kernel/kernel.h>

mapping commands;

static void create(varargs int clone) {
  commands = ([
    "admin": ([ ]),
    "creator": ([ ]),
    "builder": ([ ]),
    "player": ([ ]),
  ]);
}

mapping get_command_mapping() { return commands; }

string *get_commands(string path) {
  string *cmds;
  
  cmds = ({ });
  switch(path) {
    case "admin": cmds += map_indices(commands["admin"]);
    case "creator": cmds += map_indices(commands["creator"]);
    case "builder": cmds += map_indices(commands["builder"]);
    default: cmds += map_indices(commands["player"]);
  }
  return cmds;
}

object find_command(string path, string name) {
  switch(path) {
    case "admin": if(commands["admin"][name]) return commands["admin"][name];
    case "creator": if(commands["creator"][name]) return commands["creator"][name];
    case "builder": if(commands["builder"][name]) return commands["builder"][name];
    default: if(commands["player"][name]) return commands["player"][name];
  }
  return nil;
}

static void do_command_hash(string path, string creator) {
  mixed *files;
  string dir_name;
  string cmd_file;
  string cmd_name;
  object ob;
  int i, n;
  
  dir_name = find_object(DRIVER) -> normalize_path(path, "/usr/" + creator + "/cmd", creator);
  files = get_dir(dir_name + "/*");
  for(i = 0, n = sizeof(files[0]); i < n; i++) {
    if(files[1][i] > 0 && files[0][i][0] != ".") {
      cmd_name = files[0][i];
      if(cmd_name[strlen(cmd_name)-2..] == ".c") {
        cmd_file = find_object(DRIVER) -> normalize_path(cmd_name, dir_name, creator);
        cmd_file = cmd_file[0..strlen(cmd_file)-3];
        cmd_name = cmd_name[0..strlen(cmd_name)-3];
        if(!commands[path][cmd_name]) {
          ob = find_object(cmd_file);
          if(!ob) {
            if(!catch(compile_object(cmd_file))) {
              ob = find_object(cmd_file);
            }
          }
          if(ob) commands[path][cmd_name] = ob;
        }
      }
    }
  }
}

void hash_commands(varargs string path) {
  if(!path) {
    call_out("hash_commands", 0, "admin");
    call_out("hash_commands", 1, "creator");
    call_out("hash_commands", 2, "builder");
    call_out("hash_commands", 3, "player");
  }
  else {
    /* We look in "/usr/IFLib/cmd/$path" and "/usr/System/cmd/$path" */
    /* We do /usr/System first since we won't do the usr/IFLib one if System has one */
    commands[path] = ([ ]);
    do_command_hash(path, "System");
    do_command_hash(path, "IFLib");
  }
}