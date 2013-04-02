# include <iflib.h>
# include <toollib.h>

void create(varargs int clone) {
  if(!find_object(COMMAND_DATA)) compile_object(COMMAND_DATA);
}

object *parse_command(string cmd, object actor) {
  mixed *cmds, *parts;
  int i, n, j, m;
  object *commands;
  object ob;

  cmds = GRAMMAR_P -> parse_command(cmd);
  /* if cmds is nil, we should try some alternative parsing methods */
  if(!cmds) return nil;

  actor -> message(JSON_P -> to_json(cmds)+"\n");

  commands = ({ });
  for(i = 0, n = sizeof(cmds); i < n; i++) {
    ob = new_object(COMMAND_DATA);
    for(j = 0, m = sizeof(cmds[i][1]); j < m; j++) {
/*
 * Types we need to support:
 *   verb
 *   adverb
 *   direct
 *   indirect
 *   instrument
 *   speech (string)
 *
 * Communication verbs:
 *   verb adverb indirect instrument speech
 *
 * indirect - object we're addressing
 * instrument - language we're using
 * speech - actual words
 *
 * but we also need the option of speaking through an object in a language
 *
 * By marking a verb as a communication verb, we allow the instrument to
 * be a language known by the actor.
 */
      switch(cmds[i][1][j][0]) {
        case "verb":   ob -> set_verb(cmds[i][1][j][1]); break;
        case "adverb": ob -> add_adverb(cmds[i][1][j][1]); break;
        case "direct": ob -> add_direct(cmds[i][1][j][1]); break;
        case "indirect": ob -> add_indirect(cmds[i][1][j][1], cmds[i][1][j][2]); break;
        case "instrument": ob -> add_instrument(cmds[i][1][j][1], cmds[i][1][j][2]); break;
        case "evocation": ob -> add_evocation(cmds[i][1][j][1]); break;
        case "topic": ob -> add_topic(cmds[i][1][j][1]); break;
      }
    }
    commands += ({ ob });
  }
  return commands;
}

int run_command(object actor, string cmd) {
  object *commands;
  int i, n;

  commands = parse_command(cmd, actor);
  if(!commands) return FALSE;

  /* now we can run through each of the commands and see if we can run them */
  for(i = 0, n = sizeof(commands); i < n; i++) {
    if(commands[i]->execute(actor)) {
      return TRUE;
    }
  }
  return TRUE;
}
