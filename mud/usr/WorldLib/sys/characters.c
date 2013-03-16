# include <kernel/kernel.h>
# include <system.h>
# include <worldlib.h>
# include <worldlib/proximity.h>

/*
 * tracks user accounts/passwords
 */

mapping characters;

void create() {
  characters = ([ ]);
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

  if(!SYSTEM()) {
    /* specifically, it's the login/character creation process managed by
     * System - but it could be through the web interface as well.
     */
    error("Only System may create new characters for players.\n");
  }

  name = STRING_D -> lower_case(name);
  if(character_exists(name)) return nil;

  ob = HOSPITAL_D -> create_placed_object("character-archetypes", template, HOSPITAL_D -> get_world_object(), PROX_INSIDE);
  if(!ob) return nil; /* oops! couldn't create the character */

  ob -> set_name(name);
  ob -> set_cap_name(cap_name);

  MAPPING_D -> specific_mapping(characters, name)[name] = ob;
  
  return ob;
}
