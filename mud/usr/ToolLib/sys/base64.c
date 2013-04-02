# include <kernel/kernel.h>

string from_base64;

void create(varargs int clone) {
  from_base64 = "...........................................\x3e..." +
                "\x3f\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d...=..." +
                "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c" +
                "\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19" +
                "......" +
                "\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26" +
                "\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33" +
                "...................................................." +
                "...................................................." +
                ".............................";
}

string decode(string str) {
  string result, bits;
  int i, len, b1, b2, b3, b4;

  len = strlen(str);
  while(len % 4) {
    str += "=";
    len++;
  }
  result = "";
  bits = "...";
  for (i = 0, len = strlen(str); i < len; i += 4) {
    b1 = from_base64[str[i]];
    b2 = from_base64[str[i + 1]];
    b3 = from_base64[str[i + 2]];
    b4 = from_base64[str[i + 3]];
    bits[0] = (b1 << 2) | (b2 >> 4);
    bits[1] = (b2 << 4) | (b3 >> 2);
    bits[2] = (b3 << 6) | b4;
    result += bits;
  }

  if (b3 == '=') {
    return result[.. strlen(result) - 3];
  } else if (b4 == '=') {
    return result[.. strlen(result) - 2];
  }
  return result;
}
