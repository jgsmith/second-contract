# include <common.h>

inherit command LIB_COMMAND;

int cmd(object user, string str) {
  if (str && str != "") {
       user -> message("Usage: swapout\n");
       return FALSE;
   }

   swapout();
   user -> message("Swapout initiated.\n");
   return TRUE;
}