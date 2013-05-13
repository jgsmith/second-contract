# include <devlib.h>
# include <data.h>

object PRIORITY_QUEUE_ITEM *items;

void create(varargs int clone) {
  if(clone) {
    items = ({ });
  }
}

int size() { return items ? sizeof(items) : 0; }

object PRIORITY_QUEUE_ITEM peek() {
  if(sizeof(items)) return items[0];
  return nil;
}

void insert(object PRIORITY_QUEUE_ITEM ob) {
  int pos, parent_pos;

  items += ({ ob });
  pos = sizeof(items);
  while(pos > 1) {
    parent_pos = pos >> 1;
    if(items[parent_pos-1]->get_priority() < items[pos-1]->get_priority()) return;
    ob = items[parent_pos-1];
    items[parent_pos-1] = items[pos-1];
    items[pos-1] = ob;
    pos = parent_pos;
  }
}

object PRIORITY_QUEUE_ITEM remove() {
  object PRIORITY_QUEUE_ITEM ret, ob;
  int pos, size, smallest_child, left_child, right_child;

  if(!items || sizeof(items) == 0) return nil;
  
  ret = items[0];
  size = sizeof(items)-1;
  items[0] = items[size];
  items = items[0..size-1];
  
  pos = 1;
  while(pos < size) {
    right_child = (left_child = pos<<1) + 1;
    if(items[left_child-1] -> get_priority() < items[right_child-1]->get_priority())
      smallest_child = left_child;
    else
      smallest_child = right_child;
    if(items[smallest_child-1]->get_priority() > items[pos-1]->get_priority())
      return ret;
    ob = items[smallest_child-1];
    items[smallest_child-1] = items[pos];
    items[pos] = ob;
    pos = smallest_child;
  }
  return ret;
}
