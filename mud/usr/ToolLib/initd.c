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
    JSON_P, VERB_SYNTAX_P, EVENT_SCRIPT_P,
    BASE64_D, ENGLISH_D, TEMPLATE_P, GRAMMAR_P,
/*
    DECLARATION_DATA, EVENT_HANDLER_DATA, STATEMENTS_DATA,
    CONSTANT_DATA, CONSTANT_STATEMENT_DATA, IF_ELSE_DATA, DO_WHILE_DATA,
    FOR_DATA, CATCH_DATA, SWITCH_DATA, CASE_DATA, RETURN_DATA, MESSAGE_DATA,
    CONTEXT_DATA, 
 */
  });

  for(i = 0, n = sizeof(files); i < n; i++)
    if(!find_object(files[i])) compile_object(files[i]);
}
