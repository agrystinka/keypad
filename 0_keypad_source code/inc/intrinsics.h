/**
 * libsk intrinsics
 *
 * We lack some intrinsic functions usually found in CMSIS. So provide our own with
 * compatible names
 */

/** WFI - Wait For Interrupt */
inline __attribute__((always_inline)) void __WFI(void)
{
	__asm__ volatile ("wfi");
}


/** WFE - Wait For Event */
inline __attribute__((always_inline)) void __WFE(void)
{
	__asm__ volatile ("wfe");
}


/** DMB - Data Memory Barrier
 *
 * The DMB instruction that all explicit data memory transfers before the DMB are completed
 * before any subsequent data memory transfers after the DMB starts
 */
inline __attribute__((always_inline)) void __DMB(void)
{
	__asm__ volatile ("dmb" ::: "memory");
}


/** DSB - Data Synchronization Barrier
 *
 * The DSB instruction ensures all explicit data transfers before the DSB are complete before any
 * instruction after the DSB is executed
 */
inline __attribute__((always_inline)) void __DSB(void)
{
	__asm__ volatile ("dsb" ::: "memory");
}
