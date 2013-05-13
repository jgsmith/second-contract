# include <common.h>

inherit command LIB_COMMAND;

int cmd(object user, string str) {
  if (str && str != "") {
       user -> message("Usage: statedump\n");
       return FALSE;
   }

   dump_state();
   user -> message("Statedump initiated.\n");
   return TRUE;
}