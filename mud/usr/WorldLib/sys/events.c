# include <status.h>
# include <worldlib.h>
# include <data.h>

/*
 * This service manages world events.
 *
 * Periodic "events" for objects as well as sensations.
 *
 * We do things on one-second boundaries here.
 *
 * We want a priority queue with the next thing to run on top.
 * Once an event runs, we put it back in the queue.
 *
 */

object PRIORITY_QUEUE timers;
object PRIORITY_QUEUE sensation_sets;
object PRIORITY_QUEUE sensations;

mapping timer_owners;

void create(varargs int clone) {
  timers = new_object(PRIORITY_QUEUE);
  sensation_sets = new_object(PRIORITY_QUEUE);
  timer_owners = ([ ]);
  call_out("heart_beat", 1);
}

int get_sensation_set_count() {
  return sensation_sets -> size();
}

int get_timer_count() {
  return timers -> size();
}

static void run_sensation_set(int now, object PRIORITY_QUEUE_ITEM item) {
  object PRIORITY_QUEUE_ITEM  q_item;
  object e;
  object ob;
  object *obs;
  int i, n, id;

  if(ob = item -> get_data()) {
    /* now we want to trigger each of the sensations in the set */
    if(obs = ob -> get_sensations()) {
      for(i = 0, n = sizeof(obs); i < n; i++) {
        if(obs[i]->get_duration()) {
          /* start the sensation, add it to the location, and schedule the end */
          obs[i] -> start_sensation();
          id = obs[i] -> get_epicenter() -> get_object() -> add_sensation(obs[i]);
          e = new_object(SENSATION_END_DATA);
          e -> set_object(obs[i]->get_epicenter()->get_object());
          e -> set_sensation_id(id);
          q_item = new_object(PRIORITY_QUEUE_ITEM);
          q_item->set_priority(now + obs[i]->get_duration());
          q_item->set_data(e);
          timers->insert(q_item);
        }
        else {
          /* acute sensation, so run it now */
          obs[i] -> observe_sensation();
        }
      }
    }
    /* then schedule the next in the sequence */
    if(ob -> get_next()) {
      item -> set_data(ob -> get_next());
      item -> set_priority(now + ob -> get_delay());
      sensation_sets->insert(item);
    }
  }
}

void add_sensation_set(object SENSATION_SET ob) {
  object PRIORITY_QUEUE_ITEM pqo;
  int now;

  now = time();
  pqo = new_object(PRIORITY_QUEUE_ITEM);
  pqo -> set_data(ob);
  pqo -> set_priority(now + (ob -> get_delay() ? ob -> get_delay() : 0));
  if(ob -> get_delay() && ob -> get_delay() > 0) {
    sensation_sets -> insert(pqo);
  }
  else {
    run_sensation_set(now, pqo);
  }
}

/* TODO: manage tick counts so we don't error out */
void heart_beat() {
  object PRIORITY_QUEUE_ITEM item, q_item;
  object e;
  object ob;
  object *obs;
  int i, n, id;
  
  int now;

  call_out("heart_beat", 1);

  now = time();

  /* manage periodic event triggers for items
   * this calls the "timer:$name" event in the item
   */

  /* We want to watch our ticks so we don't run over */
  while((item = timers->peek()) && item->get_priority() <= now) {
    timers -> remove();
    ob = item -> get_data();
    if(ob -> get_object()) {
      ob -> trigger();
      if(ob -> get_periodic()) {
        item -> set_priority(now + ob -> get_frequency());
        timers -> insert(item);
      }
    }
  }

  /* manage the sequencing and scheduling of events for sensation sets */
  item = sensation_sets->peek();
  while((item = sensation_sets->peek()) && item->get_priority() <= now) {
    sensation_sets->remove();
    run_sensation_set(now, item);
  }

  /* now we manage acute and chronic sensations */
  /* while((item = sensations -> peek()) && item -> get_priority() <= now) {
    sensations -> remove();
    
  } */
}

int add_timer(object owner, string timer, int frequency, int periodic) {
  object t;
  object PRIORITY_QUEUE_ITEM p_ob;
  int idx, i, n;

  t = new_object(EVENT_TIMER_DATA);
  t -> set_target(owner);
  t -> set_event("timer:"+timer);
  t -> set_frequency(frequency);
  t -> set_periodic(periodic);
  if(!timer_owners[owner]) {
    timer_owners[owner] = ({ t });
    idx = 1;
  }
  else {
    for(i = 0, n = sizeof(timer_owners[owner]); i < n; i++) {
      if(!timer_owners[owner][i]) {
        idx = i;
        timer_owners[owner][i] = t;
        break;
      }
    }
    if(!idx) {
      timer_owners[owner][i] += ({ t });
      idx = sizeof(timer_owners[owner]);
    }
  }
  p_ob = new_object(PRIORITY_QUEUE_ITEM);
  p_ob -> set_priority(time() + frequency);
  p_ob -> set_data(t);
  timers -> insert(p_ob);
  return idx;
}

void remove_timer(object owner, int id) {
}
