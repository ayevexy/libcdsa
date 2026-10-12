#include "collection.h"

#include "core/errors.h"
#include "list/array_list.h"
#include "list/linked_list.h"
#include "map/hash_map.h"
#include "map/tree_map.h"
#include "set/hash_set.h"
#include "set/tree_set.h"
#include "deque/deque.h"
#include "stack/stack.h"
#include "queue/queue.h"
#include "priority_queue/priority_queue.h"

static const char* collection_type_to_string(const CollectionType type) {
    static const char* names[] = {
        "ARRAY_LIST",
        "LINKED_LIST",
        "HASH_MAP",
        "TREE_MAP",
        "HASH_SET",
        "TREE_SET",
        "DEQUE",
        "STACK",
        "QUEUE",
        "PRIORITY_QUEUE"
    };
    return type >= ARRAY_LIST && type <= PRIORITY_QUEUE ? names[type] : "UNKNOWN";
}

void* collection_as(Collection collection, CollectionType type) {
    if (collection.type != type) {
        set_error(TYPE_MISMATCH_ERROR, "expected %s, received %s", collection_type_to_string(collection.type), collection_type_to_string(type));
        return nullptr;
    }
    return (void*) collection.data_structure;
}

void collection_add(Collection collection, const void *element) {
    switch (collection.type) {
        case ARRAY_LIST:      array_list_add_last((ArrayList*) collection.data_structure, element);         break;
        case LINKED_LIST:     linked_list_add_last((LinkedList*) collection.data_structure, element);       break;
        case HASH_MAP:        hash_map_put((HashMap*) collection.data_structure, element, nullptr);         break;
        case TREE_MAP:        tree_map_put((TreeMap*) collection.data_structure, element, nullptr);         break;
        case HASH_SET:        hash_set_add((HashSet*) collection.data_structure, element);                  break;
        case TREE_SET:        tree_set_add((TreeSet*) collection.data_structure, element);                  break;
        case DEQUE:           deque_add_last((Deque*) collection.data_structure, element);                  break;
        case STACK:           stack_push((Stack*) collection.data_structure, element);                      break;
        case QUEUE:           queue_enqueue((Queue*) collection.data_structure, element);                   break;
        case PRIORITY_QUEUE:  priority_queue_enqueue((PriorityQueue*) collection.data_structure, element);  break;
        default:              unreachable();
    }
}

void collection_remove(Collection collection, const void *element) {
    switch (collection.type) {
        case ARRAY_LIST:      array_list_remove_element((ArrayList*) collection.data_structure, element);    break;
        case LINKED_LIST:     linked_list_remove_element((LinkedList*) collection.data_structure, element);  break;
        case HASH_MAP:        hash_map_remove((HashMap*) collection.data_structure, element);                break;
        case TREE_MAP:        tree_map_remove((TreeMap*) collection.data_structure, element);                break;
        case HASH_SET:        hash_set_remove((HashSet*) collection.data_structure, element);                break;
        case TREE_SET:        tree_set_remove((TreeSet*) collection.data_structure, element);                break;
        case DEQUE:           deque_remove_first((Deque*) collection.data_structure);                        break;
        case STACK:           stack_pop((Stack*) collection.data_structure);                                 break;
        case QUEUE:           queue_dequeue((Queue*) collection.data_structure);                             break;
        case PRIORITY_QUEUE:  priority_queue_dequeue((PriorityQueue*) collection.data_structure);            break;
        default:              unreachable();
    }
}

int collection_size(Collection collection) {
    return collection.size(collection.data_structure);
}

bool collection_is_empty(Collection collection) {
    return collection_size(collection) == 0;
}

Iterator* collection_iterator(Collection collection) {
    return collection.iterator(collection.data_structure);
}

bool collection_equals(Collection collection, Collection other_collection) {
    if (collection.type != other_collection.type) {
        return false;
    }
    switch (collection.type) {
        case ARRAY_LIST:      return array_list_equals((ArrayList*) collection.data_structure, (ArrayList*) other_collection.data_structure);
        case LINKED_LIST:     return linked_list_equals((LinkedList*) collection.data_structure, (LinkedList*) other_collection.data_structure);
        case HASH_MAP:        return hash_map_equals((HashMap*) collection.data_structure, (HashMap*) other_collection.data_structure);
        case TREE_MAP:        return tree_map_equals((TreeMap*) collection.data_structure, (TreeMap*) other_collection.data_structure);
        case HASH_SET:        return hash_set_equals((HashSet*) collection.data_structure, (HashSet*) other_collection.data_structure);
        case TREE_SET:        return tree_set_equals((TreeSet*) collection.data_structure, (TreeSet*) other_collection.data_structure);
        case DEQUE:           return deque_equals((Deque*) collection.data_structure, (Deque*) other_collection.data_structure);
        case STACK:           return stack_equals((Stack*) collection.data_structure, (Stack*) other_collection.data_structure);
        case QUEUE:           return queue_equals((Queue*) collection.data_structure, (Queue*) other_collection.data_structure);
        case PRIORITY_QUEUE:  return priority_queue_equals((PriorityQueue*) collection.data_structure, (PriorityQueue*) other_collection.data_structure);
        default:              unreachable();
    }
}

void collection_for_each(Collection collection, Consumer action) {
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
        set_error(error, "%s of 'collection'", error_description());
        return;
    }
    while (iterator_has_next(iterator)) {
        action(iterator_next(iterator));
    }
    iterator_destroy(&iterator);
}

void collection_clear(Collection collection) {
    switch (collection.type) {
        case ARRAY_LIST:      array_list_clear((ArrayList*) collection.data_structure);          break;
        case LINKED_LIST:     linked_list_clear((LinkedList*) collection.data_structure);        break;
        case HASH_MAP:        hash_map_clear((HashMap*) collection.data_structure);              break;
        case TREE_MAP:        tree_map_clear((TreeMap*) collection.data_structure);              break;
        case HASH_SET:        hash_set_clear((HashSet*) collection.data_structure);              break;
        case TREE_SET:        tree_set_clear((TreeSet*) collection.data_structure);              break;
        case DEQUE:           deque_clear((Deque*) collection.data_structure);                   break;
        case STACK:           stack_clear((Stack*) collection.data_structure);                   break;
        case QUEUE:           queue_clear((Queue*) collection.data_structure);                   break;
        case PRIORITY_QUEUE:  priority_queue_clear((PriorityQueue*) collection.data_structure);  break;
        default:              unreachable();
    }
}

bool collection_contains(Collection collection, const void* element) {
    return collection.contains(collection.data_structure, element);
}

bool collection_contains_all(Collection collection, Collection other_collection) {
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(other_collection)))) {
        set_error(error, "%s of 'collection'", error_description());
        return false;
    }
    bool contains = true;
    while (iterator_has_next(iterator)) {
        const void* element = iterator_next(iterator);
        if (!collection_contains(collection, element)) {
            contains = false;
            break;
        }
    }
    iterator_destroy(&iterator);
    return contains;
}

Array(void*) collection_to_array(Collection collection) {
    Array(void*) elements = array_new(collection_size(collection), void*);
    if (!elements) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'array'");
        return nullptr;
    }
    Iterator* iterator; Error error;

    if ((error = attempt(iterator = collection_iterator(collection)))) {
        array_destroy(&elements);
        set_error(error, "%s of 'collection'", error_description());
        return nullptr;
    }
    for (int i = 0; iterator_has_next(iterator); i++) {
        elements[i] = iterator_next(iterator);
    }
    iterator_destroy(&iterator);
    return elements;
}

String collection_to_string(Collection collection) {
    switch (collection.type) {
        case ARRAY_LIST:      return array_list_to_string((ArrayList*) collection.data_structure);
        case LINKED_LIST:     return linked_list_to_string((LinkedList*) collection.data_structure);
        case HASH_MAP:        return hash_map_to_string((HashMap*) collection.data_structure);
        case TREE_MAP:        return tree_map_to_string((TreeMap*) collection.data_structure);
        case HASH_SET:        return hash_set_to_string((HashSet*) collection.data_structure);
        case TREE_SET:        return tree_set_to_string((TreeSet*) collection.data_structure);
        case DEQUE:           return deque_to_string((Deque*) collection.data_structure);
        case STACK:           return stack_to_string((Stack*) collection.data_structure);
        case QUEUE:           return queue_to_string((Queue*) collection.data_structure);
        case PRIORITY_QUEUE:  return priority_queue_to_string((PriorityQueue*) collection.data_structure);
        default:              unreachable();
    }
}