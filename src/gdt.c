#include "header/cpu/gdt.h"

// Define the GDT entries
struct SegmentDescriptor gdt_entries[GDT_MAX_ENTRY_COUNT];

// Initialize the Global Descriptor Table with appropriate entries
struct GlobalDescriptorTable global_descriptor_table = {
    .table = {
        // Null Descriptor (Index 0)
        {
            .segment_low = 0,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0,
            .non_system = 0,
            // TODO: Fill in the rest of the fields for Null Descriptor
        },
        // Kernel Code Segment (Index 1)
        {
            .segment_low = 0xFFFFF,  // Maximum limit value
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0xA,        // Code segment, Readable
            .non_system = 1,
            // TODO: Fill in the rest of the fields for Kernel Code Segment
        },
        // Kernel Data Segment (Index 2)
        {
            .segment_low = 0xFFFFF,  // Maximum limit value
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0x2,        // Data segment, Writable
            .non_system = 1,
            // TODO: Fill in the rest of the fields for Kernel Data Segment
        },
        // Add more entries as needed
    }
};

// Initialize the Global Descriptor Table Register (GDTR)
struct GDTR _gdt_gdtr = {
    .size = sizeof(global_descriptor_table.table) - 1,  // Size of GDT - 1
    .address = &global_descriptor_table.table            // Address of the GDT
};
