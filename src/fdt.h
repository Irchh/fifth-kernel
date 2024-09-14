#include <stdint.h>
#include <stddef.h>

#define FDT_BEGIN_NODE  0x00000001 /* Followed by a null-terminated string called the node unit name */
#define FDT_END_NODE    0x00000002 /* No extra data */
#define FDT_PROP        0x00000003 /* Followed by two 32-bit BE numbers, length and name offset. */
#define FDT_NOP         0x00000004 /* No extra data */
#define FDT_END         0x00000009 /* No extra data */

struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

struct fdt_reserve_entry {
    uint64_t address;
    uint64_t size;
};

struct device_information_t {
    size_t ram_start;
    size_t ram_size;
    struct fdt_header* dtb;
    size_t dtb_size;
};

extern struct device_information_t device_information;

int print_fdt_info(struct fdt_header* header);
