#include "sets.h"

HashSet* hash_set_union(Collection set_a, Collection set_b, HashSet* result_set) {
    hash_set_add_all(result_set, set_a);
    hash_set_add_all(result_set, set_b);
    return result_set;
}

TreeSet* tree_set_union(Collection set_a, Collection set_b, TreeSet* result_set) {
    tree_set_add_all(result_set, set_a);
    tree_set_add_all(result_set, set_b);
    return result_set;
}

HashSet* hash_set_intersection(Collection set_a, Collection set_b, HashSet* result_set) {
    hash_set_add_all(result_set, set_a);
    hash_set_retain_all(result_set, set_b);
    return result_set;
}

TreeSet* tree_set_intersection(Collection set_a, Collection set_b, TreeSet* result_set) {
    tree_set_add_all(result_set, set_a);
    tree_set_retain_all(result_set, set_b);
    return result_set;
}

HashSet* hash_set_difference(Collection set_a, Collection set_b, HashSet* result_set) {
    hash_set_add_all(result_set, set_a);
    hash_set_remove_all(result_set, set_b);
    return result_set;
}

TreeSet* tree_set_difference(Collection set_a, Collection set_b, TreeSet* result_set) {
    tree_set_add_all(result_set, set_a);
    tree_set_remove_all(result_set, set_b);
    return result_set;
}

HashSet* hash_set_symmetric_difference(Collection set_a, Collection set_b, HashSet* result_set) {
    HashSet* temporary; Error error = attempt(temporary = hash_set_clone(result_set));

    if (error) {
        set_error(error, error_description());
        return nullptr;
    }

    hash_set_add_all(result_set, set_a);
    hash_set_add_all(result_set, set_b);

    hash_set_add_all(temporary, set_a);
    hash_set_retain_all(temporary, set_b);

    hash_set_remove_all(result_set, hash_set_to_collection(temporary));

    hash_set_destroy(&temporary);
    return result_set;
}

TreeSet* tree_set_symmetric_difference(Collection set_a, Collection set_b, TreeSet* result_set) {
    TreeSet* temporary; Error error = attempt(temporary = tree_set_clone(result_set));

    if (error) {
        set_error(error, error_description());
        return nullptr;
    }

    tree_set_add_all(result_set, set_a);
    tree_set_add_all(result_set, set_b);

    tree_set_add_all(temporary, set_a);
    tree_set_retain_all(temporary, set_b);

    tree_set_remove_all(result_set, tree_set_to_collection(temporary));

    tree_set_destroy(&temporary);
    return result_set;
}

bool (sets_is_subset)(Collection set_a, Collection set_b) {
    return collection_contains_all(set_b, set_a);
}

bool (sets_is_superset)(Collection set_a, Collection set_b) {
    return collection_contains_all(set_a, set_b);
}