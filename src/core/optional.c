#include "core/optional.h"

#include "core/errors.h"

Optional optional_empty(void) {
    return (Optional) { .present = false };
}

Optional optional_of(void* value) {
    return (Optional) { .value = value, .present = true };
}

Optional optional_of_nullable(void* value) {
    return value ? optional_of(value) : optional_empty();
}

bool optional_is_present(Optional optional) {
    return optional.present;
}

bool optional_is_empty(Optional optional) {
    return !optional.present;
}

void* optional_get(Optional optional) {
    if (optional_is_empty(optional)) {
        set_error(NO_SUCH_ELEMENT_ERROR, "optional is empty");
        return nullptr;
    }
    return optional.value;
}

void* optional_or_else(Optional optional, void* fallback) {
    return optional.present ? optional.value : fallback;
}

void optional_if_present(Optional optional, Consumer action) {
    if (optional.present) {
        action(optional.value);
    }
}

Optional optional_transform(Optional optional, Operator mapper) {
    return optional.present
        ? optional_of(mapper(optional.value))
        : optional_empty();
}

Optional optional_keep_if(Optional optional, Predicate predicate) {
    return optional.present && predicate(optional.value)
        ? optional
        : optional_empty();
}