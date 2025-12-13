#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

typedef struct ArrayList ArrayList;

ArrayList* array_list_new(void);

void array_list_delete(ArrayList*);

void array_list_add(ArrayList*, void* element);

void* array_list_get(ArrayList*, int index);

void array_list_set(ArrayList*, int index, void* element);

void array_list_remove(ArrayList*, int index);

int array_list_size(ArrayList*);

int array_list_capacity(ArrayList*);

bool array_list_is_empty(ArrayList*);

void array_list_clear(ArrayList*);

#endif