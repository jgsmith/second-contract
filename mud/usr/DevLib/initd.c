# include <devlib.h>
# include <devlib/template.h>
# include <devlib/compiler.h>
# include <data.h>

static int create(varargs int clone) {
  string *files;
  int i, n;

  files = ({
    EVENT_DATA, EVENT_SET,
    TEMPLATE_DATA, TMPL_POS_DATA, TMPL_VERB_DATA,
    EVENTS_D, EVENT_SCRIPT_D, COMPILER_CONTEXT_DATA,
    MESSAGING_D,
  });

  for(i = 0, n = sizeof(files); i < n; i++)
    if(!find_object(files[i])) compile_object(files[i]);
}
