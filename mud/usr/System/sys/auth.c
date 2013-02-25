# include <kernel/kernel.h>
# include <system.h>
# include <worldlib.h>

/*
 * tracks user accounts/passwords
 */

mapping accounts;
mapping groups;

void create() {
  accounts = ([ ]);
  groups = ([ ]);
  this_object() -> set_user_password("admin", "password");
  this_object() -> create_group("ADMIN");
  this_object() -> add_user_to_group("ADMIN", "admin");
}

int create_group(string grp) {
  if(!SYSTEM()) {
    ERROR_D -> message("Only System may create new groups.");
    return FALSE;
  }
  if(groups[grp]) return FALSE;
  groups[grp] = ({ });
}

int is_in_group(string group, string user) {
  if(!groups[group]) return FALSE;
  return sizeof(({ user}) & groups[group]);
}

int add_user_to_group(string group, string user) {
  if(!SYSTEM()) {
    ERROR_D -> message("Only System may add a user to a group.");
    return FALSE;
  }
  if(!groups[group]) return FALSE;
  groups[group] |= ({ user });
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

int owns_character(string email, string char_name) {
  mapping user_m;

  user_m = MAPPING_D -> specific_mapping(accounts, email);
  if(!user_m[email] || !user_m[email]["characters"]) return 0;
  return sizeof(({ char_name }) & user_m[email]["characters"]);
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
