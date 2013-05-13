/*
 * Gets the COMMAND_D to rehash a set of directories.
 */

# include <common.h>
# include <system.h>

inherit command LIB_COMMAND;

int cmd(object user, string path) {
  if(!user) return FALSE;
  
  if(!path || path == "") {
    COMMAND_D -> hash_commands();
    user -> message("Rehashing all command directories.\n");
    return TRUE;
  }
  if(path == "admin" || path == "creator" || path == "builder" || path == "player") {
    COMMAND_D -> hash_commands(path);
    user -> message("Rehashing all " + path + " command directories.\n");
    return TRUE;
  }
  user -> message("Only admin, creator, builder, and player paths are available.\n");
  return TRUE;
} 