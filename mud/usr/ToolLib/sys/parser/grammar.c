# include <toollib.h>
# include <iflib.h>
# include <type.h>
# include <worldlib/proximity.h>

inherit parser LIB_PARSER;

void create(varargs int clone) {
  augment_bnf("\nv:'take'\nv:'get'\nv:'run'\n" 
    + "\nadv:'quickly'\nadv:'softly'\nadv:'quietly'\n");
  set_bnf_file(GRAMMAR_BNF);
}

void update_words() {
  string *verbs;
  string *adverbs;

  verbs = VERB_D -> get_verbs();
  adverbs = ADVERB_D -> get_adverbs();
  augment_bnf(
    (verbs ? "\nv:'" + implode(verbs, "'\nv:'") + "'\n" : "")+
    (adverbs ? "\nadv:'" + implode(adverbs, "'\nadv:'") + "'\n" : "")
  );
}

mixed *parse_command(string cmd) {
  return parser::parse(cmd);
}

mixed _indirect_preposition(mixed *args) {
  if(args[0] == "from" || args[0] == "to") return ({ ({ "indirect", args[0] }) });
  return 0;
}

mixed _instrument_preposition(mixed *args) {
  if(args[0] == "with" || args[0] == "using") return ({ ({ "tool", args[0] }) });
  return 0;
}

mixed _verb(mixed *args) {
  return ({ ({ "verb", args[0] }) });
}

mixed _adverb(mixed *args) {
  return ({ ({ "adverb", args[0] }) });
}

mixed _remove_and(mixed *args) {
  return args - ({ "and" });
}

/* Is the preposition one of those used in relations?
 */
mixed _rel_preposition(mixed *args) {
  if(WORDS_D -> query_in_list("prepositions", args[0]))
    return ({ ({ "relation", args[0] }) });
  return 0;
}

mixed _direct_noun_phrase(mixed *args) {
  if(args[0] == "all") return ({ ({ "direct", "all" }) });
  return ({ ({ "direct" }) + args[0] });
}

mixed _instrument_noun_phrase(mixed *args) {
  return ({ ({ "instrument" }) + args }); 
}

mixed _indirect_noun_phrase(mixed *args) {
  return ({ ({ "indirect" }) + args });
}

mixed _command(mixed *args) {
  return ({ ({ "command", args }) });
}

mixed _commands(mixed *args) {
  return args[0..sizeof(args)-3] + args[sizeof(args)-1..];
}

mixed _init_object_set(mixed *args) {
  return ({ ({ args }) });
}

mixed _object_set(mixed *args) {
  return ({ args[0] + ({ args[1..] }) });
}

mixed _noop(mixed *args) {
  return ({ });
}

mixed _noun_path(mixed *args) {
  int prep;
  switch(args[1]) {
    case "in": prep = PROX_INSIDE; break;
    case "inside": prep = PROX_INSIDE; break;
    case "on": prep = PROX_ON; break;
    case "against": prep = PROX_AGAINST; break;
    case "close": prep = PROX_CLOSE; break;
    case "close to": prep = PROX_CLOSE; break;
    case "under": prep = PROX_UNDER; break;
    case "near": prep = PROX_NEAR; break;
    case "over": prep = PROX_OVER; break;
    case "behind": prep = PROX_BEHIND; break;
    case "before": prep = PROX_BEFORE; break;
    case "in front of": prep = PROX_BEFORE; break;
    case "beside": prep = PROX_BESIDE; break;
    case "containing": prep = PROX_CONTAINS; break;
    case "holding": prep = PROX_CONTAINS; break;
    case "with": prep = PROX_CONTAINS; break;
    case "guarding": prep = PROX_GUARD; break;
    case "blocking": prep = PROX_BLOCK; break;
    default : prep = PROX_INSIDE; break;
  }
  return ({ args[0] + ({ prep, args[2..] }) });
}

mixed _noun_phrase(mixed *args) {
  return ({ ({ args }) });
}

mixed _rel_prep(mixed *args) {
  return ({ implode(args, " ") });
}
