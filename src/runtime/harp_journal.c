#include "harp_journal.h"
#include "harp_runtime.h"

#include <harp/utils/harp_helpers.h>

#include <stdlib.h>
#include <string.h>


HarpResult harp_setup_journal(HarpJournal *journal, HarpRuntime *runtime) {
    if(journal == NULL || runtime == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    journal->capacity = 16;
    journal->entries = malloc(journal->capacity * sizeof(HarpJournalEntry));
    if(journal->entries == NULL)
        return HARP_RESULT_OUT_OF_MEMORY;

    journal->runtime = runtime;

    return HARP_RESULT_OK;
}
void harp_teardown_journal(HarpJournal *journal) {
    if(journal == NULL)
        return;

    while(journal->count > 0) {
        harp_journal_remove(journal, journal->entries[journal->count - 1].name);
    }

    free(journal->entries);
    journal->capacity = 0;
    journal->count = 0;
    journal->runtime = NULL;
}

HarpResult harp_journal_add(HarpJournal *journal, HarpName name, uint8_t is_actor) {
    if(journal == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    for(uint64_t i = 0; i < journal->count; ++i) {
        if(journal->entries[i].name == name)
            return HARP_RESULT_NAME_EXISTS;
    }

    if(journal->count+1 >= journal->capacity) {
        uint64_t tmp_capacity = journal->capacity * 2;
        void *tmp = realloc(journal->entries, sizeof(HarpJournalEntry) * tmp_capacity);
        if(tmp == NULL)
            return HARP_RESULT_OUT_OF_MEMORY;

        journal->capacity = tmp_capacity;
        journal->entries = tmp;
    }

    journal->entries[journal->count].name = name;
    journal->entries[journal->count].is_actor = is_actor;
    ++journal->count;

    return HARP_RESULT_OK;
}
HarpResult harp_journal_remove(HarpJournal *journal, HarpName name) {
    if(journal == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    uint64_t id = 0;
    for(; id < journal->count; ++id) {
        if(journal->entries[id].name == name)
            break;
    }
    if(id == journal->count)
        return HARP_RESULT_NAME_NOT_FOUND;

    if(journal->entries[id].is_actor) { // Splitting responsability of memory between runtime and journal, I do not like it.
        HarpRuntimeActor *r_actor = harp_registry_get_runtime(&journal->runtime->registry, name, HARP_REGISTRY_ENTRY_TYPE_ACTOR);
        if(r_actor == NULL)
            return HARP_RESULT_CRITICAL_FAIL;

        harp_teardown_ract(r_actor);
    } else {
        HarpRuntimeHandler *rhdl =
            harp_registry_get_runtime(&journal->runtime->registry, journal->entries[id].name, HARP_REGISTRY_ENTRY_TYPE_HANDLER);
        
        HARP_CHECK_CRITICAL(rhdl != NULL);
        harp_rhdl_terminate(rhdl, journal->runtime);
    }

    --journal->count;
    if(id < journal->count) {
        memmove(
            &journal->entries[id],
            &journal->entries[id+1],
            (journal->count - id) * sizeof(HarpJournalEntry)
        );
    }

    return HARP_RESULT_OK;
}
