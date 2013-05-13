/*
 * Sends a message to all logged-in users.
 */

# include <common.h>

inherit command LIB_COMMAND;

int cmd(object user, string arg) {
  object *users;
  string char_name;
  int i, n;
  
  if(!arg || arg == "") return FALSE;
  
  char_name = user -> get_character() -> get_cap_name();
  users = users() - ({ user });
  arg = "\n***\n*** BROADCAST MESSAGE from " + char_name + ": " + arg + "\n***\n";
  for(i = 0, n = sizeof(users); i < n; i++) {
    users[i] -> message(arg);
  }
  user -> message("Your message has been broadcast.\n");
  return TRUE;
}