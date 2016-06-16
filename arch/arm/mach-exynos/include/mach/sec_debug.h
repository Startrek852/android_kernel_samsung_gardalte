/*
 * Copyright (c) 2012-2013 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com
 *
 * sec_debug.h - Samsung mobile debugging features
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef SEC_DEBUG_H
#define SEC_DEBUG_H

#include <linux/sched.h>
#include <linux/semaphore.h>

#if defined(CONFIG_SEC_DEBUG)

union sec_debug_level_t {
	struct {
		u16 kernel_fault;
		u16 user_fault;
	} en;
	u32 uint_val;
};

extern union sec_debug_level_t sec_debug_level;

extern unsigned int get_sec_debug_level(void);
extern int __init sec_debug_init(void);
extern int __init sec_debug_magic_init(void);
extern void sec_debug_set_upload_magic(unsigned magic, char *str);
extern void sec_debug_check_crash_key(unsigned int code, int value);
extern void sec_getlog_supply_fbinfo(void *p_fb, u32 res_x, u32 res_y, u32 bpp,
				     u32 frames);
extern void sec_getlog_supply_loggerinfo(void *p_main, void *p_radio,
					 void *p_events, void *p_system);
extern void sec_getlog_supply_kloginfo(void *klog_buf);
extern void sec_gaf_supply_rqinfo(unsigned short curr_offset,
				  unsigned short rq_offset);
extern void register_log_char_hook(void (*f) (char c));

#else

extern void register_log_char_hook(void (*f) (char c));

static unsigned int get_sec_debug_level(void)
{
	return 0;
}
static inline int __init sec_debug_init(void)
{
	return 0;
}
static inline int __init sec_debug_magic_init(void)
{
	return 0;
}
static inline void sec_debug_set_upload_magic(unsigned magic, char *str)
{
}
static inline void sec_debug_check_crash_key(unsigned int code, int value)
{
}
static inline void sec_getlog_supply_fbinfo(void *p_fb, u32 res_x, u32 res_y,
					    u32 bpp, u32 frames)
{
}

static inline void sec_getlog_supply_meminfo(u32 size0, u32 addr0, u32 size1,
					     u32 addr1)
{
}

static inline void sec_getlog_supply_loggerinfo(void *p_main,
						void *p_radio, void *p_events,
						void *p_system)
{
}

static inline void sec_getlog_supply_kloginfo(void *klog_buf)
{
}

static inline void sec_gaf_supply_rqinfo(unsigned short curr_offset,
					 unsigned short rq_offset)
{
}

#endif	/* CONFIG_SEC_DEBUG */

#ifdef CONFIG_SEC_DEBUG_SCHED_LOG
struct worker;
struct work_struct;

extern void __sec_debug_task_log(int cpu, struct task_struct *task, char *msg);
extern void __sec_debug_irq_log(unsigned int irq, void *fn, int en);
extern void __sec_debug_work_log(struct worker *worker,
				 struct work_struct *work, work_func_t f, int en);
#ifdef CONFIG_SEC_DEBUG_I2C_LOG
extern void __sec_debug_i2c_log(int cpu, const char *func, int en, unsigned long reg);
#endif
#ifdef CONFIG_SEC_DEBUG_PM_LOG
extern void __sec_debug_pm_log(int cpu, const char *dev_name, int en);
#endif
static inline void sec_debug_task_log(int cpu, struct task_struct *task)
{
	if (unlikely(sec_debug_level.en.kernel_fault))
		__sec_debug_task_log(cpu, task, NULL);
}

static inline void sec_debug_task_log_msg(int cpu, char *msg)
{
	if (unlikely(sec_debug_level.en.kernel_fault))
		__sec_debug_task_log(cpu, NULL, msg);
}

static inline void sec_debug_irq_log(unsigned int irq, void *fn, int en)
{
	if (unlikely(sec_debug_level.en.kernel_fault))
		__sec_debug_irq_log(irq, fn, en);
}

static inline void sec_debug_work_log(struct worker *worker,
				      struct work_struct *work, work_func_t f, int en)
{
	if (unlikely(sec_debug_level.en.kernel_fault))
		__sec_debug_work_log(worker, work, f, en);
}
#ifdef CONFIG_SEC_DEBUG_I2C_LOG
static inline void sec_debug_i2c_log(int cpu, const char *func, int en, unsigned long reg)
{
	__sec_debug_i2c_log(cpu, func, en, reg);
}
#endif
#ifdef CONFIG_SEC_DEBUG_PM_LOG
static inline void sec_debug_pm_log(int cpu, const char *dev_name, int en)
{
	__sec_debug_pm_log(cpu, dev_name, en);
}
#endif
#ifdef CONFIG_SEC_DEBUG_SOFTIRQ_LOG
static inline void sec_debug_softirq_log(unsigned int irq, void *fn, int en)
{
	if (unlikely(sec_debug_level.en.kernel_fault))
		__sec_debug_irq_log(irq, fn, en);
}
#else
static inline void sec_debug_softirq_log(unsigned int irq, void *fn, int en)
{
}
#endif	/* CONFIG_SEC_DEBUG_SOFTIRQ_LOG */
#else
struct worker;

static inline void sec_debug_task_log(int cpu, struct task_struct *task)
{
}

static inline void sec_debug_task_log_msg(int cpu, char *msg)
{
}

static inline void sec_debug_irq_log(unsigned int irq, void *fn, int en)
{
}

static inline void sec_debug_work_log(struct worker *worker,
				      struct work_struct *work, work_func_t f)
{
}
#ifdef CONFIG_SEC_DEBUG_I2C_LOG
static inline void sec_debug_i2c_log(int cpu, const char *func, int en, unsigned long reg)
{
}
#endif
#ifdef CONFIG_SEC_DEBUG_PM_LOG
static inline void sec_debug_pm_log(int cpu, const char *dev_name, int en)
{
}
#endif
static inline void sec_debug_softirq_log(unsigned int irq, void *fn, int en)
{
}
#endif	/* CONFIG_SEC_DEBUG_SCHED_LOG */

#ifdef CONFIG_SEC_DEBUG_SEMAPHORE_LOG
extern void debug_semaphore_init(void);
extern void debug_semaphore_down_log(struct semaphore *sem);
extern void debug_semaphore_up_log(struct semaphore *sem);
extern void debug_rwsemaphore_init(void);
extern void debug_rwsemaphore_down_log(struct rw_semaphore *sem, int dir);
extern void debug_rwsemaphore_up_log(struct rw_semaphore *sem);
#define debug_rwsemaphore_down_read_log(x) \
	debug_rwsemaphore_down_log(x, READ_SEM)
#define debug_rwsemaphore_down_write_log(x) \
	debug_rwsemaphore_down_log(x, WRITE_SEM)
#else
static inline void debug_semaphore_init(void)
{
}

static inline void debug_semaphore_down_log(struct semaphore *sem)
{
}

static inline void debug_semaphore_up_log(struct semaphore *sem)
{
}

static inline void debug_rwsemaphore_init(void)
{
}

static inline void debug_rwsemaphore_down_read_log(struct rw_semaphore *sem)
{
}

static inline void debug_rwsemaphore_down_write_log(struct rw_semaphore *sem)
{
}

static inline void debug_rwsemaphore_up_log(struct rw_semaphore *sem)
{
}
#endif	/* CONFIG_SEC_DEBUG_SEMAPHORE_LOG */

#ifdef CONFIG_SEC_DEBUG_AUXILIARY_LOG
extern void sec_debug_aux_log(int idx, char *fmt, ...);
#else
#define sec_debug_aux_log(idx, ...) do { } while (0)
#endif	/* CONFIG_SEC_DEBUG_AUXILIARY_LOG */

enum sec_debug_aux_log_idx {
	SEC_DEBUG_AUXLOG_CPU_BUS_CLOCK_CHANGE,
	SEC_DEBUG_AUXLOG_LOGBUF_LOCK_CHANGE,
	SEC_DEBUG_AUXLOG_DVFS_LOCK_CHANGE,
	SEC_DEBUG_AUXLOG_ITEM_MAX,
};

#endif	/* SEC_DEBUG_H */
