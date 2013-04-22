# include <kernel/kernel.h>
# include <system.h>
# include <worldlib.h>
# include <worldlib/proximity.h>

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
 * ward of the primary hospital (HOSPITAL_D). We try to come up with a
 * good, two- or three-tiered system of selection.
 */
mapping get_templates(varargs int tiers) {
  if(!tiers) tiers = 2;
  /* if the nouns change, then we use that as the last criteria and call it
   * "race".
   */
  /* the parts that are constant across all archetypes are placed in the
   * "constant-adj" or "constant-race" part
   */
  /* eventually, we should cache this and only recalculate when we add a
   * new character archetype. But we also don't expect thousands of archetypes.
   */

}

atomic object create_character(string name, string cap_name, string template) {
  object ob;
  object area;

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

  ob = area -> create_placed_object("character-archetypes", template, area -> get_scene("start"), PROX_INSIDE);
  if(!ob) return nil; /* oops! couldn't create the character */

  ob -> set_name(name);
  ob -> set_cap_name(cap_name);
  ob -> set_property(({ "basic", "position" }), "floating");
  ob -> set_property(({ "basic", "gait" }), "walking");

  MAPPING_D -> specific_mapping(characters, name)[name] = ob;
  
  return ob;
}
