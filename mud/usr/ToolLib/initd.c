# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <config.h>
# include <system.h>
# include <toollib.h>
# include <toollib/event_script.h>
# include <toollib/template.h>

static int create(varargs int clone) {
  string *files;
  int i, n;

  files = ({ 
    LIB_PARSER,
    JSON_P, EVENT_SCRIPT_P,
    BASE64_D, ENGLISH_D, TEMPLATE_P, GRAMMAR_P, URI_PATH_P,
  });

  for(i = 0, n = sizeof(files); i < n; i++)
    if(!find_object(files[i])) compile_object(files[i]);
}
