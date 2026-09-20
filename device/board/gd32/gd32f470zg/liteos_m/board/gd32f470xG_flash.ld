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
 * Linker script for GD32F470ZG + OpenHarmony LiteOS-M.
 *
 * Based on the GigaDevice CMSIS gd32f470xG_flash.ld (V3.3.2, firmware for
 * GD32F4xx), with LiteOS-M/OHOS adaptations layered on top. Memory map:
 *   Flash:  512K @ 0x08000000 (GD32F470ZG, xG = 512K)
 *   SRAM:   192K @ 0x20000000
 *   TCMRAM:  64K @ 0x10000000 (core-coupled TCM, single-cycle; opt-in via
 *            __attribute__((section(".tcmram"))) — see .tcmram note below)
 * Code executes in place from flash (XIP); .data is copied to SRAM by
 * Reset_Handler, .bss is zeroed. The 4K .stack region reserves the MSP;
 * the remainder of SRAM is the LiteOS-M kernel heap.
 *
 * Differences from the pristine GigaDevice reference, required by this
 * build (dropping any of these breaks the board):
 *   - .zinitcall output section + __zinitcall_<layer>_start/_end boundary
 *     symbols. OHOS_SystemInit() (core_main.h / bootstrap_service.h) walks
 *     these to run SYS_RUN / APP_FEATURE_INIT entries; KEEP() protects them
 *     from --gc-sections. Without this, the LED/helloworld apps never start.
 *   - __heap_start / __heap_size as the LiteOS-M kernel heap. target_config.h
 *     defines LOSCFG_SYS_HEAP_ADDR/SIZE as &__heap_start / &__heap_size, and
 *     __heap_size = RAM_SIZE - (__heap_start - RAM_START) so the heap absorbs
 *     all SRAM left after the 4K MSP stack.
 *   - Separate 4K .stack (MSP) + dynamic .heap, instead of the reference's
 *     fixed-size .heap_stack (1K heap + 2K stack default).
 *   - No trailing GROUP(libgcc.a libc.a libm.a libnosys.a): the GN build
 *     links libs itself (BUILD.gn: -nostdlib + nosys/gcc, or musl under
 *     LOSCFG_LIBC_NEWLIB); a linker GROUP() would pull newlib's libc.a,
 *     which is not built here (this is a musl build).
 * Kept from the reference: TCMRAM region + .tcmram section, .ARM.attributes
 * in flash, .preinit_array/.init_array/.fini_array, __Vectors_Size.
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(Reset_Handler)

SVC_IRQ_STACK_SIZE = 4K;
FLASH_START         = 0x08000000;
FLASH_SIZE          = 512K;
RAM_START           = 0x20000000;
RAM_SIZE            = 192K;
TCMRAM_START        = 0x10000000;
TCMRAM_SIZE         = 64K;

MEMORY
{
   FLASH  (rx)  : ORIGIN = FLASH_START,  LENGTH = FLASH_SIZE
   RAM    (rwx): ORIGIN = RAM_START,    LENGTH = RAM_SIZE
   TCMRAM (xrw): ORIGIN = TCMRAM_START, LENGTH = TCMRAM_SIZE
}

SECTIONS
{
  .vectors : {
      . = ALIGN(4);
      KEEP(*(.vectors))
      . = ALIGN(4);
      __Vectors_End = .;
      __Vectors_Size = __Vectors_End - __gVectors;
  } > FLASH

  .text : {
      . = ALIGN(4);
      __text_start = .;
      _stext = .;
      *(.text)
      *(.text*)
      *(.glue_7)
      *(.glue_7t)
      *(.eh_frame)
      KEEP (*(.init))
      KEEP (*(.fini))
      . = ALIGN(4);
      _etext = .;
  } > FLASH

  .rodata : {
      . = ALIGN(4);
      *(.rodata)
      *(.rodata*)
      *(.gnu.linkonce.r.*)
      . = ALIGN(4);
  } > FLASH

  .ARM.extab : {
      *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > FLASH

  .ARM : {
      __exidx_start = .;
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
      __exidx_end = .;
  } > FLASH

  /*
   * OHOS auto-init call table. OHOS_SystemInit() traverses these sections via
   * the __zinitcall_<layer>_start/_end boundary symbols (see core_main.h and
   * ohos_init.h). SYS_RUN entries land in the .zinitcall.run<N>.init input
   * sections. Function pointers are const, so the table lives in flash; KEEP()
   * prevents the enabled --gc-sections from discarding otherwise-unreferenced
   * entries. All eight standard layers are covered: bsp, device, core,
   * sys.service, sys.feature, run, app.service, app.feature.
   */
  .zinitcall : {
      . = ALIGN(4);
      __zinitcall_bsp_start = .;
      KEEP (*(.zinitcall.bsp0.init))
      KEEP (*(.zinitcall.bsp1.init))
      KEEP (*(.zinitcall.bsp2.init))
      KEEP (*(.zinitcall.bsp3.init))
      KEEP (*(.zinitcall.bsp4.init))
      __zinitcall_bsp_end = .;
      . = ALIGN(4);
      __zinitcall_device_start = .;
      KEEP (*(.zinitcall.device0.init))
      KEEP (*(.zinitcall.device1.init))
      KEEP (*(.zinitcall.device2.init))
      KEEP (*(.zinitcall.device3.init))
      KEEP (*(.zinitcall.device4.init))
      __zinitcall_device_end = .;
      . = ALIGN(4);
      __zinitcall_core_start = .;
      KEEP (*(.zinitcall.core0.init))
      KEEP (*(.zinitcall.core1.init))
      KEEP (*(.zinitcall.core2.init))
      KEEP (*(.zinitcall.core3.init))
      KEEP (*(.zinitcall.core4.init))
      __zinitcall_core_end = .;
      . = ALIGN(4);
      __zinitcall_sys_service_start = .;
      KEEP (*(.zinitcall.sys.service0.init))
      KEEP (*(.zinitcall.sys.service1.init))
      KEEP (*(.zinitcall.sys.service2.init))
      KEEP (*(.zinitcall.sys.service3.init))
      KEEP (*(.zinitcall.sys.service4.init))
      __zinitcall_sys_service_end = .;
      . = ALIGN(4);
      __zinitcall_sys_feature_start = .;
      KEEP (*(.zinitcall.sys.feature0.init))
      KEEP (*(.zinitcall.sys.feature1.init))
      KEEP (*(.zinitcall.sys.feature2.init))
      KEEP (*(.zinitcall.sys.feature3.init))
      KEEP (*(.zinitcall.sys.feature4.init))
      __zinitcall_sys_feature_end = .;
      . = ALIGN(4);
      __zinitcall_run_start = .;
      KEEP (*(.zinitcall.run0.init))
      KEEP (*(.zinitcall.run1.init))
      KEEP (*(.zinitcall.run2.init))
      KEEP (*(.zinitcall.run3.init))
      KEEP (*(.zinitcall.run4.init))
      __zinitcall_run_end = .;
      . = ALIGN(4);
      __zinitcall_app_service_start = .;
      KEEP (*(.zinitcall.app.service0.init))
      KEEP (*(.zinitcall.app.service1.init))
      KEEP (*(.zinitcall.app.service2.init))
      KEEP (*(.zinitcall.app.service3.init))
      KEEP (*(.zinitcall.app.service4.init))
      __zinitcall_app_service_end = .;
      . = ALIGN(4);
      __zinitcall_app_feature_start = .;
      KEEP (*(.zinitcall.app.feature0.init))
      KEEP (*(.zinitcall.app.feature1.init))
      KEEP (*(.zinitcall.app.feature2.init))
      KEEP (*(.zinitcall.app.feature3.init))
      KEEP (*(.zinitcall.app.feature4.init))
      __zinitcall_app_feature_end = .;
      . = ALIGN(4);
  } > FLASH

  /* ARM attributes */
  .ARM.attributes : {
      *(.ARM.attributes)
  } > FLASH

  .preinit_array : {
      PROVIDE_HIDDEN (__preinit_array_start = .);
      KEEP (*(.preinit_array*))
      PROVIDE_HIDDEN (__preinit_array_end = .);
  } > FLASH

  .init_array : {
      PROVIDE_HIDDEN (__init_array_start = .);
      KEEP (*(SORT(.init_array.*)))
      KEEP (*(.init_array*))
      PROVIDE_HIDDEN (__init_array_end = .);
  } > FLASH

  .fini_array : {
      PROVIDE_HIDDEN (__fini_array_start = .);
      KEEP (*(.fini_array*))
      KEEP (*(SORT(.fini_array.*)))
      PROVIDE_HIDDEN (__fini_array_end = .);
  } > FLASH

  /* .data is loaded after .rodata in flash but lives in RAM */
  _sidata = LOADADDR(.data);
  .data : {
      . = ALIGN(4);
      _sdata = .;
      __data_start = .;
      *(.data)
      *(.data*)
      *(.gnu.linkonce.d.*)
      . = ALIGN(4);
      _edata = .;
      __data_end = .;
  } > RAM AT > FLASH

  .bss (NOLOAD) : {
      . = ALIGN(4);
      _sbss = .;
      __bss_start = .;
      *(.dynbss)
      *(.bss)
      *(.bss*)
      *(.gnu.linkonce.b.*)
      *(COMMON)
      . = ALIGN(4);
      _ebss = .;
      __bss_end = .;
  } > RAM

  /* 4K MSP (interrupt/exception) stack */
  .stack (NOLOAD) : {
      . = ALIGN(8);
      _sstack = .;
      __irq_stack_bottom = .;
      . += SVC_IRQ_STACK_SIZE;
      __irq_stack_top = .;
      _sp = .;
      _estack = .;
  } > RAM

  /* LiteOS-M kernel heap: remainder of SRAM */
  .heap (NOLOAD) : {
      . = ALIGN(0x40);
      __heap_start = .;
  } > RAM

  __heap_size = RAM_SIZE - (__heap_start - RAM_START);

  end = .;
  _end = .;
  PROVIDE(end = .);
  PROVIDE(_end = .);

  /DISCARD/ : {
      *(.note.GNU-stack)
      *(.gnu_debuglink)
      *(.gnu.lto_*)
  }

  /*
   * TCMRAM (64K @ 0x10000000): core-coupled, zero-wait state. To use it,
   * place data/code with __attribute__((section(".tcmram"))). Such data is
   * loaded from flash (LMA = _sitcmram) and must be copied to TCMRAM by the
   * startup file at boot, mirroring the .data copy path. Left here dormant
   * until a driver opts in via a .tcmram section.
   */
  _sitcmram = LOADADDR(.tcmram);
  .tcmram : {
      . = ALIGN(4);
      _stcmram = .;
      *(.tcmram)
      *(.tcmram*)
      . = ALIGN(4);
      _etcmram = .;
  } > TCMRAM AT > FLASH
}
