// Communication thread header
#ifndef _COMMTHREAD_H_
#define _COMMTHREAD_H_

#include <mpi.h>
#include <unordered_set>

#include "chameleon.h"
#include "chameleon_common.h"
#include "chameleon_tools.h"
#include "request_manager.h"

// communicator for remote task requests
extern MPI_Comm chameleon_comm;
// communicator for sending back mapped values
extern MPI_Comm chameleon_comm_mapped;
// communicator for load information
extern MPI_Comm chameleon_comm_load;
// communicator for task cancellation
extern MPI_Comm chameleon_comm_cancel;

extern RequestManager request_manager_send;
extern RequestManager request_manager_receive;
extern RequestManager request_manager_cancel;

extern int chameleon_comm_rank;
extern int chameleon_comm_size;

extern std::vector<intptr_t> _image_base_addresses;

// list with local task entries
// these can either be executed locally or offloaded/replicated to a different rank
extern thread_safe_task_list_t _local_tasks;
extern std::atomic<int32_t> _num_local_tasks_outstanding;

// list with stolen task entries that should be executed
extern thread_safe_task_list_t _stolen_remote_tasks;
extern std::atomic<int32_t> _num_remote_tasks_outstanding;

// list of replicated (i.e. offloaded) tasks
// they can be executed either on the remote rank or on the local rank
extern thread_safe_task_list_t _replicated_local_tasks;
extern std::atomic<int32_t> _num_replicated_local_tasks_outstanding;

//extern std::atomic<int32_t> _num_outstanding_comm_requests;

extern thread_safe_task_list_t _replicated_remote_tasks;
extern std::atomic<int32_t> _num_replicated_remote_tasks_outstanding;

// list with stolen task entries that need output data transfer
extern thread_safe_task_list_t _remote_tasks_send_back;
// list with replicated task entries that need initial transfer
extern thread_safe_list_t<cham_t_replication_info_t*> _replication_infos_list;

// map that maps tag ids back to local tasks that have been offloaded and expect result data
extern thread_safe_task_map_t _map_offloaded_tasks_with_outputs;
// map that maps tag ids back to stolen tasks
extern thread_safe_task_map_t _map_tag_to_remote_task;
// mapping of all active task ids and task
extern thread_safe_task_map_t _map_overall_tasks;

extern std::unordered_set<TYPE_TASK_ID> _cancelled_task_ids;

// for now use a single mutex for box info
extern std::mutex _mtx_load_exchange;
// ====== Info about outstanding jobs (local & stolen & offloaded (communication)) ======
extern std::vector<int32_t> _outstanding_jobs_ranks;
extern std::atomic<int32_t> _outstanding_jobs_local;
extern std::atomic<int32_t> _outstanding_jobs_sum;
// ====== Info about real load that is open or is beeing processed ======
extern std::vector<int32_t> _load_info_ranks;

// list that holds task ids (created at the current rank) that are not finsihed yet
extern thread_safe_list_t<TYPE_TASK_ID> _unfinished_locally_created_tasks;

// Threading section
extern std::atomic<int> _comm_thread_load_exchange_happend;

// variables to indicate when it is save to break out of taskwait
extern std::mutex _mtx_taskwait;
extern std::atomic<int> _num_threads_involved_in_taskwait;
extern std::atomic<int> _num_threads_active_in_taskwait;
extern std::atomic<int32_t> _num_threads_idle;
extern std::atomic<int> _num_ranks_not_completely_idle;

// number of active migrations per target rank
// desired: should block new migration to target as long as there are still active migrations ongoing
extern std::vector<int> _active_migrations_per_target_rank;

#ifdef __cplusplus
extern "C" {
#endif

void print_arg_info(std::string prefix, cham_migratable_task_t *task, int idx);

void print_arg_info_w_tgt(std::string prefix, cham_migratable_task_t *task, int idx);

void cancel_offloaded_task(cham_migratable_task_t *task);

void cancel_offloaded_task_on_rank(cham_migratable_task_t *task, int rank);

void* comm_thread_action(void *arg);

int32_t start_communication_threads();

int32_t stop_communication_threads();

int32_t put_comm_threads_to_sleep();

void trigger_update_outstanding();

int exit_condition_met(int from_taskwait, int print);

#ifdef __cplusplus
}
#endif
#endif
