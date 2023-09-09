// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2012-2014
 *     Texas Instruments Incorporated, <www.ti.com>
 */

#include <common.h>
#include <init.h>
#include <time.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <div64.h>
#include <bootstage.h>
#include <asm/maglev_pv.h>

DECLARE_GLOBAL_DATA_PTR;

#ifndef CFG_SYS_HZ_CLOCK
static inline u32 read_cntfrq(void)
{
	u32 frq;

	asm volatile("mrc p15, 0, %0, c14, c0, 0" : "=r" (frq));
	return frq;
}
#endif

int timer_init(void)
{
#ifndef CONFIG_MAGLEV_PV

	gd->arch.tbl = 0;
	gd->arch.tbu = 0;

#ifdef CFG_SYS_HZ_CLOCK
	gd->arch.timer_rate_hz = CFG_SYS_HZ_CLOCK;
#else
	gd->arch.timer_rate_hz = read_cntfrq();
#endif

#endif

	return 0;
}

unsigned long long get_ticks(void)
{
#ifdef CONFIG_MAGLEV_PV
	return timer_get_boot_us();
#else
	ulong nowl, nowu;

	asm volatile("mrrc p15, 0, %0, %1, c14" : "=r" (nowl), "=r" (nowu));

	gd->arch.tbl = nowl;
	gd->arch.tbu = nowu;

	return (((unsigned long long)gd->arch.tbu) << 32) | gd->arch.tbl;
#endif
}


ulong timer_get_boot_us(void)
{
#ifdef CONFIG_MAGLEV_PV 
	unsigned long out_badge, out_info, mr0 = 0, mr1 = 0, mr2 = 0, mr3 = 0;
	sel4_arm_sys_send_recv(seL4_SysCall, MAGLEV_VCPU_CAP_TIMER_ENDPOINT, &out_badge, 1ul << 12, &out_info, &mr0, &mr1, &mr2, &mr3, 0);
	return (((unsigned long long)mr1 << 32) | mr0) * 1000;
#else
	if (!gd->arch.timer_rate_hz)
		timer_init();

	return lldiv(get_ticks(), gd->arch.timer_rate_hz / 1000000);
#endif
}

ulong get_tbclk(void)
{
#ifdef CONFIG_MAGLEV_PV
	return 1000000;
#else
	return gd->arch.timer_rate_hz;
#endif
}
