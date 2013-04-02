# include <type.h>
# include <toollib.h>
# include <toollib/template.h>

inherit parser LIB_PARSER;

static void create(varargs int clone) {
  set_bnf_file(TEMPLATE_BNF);
}

mixed parse_template(string str) {
  mixed *result;

  return parser::parse(str);

  if(typeof(result) == T_ARRAY) return result[0];
  return nil;
}

mixed _combine_words(mixed *args) {
  return ({ implode(args, "") });
}

mixed _pass_second(mixed *args) {
  return ({ args[1] });
}

mixed _varname(mixed *args) {
  return ({ ({ "var", args[1] }) });
}

mixed _comment(mixed *args) {
  return ({ });
}

mixed _simple_substitution(mixed *args) {
  string name;
  int pos;

  name = "last_substitution";

  switch(args[0]) {
    case "this": case "actor": 
      name = args[0];
      pos = POS_NOMINATIVE;
      break;
    case "direct": case "indirect": case "instrument":
      name = args[0];
      pos = POS_OBJECTIVE;
      break;
    case "nominative":
      pos = POS_NOMINATIVE; break;
    case "objective":
      pos = POS_OBJECTIVE; break;
    case "possessive":
      pos = POS_POSSESSIVE; break;
    case "name":
      pos = POS_NAME; break;
    case "possessive-noun":
      pos = POS_POSSESSIVE_NOUN; break;
    case "reflexive":
      pos = POS_REFLEXIVE; break;
    case "verb":
      return ({ ({ "verb", name, nil }) });
    default:
      return ({ ({ "verb", name, args[0] }) });
  }
  return ({ ({ "substitution", name, pos }) });
}

mixed _targeted_substitution(mixed *args) {
  string name;
  int pos;

  name = "last_substitution";
  switch(args[0]) {
    case "this": case "actor": case "direct": case "indirect": case "instrument"
:
      name = args[0];
      pos = POS_NOMINATIVE;
      break;
    case "verb":
      return ({ ({ "verb", name, args[2] }) });
  }
  switch(args[2]) {
    case "nominative":
      pos = POS_NOMINATIVE; break;
    case "name":
      pos = POS_NAME; break;
    case "objective":
      pos = POS_OBJECTIVE; break;
    case "possessive":
      pos = POS_POSSESSIVE; break;
    case "possessive-noun":
      pos = POS_POSSESSIVE_NOUN; break;
    case "reflexive":
      pos = POS_REFLEXIVE; break;
    case "verb":
      return ({ ({ "verb", name, nil }) });
    default: 
      return ({ ({ "verb", name, args[2] }) });
  }
  return ({ ({ "substitution", name, pos }) });
}

mixed _choose_one_word(mixed *args) {
  return ({ ({ "chooseOne", args[1] }) });
}

mixed _choose_one_value(mixed *args) {
  return ({ ({ "chooseOneV", args[1] }) });
}

mixed _create_word_list(mixed *args) {
  return ({ ({ "wordList", args }) });
}

mixed _append_word_list(mixed *args) {
  return ({ args[0] + ({ args[2..] }) });
}

mixed _default_property(mixed *args) {
  return ({ ({ "topicalProperty", args }) });
}

mixed _exp_property(mixed *args) {
  return ({ ({ "targetedProperty", args[0], args[2] }) });
}

mixed _if(mixed *args) {
  return ({ ({ "if", args[2], args[4] }) });
}

mixed _unless(mixed *args) {
  return ({ ({ "unless", args[1], args[3] }) });
}

mixed _if_else(mixed *args) {
  return ({ ({ "if", args[2], args[4], args[8] }) });
}

mixed _exp(mixed *args) {
  return ({ ({ "exp", args }) });
}

mixed _noop(mixed *args) {
  return ({ });
}
