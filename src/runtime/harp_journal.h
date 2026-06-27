#ifndef RUNTIME_JOURNAL_H
#define RUNTIME_JOURNAL_H

#include <harp/harp_core.h>

#include "harp_runtime.h"


typedef struct HarpJournalEntry {
    HarpName name;
    uint8_t is_actor;
} HarpJournalEntry;


typedef struct HarpJournal {
    HarpRuntime *runtime;

    HarpJournalEntry *entries;
    uint64_t count;
    uint64_t capacity;
} HarpJournal;


HarpResult harp_setup_journal(HarpJournal *journal, HarpRuntime *runtime);
void harp_teardown_journal(HarpJournal *journal);

HarpResult harp_journal_add(HarpJournal *journal, HarpName name, uint8_t is_actor);
HarpResult harp_journal_remove(HarpJournal *journal, HarpName name);


#endif /* RUNTIME_JOURNAL_H */