OUTPUT_ARCH("riscv")

ENTRY(_enter)

MEMORY
{
    ram (arw!xi) : ORIGIN = 0x00000200, LENGTH = 0x100000
    disk(irx!wa) : ORIGIN = 0x00400000, LENGTH = 0x400000
}

PHDRS
{
    ram PT_LOAD;
    disk PT_LOAD;
}

SECTIONS
{
    /* ROM Sections */
    .init : {
        *(.text.enter)
    } >ram :ram

    .text : {
        *(.text .text.*)
    } >ram :ram

    .rodata : {
        *(.rdata)
        *(.rodata .rodata.*)
        . = ALIGN(8);
        *(.srodata .srodata.*)
    } >ram :ram

    /* RAM Sections */
    .data : ALIGN(8) {
        *(.data .data.*)
        . = ALIGN(8);
        *(.sdata .sdata.* .sdata2.*)
        _gp = . + 0x800;
    } >ram :ram

    .bss (NOLOAD): ALIGN(8) {
        *(.sbss*)
        *(.bss .bss.*)
        *(COMMON)
    } >ram :ram

    .heap (NOLOAD) : ALIGN(8) {
        PROVIDE( __heap_start = . );
    } >ram :ram

    .image : {
        *(.image.placeholder);
        . += 0x400000;
    } >disk :disk

    PROVIDE( __heap_end = ADDR(.data) + 0x2800 );
}
