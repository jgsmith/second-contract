# include <toollib.h>
# include <toollib/verb_syntax.h>
# include <type.h>

string syntax_bnf;

mixed *compile_syntax(string syntax);

void update_syntax_bnf() {
  int i, sz;
  string *lines;
  string grammar;

  grammar = read_file(VERB_SYNTAX_BNF);
  lines = explode(grammar, "\n");
  for(i = 0, sz = sizeof(lines); i < sz; i++) {
    if(lines[i] && strlen(lines[i]) && lines[i][0] == '#') {
      lines[i] = nil;
    }
  }
  lines -= ({ nil });
  syntax_bnf = implode(lines, "\n");
}

static void create() {
  update_syntax_bnf();
}

mixed *compile_syntax(string syntax) {
  mixed *pat;
  int weight;
  int i, n;

  pat = parse_string(syntax_bnf, syntax);
  /* calculate weight here based on pat */
  if(!pat) return nil;

  weight = 0;
  for(i = 0, n = sizeof(pat); i < n; i++) {
    switch(typeof(pat[i])) {
      case T_ARRAY:
        switch(pat[i][0]) {
          case DIRECT_OBJECT: weight += 7; break;
          case INDIRECT_OBJECT: weight += 7; break;
          case SHORT_STRING: weight += 6; break;
          case STRING: weight += 5; break;
          case QUOTED_STRING: weight += 8; break;
          case NUMBER: weight += 10;
          case SINGLE_WORD: weight += 7;
          case FRACTION: weight += 10;
          case WORD_LIST: weight += 10;
          case OPTIONAL: weight += 4;
        }
        break;
    }
  }
  return ({ weight }) + pat;
}

mixed _direct(varargs string *bits) {
  return ({ ({ DIRECT_OBJECT, ANY_OBJECT, ENV_NEAR|ENV_ME }) });
}

mixed _direct_obj(string *bits) {
  int type_n;
  mixed ret;

  ret = _direct();
  if(!ret) return 0;

  switch(bits[2]) {
    case "detail": type_n = ANY_DETAIL; break;
    case "details": type_n = ANY_DETAIL|PLURAL; break;
    case "object": type_n = ANY_OBJECT; break;
    case "objects": type_n = ANY_OBJECT|PLURAL; break;
    case "living": type_n = ANY_LIVING; break;
    case "livings": type_n = ANY_LIVING|PLURAL; break;
    case "player": type_n = TARGET_PLAYER; break;
    case "players": type_n = TARGET_PLAYER|PLURAL; break;
    default: return 0; /* fail */
  }
  ret[0][1] = type_n;
  return ret;
}

mixed _direct_obj_env(string *bits) {
  int env_n;
  mixed ret;
  string *stmp;

  ret = _direct_obj(bits);
  if(!ret) return 0;

  stmp = explode(bits[4], "-");
  if(sizeof(stmp) > 2) stmp[1] = implode(stmp[1..], "-");

  switch(stmp[0]) {
    case "in": env_n = ENV_IN; break;
    case "on": env_n = ENV_ON; break;
    case "close": env_n = ENV_CLOSE; break;
    case "near": env_n = ENV_NEAR; break;
    case "vicinity": env_n = ENV_VICINITY; break;
    case "far": env_n = ENV_FAR; break;
    case "backdrop": env_n = ENV_BACKDROP; break;
    default: return 0;
  }
  switch(stmp[1]) {
    case "me": env_n |= ENV_ME; break;
    case "scene": env_n |= ENV_SCENE; break;
    case "area": env_n |= ENV_AREA; break;
    case "region": env_n |= ENV_REGION; break;
    case "world": env_n |= ENV_WORLD; break;
    default: return 0;
  }
  ret[0][2] = env_n;
  return ret;
}

mixed _slot(mixed *bits) {
  return ({ bits[1] });
}

mixed _slot_qstring(mixed *bits) {
  return ({ bits[1] + ({ bits[2][1..strlen(bits[2])-2] }) });
}

mixed _indirect(varargs string *bits) {
  return ({ ({ INDIRECT_OBJECT, ANY_OBJECT, ENV_NEAR|ENV_ME }) });
}

mixed _indirect_obj(string *bits) {
  int type_n;
  mixed ret;

  ret = _indirect();
  if(!ret) return 0;

  switch(bits[2]) {
    case "detail": type_n = ANY_DETAIL; break;
    case "details": type_n = ANY_DETAIL|PLURAL; break;
    case "object": type_n = ANY_OBJECT; break;
    case "objects": type_n = ANY_OBJECT|PLURAL; break;
    case "living": type_n = ANY_LIVING; break;
    case "livings": type_n = ANY_LIVING|PLURAL; break;
    case "player": type_n = TARGET_PLAYER; break;
    case "players": type_n = TARGET_PLAYER|PLURAL; break;
    default: return 0;
  }
  ret[0][1] = type_n;
  return ret;
}

mixed _indirect_obj_env(string *bits) {
  int env_n;
  mixed ret;
  string *stmp;

  ret = _indirect_obj(bits);
  if(!ret) return 0;

  stmp = explode(bits[4], "-");
  if(sizeof(stmp) > 2) stmp[1] = implode(stmp[1..], "-");

  switch(stmp[0]) {
    case "in": env_n = ENV_IN; break;
    case "on": env_n = ENV_ON; break;
    case "close": env_n = ENV_CLOSE; break;
    case "near": env_n = ENV_NEAR; break;
    case "vicinity": env_n = ENV_VICINITY; break;
    case "far": env_n = ENV_FAR; break;
    case "backdrop": env_n = ENV_BACKDROP; break;
    default: return 0;
  }
  switch(stmp[1]) {
    case "me": env_n |= ENV_ME; break;
    case "scene": env_n |= ENV_SCENE; break;
    case "area": env_n |= ENV_AREA; break;
    case "region": env_n |= ENV_REGION; break;
    case "world": env_n |= ENV_WORLD; break;
    case "direct-obs": env_n |= ENV_DIRECT_OBS; break;
    default: return 0;
  }

  ret[0][2] = env_n;
  return ret;
}

mixed _concat(mixed *args) {
  return ({ implode(args, "") });
}
