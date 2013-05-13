# include <iflib.h>
# include <data.h>
# include <toollib.h>
# include <common.h>
# include <worldlib.h>
# include <system.h>
# include <gamelib.h>

inherit command LIB_COMMAND;

static string *foodDegree;
static string *drunkDegree;
static string *drinkDegree;

static void create(varargs int clone) {
  command::create(clone);
  
  /* TODO: make these configurable via the REST api */
  foodDegree = ({ "starving!", "very hungry.", "hungry.",
    "partially hungry.", "feeling full.", "quite full." });
  drunkDegree = ({ "sober", "tipsy", "drunk", "blitzed",
    "three sheets to the wind", "FUBAR" });
  drinkDegree = ({ "parched", "extremely thirsty", "very thirsty",
    "thirsty", "somewhat thirsty", "unthirsty" });
}

int cmd(object user, string args) {
  string str, tmp;
  object who, ob;
  
  if(!user) return FALSE;
  
  who = user -> get_character();
  if(!who) return FALSE;

  str = "You are " + who -> get_cap_name() + " (<morality>).";
  if(who -> get_property(({ "counter", "level" }))) {
    str += "\nYou are a level " + who -> get_property(({ "counter", "level" }));
    if(who -> get_property(({ "trait", "dead" }))) {
      str += " undead";
    }
    else if(ob = who -> get_template_object()) {
      tmp = ob -> get_property(({ "detail", "default", "description", "brief" }));
      if(tmp) str += " " + tmp;
      else str += " something";
    }
    tmp = ob -> get_property(({ "trait", "class" }));
    if(tmp) str += " " + tmp;
    else str += " citizen";
  }
  str += "\n";
  user->message(str);
  return TRUE;
}