# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <config.h>
# include <system.h>
# include <toollib.h>
# include <iflib.h>

static int create(varargs int clone) {
  string *files;
  int i, n;

  files = ({
    WORDS_D, VERB_D, ADVERB_D, BINDER_D, IFPARSER_D,
    VERB_DATA, ADVERB_DATA, COMMAND_DATA,
    HTTP_VERB_RESOURCE, HTTP_ADVERB_RESOURCE,
  });

  for(i = 0, n = sizeof(files); i < n; i++)
    if(!find_object(files[i])) compile_object(files[i]);
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

  HTTP_D -> add_resource_handler("/api/iflib/verb", HTTP_VERB_RESOURCE);
  HTTP_D -> add_resource_handler("/api/iflib/verb/:id", HTTP_VERB_RESOURCE);

  dir = get_dir("/usr/IFLib/json/verbs/*");
  for(i = 0, n = sizeof(dir[0]); i < n; i++) {
    if(dir[1][i] > 0 && dir[0][i][0] != '.') {
      json = JSON_P -> from_json(read_file("/usr/IFLib/json/verbs/" + dir[0][i]));
      if(FALSE && json) {
        ob = new_object(VERB_DATA);
        ob -> set_verbs(json["verbs"]);
        ob -> set_brief(json["brief"]);
        ob -> set_help(json["help"]);
        ob -> set_see_alsos(json["see_also"]);
        ob -> set_action(json["action"]);
        
        /* still need actor requirements, climates, terrains, weathers,
         * and seasons */
        VERB_D -> add_verb(ob);
      }
    }
  }
}
