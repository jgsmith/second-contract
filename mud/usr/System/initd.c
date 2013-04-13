# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <kernel/access.h>
# include <config.h>
# include <system.h>
# include <worldlib.h>
# include <toollib.h>
# include <iflib.h>
# include <devlib.h>
# include <gamelib.h>

inherit rsrc API_RSRC;
inherit access API_ACCESS;

static void create(varargs int clone) {
    object driver;
    object telnet_manager;
    object http_manager;
    object rsrc_manager;
    object objreg_manager;

    driver = find_object(DRIVER);

    if(!find_object(LOG_D)) {
      compile_object(LOG_D);
    }

    if(find_object(LOG_D)) {
      /* driver -> set_error_manager(find_object(LOG_D)); */
    }

    if(!find_object(TELNET_D)) {
        compile_object(TELNET_D);
    }

    telnet_manager = find_object(TELNET_D);
    "/kernel/sys/userd" -> set_telnet_manager(0, telnet_manager);

    if(!find_object(HTTP_D)) {
      compile_object(HTTP_D);
    }

    http_manager = find_object(HTTP_D);

    "/kernel/sys/userd" -> set_binary_manager(1, http_manager);

    if(!find_object(HTTP_SYSTEM_RESOURCE)) {
      compile_object(HTTP_SYSTEM_RESOURCE);
    }


    if(!find_object("/usr/System/obj/wiztool")) {
        compile_object("/usr/System/obj/wiztool");
    }

    if(!find_object(MAPPING_D)) compile_object(MAPPING_D);
    if(!find_object(STRING_D)) compile_object(STRING_D);
    if(!find_object(AUTH_D)) compile_object(AUTH_D);
    if(!find_object(ERROR_D)) compile_object(ERROR_D);

    rsrc::create();

    rsrc::add_owner("ToolLib");
    rsrc::add_owner("DevLib");
    rsrc::add_owner("IFLib");
    rsrc::add_owner("WorldLib");
    rsrc::add_owner("GameLib");

    if(!find_object(TOOLLIB_INIT)) compile_object(TOOLLIB_INIT);
    if(!find_object(DEVLIB_INIT)) compile_object(DEVLIB_INIT);
    if(!find_object(WORLDLIB_INIT)) compile_object(WORLDLIB_INIT);
    if(!find_object(IFLIB_INIT)) compile_object(IFLIB_INIT);
    if(!find_object(GAMELIB_INIT)) compile_object(GAMELIB_INIT);

    http_manager -> update_resource_handlers_from_config();
}

void upgraded(varargs int clone) {
}
