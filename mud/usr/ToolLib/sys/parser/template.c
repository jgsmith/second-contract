# include <type.h>
# include <toollib.h>
# include <toollib/template.h>

string syntax_bnf;

void update_syntax_bnf() {
  int i, sz;
  string *lines;
  string grammar;

  grammar = read_file(TEMPLATE_BNF);
  lines = explode(grammar, "\n");
  for(i = 0, sz = sizeof(lines); i < sz; i++) {
    if(lines[i] && strlen(lines[i]) && lines[i][0] == '#') {
      lines[i] = nil;
    }
  }
  lines -= ({ nil });
  syntax_bnf = implode(lines, "\n");
  syntax_bnf = implode(explode(syntax_bnf, "\\n"), "\n");
  syntax_bnf = implode(explode(syntax_bnf, "\\b"), "\b");
  syntax_bnf = implode(explode(syntax_bnf, "\\t"), "\t");
  syntax_bnf = implode(explode(syntax_bnf, "\\r"), "\r");
}

static void create(varargs int clone) {
  update_syntax_bnf();
}

mixed parse_template(string str) {
  mixed *result;
  result = parse_string(syntax_bnf, str);

  if(typeof(result) == T_ARRAY) return result[0];
  return nil;
}

mixed _create_template_obj(mixed *args) {
  object ob;

  ob = new_object(TEMPLATE_DATA);
  ob -> add_bit(args[0]);
  return ({ ob });
}

mixed _add_bit(mixed *args) {
  if(sizeof(args) > 1) {
    args[0] -> add_bit(args[1]);
  }
  return args[0..0];
}

mixed _pass_second(mixed *args) {
  return ({ args[1] });
}

mixed _varname(mixed *args) {
  object ob;

  ob = new_object(TMPL_VAR_DATA);
  ob -> set_variable(args[1]);
  return ({ ob });
}

mixed _comment(mixed *args) {
  return ({ });
}

mixed _simple_substitution(mixed *args) {
  object ob;

  ob = new_object(TMPL_POS_DATA);
  ob -> set_name("last_var");

  switch(args[0]) {
    case "this": case "actor": 
      ob -> set_name(args[0]);
      ob -> set_pos(POS_NOMINATIVE);
      break;
    case "direct": case "indirect": case "instrument":
      ob -> set_name(args[0]);
      ob -> set_pos(POS_OBJECTIVE);
      break;
    case "nominative": ob -> set_pos(POS_NOMINATIVE); break;
    case "objective": ob -> set_pos(POS_OBJECTIVE); break;
    case "possessive": ob -> set_pos(POS_POSSESSIVE); break;
    case "name": ob -> set_pos(POS_NAME); break;
    case "possessive-noun": ob -> set_pos(POS_POSSESSIVE_NOUN); break;
    case "reflexive": ob -> set_pos(POS_REFLEXIVE); break;
    default:
      ob -> set_verb(args[0]);
      ob -> set_pos(POS_VERB);
      break;
  }
  return ({ ob });
}

mixed _targeted_substitution(mixed *args) {
  object ob;
  ob = new_object(TMPL_POS_DATA);

  ob -> set_name("last_var");
  switch(args[0]) {
    case "this": case "actor": case "direct": case "indirect": case "instrument"
:
      ob -> set_name(args[0]);
      ob -> set_pos(POS_NOMINATIVE);
      break;
    case "verb":
      ob -> set_pos(POS_VERB);
      ob -> set_verb(args[2]);
      return ({ ob });
  }
  switch(args[2]) {
    case "nominative": ob -> set_pos(POS_NOMINATIVE); break;
    case "name": ob -> set_pos(POS_NAME); break;
    case "objective": ob -> set_pos(POS_OBJECTIVE); break;
    case "possessive": ob -> set_pos(POS_POSSESSIVE); break;
    case "possessive-noun": ob -> set_pos(POS_POSSESSIVE_NOUN); break;
    case "reflexive": ob -> set_pos(POS_REFLEXIVE); break;
    default: 
      ob -> set_verb(args[2]); ob -> set_pos(POS_VERB);
      break;
  }
  return ({ ob });
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
  return ({ ({ "if", args[1], args[3] }) });
}

mixed _unless(mixed *args) {
  return ({ ({ "unless", args[1], args[3] }) });
}

mixed _if_else(mixed *args) {
  return ({ ({ "if", args[1], args[3], args[7] }) });
}

mixed _exp(mixed *args) {
  return ({ ({ "exp", args }) });
}
