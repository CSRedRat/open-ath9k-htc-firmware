#include <stdarg.h>
#include <adf_os_types.h>
#include <adf_os_dma.h>
#include <adf_os_timer.h>
#include <adf_os_lock.h>
#include <adf_os_io.h>
#include <adf_os_mem.h>
#include <adf_os_module.h>
#include <adf_os_pci.h>
#include <adf_os_util.h>
#include <adf_os_stdtypes.h>
#include <adf_os_defer.h>
#include <adf_os_atomic.h>
#include <adf_nbuf.h>
#include <adf_net.h>
#include <adf_net_types.h>

#include "ah.h"
#include<ah_internal.h>
#include "ah_osdep.h"

a_uint32_t __ahdecl
ath_hal_getuptime(struct ath_hal *ah)
{
	return adf_os_getuptime();
}

struct ath_hal *
_ath_hal_attach_tgt(a_uint32_t devid, HAL_SOFTC sc,
		    adf_os_device_t dev,HAL_BUS_HANDLE sh, a_uint32_t flags, void* s)
{
	HAL_STATUS status;
	struct ath_hal *ah = ath_hal_attach_tgt(devid,sc,dev,sh, flags, &status);
	adf_os_print(" ath_hal = %p \n",ah);
	*(HAL_STATUS *)s = status;
	return ah;
}

void
ath_hal_detach(struct ath_hal *ah)
{
	(*ah->ah_detach)(ah);
}

extern void *global_hdl;

/*
 * Memory-mapped device register read/write.  These are here
 * as routines when debugging support is enabled and/or when
 * explicitly configured to use function calls.  The latter is
 * for architectures that might need to do something before
 * referencing memory (e.g. remap an i/o window).
 *
 * NB: see the comments in ah_osdep.h about byte-swapping register
 *     reads and writes to understand what's going on below.
 */
void __ahdecl
ath_hal_reg_write_target(struct ath_hal *ah, a_uint32_t reg, a_uint32_t val)
{ 
	adf_os_reg_write32(ah->ah_dev, reg, val); 
}

a_uint32_t __ahdecl
ath_hal_reg_read_target(struct ath_hal *ah, a_uint32_t reg)
{
    	a_uint32_t val;

	val = adf_os_reg_read32(ah->ah_dev, reg);

	return val;
}

/*
 * Delay n microseconds.
 */
void __ahdecl
ath_hal_delay(a_int32_t n)
{
	adf_os_udelay(n);
}

/*
 * Allocate/free memory.
 */
void * __ahdecl
ath_hal_malloc(adf_os_size_t size)
{
	void *p;

	p = adf_os_mem_alloc(size);
	if (p)
		adf_os_mem_zero(p, size);

	return p;
}

void __ahdecl
ath_hal_free(void* p)
{
	adf_os_mem_free(p);
}

void __ahdecl
ath_hal_memzero(void *dst, adf_os_size_t n)
{
	adf_os_mem_set(dst, 0, n);
}

void * __ahdecl
ath_hal_memcpy(void *dst, void *src, adf_os_size_t n)
{
	adf_os_mem_copy(dst, src, n);
	return 0;
}

/*
 * Print/log message support.
 */
void __ahdecl
ath_hal_vprintf(struct ath_hal *ah, const char* fmt, va_list ap)
{
}

enum {
	DEV_ATH     = 9,            /* XXX must match driver */
};

adf_os_module_dep(hal, adf_net);
adf_os_module_dep(hal, hal);
adf_os_virt_module_name(hal);
