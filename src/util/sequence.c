#include "sequence.h"

#include "list/list.h"
#include "set/set.h"
#include "core/memory.h"
#include "core/errors.h"
#include "constraints.h"
#include <assert.h>

constexpr int OPERATION_LIMIT = 15;
constexpr int SORT_OPERATION_LIMIT = 5;

typedef enum {
    FILTER,
    MAP,
    PEEK,
    LIMIT,
    SKIP,
    TAKE_WHILE,
    DROP_WHILE,
    DISTINCT,
    SORT
} OperationType;

typedef enum {
    FOR_EACH,
    REDUCE,
    FIND,
    MAX,
    MIN,
    ALL_MATCH,
    ANY_MATCH,
    NONE_MATCH,
    COUNT,
    TO_ARRAY
} TerminalOperationType;

typedef struct {
    OperationType type;
    union {
        struct { Predicate test; } filter;
        struct { Operator operate; } map;
        struct { Consumer consume; } peek;
        struct { int count; int max; } limit;
        struct { int count; int max; } skip;
        struct { Predicate test; } take_while;
        struct { Predicate test; } drop_while;
        struct { HashSet* set; } distinct;
        struct { ArrayList* list; Comparator comparator; SortingAlgorithm algorithm; } sort;
    };
} Operation;

typedef struct {
    TerminalOperationType type;
    union {
        struct { Consumer consume; } for_each;
        struct { void* result; BiOperator accumulate; } reduce;
        struct { void* element; bool found; } find;
        struct { void* element; bool found; Comparator compare; } max_min;
        struct { int count; int matches; Predicate test; } match;
        struct { int count; } count;
        struct { ArrayList* list; } to_array;
    };
} TerminalOperation;

typedef union {
    void* element;
    Optional optional;
    bool match;
    int count;
    Array(void*) array;
} SequencePipeLineResult;

typedef enum {
    PENDING,
    PROCESSING,
    BUFFERING,
    EXHAUSTED,
    ABORTED
} SequencePipeLineState;

typedef struct {
    Iterator* source;
    SequencePipeLineState state;
    Operation operations[OPERATION_LIMIT];
    int operation_count;
    int operation_index;
    int operation_last_index;
    Operation* distinct;
    Operation* sort[SORT_OPERATION_LIMIT];
    int sort_count;
    int sort_index;
    TerminalOperation terminal_operation;
} SequencePipeline;

struct Sequence {
    SequencePipeline pipeline;
};

typedef struct IterationContextOptions IterationContextOptions;

static SequencePipeLineResult process_sequence_pipeline(Sequence*);

static void execute_pipeline_operations(Sequence*);

static void replace_pipeline_source(Sequence*);

static void release_pipeline_resources(Sequence*);

static SequencePipeLineResult pipeline_result(Sequence*);

static SequencePipeLineResult pipeline_result_array(Sequence*, TerminalOperation*);

static bool execute_operation(Sequence*, int, void**);

static bool operation_distinct(Sequence*, Operation*, void*);

static bool operation_sort(Sequence*, Operation*, int, void*);

static void execute_terminal_operation(Sequence*, void*);

static void operation_reduce(TerminalOperation*, void*);

static void operation_max_min(TerminalOperation*, void*);

static void operation_find(TerminalOperation*, void*);

static void operation_match(TerminalOperation*, void*);

static void operation_to_array(Sequence*, TerminalOperation*, void*);

static Iterator* create_iterator(IterationContextOptions);

static bool iterator_has_next_internal(const void*);

static void* iterator_next_internal(void*);

static void iterator_destroy_internal(void*);

static Sequence* sequence_create(Iterator* source) {
    if (!source) {
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'sequence'");
        return nullptr;
    }
    Sequence* sequence = memory_try_alloc(sizeof(Sequence));
    if (!sequence) {
        iterator_destroy(&source);
        set_error(MEMORY_ALLOCATION_ERROR, "failed to allocate memory for 'sequence'");
        return nullptr;
    }
    sequence->pipeline = (SequencePipeline) {
        .source = source,
        .state = PENDING
    };
    return sequence;
}

Sequence* sequence_from(Collection collection) {
    Iterator* iterator; Error error;
    if ((error = attempt(iterator = collection_iterator(collection)))) {
        set_error(error, "%s of 'collection'", plain_error_message());
        return nullptr;
    }
    return sequence_create(iterator);
}

struct IterationContextOptions {
    void* seed;
    int limit;
    Predicate test;
    Supplier get;
    Operator operate;
    Array(void*) elements;
};

Sequence* (sequence_of)(Array(void*) elements) {
    if (require_non_null(elements)) return nullptr;
    return sequence_create(create_iterator((IterationContextOptions) { .limit = array_length(elements), .elements = elements }));
}

Sequence* sequence_empty() {
    return sequence_create(create_iterator((IterationContextOptions) { .limit = -1 }));
}

Sequence* sequence_generate(Supplier generator, int limit) {
    if (require_non_null(generator)) return nullptr;
    if (limit < 1) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'limit' can't be less than 1");
        return nullptr;
    }
    return sequence_create(create_iterator((IterationContextOptions) { .limit = limit, .get = generator }));
}

Sequence* sequence_iterate(void* seed, Predicate has_next, Operator next) {
    if (require_non_null(has_next, next)) return nullptr;
    return sequence_create(create_iterator((IterationContextOptions) { .seed = seed, .test = has_next, .operate = next }));
}

void sequence_close(Sequence* sequence) {
    release_pipeline_resources(sequence);
}

static bool require_operation_count(Sequence* sequence) {
    if (sequence->pipeline.operation_count >= OPERATION_LIMIT) {
        set_error(ILLEGAL_STATE_ERROR, "Sequence has more than 15 chained operations", OPERATION_LIMIT);
        return true;
    }
    return false;
}

void sequence_filter(Sequence* sequence, Predicate predicate) {
    if (require_non_null(sequence, predicate) || require_operation_count(sequence)) return;

    sequence->pipeline.operations[sequence->pipeline.operation_count++] = (Operation) { .type = FILTER, .filter = { predicate } };
}

void sequence_map(Sequence* sequence, Operator operator) {
    if (require_non_null(sequence, operator) || require_operation_count(sequence)) return;

    sequence->pipeline.operations[sequence->pipeline.operation_count++] = (Operation) { .type = MAP, .map = { operator } };
}

void sequence_peek(Sequence* sequence, Consumer action) {
    if (require_non_null(sequence, action) || require_operation_count(sequence)) return;

    sequence->pipeline.operations[sequence->pipeline.operation_count++] = (Operation) { .type = PEEK, .peek = { action } };
}

void sequence_limit(Sequence* sequence, int max_size) {
    if (require_non_null(sequence) || require_operation_count(sequence)) return;
    if (max_size < 0) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'max_size' can't be negative");
        return;
    }
    sequence->pipeline.operations[sequence->pipeline.operation_count++] = (Operation) { .type = LIMIT, .limit = { 0, max_size } };
}

void sequence_skip(Sequence* sequence, int count) {
    if (require_non_null(sequence) || require_operation_count(sequence)) return;
    if (count < 0) {
        set_error(ILLEGAL_ARGUMENT_ERROR, "'count' can't be negative");
        return;
    }
    sequence->pipeline.operations[sequence->pipeline.operation_count++] = (Operation) { .type = SKIP, .skip = { 0, count } };
}

void sequence_take_while(Sequence* sequence, Predicate predicate) {
    if (require_non_null(sequence, predicate) || require_operation_count(sequence)) return;

    sequence->pipeline.operations[sequence->pipeline.operation_count++] = (Operation) { .type = TAKE_WHILE, .take_while = { predicate } };
}

void sequence_drop_while(Sequence* sequence, Predicate predicate) {
    if (require_non_null(sequence, predicate) || require_operation_count(sequence)) return;

    sequence->pipeline.operations[sequence->pipeline.operation_count++] = (Operation) { .type = DROP_WHILE, .drop_while = { predicate } };
}

void sequence_distinct(Sequence* sequence, bool (*equals)(const void*, const void*)) {
    if (require_non_null(sequence, equals) || require_operation_count(sequence)) return;

    if (!sequence->pipeline.distinct) {
        HashSet* set; Error error = attempt(set = set_new((&(HashSetOptions) {
            .initial_capacity = 16,
            .load_factor = 0.75f,
            .hash = pointer_hash,
            .destruct = noop_destruct,
            .equals = equals,
            .to_string = pointer_to_string,
            .memory_alloc = memory_try_alloc,
            .memory_dealloc = memory_dealloc
        })));
        if (error) {
            set_error(error, "%s", plain_error_message());
            return;
        }
        sequence->pipeline.operations[sequence->pipeline.operation_count++] = (Operation) { .type = DISTINCT, .distinct = { set } };
        sequence->pipeline.distinct = &sequence->pipeline.operations[sequence->pipeline.operation_count - 1];
    }
}

void sequence_sort(Sequence* sequence, Comparator comparator, SortingAlgorithm algorithm) {
    if (require_non_null(sequence, comparator) || require_operation_count(sequence)) return;

    if (sequence->pipeline.sort_count >= SORT_OPERATION_LIMIT) {
        set_error(ILLEGAL_STATE_ERROR, "Sequence has more than 5 chained sort operations", SORT_OPERATION_LIMIT);
        return;
    }
    ArrayList* list; Error error = attempt(list = list_new(DEFAULT_ARRAY_LIST_OPTIONS()));
    if (error) {
        set_error(error, "%s", plain_error_message());
        return;
    }
    sequence->pipeline.operations[sequence->pipeline.operation_count++] = (Operation) { .type = SORT, .sort = { list, comparator, algorithm } };
    sequence->pipeline.sort[sequence->pipeline.sort_count++] = &sequence->pipeline.operations[sequence->pipeline.operation_count - 1];
}

void sequence_for_each(Sequence* sequence, Consumer action) {
    if (require_non_null(sequence, action)) return;
    sequence->pipeline.terminal_operation = (TerminalOperation) { .type = FOR_EACH, .for_each = { action } };

    process_sequence_pipeline(sequence);
}

void* sequence_reduce(Sequence* sequence, void* identity, BiOperator accumulator) {
    if (require_non_null(sequence, identity, accumulator)) return nullptr;
    sequence->pipeline.terminal_operation = (TerminalOperation) { .type = REDUCE, .reduce = { identity, accumulator } };

    return process_sequence_pipeline(sequence).element;
}

Optional sequence_find(Sequence* sequence) {
    if (require_non_null(sequence)) return optional_empty();
    sequence->pipeline.terminal_operation = (TerminalOperation) { .type = FIND, .find = { nullptr, false } };

    return process_sequence_pipeline(sequence).optional;
}

Optional sequence_max(Sequence* sequence, Comparator comparator) {
    if (require_non_null(sequence, comparator)) return optional_empty();
    sequence->pipeline.terminal_operation = (TerminalOperation) { .type = MAX, .max_min = { nullptr, false, comparator } };

    return process_sequence_pipeline(sequence).optional;
}

Optional sequence_min(Sequence* sequence, Comparator comparator) {
    if (require_non_null(sequence, comparator)) return optional_empty();
    sequence->pipeline.terminal_operation = (TerminalOperation) { .type = MIN, .max_min = { nullptr, false, comparator } };

    return process_sequence_pipeline(sequence).optional;
}

bool sequence_all_match(Sequence* sequence, Predicate predicate) {
    if (require_non_null(sequence, predicate)) return false;
    sequence->pipeline.terminal_operation = (TerminalOperation) { .type = ALL_MATCH, .match = { .test = predicate } };

    return process_sequence_pipeline(sequence).match;
}

bool sequence_any_match(Sequence* sequence, Predicate predicate) {
    if (require_non_null(sequence, predicate)) return false;
    sequence->pipeline.terminal_operation = (TerminalOperation) { .type = ANY_MATCH, .match = { .test = predicate } };

    return process_sequence_pipeline(sequence).match;
}

bool sequence_none_match(Sequence* sequence, Predicate predicate) {
    if (require_non_null(sequence, predicate)) return false;
    sequence->pipeline.terminal_operation = (TerminalOperation) { .type = NONE_MATCH, .match = { .test = predicate } };

    return process_sequence_pipeline(sequence).match;
}

int sequence_count(Sequence* sequence) {
    if (require_non_null(sequence)) return 0;
    sequence->pipeline.terminal_operation = (TerminalOperation) { .type = COUNT, .count = { 0 } };

    return process_sequence_pipeline(sequence).count;
}

Array(void*) sequence_to_array(Sequence* sequence) {
    if (require_non_null(sequence)) return nullptr;
    sequence->pipeline.terminal_operation = (TerminalOperation) { .type = TO_ARRAY, .to_array = { list_new(DEFAULT_ARRAY_LIST_OPTIONS()) } };

    return process_sequence_pipeline(sequence).array;
}

static SequencePipeLineResult process_sequence_pipeline(Sequence* sequence) {
    while (true) {
        if (sequence->pipeline.state == PENDING) {
            execute_pipeline_operations(sequence);
        }
        if (sequence->pipeline.state == BUFFERING) {
            replace_pipeline_source(sequence);
        }
        if (sequence->pipeline.state == ABORTED) {
            set_plain_error(SEQUENCE_PROCESSING_ERROR, "SEQUENCE_PROCESSING_ERROR: %s", plain_error_message());
            return (SequencePipeLineResult) {};
        }
        if (sequence->pipeline.state == EXHAUSTED) {
            release_pipeline_resources(sequence);
            return pipeline_result(sequence);
        }
    }
}

static void execute_pipeline_operations(Sequence* sequence) {
    sequence->pipeline.state = PROCESSING;

    next_element:
    while (sequence->pipeline.state != ABORTED && iterator_has_next(sequence->pipeline.source)) {
        void* element = iterator_next(sequence->pipeline.source);

        for (int i = sequence->pipeline.operation_index; i < sequence->pipeline.operation_count; i++) {
            const bool skip = execute_operation(sequence, i, &element);
            if (skip) goto next_element;
        }
        execute_terminal_operation(sequence, element);
    }
    iterator_destroy(&sequence->pipeline.source);
    sequence->pipeline.source = nullptr;

    if (sequence->pipeline.state == PROCESSING) {
        sequence->pipeline.state = EXHAUSTED;
    }
}

static void replace_pipeline_source(Sequence* sequence) {
    auto sort = sequence->pipeline.sort[sequence->pipeline.sort_index++]->sort;
    Error error;
    if ((error = attempt(list_sort(sort.list, sort.comparator, sort.algorithm)))) {
        sequence->pipeline.state = ABORTED;
        return;
    }
    Iterator* iterator;
    if ((error = attempt(iterator = list_iterator(sort.list)))) {
        sequence->pipeline.state = ABORTED;
        return;
    }
    sequence->pipeline.source = iterator;
    sequence->pipeline.operation_index = sequence->pipeline.operation_last_index + 1;
    sequence->pipeline.state = PENDING;
}

static void release_pipeline_resources(Sequence* sequence) {
    if (sequence->pipeline.source) {
        iterator_destroy(&sequence->pipeline.source);
    }
    if (sequence->pipeline.distinct) {
        set_destroy(&sequence->pipeline.distinct->distinct.set);
    }
    for (int i = 0; i < sequence->pipeline.sort_count; i++) {
        list_destroy(&sequence->pipeline.sort[i]->sort.list);
    }
    memory_dealloc(sequence);
}

static SequencePipeLineResult pipeline_result(Sequence* sequence) {
    TerminalOperation operation = sequence->pipeline.terminal_operation;
    switch (operation.type) {
        case FOR_EACH:    return (SequencePipeLineResult) {};
        case REDUCE:      return (SequencePipeLineResult) { .element = operation.reduce.result };
        case COUNT:       return (SequencePipeLineResult) { .count = operation.count.count };
        case MAX:         return (SequencePipeLineResult) { .optional = (Optional) { operation.max_min.element, operation.max_min.found } };
        case MIN:         return (SequencePipeLineResult) { .optional = (Optional) { operation.max_min.element, operation.max_min.found } };
        case FIND:        return (SequencePipeLineResult) { .optional = (Optional) { operation.find.element, operation.find.found } };
        case ALL_MATCH:   return (SequencePipeLineResult) { .match = operation.match.matches == operation.match.count };
        case ANY_MATCH:   return (SequencePipeLineResult) { .match = operation.match.matches > 0 };
        case NONE_MATCH:  return (SequencePipeLineResult) { .match = operation.match.matches == 0 };
        case TO_ARRAY:    return pipeline_result_array(sequence, &operation);
    }
    assert(!"unreachable code");
}

static SequencePipeLineResult pipeline_result_array(Sequence* sequence, TerminalOperation* operation) {
    ArrayList* list = operation->to_array.list;
    Array(void*) array; Error error = attempt(array = list_to_array(list));

    if (error) {
        sequence->pipeline.state = ABORTED;
        set_plain_error(SEQUENCE_PROCESSING_ERROR, "SEQUENCE_PROCESSING_ERROR: %s", plain_error_message());
        return (SequencePipeLineResult) {};
    }
    list_destroy(&list);
    return (SequencePipeLineResult) { .array = array };
}

static bool execute_operation(Sequence* sequence, int index, void** element) {
    Operation* operation = &sequence->pipeline.operations[index];
    bool skip = false;
    switch (operation->type) {
        case FILTER:      skip = !operation->filter.test(*element);                     break;
        case MAP:         *element = operation->map.operate(*element);                  break;
        case PEEK:        operation->peek.consume(*element);                            break;
        case LIMIT:       skip = operation->limit.count++ >= operation->limit.max;      break;
        case SKIP:        skip = operation->skip.count++ < operation->skip.max;         break;
        case TAKE_WHILE:  skip = !operation->take_while.test(*element);                 break;
        case DROP_WHILE:  skip = operation->drop_while.test(*element);                  break;
        case DISTINCT:    skip = operation_distinct(sequence, operation, *element);     break;
        case SORT:        skip = operation_sort(sequence, operation, index, *element);  break;
    }
    return skip;
}

static bool operation_distinct(Sequence* sequence, Operation* operation, void* element) {
    bool added; Error error = attempt(added = set_add(operation->distinct.set, element));
    if (error) {
        sequence->pipeline.state = ABORTED;
        return true;
    }
    return !added;
}

static bool operation_sort(Sequence* sequence, Operation* operation, int index, void* element) {
    Error error = attempt(list_add_last(operation->sort.list, element));
    if (error) {
        sequence->pipeline.state = ABORTED;
    } else {
        sequence->pipeline.state = BUFFERING;
        sequence->pipeline.operation_last_index = index;
    }
    return true;
}

static void execute_terminal_operation(Sequence* sequence, void* element) {
    TerminalOperation* operation = &sequence->pipeline.terminal_operation;
    switch (operation->type) {
        case FOR_EACH:    operation->for_each.consume(element);              break;
        case REDUCE:      operation_reduce(operation, element);              break;
        case COUNT:       operation->count.count++;                          break;
        case MAX:         operation_max_min(operation, element);             break;
        case MIN:         operation_max_min(operation, element);             break;
        case FIND:        operation_find(operation, element);                break;
        case ALL_MATCH:   operation_match(operation, element);               break;
        case ANY_MATCH:   operation_match(operation, element);               break;
        case NONE_MATCH:  operation_match(operation, element);               break;
        case TO_ARRAY:    operation_to_array(sequence, operation, element);  break;
    }
}

static void operation_reduce(TerminalOperation* operation, void* element) {
    auto reduce = &operation->reduce;
    reduce->result = reduce->accumulate(reduce->result, element);
}

static void operation_max_min(TerminalOperation* operation, void* element) {
    auto max_min = &operation->max_min;
    if (!max_min->element) {
        max_min->element = element;
        max_min->found = true;
    } else if (operation->type == MAX && max_min->compare(max_min->element, element) <= 0) {
        max_min->element = element;
    } else if (operation->type == MIN && max_min->compare(max_min->element, element) >= 0) {
        max_min->element = element;
    }
}

static void operation_find(TerminalOperation* operation, void* element) {
    auto find = &operation->find;
    if (!find->found) {
        find->element = element;
        find->found = true;
    }
}

static void operation_match(TerminalOperation* operation, void* element) {
    auto match = &operation->match;
    match->count++;
    if (match->test(element)) {
        match->matches++;
    }
}

static void operation_to_array(Sequence* sequence, TerminalOperation* operation, void* element) {
    Error error = attempt(list_add_last(operation->to_array.list, element));
    if (error) {
        sequence->pipeline.state = ABORTED;
    }
}

typedef struct {
    Iterator iterator;
    void* seed;
    int count;
    int limit;
    Predicate test;
    Supplier get;
    Operator operate;
    Array(void*) elements;
} IterationContext;

static Iterator* create_iterator(IterationContextOptions options) {
    IterationContext* iteration_context = memory_try_alloc(sizeof(IterationContext));

    if (!iteration_context) {
        return nullptr;
    }
    iteration_context->iterator = (Iterator) {
        .iteration_context = iteration_context,
        .has_next = iterator_has_next_internal,
        .next = iterator_next_internal,
        .destroy = iterator_destroy_internal
    };

    iteration_context->seed = options.seed;
    iteration_context->count = 0;
    iteration_context->limit = options.limit;
    iteration_context->test = options.test;
    iteration_context->get = options.get;
    iteration_context->operate = options.operate;
    iteration_context->elements = options.elements;

    return &iteration_context->iterator;
}

static bool iterator_has_next_internal(const void* raw_iteration_context) {
    const IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->test) {
        return iteration_context->test(iteration_context->seed);
    }
    return iteration_context->count < iteration_context->limit;
}

static void* iterator_next_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->get) {
        iteration_context->count++;
        return iteration_context->get();
    }
    if (iteration_context->operate) {
        void* element = iteration_context->seed;
        iteration_context->seed = iteration_context->operate(iteration_context->seed);
        return element;
    }
    return iteration_context->elements[iteration_context->count++];
}

static void iterator_destroy_internal(void* raw_iteration_context) {
    IterationContext* iteration_context = raw_iteration_context;
    if (iteration_context->elements) {
        array_destroy(&iteration_context->elements);
    }
    memory_dealloc(iteration_context);
}