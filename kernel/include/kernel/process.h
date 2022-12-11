#include <kernel/x86.h>

typedef struct process {
	pid_t         id;                /* Process ID (pid) */
	char*        name;              /* Process Name */
	char*        description;       /* Process description */
	user_t        user;              /* Effective user */
	group_t       group;             /* Process scheduling group */
	thread_t      thread;            /* Associated task information */
	fs_node_t *   wd_node;           /* Working directory VFS node */
	char *        wd_name;           /* Working directory path name */
	fd_table_t *  fds;               /* File descriptor table */
	status_t      status;            /* Process status */
	sig_table_t   signals;           /* Signal table */
	uint8_t       finished : 2;      /* Status indicator */
	uint8_t       started  : 2;
	registers_t* syscall_registers; /* Registers at interrupt */
	void *      wait_queue;
} process_t;

volatile process_t * current_process;