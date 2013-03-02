# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <config.h>
# include <system.h>
# include <toollib.h>
# include <iflib.h>

static int create(varargs int clone) {
  if(!find_object(ENGLISH_D)) compile_object(ENGLISH_D);
  if(!find_object(VERB_DATA)) compile_object(VERB_DATA);
  if(!find_object(VERB_D)) compile_object(VERB_D);

  if(!find_object("/usr/IFLib/data/resource/verb"))
    compile_object("/usr/IFLib/data/resource/verb");
}

void initialize_data() {
  object ob;
  string *stmp;
  mixed *dir;
  mixed json;
  int i, n, j, m, tmp;
  /*
   * We want to initialize various things with data from the json/
   * directory.
   */
  /* /usr/WorldLib/json/verbs/$verb.json */

  HTTP_D -> add_resource_handler("/foo", "/usr/IFLib/data/resource/verb");

  dir = get_dir("/usr/IFLib/json/verbs/*");
  for(i = 0, n = sizeof(dir[0]); i < n; i++) {
    if(dir[1][i] > 0 && dir[0][i][0] != '.') {
      json = JSON_P -> from_json(read_file("/usr/IFLib/json/verbs/" + dir[0][i]));
      if(json) {
        ob = new_object(VERB_DATA);
        ob -> set_verbs(json["verbs"]);
        ob -> set_help(json["help"]);
        ob -> set_see_also(json["see_also"]);
        if(json["syntaxes"]) {
          stmp = map_indices(json["syntaxes"]);
          for(j = 0, m = sizeof(stmp); j < m; j++) {
            ob -> add_syntax(stmp[j], json["syntaxes"][stmp[j]]);
          }
        }
        /* still need actor requirements, climates, terrains, weathers,
         * and seasons */
        VERB_D -> add_verb(ob);
      }
    }
  }
}
      
