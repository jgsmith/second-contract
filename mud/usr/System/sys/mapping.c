/*
 * Various map-related functions that we use in multiple places.
 */

/*
 * Given a root mapping and a key, this will create a two-level mapping
 * with up to 1024 keys at each level.
 */
mapping specific_mapping(mapping root, string key, varargs int levels, int bits) {
  string hash;
  string *idx;

  if(!levels) levels = 2;
  if(!bits) bits = 10;
  if(bits > 16) bits = 16;
  if(levels > 15) levels = 15;

  hash = hash_string("MD5", key);
  idx = ({ });
  bits = (1<<bits)-1;
  while(levels) {
    idx += ({ "" + ((hash[0] << 8 + hash[1]) & bits) });
    hash = hash[2..];
    levels--;
  }
  while(sizeof(idx) > 0) {
    if(!root[idx[0]]) root[idx[0]] = ([ ]);
    root = root[idx[0]];
    if(sizeof(idx) > 1) idx = idx[1..];
    else idx = ({ });
  }
  return root;
}
