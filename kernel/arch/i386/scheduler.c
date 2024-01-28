#include <kernel/kmalloc.h>
#include <kernel/paging.h>
#include <kernel/proc.h>
#include <kernel/x86.h>
#include <stdlib.h>
#include <string.h>

typedef struct _proc_node_t {
  process_t* process;
  struct _proc_node_t* next;
} proc_node_t;

typedef struct {
  scheduler_t sched;
  proc_node_t* processes;
} scheduler_medusaos_t;

process_t* pqueue;
process_t* current_proc;

volatile void switch_task(registers_t* regs) {
  memcpy(&current_proc->regs, regs, sizeof(registers_t));
  if (current_proc->next != NULL) {
    current_proc = current_proc->next;
  } else {
    current_proc = pqueue;
  }
  memcpy(regs, &current_proc->regs, sizeof(registers_t));
  paging_switch_directory(current_proc->directory);
}

/*

process_t* scheduler_get_current(scheduler_t* sched){
    scheduler_medusaos_t* sc = (scheduler_medusaos_t*)sched;
    if (sc->processes) {
        return sc->processes->process;
    }

    return NULL;
}

void scheduler_add(scheduler_t* sched, process_t* new_process) {
    scheduler_medusaos_t* sc = (scheduler_medusaos_t*) sched;
    proc_node_t* new = kmalloc(sizeof(proc_node_t));

    new->process = new_process;

    // Insert the process in the ring, create it if empty
    if (!sc->processes) {
        new->next = new;
        sc->processes = new;
    } else {
        proc_node_t* p = sc->processes->next;
        sc->processes->next = new;
        new->next = p;
    }
}

process_t* scheduler_next(scheduler_t* sched) {
    scheduler_medusaos_t* sc = (scheduler_medusaos_t*) sched;
    proc_node_t* p = sc->processes;

    // Avoid switching to a sleeping process if possible
    do {
        if (p->next->process->sleep_ticks > 0) {
            p->next->process->sleep_ticks--;
        } else {
            // We don't need to switch process
            if (p->next == sc->processes) {
                return sc->processes->process;
            }

            // We don't need to modify the process queue
            if (p->next == sc->processes->next) {
                break;
            }

            // We insert the next process between the current one and the one
            // previously scheduled to be switched to.
            proc_node_t* previous = p;
            proc_node_t* next_proc = p->next;
            proc_node_t* moved = sc->processes->next;

            previous->next = next_proc->next;
            next_proc->next = moved;
            sc->processes->next = next_proc;

            break;
        }

        p = p->next;
    } while (p != sc->processes);

    sc->processes = sc->processes->next;

    return sc->processes->process;
}

void scheduler_exit(scheduler_t* sched, process_t* process) {
    scheduler_medusaos_t* sc = (scheduler_medusaos_t*) sched;
    proc_node_t* p = sc->processes;

    if (sc->processes == sc->processes->next) {
        abort();
    }

    while (p->next->process != process) {
        p = p->next;
    }

    proc_node_t* to_remove = p->next;
    p->next = p->next->next;

    sc->processes = p;

    kfree(to_remove);
}

// Allocates a scheduler.
scheduler_t* scheduler_medusaos() {
    scheduler_medusaos_t* sched = kmalloc(sizeof(scheduler_medusaos_t));

    sched->sched = (scheduler_t) {
        .sched_get_current = scheduler_get_current,
        .sched_add = scheduler_add,
        .sched_next = scheduler_next,
        .sched_exit = scheduler_exit
    };

    sched->processes = NULL;

    return (scheduler_t*) sched;
}*/