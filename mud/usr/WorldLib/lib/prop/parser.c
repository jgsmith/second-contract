# include <iflib/binder.h>

/*
 * This provides support for the parsing/binding system
 */

int is_matching_object(string *input, object actor, object BINDER_MATCH_CONTEXT context) {
  string last_bit;
  string *names;
  string *pnames;
  string *adj;
  string *padj;
  object env;
  int ret;
  int i, n;

  last_bit = input[sizeof(input)-1];
  switch(last_bit) {
    case "him":
      if(this_object() == context -> get_him())
        ret |= BINDER_MATCH_SINGULAR;
      break;
    case "her":
      if(this_object() == context -> get_her())
        ret |= BINDER_MATCH_SINGULAR;
      break;
    case "it":
      if(this_object() == context -> get_it())
        ret |= BINDER_MATCH_SINGULAR;
      break;
    case "them":
      if(context -> is_plural() && sizeof(({ this_object() }) & context -> get_plural_objects()))
        ret |= BINDER_MATCH_PLURAL;
      break;
    case "me":
      if(this_object() == actor)
        ret |= BINDER_MATCH_SINGULAR;
      break;
    case "all": case "things": case "ones":
      if(!this_object() -> get_property(({ "parser", "no-match-all" })))
        ret |= BINDER_MATCH_PLURAL;
      break;
    case "thing": case "one":
      if(!this_object() -> get_property(({ "parser", "no-match-all" })))
        ret |= BINDER_MATCH_SINGULAR;
      break;
  }

  if(!ret) {
    env = this_object() -> environment();
    if(last_bit == "here" && 
       actor != env &&
       sizeof(input) > 1) {
      input = input[0..sizeof(input)-2];
      last_bit = input[sizeof(input)-1];
    }
    names = this_object() -> parse_command_id_list();
    pnames = this_object() -> parse_command_plural_id_list();

    if(sizeof(({ last_bit }) & names)) {
      ret |= BINDER_MATCH_SINGULAR;
    }
    if(sizeof(({ last_bit }) & pnames)) {
      ret |= BINDER_MATCH_PLURAL;
    }
    if(!ret) return 0;
  }

  /* now match adjectives */

  adj = this_object() -> parse_command_adjective_id_list();
  padj = this_object() -> parse_command_plural_adjective_id_list();

  if(env == context->get_him()) adj += ({ "his" });
  else if(env == context->get_her()) adj += ({ "her" });
  else if(env == context->get_it()) adj += ({ "its" });
  else if(env == actor) adj += ({ "my" });
  else if(context->is_plural() && sizeof(({ env }) & context->get_plural_objects()))
    adj += ({ "their" });

  for(i = 0, n = sizeof(input)-1; i < n; i++) {
    if(!sizeof(input[i..i] & adj)) {
      if(!padj || !sizeof(input[i..i] & padj)) return 0;
      ret &= ~BINDER_MATCH_SINGULAR;
      ret |= BINDER_MATCH_PLURAL;
    }
  }

  return ret;
}

mixed *parse_match_object( string *input, object actor, object BINDER_MATCH_CONTEXT context ) {
  string bit;
  int ret;

  if(!(ret = is_matching_object(input, actor, context))) return ({ BINDER_NO_MATCH, ({ }) });

  if(!this_object() -> is_continuous() && !this_object() -> is_money()) {
    if(!context -> update_number(1, ret)) return nil;

    return ({ ret, ({ this_object() }) });
  }

  /* here is where we need to hook in alternate matching code for money,
   * liquids, and continuous media
   */
  return ({ BINDER_NO_MATCH, ({ }) });
}
