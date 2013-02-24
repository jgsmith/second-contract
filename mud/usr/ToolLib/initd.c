# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <config.h>
# include <system.h>
# include <toollib.h>
# include <toollib/event_script.h>

static int create(varargs int clone) {
  string *files;
  int i, n;

  files = ({ DECLARATION_DATA, EVENT_HANDLER_DATA, STATEMENTS_DATA,
    CONSTANT_DATA, CONSTANT_STATEMENT_DATA, IF_ELSE_DATA, DO_WHILE_DATA,
    FOR_DATA, CATCH_DATA, SWITCH_DATA, CASE_DATA, RETURN_DATA, MESSAGE_DATA,
    CONTEXT_DATA, });

  if(!find_object(JSON_P)) compile_object(JSON_P);
  if(!find_object(VERB_SYNTAX_P)) compile_object(VERB_SYNTAX_P);
  if(!find_object(EVENT_SCRIPT_P)) compile_object(EVENT_SCRIPT_P);

  for(i = 0, n = sizeof(files); i < n; i++)
    if(!find_object(files[i])) compile_object(files[i]);
}
