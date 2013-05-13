# include <common.h>
# include <status.h>
# include <kernel/kernel.h>
# include <kernel/access.h>
# include <kernel/rsrc.h>
# include <kernel/user.h>
# include <type.h>

inherit command LIB_COMMAND;
inherit access  API_ACCESS;
inherit rsrc    API_RSRC;
inherit user    API_USER;

# define SPACE16        "                "

static int create(varargs int clone) {
  command::create(clone);
  user::create();
  rsrc::create();
}
private string swapnum(int num, int div)
{
    string str;

    str = (string) ((float) num / (float) div);
    str += (sscanf(str, "%*s.") != 0) ? "00" : ".00";
    if (strlen(str) > 4) {
        str = (str[3] == '.') ? str[.. 2] : str[.. 3];
    }
    return str;
}

private string ralign(mixed num, int width)
{
    string str;

    str = SPACE16 + (string) num;
    return str[strlen(str) - width ..];
}

private string ntoa(mixed num)
{
    string str;
    float mantissa;
    int exponent;

    num = (float) num;
    if (num <= 999999999.0) {
        return ralign((int) num, 9);
    }

    str = (string) num;
    sscanf(str, "%*se+%d", exponent);
    mantissa = num / pow(10.0, (float) exponent);
    if (strlen(str) > 10) {
        num = (exponent < 10) ? 100000.0 : 10000.0;
        mantissa = floor(mantissa * num + 0.5) / num;
    }
    return ralign(mantissa + "e" + exponent, 9);
}

private string percentage(mixed part, mixed total)
{
    if (total == 0) {
        return "(  0%)";
    }
    return "(" + ralign((int) ((float) part * 100.0 / (float) total), 3) + "%)";
}

int cmd(object user, string arg) {
  mixed *status;
  int uptime, hours, minutes, seconds;
  int short, long;
  int i;
  mixed obj;
  string str;
  
  if (!arg || arg == "") {
    status = status();
    str =
      "                                          Server:       " +
      (string) status[ST_VERSION] + "\n" +
      "------------ Swap device -------------\n" +
      "sectors:  " + ntoa(status[ST_SWAPUSED]) + " / " +
                 ntoa(status[ST_SWAPSIZE]) + " " +
      percentage(status[ST_SWAPUSED], status(ST_SWAPSIZE)) +
      "    Start time:   " + ctime(status[ST_STARTTIME])[4 ..] + "\n" +
      "sector size:   " + (((float) status[ST_SECTORSIZE] / 1024.0) + "K" +
                       SPACE16)[..15];
    if ((int) status[ST_STARTTIME] != (int) status[ST_BOOTTIME]) {
      str += "           Last reboot:  " +
           ctime(status[ST_BOOTTIME])[4 ..];
    }
    uptime = status[ST_UPTIME];
    seconds = uptime % 60;
    uptime /= 60;
    minutes = uptime % 60;
    uptime /= 60;
    hours = uptime % 24;
    uptime /= 24;
    short = status[ST_NCOSHORT];
    long = status[ST_NCOLONG];
    i = sizeof(query_connections());
    str += "\n" +
      "swap average:  " + (swapnum(status[ST_SWAPRATE1], 60) + ", " +
                           swapnum(status[ST_SWAPRATE5], 300) + SPACE16)[.. 15] +
        "           Uptime:       " +
        ((uptime == 0) ? "" : uptime + ((uptime == 1) ? " day, " : " days, ")) +
        ralign("00" + hours, 2) + ":" + ralign("00" + minutes, 2) + ":" +
        ralign("00" + seconds, 2) + "\n\n" +
      "--------------- Memory ---------------" +
        "    ------------ Callouts ------------\n" +
      "static:   " +
        ntoa(status[ST_SMEMUSED]) + " / " + ntoa(status[ST_SMEMSIZE]) + " " +
        percentage(status[ST_SMEMUSED], status[ST_SMEMSIZE]) +
        "    short: " + ntoa(short) + "            " +
        percentage(short, short + long) + "\n" +
      "dynamic:  " + ntoa(status[ST_DMEMUSED]) + " / " +
                     ntoa(status[ST_DMEMSIZE]) + " " +
        percentage(status[ST_DMEMUSED], status[ST_DMEMSIZE]) +
        " +  other: " + ntoa(long) + "            " +
        percentage(long, short + long) + " +\n" +
      "          " +
        ntoa((float) status[ST_SMEMUSED] + (float) status[ST_DMEMUSED]) + " / " +
        ntoa((float) status[ST_SMEMSIZE] + (float) status[ST_DMEMSIZE]) + " " +
        percentage((float) status[ST_SMEMUSED] + (float) status[ST_DMEMUSED],
                     (float) status[ST_SMEMSIZE] + (float) status[ST_DMEMSIZE]) +
          "           " +
          ntoa(short + long) + " /" + ntoa(status[ST_COTABSIZE]) + " " +
          percentage(short + long, status[ST_COTABSIZE]) + "\n\n" +
        "Objects:  " +
          ntoa(status[ST_NOBJECTS]) + " / " + ntoa(status[ST_OTABSIZE]) + " " +
          percentage(status[ST_NOBJECTS], status[ST_OTABSIZE]) +
          "    Users: " + ntoa(i) + " /" + ntoa(status[ST_UTABSIZE]) + " " +
          percentage(i, status[ST_UTABSIZE]) + "\n\n";
        user -> message(str);
        return TRUE;
  } 
  user -> message("Usage: status\nNo argument is currently accepted or used.\n");
}