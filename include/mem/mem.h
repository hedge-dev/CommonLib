#pragma once

#include <array>
#include <vector>

#include "ut/expr/string_types.h"

///
/// Transforms a virtual address to the current module's ASLR base.
///
/// \param ADDRESS The address to transform.
///
/// \returns The input address transformed into the ASLR base.
///
#define ASLR(ADDRESS) \
    (hedgedev::csl::mem::to_aslr((void*)(ADDRESS)))

///
/// Checks if the instruction at the given memory address is a
/// no-operation (NOP) instruction.
///
/// \param ADDRESS The address to check.
///
/// \returns `true` if the instruction is no-operation (NOP). Otherwise, `false`.
///
#define IS_NOP(ADDRESS) \
    (hedgedev::csl::mem::is_nop((void*)(ADDRESS)))

///
/// Reads a value in memory.
///
/// \param ADDRESS The address to read from.
/// \param TYPE    The type to read.
///
/// \returns An instance of the type read from memory.
///
#define READ(ADDRESS, TYPE) \
    (hedgedev::csl::mem::read<TYPE>((void*)(ADDRESS)))

///
/// Reads an array of values in memory.
///
/// \param ADDRESS The address to read from.
/// \param TYPE    The type to read.
/// \param COUNT   The total number of values to read.
///
/// \returns An array of instances of the type read from memory.
///
#define READ_ARRAY(ADDRESS, TYPE, COUNT) \
    (hedgedev::csl::mem::read<TYPE, COUNT>((void*)(ADDRESS)))

///
/// Reads the address of an instruction in memory.
///
/// \param ADDRESS The address of the instruction to read.
/// \param TYPE    The type to read.
/// \param OFFSET  The offset of the address after `ADDRESS`.
/// \param STRIDE  The length of the instruction that was read.
///
/// \returns The address referenced by the instruction.
///
#define READ_INSTR_ADDRESS(ADDRESS, TYPE, OFFSET, STRIDE) \
    (hedgedev::csl::mem::read_instr_address<TYPE>((void*)(ADDRESS), OFFSET, STRIDE))

///
/// Reads the address of a call instruction in memory.
///
/// \param ADDRESS The address of the call instruction to read.
///
/// \returns The address referenced by the call instruction.
///
#define READ_CALL(ADDRESS) \
    (hedgedev::csl::mem::read_call((void*)(ADDRESS)))

///
/// Reads the address of a jump instruction in memory.
///
/// \param ADDRESS The address of the jump instruction to read.
///
/// \returns The address referenced by the jump instruction.
///
#define READ_JUMP(ADDRESS) \
    (hedgedev::csl::mem::read_jump((void*)(ADDRESS)))

///
/// Writes any number of values in memory.
///
/// \param ADDRESS     The address to write to.
/// \param TYPE        The type to write.
/// \param __VA_ARGS__ The values to write.
///
/// \returns `true` if the data was written successfully. Otherwise, `false`.
///
#define WRITE(ADDRESS, TYPE, ...) \
    (hedgedev::csl::mem::write<TYPE>((void*)(ADDRESS), { __VA_ARGS__ }))

///
/// Writes an array of values in memory.
///
/// \param ADDRESS The address to write to.
/// \param TYPE    The type to write.
/// \param DATA    The collection of values to write.
///
/// \returns `true` if the data was written successfully. Otherwise, `false`.
///
#define WRITE_ARRAY(ADDRESS, TYPE, DATA) \
    (hedgedev::csl::mem::write<TYPE>((void*)(ADDRESS), DATA))

///
/// Writes a call instruction in memory.
///
/// \param ADDRESS     The address to write to.
/// \param DESTINATION The address of the function to call.
///
/// \returns `true` if the call was written successfully. Otherwise, `false`.
///
#define WRITE_CALL(ADDRESS, DESTINATION) \
    (hedgedev::csl::mem::write_call((void*)(ADDRESS), (void*)(DESTINATION)))

///
/// Writes a jump instruction in memory.
///
/// \param ADDRESS     The address to write to.
/// \param DESTINATION The address of the address to jump to.
///
/// \returns `true` if the jump was written successfully. Otherwise, `false`.
///
#define WRITE_JUMP(ADDRESS, DESTINATION) \
    (hedgedev::csl::mem::write_jump((void*)(ADDRESS), (void*)(DESTINATION)))

///
/// Writes a no-operation (NOP) instruction in memory.
///
/// \param ADDRESS The address to write to.
/// \param COUNT   The total number of no-operation (NOP) instructions to write.
///
/// \returns `true` if the no-operation (NOP) instruction was written successfully.
///          Otherwise, `false`.
///
#define WRITE_NOP(ADDRESS, COUNT) \
    (hedgedev::csl::mem::write_nop((void*)(ADDRESS), COUNT))

///
/// Writes a string in memory.
///
/// \param ADDRESS The address to write to.
/// \param STR     The string to write.
///
/// \returns `true` if the string was written successfully. Otherwise, `false`.
///
#define WRITE_STRING(ADDRESS, STR) \
    (hedgedev::csl::mem::write_string((void*)(ADDRESS), STR))

///
/// Writes a string of fixed length in memory.
///
/// \param ADDRESS     The address to write to.
/// \param STR         The string to write.
/// \param __VA_ARGS__ The length of the string to write.
///                    If zero or unspecified, the length will be determined by
///                    the length of an existing string located at `ADDRESS`.
///
/// \returns `true` if the string was written successfully. Otherwise, `false`.
///
#define WRITE_STRING_FIXED(ADDRESS, STR, ...) \
    (hedgedev::csl::mem::write_string_fixed_length((void*)(ADDRESS), STR, __VA_ARGS__))

namespace hedgedev::csl::mem
{
    enum class page_protection
    {
        no_access = 0 << 0,

        read = 1 << 0,
        write = 1 << 1,
        execute = 1 << 2,

        read_write = read | write,
        read_execute = read | execute,
        read_write_execute = read | write | execute,

        r = read,
        w = write,
        x = execute,

        rw = read_write,
        rx = read_execute,
        rwx = read_write_execute
    };

    enum class branch_type;
    enum class branch_distance;
    enum class branch_condition;

    struct branch_info
    {
        branch_type type{};
        branch_distance distance{};
        branch_condition condition{};
        uint8_t opcode_length{};
        uint8_t instr_length{};
        void* destination{};
    };

    ///
    /// Gets the original base address of the current module.
    ///
    inline void* get_original_module_base();

    ///
    /// Gets the system flags for the specified page protection.
    ///
    inline uint32_t get_protect_flags(page_protection in_protection);

    ///
    /// Sets the protection of a page of memory.
    ///
    inline bool protect(void* in_address, size_t in_length, uint32_t in_new_protect_flags, uint32_t* out_old_protect_flags = nullptr);

    ///
    /// Transforms a virtual address to the current module's ASLR base.
    ///
    /// \param in_address      The address to transform.
    /// \param in_base_address The original base address of the module containing the
    ///                        address (optional).
    ///
    /// \returns The input address transformed into the ASLR base.
    ///
    inline void* to_aslr(void* in_address, void* in_base_address = nullptr);

    ///
    /// Transforms a virtual address from the current module's ASLR base.
    ///
    /// \param in_address      The address to transform.
    /// \param in_base_address The original base address of the module containing the
    ///                        address (optional).
    ///
    /// \returns The input address transformed from the ASLR base.
    ///
    inline void* from_aslr(void* in_address, void* in_base_address = nullptr);

    ///
    /// Checks if the instruction at the given memory address is a
    /// no-operation (NOP) instruction.
    ///
    /// \param in_address The address to check.
    ///
    /// \returns `true` if the instruction is no-operation (NOP). Otherwise, `false`.
    ///
    inline bool is_nop(void* in_address);

    ///
    /// Reads a value in memory.
    ///
    /// \tparam T The type to read.
    ///
    /// \param in_address The address to read from.
    ///
    /// \returns An instance of the type read from memory.
    ///
    template <typename T>
    inline T read(void* in_address);

    ///
    /// Reads an array of values in memory.
    ///
    /// \tparam T       The type to read.
    /// \tparam K_count The total number of values to read.
    ///
    /// \param in_address The address to read from.
    ///
    /// \returns An array of instances of the type read from memory.
    ///
    template <typename T, size_t K_count>
    inline std::array<T, K_count> read(void* in_address);

    ///
    /// Gets information about a branch instruction in memory.
    ///
    /// \param in_address The address of the branch instruction.
    ///
    /// \returns A structure containing information about the branch.
    ///
    inline branch_info get_branch_info(void* in_address);

    ///
    /// Reads the address of an instruction in memory.
    ///
    /// \tparam T The type to read.
    ///
    /// \param in_address The address of the instruction to read.
    /// \param in_offset  The offset of the address after `in_address`.
    /// \param in_stride  The length of the instruction that was read.
    ///
    /// \returns The address referenced by the instruction.
    ///
    template <typename T>
    inline void* read_instr_address(void* in_address, ptrdiff_t in_offset, size_t in_stride);

    ///
    /// Reads the address of a call instruction in memory.
    ///
    /// \param in_address The address of the call instruction to read.
    ///
    /// \returns The address referenced by the call instruction.
    ///
    inline void* read_call(void* in_address);

    ///
    /// Reads the address of a jump instruction in memory.
    ///
    /// \param in_address The address of the jump instruction to read.
    ///
    /// \returns The address referenced by the jump instruction.
    ///
    inline void* read_jump(void* in_address);

    ///
    /// Writes a value in memory.
    ///
    /// \tparam T The type to write.
    ///
    /// \param in_address The address to write to.
    /// \param in_data    The data to write.
    /// \param in_count   The total number of values to write.
    ///
    /// \returns `true` if the data was written successfully. Otherwise, `false`.
    ///
    template <typename T>
    inline bool write(void* in_address, const T& in_data, size_t in_count = 1);

    ///
    /// Writes an array of values in memory.
    ///
    /// \tparam T The type to write.
    ///
    /// \param in_address The address to write to.
    /// \param in_data    The collection of values to write.
    ///
    /// \returns `true` if the data was written successfully. Otherwise, `false`.
    ///
    template <typename T>
    inline bool write(void* in_address, const std::vector<T>& in_data);

    ///
    /// Writes a call instruction in memory.
    ///
    /// \param in_address     The address to write to.
    /// \param in_destination The address of the function to call.
    ///
    /// \returns `true` if the call was written successfully. Otherwise, `false`.
    ///
    inline bool write_call(void* in_address, void* in_destination);

    ///
    /// Writes a jump instruction in memory.
    ///
    /// \param in_address     The address to write to.
    /// \param in_destination The address of the address to jump to.
    /// \param in_call        Determines whether the jump should be encoded as a
    ///                       call instruction.
    ///
    /// \returns `true` if the jump was written successfully. Otherwise, `false`.
    ///
    inline bool write_jump(void* in_address, void* in_destination, bool in_call = false);

    ///
    /// Writes a no-operation (NOP) instruction in memory.
    ///
    /// \param in_address The address to write to.
    /// \param in_count   The total number of no-operation (NOP) instructions to write.
    ///
    /// \returns `true` if the no-operation (NOP) instruction was written successfully.
    ///          Otherwise, `false`.
    ///
    inline bool write_nop(void* in_address, size_t in_count = 1);

    ///
    /// Writes a string in memory.
    ///
    /// \param in_address The address to write to.
    /// \param in_str     The string to write.
    ///
    /// \returns `true` if the string was written successfully. Otherwise, `false`.
    ///
    template <ut::expr::any_string_t T>
    inline bool write_string(void* in_address, const T& in_str);

    ///
    /// Writes a string of fixed length in memory.
    ///
    /// \param in_address The address to write to.
    /// \param in_str     The string to write.
    /// \param in_length  The length of the string to write.
    ///                   If zero, the length will be determined by the length of an existing string
    ///                   located at `in_address`.
    ///
    /// \returns `true` if the string was written successfully. Otherwise, `false`.
    ///
    template <ut::expr::any_string_t T>
    inline bool write_string_fixed_length(void* in_address, const T& in_str, size_t in_length = 0);
}

#include "mem.inl"

#if defined(CMNLIB_X64) || defined(CMNLIB_X86)
#include "arch/mem_x86.inl"
#endif

#ifdef WIN32
#include "os/mem_win32.inl"
#endif
