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
 * Linker script for GD32F470VG + LiteOS-M.
 *   Flash: 1024K @ 0x08000000 (GD32F470VG, xG = 1024K)
 *   SRAM:  192K @ 0x20000000
 * Code executes in place from flash (XIP); .data is copied to SRAM by
 * Reset_Handler, .bss is zeroed. The 4K .stack region reserves the MSP;
 * the remainder of SRAM is the LiteOS-M kernel heap.
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(Reset_Handler)

SVC_IRQ_STACK_SIZE = 4K;
FLASH_START         = 0x08000000;
FLASH_SIZE          = 1024K;
RAM_START           = 0x20000000;
RAM_SIZE            = 192K;

MEMORY
{
   FLASH (rx)  : ORIGIN = FLASH_START, LENGTH = FLASH_SIZE
   RAM   (rwx): ORIGIN = RAM_START,   LENGTH = RAM_SIZE
}

SECTIONS
{
  .vectors : {
      . = ALIGN(4);
      KEEP(*(.vectors))
      . = ALIGN(4);
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
      *(.ARM.attributes)
  }
}
