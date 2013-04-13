# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <config.h>
# include <system.h>
# include <worldlib.h>
# include <toollib.h>
# include <worldlib/proximity.h>
# include <data.h>

static int create(varargs int clone) {
  string *files;
  int i, n;

  files = ({
    DETAIL_DATA, EXIT_DATA, LOCATION_DATA, SENSATION_DATA,
    SENSATION_SET, SENSATION_END_DATA, EVENT_TIMER_DATA,
    GENETICS_DATA, DOMAIN_DATA, AREA_DATA,
    HTTP_DOMAIN_RESOURCE, HTTP_AREA_RESOURCE, HTTP_THING_RESOURCE,
    HTTP_WARD_RESOURCE,
    THING_OBJ, WARD_OBJ, HOSPITAL_OBJ,
    PRIORITY_QUEUE, PRIORITY_QUEUE_ITEM,
    EXITS_D, HOSPITAL_D, CHARACTER_D, PROXIMITY_D,
    WORLD_EVENTS_D, DOMAINS_D,
  });

  for(i = 0, n = sizeof(files); i < n; i++)
    if(!find_object(files[i])) compile_object(files[i]);
}
