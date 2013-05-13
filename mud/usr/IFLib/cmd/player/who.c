# include <iflib.h>
# include <data.h>
# include <toollib.h>
# include <common.h>
# include <worldlib.h>
# include <system.h>
# include <gamelib.h>

inherit command LIB_COMMAND;

static void create(varargs int clone) {
  command::create(clone);
}

int cmd(object user, string args) {
  object ob;
  object *users;
  object PRIORITY_QUEUE arches;
  object PRIORITY_QUEUE cres;
  object PRIORITY_QUEUE players;
  object PRIORITY_QUEUE_ITEM player;
  string owner;
  string ret;
  int i, n;
  
  if(args && strlen(args) > 0 && args[0] == '@') {
    /* It's a remote who request */
    ob = find_object("/usr/System/cmd/player/rwho");
    if(ob) return ob -> cmd(args[1..]);
    return FALSE;
  }
  
  /* we build a priority queue of each set of players */
  arches = new_object(PRIORITY_QUEUE);
  cres = new_object(PRIORITY_QUEUE);
  players = new_object(PRIORITY_QUEUE);
  
  users = CHARACTER_D -> active_characters();
  for(i = 0, n = sizeof(users); i < n; i++) {
    player = new_object(PRIORITY_QUEUE_ITEM);
    player -> set_data(users[i]);
    if(users[i]->get_property(({ "counter", "level" })))
      player -> set_priority(-users[i]->get_property(({ "counter", "level" })));
    else
      player -> set_priority(0);
    owner = AUTH_D -> get_character_owner(player -> get_name());
    if(AUTH_D -> get_is_in_group("ADMIN", owner)) {
      arches -> insert(player);
    }
    else if(AUTH_D -> get_is_in_group("CREATOR", owner)) {
      cres -> insert(player);
    }
    else {
      players -> insert(player);
    }
  }
  
  ret =  GAME_D -> get_name() + "\n";
  ret += "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n";
  while(player = arches -> remove()) {
    ob = player -> get_data();
    ret += ob -> get_cap_name();
    if(ob -> get_title()) {
      ret += ": " + ob -> get_title();
    }
    if(ob -> get_property(({ "counter", "level" })))
      ret += " Level " + ob -> get_property(({ "counter", "level" }));
    ret += " (admin)\n";
  }
  while(player = cres -> remove()) {
    ob = player -> get_data();
    ret += ob -> get_cap_name();
    if(ob -> get_title()) {
      ret += ": " + ob -> get_title();
    }
    if(ob -> get_property(({ "counter", "level" })))
      ret += " Level " + ob -> get_property(({ "counter", "level" }));
    ret += " (creator)\n";
  }
  while(player = players -> remove()) {
    ob = player -> get_data();
    ret += ob -> get_cap_name();
    if(ob -> get_title()) {
      ret += ": " + ob -> get_title();
    }
    if(ob -> get_property(({ "counter", "level" })))
      ret += " Level " + ob -> get_property(({ "counter", "level" }));
    ret += "\n";
  }
  ret += "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*\n";
  ret += "There " + ( n == 1 ? "is" : "are" ) + " " + ENGLISH_D -> cardinal(n) + " member";
  ret += (n == 1 ? "" : "s") + " of our reality\n";
  user -> message(ret);
  return TRUE;
}