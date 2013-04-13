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
