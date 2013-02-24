# include <kernel/kernel.h>
# include <system.h>
# include <worldlib.h>

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

object create_character(string name, string cap_name, string template) {
  object ob;

  if(!SYSTEM()) {
    /* specifically, it's the login/character creation process managed by
     * System - but it could be through the web interface as well.
     */
    ERROR_D -> message("Only System may create new characters for players.\n");
    return nil;
  }

  name = STRING_D -> lower_case(name);
  if(character_exists(name)) return nil;

  ob = HOSPITAL_D -> create_object("character-archetypes", template);
  if(!ob) return nil; /* oops! couldn't create the character */

  ob -> set_property(({ "basic", "name" }), name);
  ob -> set_property(({ "basic", "capitalized-name" }), cap_name);

  MAPPING_D -> specific_mapping(characters, name)[name] = ob;
  
  return ob;
}
