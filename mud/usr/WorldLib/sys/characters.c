# include <kernel/kernel.h>
# include <system.h>
# include <worldlib.h>
# include <worldlib/proximity.h>
# include <data.h>

/*
 * tracks user accounts/passwords
 */

mapping characters;

object *active_characters;

void create() {
  characters = ([ ]);
  active_characters = ({ });
}

int character_exists(string name) {
  name = STRING_D -> lower_case(name);
  if(MAPPING_D -> specific_mapping(characters, name)[name]) return 1;
  return 0;
}

/*
 * since this is a persitent game, we don't have to worry about loading
 * player files to find the character information. The object is always
 * available.
 */
object find_character(string name) {
  name = STRING_D -> lower_case(name);
  return MAPPING_D -> specific_mapping(characters, name)[name];
}

object *active_characters() {
  /* we want to return all active characters - characters currently logged in */
  return active_characters + ({});
}

int enter_game(object char) {
  if(SYSTEM()) {
    active_characters |= ({ char });
    char -> call_event_handler("post-scan:brief", ([ "this": char, "actor": char ]));
    return TRUE;
  }
  return FALSE;
}

/*
 * Here is where we can move them to the meat locker if we have one.
 */
int leave_game(object char) {
  if(SYSTEM()) {
    active_characters -= ({ char });
    return TRUE;
  }
  return FALSE;
}

/* returns a list of pieces that can be assembled into a template name.
 * These are based on the ur-objects in the "character-archetypes"
 * ward of the Default:Default domain/area.
 */
string *get_character_templates() {
  object area;

  area = DOMAINS_D -> get_domain("Default");
  if(!area) return nil;
  area = area -> get_area("Default");
  if(!area) return nil;
  return area -> get_objects_in_ward("character-archetypes");
}

atomic object create_character(string name, string cap_name, string template) {
  object ob;
  object area, scene;
  object loc;

  if(!SYSTEM()) {
    /* specifically, it's the login/character creation process managed by
     * System - but it could be through the web interface as well.
     */
    error("Only System may create new characters for players.\n");
  }

  name = STRING_D -> lower_case(name);
  if(character_exists(name)) return nil;

  /* Character types are in Default:Default:character-archetypes */
  area = DOMAINS_D -> get_domain("Default");
  if(!area) return nil;
  area = area -> get_area("Default");
  if(!area) return nil;
  scene = area -> get_scene("start");
  if(!scene) return nil;

  if(!area -> query_object_exists("character-archetypes", template)) return nil;

  ob = clone_object(THING_OBJ);
  ob -> set_template_path("Default:Default:character-archetypes:"+template);
  loc = new_object(LOCATION_DATA);
  loc -> set_object(scene);
  if(ob -> move_to(PROX_INSIDE, loc)) {
    ob -> set_name(name);
    ob -> set_cap_name(cap_name);
    ob -> set_property(({ "basic", "position" }), "floating");
    ob -> set_property(({ "basic", "gait" }), "walking");

    MAPPING_D -> specific_mapping(characters, name)[name] = ob;
  
    return ob;
  }
  else {
    destruct_object(ob);
    return nil;
  }
}
