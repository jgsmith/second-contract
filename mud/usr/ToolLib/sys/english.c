# include <toollib.h>
# include <system.h>
# include <type.h>

/*
 * This manages producing various English things, like plurals and lists
 *
 * Based in part on code from the Dead Souls public domain distribution.
 */

mapping abnormal_plurals;

void create() {
  abnormal_plurals = ([
    "moose": "moose",
    "mouse": "mice",
    "die": "dice",
    "index": "indices",
    "human":"humans",
    "sheep":"sheep",
    "fish":"fish",
    "child":"children",
    "ox":"oxen",
    "tooth":"teeth",
    "deer":"deer",
    "sphinx":"sphinges"
  ]);
}

void add_plural(string x, string xes) {
  abnormal_plurals[x] = xes;
}

mapping get_plurals() { return abnormal_plurals; }

void remove_plural(string x) {
  abnormal_plurals[x] = nil;
}

string pluralize(string str) {
  int len;

  if(!str) return str;

  if(abnormal_plurals[str]) return abnormal_plurals[str];

  len = strlen(str);

  if(len > 1) {
    switch(str[len-2..len-1]) {
      case "ch": case "sh":
        return str + "es";
      case "ff": case "fe":
        return str[0..len-3] + "ves";
      case "us":
        return str[0..len-3] + "i";
      case "um":
        return str[0..len-3] + "a";
      case "ef":
        return str + "s";
    }
  }

  switch(str[len-1..len-1]) {
    case "o": case "x": case "s":
      return str + "es";
    case "f":
      return str[0..len-2] + "ves";
    case "y":
      if(sizeof(({ str[len-2..len-2] }) & ({ "a", "e", "i", "o", "u" })))
        return str + "s";
      else
        return str[0..len-2] + "ies";
  }

  return str + "s";
}

string strip_article(string val) {
  int x;

  x = strlen(val);
  if(x <= 2) return val;
  if(val[0..1] == "a " || val[0..1] == "A ") return val[2..];
  if(x <= 3) return val;
  if(val[0..2] == "an " || val[0..2] == "An ") return val[3..];
  if(x <= 4) return val;
  if(val[0..3] == "the " || val[0..3] == "The ") return val[4..];
  return val;
}

string cardinal(int x) {
  string tmp;
  int a;

  if(!x) return "zero";
  if(x < 0) {
    tmp = "negative ";
    x = -x;
  }
  else tmp = "";
  switch(x) {
    case 1: return tmp+"one";
    case 2: return tmp+"two";
    case 3: return tmp+"three";
    case 4: return tmp+"four";
    case 5: return tmp+"five";
    case 6: return tmp+"six";
    case 7: return tmp+"seven";
    case 8: return tmp+"eight";
    case 9: return tmp+"nine";
    case 10: return tmp+"ten";
    case 11: return tmp+"eleven";
    case 12: return tmp+"twelve";
    case 13: return tmp+"thirteen";
    case 14: return tmp+"fourteen";
    case 15: return tmp+"fifteen";
    case 16: return tmp+"sixteen";
    case 17: return tmp+"seventeen";
    case 18: return tmp+"eighteen";
    case 19: return tmp+"nineteen";
    case 20: return tmp+"twenty";
    default:
      if(x > 1000000000) return "over a billion";
      else if(a = x /1000000) {
        if(x = x %1000000) 
          return cardinal(a) + " million " + cardinal(x);
        else return cardinal(a) + " million";
      }
      else if(a = x / 1000) {
        if(x = x % 1000) 
          return cardinal(a) + " thousand " + cardinal(x);
        else return cardinal(a) + "thousand";
      }
      else if(a = x / 100) {
        if(x = x % 100) 
          return cardinal(a) + " hundred " + cardinal(x);
        else cardinal(a) + " hundred";
      }
      else {
        a = x / 10;
        if(x = x % 10) tmp = "-"+cardinal(x);
        else tmp = "";
        switch(a) {
          case 2: return "twenty"+tmp;
          case 3: return "thirty"+tmp;
          case 4: return "forty"+tmp;
          case 5: return "fifty"+tmp;
          case 6: return "sixty"+tmp;
          case 7: return "seventy"+tmp;
          case 8: return "eighty"+tmp;
          case 9: return "ninety"+tmp;
          default: return "error";
        }
      }
  }
}

string consolidate(int count, string str) {
  string *words;
  string tmp;

  if(count == 1 || !strlen(str)) return str;

  words = explode(str, " ");

  if(sscanf(words[sizeof(words)-1], "(%s)", tmp)) {
    if(sizeof(words) == 1) return "(" + consolidate(count, tmp) + ")";
    else return consolidate(count, implode(words[0..sizeof(words)-2], " ")) + " (" + tmp + ")";
  }
  if(sscanf(words[sizeof(words)-1], "[%s]", tmp)) {
    if(sizeof(words) == 1) return "[" + consolidate(count, tmp) + "]";
    else return consolidate(count, implode(words[0..sizeof(words)-2], " ")) + " [" + tmp + "]";
  }
  if(sizeof(({ STRING_D -> lower_case(words[0]) }) & ({ "a", "an", "the", "one" }))) words = words[1..];
  return cardinal(count) + " " + pluralize(implode(words, " "));
}

string item_list(mixed items) {
  mapping list;
  string str;
  int maxi, i, n;
  mixed value;

  if(typeof(items) == T_NIL) return "nothing";

  if(typeof(items) != T_ARRAY) items = ({ items });

  if(!sizeof(items)) return "nothing";

  if(typeof(items[0]) == T_ARRAY) {
    if(!sizeof(items[0])) return "nothing";
    items = items[0];
  }
  list = ([ ]);
  for(i = 0, n = sizeof(items); i < n; i++) {
    value = items[i];
    if(typeof(value) == T_OBJECT) {
      if(value -> is_living()) {
        value = value -> get_name();
      }
      else {
        value = value -> get_short();
      }
    }
    if(!value) continue;
    if(!list[value]) list[value] = 1;
    else list[value] ++;
  }

  maxi = sizeof(items = map_indices(list));
  if(maxi < 1) return "";

  str = consolidate(list[items[0]], items[0]);
  if( maxi == 1 ) return str;

  if(maxi > 2) str += ",";
  for(i = 1; i < maxi; i++) {
    if( i == maxi-1 )
      str += " and ";
    else
      str += " ";
    str += consolidate(list[items[i]], items[i]);
    if( i < maxi-1 ) str += ",";
  }
  return str;
}

string nominative(mixed item) {
  switch(typeof(item)) {
    case T_NIL: return "";
    case T_ARRAY:
      if(sizeof(item) == 1) return nominative(item[0]);
      if(sizeof(item) == 0) return "";
      return "they";
    case T_STRING: return item;
    case T_OBJECT:
      switch(item->get_property(({ "body", "gender" }))) {
        case "male": return "he";
        case "female": return "she";
        case "neutral": return "hi";
        default: return "it";
      }
    default : return "";
  }
}

string objective(mixed item) {
  switch(typeof(item)) {
    case T_NIL: return "";
    case T_ARRAY:
      if(sizeof(item) == 1) return objective(item[0]);
      if(sizeof(item) == 0) return "";
      return "them";
    case T_STRING: return item;
    case T_OBJECT:
      switch(item->get_property(({ "body", "gender" }))) {
        case "male": return "him";
        case "female": return "her";
        case "neutral": return "hir";
        default: return "it";
      }
    default : return "";
  }
}

string possessive(mixed item) {
  switch(typeof(item)) {
    case T_NIL: return "";
    case T_ARRAY:
      if(sizeof(item) == 1) return possessive(item[0]);
      if(sizeof(item) == 0) return "";
      return "their";
    case T_STRING: return item;
    case T_OBJECT:
      switch(item->get_property(({ "body", "gender" }))) {
        case "male": return "his";
        case "female": return "her";
        case "neutral": return "hir";
        default: return "its";
      }
    default : return "";
  }
}

string possessive_noun(mixed item) {
  switch(typeof(item)) {
    case T_NIL: return "its";
    case T_ARRAY:
      if(sizeof(item) == 1) return possessive_noun(item[0]);
      if(sizeof(item) == 0) return "";
      return "their";
    case T_OBJECT:
      item = item -> get_name();
      if(!item) return "its";
    case T_STRING:
      switch(item[strlen(item)-1]) {
        case 'x': case 'z': case 's': return item + "'";
        default: return item + "'s";
      }
  }
  return "";
}

/* used for sentences such as "it is hers" */
string possessive_adjective(mixed item) {
  switch(typeof(item)) {
    case T_NIL: return "";
    case T_ARRAY:
      if(sizeof(item) == 1) return possessive_adjective(item[0]);
      if(sizeof(item) == 0) return "";
      return "theirs";
    case T_STRING: return item;
    case T_OBJECT:
      switch(item->get_property(({ "body", "gender" }))) {
        case "male": return "his";
        case "female": return "hers";
        case "neutral": return "hirs";
        default: return "its";
      }
    default : return "";
  }
}

string reflexive(mixed item) {
  switch(typeof(item)) {
    case T_NIL: return "";
    case T_ARRAY:
      if(sizeof(item) == 1) return reflexive(item[0]);
      if(sizeof(item) == 0) return "";
      return "themselves";
    case T_STRING: return item;
    case T_OBJECT:
      switch(item->get_property(({ "body", "gender" }))) {
        case "male": return "hisself";
        case "female": return "herself";
        case "neutral": return "hirself";
        default: return "itself";
      }
    default : return "";
  }
}
