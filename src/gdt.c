#include "header/cpu/gdt.h"

/**
 * global_descriptor_table, predefined GDT.
 * Initial SegmentDescriptor already set properly according to Intel Manual & OSDev.
 * Table entry : [{Null Descriptor}, {Kernel Code}, {Kernel Data (variable, etc)}, ...].
 */
struct GlobalDescriptorTable global_descriptor_table = {
    .table = {
        {// TODO : Implement
         .segment_low = 0,
         .base_low = 0,
         .base_mid = 0,
         .type_bit = 0,
         .non_system = 0,
         .dpl = 0,
         .p = 0,
         .segment_limit = 0,
         .avl = 0,
         .l = 0,
         .dop = 0,
         .g = 0,
         .base_high = 0},
        {// TODO : Implement
         .segment_low = 0xFFFF,
         .base_low = 0,
         .base_mid = 0,
         .type_bit = 0b1010,
         .non_system = 1,
         .dpl = 0,
         .p = 1,
         .segment_limit = 0b1111,
         .avl = 0,
         .l = 0,
         .dop = 1,
         .g = 1,
         .base_high = 0},
        {// TODO : Implement
         .segment_low = 0xFFFF,
         .base_low = 0,
         .base_mid = 0,
         .type_bit = 0b0010,
         .non_system = 1,
         .dpl = 0,
         .p = 1,
         .segment_limit = 0b1111,
         .avl = 0,
         .l = 0,
         .dop = 1,
         .g = 1,
         .base_high = 0}}};

/**
 * _gdt_gdtr, predefined system GDTR.
 * GDT pointed by this variable is already set to point global_descriptor_table above.
 * From: https://wiki.osdev.org/Global_Descriptor_Table, GDTR.size is GDT size minus 1.
 */
struct GDTR _gdt_gdtr = {
    // TODO : Implement, this GDTR will point to global_descriptor_table.
    //        Use sizeof operator
    sizeof(global_descriptor_table) - 1,
    &global_descriptor_table};

