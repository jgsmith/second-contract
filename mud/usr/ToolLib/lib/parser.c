#include <kernel/kernel.h>

string file_name;
string syntax_file;
string syntax_aug;
string syntax_bnf;

atomic mixed parse(string src) {
  return parse_string(syntax_bnf, src);
}

static string read_grammar(string file_name) {
  string grammar;
  string *lines;
  int i, sz;

  file_name = find_object(DRIVER)->normalize_path(file_name, "/usr/ToolLib/text/", "ToolLib");
  grammar = read_file(file_name);
  lines = explode(grammar, "\n");
  for(i = 0, sz = sizeof(lines); i < sz; i++) {
    if(lines[i] && strlen(lines[i])) {
      if(lines[i][0] == '#') lines[i] = nil;
      else if(lines[i][0..8] == "@include ") {
        lines[i] = read_grammar(lines[i][9..]);
      }
      else {
        lines[i] = implode(explode(lines[i], "\\n"), "\n");
        lines[i] = implode(explode(lines[i], "\\b"), "\b");
        lines[i] = implode(explode(lines[i], "\\t"), "\t");
        lines[i] = implode(explode(lines[i], "\\r"), "\r");
      }
    }
  }
  lines -= ({ nil });
  return implode(lines, "\n");
}

atomic void update_syntax_bnf() {
  int i, sz;
  string *lines;
  string grammar;

  syntax_file = read_grammar(file_name);
  syntax_bnf = syntax_file;
  if(syntax_aug) syntax_bnf += "\n" + syntax_aug;
}

string get_bnf() { return syntax_bnf; }
string get_bnf_file() { return file_name; }

atomic void set_bnf_file(string fname) {
  file_name = fname;
  update_syntax_bnf();
}

atomic void augment_bnf(string aug) {
  syntax_aug = aug;
  if(syntax_file) syntax_bnf = syntax_file + "\n" + syntax_aug;
  else syntax_bnf = syntax_aug;
}
