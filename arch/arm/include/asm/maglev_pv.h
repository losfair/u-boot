#ifndef __ASM_ARM_MAGLEV_PV_H
#define __ASM_ARM_MAGLEV_PV_H

#ifdef CONFIG_MAGLEV_PV

// seL4 mechanisms
#define seL4_SysCall -1

static inline void sel4_arm_sys_send_recv(long sys, unsigned long dest, unsigned long *out_badge, unsigned long info_arg,
                                          unsigned long *out_info, unsigned long *in_out_mr0, unsigned long *in_out_mr1, unsigned long *in_out_mr2, unsigned long *in_out_mr3,
                                          __attribute__((unused)) unsigned long reply)
{
  register unsigned long destptr asm("r0") = dest;
  register unsigned long info asm("r1") = info_arg;

  /* Load beginning of the message into registers. */
  register unsigned long msg0 asm("r2") = *in_out_mr0;
  register unsigned long msg1 asm("r3") = *in_out_mr1;
  register unsigned long msg2 asm("r4") = *in_out_mr2;
  register unsigned long msg3 asm("r5") = *in_out_mr3;

  /* Perform the system call. */
  register unsigned long scno asm("r7") = sys;
  asm volatile(
      "swi #0"
      : "+r"(msg0), "+r"(msg1), "+r"(msg2), "+r"(msg3),
        "+r"(info), "+r"(destptr)
      : "r"(scno)
      : "memory");
  *out_info = info;
  *out_badge = destptr;
  *in_out_mr0 = msg0;
  *in_out_mr1 = msg1;
  *in_out_mr2 = msg2;
  *in_out_mr3 = msg3;
}

// Maglev services
#define MAGLEV_VM_IPC_BUFFER ((void *) (0xc0802000ul))
#define MAGLEV_VCPU_CAP_TIMER_ENDPOINT 2
#define MAGLEV_VCPU_CAP_CUSTOM_START 0x20

#define MAGLEV_VCPU_CTLCAP_ENET2_IPC (MAGLEV_VCPU_CAP_CUSTOM_START + 0x00)

#endif /* CONFIG_MAGLEV_PV */

#endif
