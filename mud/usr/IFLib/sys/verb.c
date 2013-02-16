# include <iflib.h>
# include <iflib/verbd.h>
# include <toollib.h>
# include <type.h>

mapping verbs;
mapping syntaxes;
object syntax_parser;

static void create() {
  verbs = ([ ]);

  syntaxes = ([
    "": ({ ({ 100 }), "" }),
  ]);

  syntax_parser = find_object(VERB_SYNTAX_D);
}

void add_verb(object VERB_DATA v) {
  string *names;
  int i, n;

  names = v -> get_verbs();
  for(i = 0, n = sizeof(names); i < n; i++) {
    if(verbs[names[i]]) verbs[names[i]] |= ({ v });
    else verbs[names[i]] = ({ v });
  }

  names = v -> get_syntaxes();
  for(i = 0, n = sizeof(names); i < n; i++)
    if(!syntaxes[names[i]]) 
      syntaxes[names[i]] = syntax_parser -> compile_syntax(names[i]);
}

void remove_verb(object VERB_DATA v) {
  string *names;
  int i, n;


  names = v -> get_verbs();
  for(i = 0, n = sizeof(names); i < n; i++) {
    if(verbs[names[i]]) {
      verbs[names[i]] -= ({ v });
      if(sizeof(verbs[names[i]]) == 0) verbs[names[i]] = nil;
    }
  }
}

/*
  when parsing, we select the first word as a verb. If it's not,
  we test if it's an adverb and move on to the second word.
 */
object *get_verb_handlers(string verb) {
  string *candidates;
  object *handlers;
  int i, n;
  if(verbs[verb]) return verbs[verb];

  /* we might want to find a key that matches verb if verb is an
     abbreviation */
  candidates = map_indices(verbs);
  for(i = 0, n = sizeof(candidates); i < n; i++) 
    if(candidates[i][0..strlen(verb)] != verb) candidates[i] = nil;
  handlers = ({ });
  // TODO: sort array of candidates so the short matches are first
  for(i = 0, n = sizeof(candidates); i < n; i++) {
    if(candidates[i]) handlers += ({ verbs[candidates[i]] });
  }
  return handlers;
}
