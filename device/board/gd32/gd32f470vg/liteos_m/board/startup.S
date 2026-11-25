/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * GD32F470VG (Cortex-M4) startup for OpenHarmony LiteOS-M.
 *
 * The LiteOS-M cortex-m4 kernel takes over the vector table at runtime
 * (HalHwiInit reassigns SCB->VTOR to its own g_hwiForm). This startup only
 * needs to: load the initial MSP, run Reset_Handler (copy .data, zero .bss,
 * call SystemInit, call main) and provide a vector table whose system
 * exceptions point at the kernel exception handlers so any early fault is
 * caught. External IRQs default to Default_Handler and are re-registered by
 * LOS_HwiCreate at runtime.
 */

    .syntax unified
    .cpu cortex-m4
    .fpu softvfp
    .thumb

    .global  Reset_Handler
    .global  Default_Handler
    .global  __gVectors

    .extern  SystemInit
    .extern  main
    /* LiteOS-M cortex-m4 exception handlers (defined in arch/arm/cortex-m4/gcc) */
    .extern  HalExcNMI
    .extern  HalExcHardFault
    .extern  HalExcMemFault
    .extern  HalExcBusFault
    .extern  HalExcUsageFault
    .extern  HalExcSvcCall
    .extern  HalPendSV
    .extern  SysTick_Handler

    .section  .text.Reset_Handler, "ax", %progbits
    .weak  Reset_Handler
    .type  Reset_Handler, %function
Reset_Handler:
    /* copy .data from flash to RAM */
    ldr  r0, =_sidata
    ldr  r1, =_sdata
    ldr  r2, =_edata
1:
    cmp  r1, r2
    bcc  2f
    b    3f
2:
    ldr  r3, [r0], #4
    str  r3, [r1], #4
    b    1b
3:
    /* zero the .bss section */
    ldr  r0, =_sbss
    ldr  r1, =_ebss
    mov  r2, #0
4:
    cmp  r0, r1
    bcc  5f
    b    6f
5:
    str  r2, [r0], #4
    b    4b
6:
    /* set up system clock & FPU, then enter main */
    bl   SystemInit
    bl   main
    b    Default_Handler
    .size  Reset_Handler, .-Reset_Handler

    .section  .text.Default_Handler, "ax", %progbits
    .weak  Default_Handler
    .type  Default_Handler, %function
Default_Handler:
7:
    b    7b
    .size  Default_Handler, .-Default_Handler

    .section  .vectors, "a", %progbits
    .type  __gVectors, %object
__gVectors:
    .word  _sp                       /* 0  Initial stack pointer (MSP)      */
    .word  Reset_Handler             /* 1  Reset                            */
    .word  HalExcNMI                 /* 2  NMI                              */
    .word  HalExcHardFault           /* 3  HardFault                        */
    .word  HalExcMemFault            /* 4  MemManage                        */
    .word  HalExcBusFault            /* 5  BusFault                         */
    .word  HalExcUsageFault          /* 6  UsageFault                       */
    .word  0                         /* 7  Reserved                         */
    .word  0                         /* 8  Reserved                         */
    .word  0                         /* 9  Reserved                         */
    .word  0                         /* 10 Reserved                         */
    .word  HalExcSvcCall             /* 11 SVCall                           */
    .word  Default_Handler           /* 12 Debug monitor                    */
    .word  0                         /* 13 Reserved                         */
    .word  HalPendSV                 /* 14 PendSV                           */
    .word  SysTick_Handler           /* 15 SysTick                          */

    /* External interrupts 0..90 (WWDGT_IRQn .. IPA_IRQn). The kernel
     * registers real handlers via LOS_HwiCreate at runtime. */
    .rept  91
    .word  Default_Handler
    .endr
    .size  __gVectors, . - __gVectors
