/*
 * Reboots the mud. Don't do this unless you must.
 */

# include <common.h>

inherit command LIB_COMMAND;

int cmd(object user, string arg) {
  object *users;
  string char_name;
  int i, n;
  
  if(!arg || arg == "") arg = "System is rebooting now!";
  
  char_name = user -> get_character() -> get_cap_name();
  users = users() - ({ user });
  arg = "\n***\n*** REBOOT MESSAGE from " + char_name + ": " + arg + "\n***\n";
  for(i = 0, n = sizeof(users); i < n; i++) {
    users[i] -> message(arg);
  }
  user -> message("Shutdown has been broadcast. Continuing with reboot.\n");
  ::dump_state();
  ::shutdown();
  return TRUE;
}