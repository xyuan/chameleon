#ifndef _CHAMELEON_TOOLS_H_
#define _CHAMELEON_TOOLS_H_

// #include "chameleon_common.h"
#include <stddef.h>
#include <stdint.h>
#include <dlfcn.h>

#pragma region Enums and Definitions
/*****************************************************************************
 * Enums
 ****************************************************************************/
typedef enum cham_t_callback_types_t {
    cham_t_callback_thread_init                 = 1,
    cham_t_callback_thread_finalize             = 2,
    cham_t_callback_task_create                 = 3,
    cham_t_callback_encode_task_tool_data       = 4,
    cham_t_callback_decode_task_tool_data       = 5,
    cham_t_callback_task_schedule               = 6,
    cham_t_callback_sync_region                 = 7
    // cham_t_callback_implicit_task            = 7,
    // cham_t_callback_target                   = 8,
    // cham_t_callback_target_data_op           = 9,
    // cham_t_callback_target_submit            = 10,
    // cham_t_callback_control_tool             = 11,
    // cham_t_callback_device_initialize        = 12,
    // cham_t_callback_device_finalize          = 13,
    // cham_t_callback_device_load              = 14,
    // cham_t_callback_device_unload            = 15,
    // cham_t_callback_sync_region_wait         = 16,
    // cham_t_callback_mutex_released           = 17,
    // cham_t_callback_dependences              = 18,
    // cham_t_callback_task_dependence          = 19,
    // cham_t_callback_work                     = 20,
    // cham_t_callback_master                   = 21,
    // cham_t_callback_target_map               = 22,
    // cham_t_callback_sync_region              = 23,
    // cham_t_callback_lock_init                = 24,
    // cham_t_callback_lock_destroy             = 25,
    // cham_t_callback_mutex_acquire            = 26,
    // cham_t_callback_mutex_acquired           = 27,
    // cham_t_callback_nest_lock                = 28,
    // cham_t_callback_flush                    = 29,
    // cham_t_callback_cancel                   = 30,
    // cham_t_callback_reduction                = 31,
    // cham_t_callback_dispatch                 = 32
} cham_t_callback_types_t;

typedef enum cham_t_set_result_t {
    cham_t_set_error            = 0,
    cham_t_set_never            = 1,
    cham_t_set_impossible       = 2,
    cham_t_set_sometimes        = 3,
    cham_t_set_sometimes_paired = 4,
    cham_t_set_always           = 5
} cham_t_set_result_t;

typedef enum cham_t_task_schedule_type_t {
    cham_t_task_start           = 1,
    cham_t_task_yield           = 2,
    cham_t_task_end             = 3,
    cham_t_task_cancel          = 4
} cham_t_task_schedule_type_t;

static const char* cham_t_task_schedule_type_t_values[] = {
    NULL,
    "cham_t_task_start",        // 1
    "cham_t_task_yield",        // 2
    "cham_t_task_end",          // 3
    "cham_t_task_cancel"        // 4
};

typedef enum cham_t_task_flag_t {
    cham_t_task_local           = 0x00000001,
    cham_t_task_remote          = 0x00000002,
    cham_t_task_replicated      = 0x00000004
} cham_t_task_flag_t;

static void cham_t_task_flag_t_value(int type, char *buffer) {
  char *progress = buffer;
  if (type & cham_t_task_local)
    progress += sprintf(progress, "cham_t_task_local");
  if (type & cham_t_task_remote)
    progress += sprintf(progress, "cham_t_task_remote");
  if (type & cham_t_task_replicated)
    progress += sprintf(progress, "cham_t_task_replicated");
}

typedef enum cham_t_sync_region_type_t {
    cham_t_sync_region_taskwait  = 1
} cham_t_sync_region_type_t;

static const char* cham_t_sync_region_type_t_values[] = {
    NULL,
    "cham_t_sync_region_taskwait"       // 1
};

typedef enum cham_t_sync_region_status_t {
    cham_t_sync_region_start    = 1,
    cham_t_sync_region_end      = 2
} cham_t_sync_region_status_t;

static const char* cham_t_sync_region_status_t_values[] = {
    NULL,
    "cham_t_sync_region_start",         // 1
    "cham_t_sync_region_end"            // 2
};

typedef struct cham_t_rank_info_t {
    int32_t comm_rank;
    int32_t comm_size;
} cham_t_rank_info_t;

/*****************************************************************************
 * General definitions
 ****************************************************************************/
typedef void (*cham_t_interface_fn_t) (void);

typedef cham_t_interface_fn_t (*cham_t_function_lookup_t) (
    const char *interface_function_name
);

typedef void (*cham_t_callback_t) (void);

// either interprete data type as value or pointer
typedef union cham_t_data_t {
    uint64_t value;
    void *ptr;
} cham_t_data_t;

/*****************************************************************************
 * Init / Finalize / Start Tool
 ****************************************************************************/
typedef void (*cham_t_finalize_t) (
    cham_t_data_t *tool_data
);

typedef int (*cham_t_initialize_t) (
    cham_t_function_lookup_t lookup,
    cham_t_data_t *tool_data
);

typedef struct cham_t_start_tool_result_t {
    cham_t_initialize_t initialize;
    cham_t_finalize_t finalize;
    cham_t_data_t tool_data;
} cham_t_start_tool_result_t;

/*****************************************************************************
 * Getter / Setter
 ****************************************************************************/
typedef cham_t_set_result_t (*cham_t_set_callback_t) (
    cham_t_callback_types_t event,
    cham_t_callback_t callback
);

typedef int (*cham_t_get_callback_t) (
    cham_t_callback_types_t event,
    cham_t_callback_t *callback
);

typedef cham_t_data_t *(*cham_t_get_thread_data_t) (void);
typedef cham_t_data_t *(*cham_t_get_rank_data_t) (void);

typedef cham_t_rank_info_t *(*cham_t_get_rank_info_t) (void);

/*****************************************************************************
 * List of callbacks
 ****************************************************************************/
typedef void (*cham_t_callback_thread_init_t) (
    cham_t_data_t *thread_data
);

typedef void (*cham_t_callback_thread_finalize_t) (
    cham_t_data_t *thread_data
);

typedef void (*cham_t_callback_task_create_t) (
    TargetTaskEntryTy * task,                   // opaque data type for internal task
    cham_t_data_t *task_data,
    const void *codeptr_ra
);

typedef void (*cham_t_callback_task_schedule_t) (
    TargetTaskEntryTy * task,                   // opaque data type for internal task
    cham_t_task_flag_t task_flag,
    cham_t_data_t *task_data,
    cham_t_task_schedule_type_t schedule_type,
    TargetTaskEntryTy * prior_task,             // opaque data type for internal task
    cham_t_task_flag_t prior_task_flag,
    cham_t_data_t *prior_task_data
);

// Encode custom task tool data (if any has been set) for a task that will be migrated to remote rank.
// Ensures that this data is also send to remote rank and available in tool calls.
// Note: Only necessary when task specific data is required
// Note: Only works in combination with cham_t_callback_decode_task_tool_data_t
typedef void *(*cham_t_callback_encode_task_tool_data_t) (
    TargetTaskEntryTy * task,                   // opaque data type for internal task
    cham_t_data_t *task_data,
    int32_t *size
);

// Decode custom task tool data (if any has been set) for a task that has been migrated to remote rank.
// Restore data in corresponding task_data struct
// Note: Only necessary when task specific data is required
// Note: Only works in combination with cham_t_callback_encode_task_tool_data_t
typedef void (*cham_t_callback_decode_task_tool_data_t) (
    TargetTaskEntryTy * task,                   // opaque data type for internal task
    cham_t_data_t *task_data,
    void *buffer,
    int32_t size
);

typedef void (*cham_t_callback_sync_region_t) (
    cham_t_sync_region_type_t sync_region_type,
    cham_t_sync_region_status_t sync_region_status,
    cham_t_data_t *thread_data,
    const void *codeptr_ra
);

#pragma endregion

#endif