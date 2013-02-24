# include <system.h>
# include <worldlib.h>

/*
 * tracks user accounts/passwords
 */

mapping accounts;

void create() {
  accounts = ([ ]);
  this_object() -> set_user_password("admin", "password");
}

int user_exists(string name) {
  name = STRING_D -> lower_case(name);
  if(MAPPING_D -> specific_mapping(accounts, name)[name]) return 1;
  return 0;
}

int character_exists(string name) { 
  return CHARACTER_D -> character_exists(name);
}

int user_authenticated(string name, string password) {
  mapping m;

  name = STRING_D -> lower_case(name);
  m = MAPPING_D -> specific_mapping(accounts, name);
  if(!m[name] && m[name]["password"]) return 0;
  if(crypt(password, m[name]["password"]) == m[name]["password"]) return 1;
  return 0;
}

int set_user_password(string name, string password, varargs string old_password) {
  mapping m;

  name = STRING_D -> lower_case(name);
  m = MAPPING_D -> specific_mapping(accounts, name);
  if(m[name] && m[name]["password"]) {
    if(crypt(password,m[name]["password"]) != m[name]["password"]) return 0;
  }
  else {
    m[name] = ([ "password": crypt(password) ]);
    return 1;
  }
}

int add_character(string email, object char) {
  string name;
  mapping user_m;

  user_m = MAPPING_D -> specific_mapping(accounts, email);

  if(!user_m[email]) return 0;

  if(!user_m[email]["characters"]) user_m[email]["characters"] = ({ });
  user_m[email]["characters"] += char;
  return 1;
}
