#pragma once

#include <array>
#include <vector>

#define ASLR(ADDRESS)                                     (hedgedev::csl::mem::ToASLR((void*)(ADDRESS)))

#define IS_NOP(ADDRESS)                                   (hedgedev::csl::mem::IsNop((void*)(ADDRESS)))

#define READ(ADDRESS, TYPE)                               (hedgedev::csl::mem::Read<TYPE>((void*)(ADDRESS)))
#define READ_ARRAY(ADDRESS, TYPE, COUNT)                  (hedgedev::csl::mem::Read<TYPE, COUNT>((void*)(ADDRESS)))
#define READ_INSTR_ADDRESS(ADDRESS, TYPE, OFFSET, STRIDE) (hedgedev::csl::mem::ReadInstructionAddress<TYPE>((void*)(ADDRESS), OFFSET, STRIDE))
#define READ_CALL(ADDRESS)                                (hedgedev::csl::mem::ReadCall((void*)(ADDRESS)))
#define READ_JUMP(ADDRESS)                                (hedgedev::csl::mem::ReadJump((void*)(ADDRESS)))

#define WRITE(ADDRESS, TYPE, ...)                         (hedgedev::csl::mem::Write<TYPE>((void*)(ADDRESS), { __VA_ARGS__ }))
#define WRITE_ARRAY(ADDRESS, TYPE, DATA)                  (hedgedev::csl::mem::Write<TYPE>((void*)(ADDRESS), DATA))
#define WRITE_CALL(ADDRESS, DESTINATION)                  (hedgedev::csl::mem::WriteCall((void*)(ADDRESS), (void*)(DESTINATION)))
#define WRITE_JUMP(ADDRESS, DESTINATION)                  (hedgedev::csl::mem::WriteJump((void*)(ADDRESS), (void*)(DESTINATION)))
#define WRITE_NOP(ADDRESS, LENGTH)                        (hedgedev::csl::mem::WriteNop((void*)(ADDRESS), LENGTH))
#define WRITE_STRING(ADDRESS, STR)                        (hedgedev::csl::mem::WriteString((void*)(ADDRESS), STR))
#define WRITE_STRING_FIXED(ADDRESS, STR, ...)             (hedgedev::csl::mem::WriteStringFixedLength((void*)(ADDRESS), STR, __VA_ARGS__))

namespace hedgedev::csl::mem
{
    enum class BranchType;
    enum class BranchDistance;
    enum class BranchCondition;

    struct BranchInfo
    {
        BranchType Type{};
        BranchDistance Distance{};
        BranchCondition Condition{};
        uint8_t OpcodeLength{};
        uint8_t InstrLength{};
        void* pDestination{};
    };

    ///
    /// Gets the original base address of the current module.
    ///
    inline void* GetOriginalModuleBase();

    ///
    /// Transforms a virtual address to the current module's ASLR base.
    ///
    /// \param in_pAddress     The address to transform.
    /// \param in_pBaseAddress The original base address of the module containing the address (optional).
    /// 
    /// \returns The input address transformed into the ASLR base.
    ///
    inline void* ToASLR(void* in_pAddress, void* in_pBaseAddress = nullptr);

    ///
    /// Transforms a virtual address from the current module's ASLR base.
    ///
    /// \param in_pAddress     The address to transform.
    /// \param in_pBaseAddress The original base address of the module containing the address (optional).
    /// 
    /// \returns The input address transformed from the ASLR base.
    ///
    inline void* FromASLR(void* in_pAddress, void* in_pBaseAddress = nullptr);

    ///
    /// Checks if the instruction at the given memory address is a no-operation (NOP) instruction.
    ///
    /// \param in_pAddress The address to check.
    /// 
    /// \returns `true` if the instruction is no-operation (NOP). Otherwise, `false`.
    ///
    inline bool IsNop(void* in_pAddress);

    ///
    /// Reads a value in memory.
    ///
    /// \tparam T The type to read.
    ///
    /// \param in_pAddress The address to read from.
    /// 
    /// \returns An instance of the type read from memory.
    ///
    template <typename T>
    inline T Read(void* in_pAddress);

    ///
    /// Reads an array of values in memory.
    ///
    /// \tparam T     The type to read.
    /// \tparam Count The total number of values to read.
    ///
    /// \param in_pAddress The address to read from.
    /// 
    /// \returns An array of instances of the type read from memory.
    ///
    template <typename T, size_t Count>
    inline std::array<T, Count> Read(void* in_pAddress);

    ///
    /// Gets information about a branch instruction in memory.
    ///
    /// \param in_pAddress The address of the branch instruction.
    /// 
    /// \returns A structure containing information about the branch.
    ///
    inline BranchInfo GetBranchInfo(void* in_pAddress);

    ///
    /// Reads the address of an instruction in memory.
    ///
    /// \tparam T The type to read.
    ///
    /// \param in_pAddress The address of the instruction to read.
    /// \param in_offset   The offset of the address after \ref in_pAddress.
    /// \param in_stride   The length of the instruction that was read.
    /// 
    /// \returns The address referenced by the instruction.
    ///
    template <typename T>
    inline void* ReadInstructionAddress(void* in_pAddress, size_t in_offset, size_t in_stride);

    ///
    /// Reads the address of a call instruction in memory.
    ///
    /// \param in_pAddress The address of the call instruction to read.
    /// 
    /// \returns The address referenced by the call instruction.
    ///
    inline void* ReadCall(void* in_pAddress);

    ///
    /// Reads the address of a jump instruction in memory.
    ///
    /// \param in_pAddress The address of the jump instruction to read.
    /// 
    /// \returns The address referenced by the jump instruction.
    ///
    inline void* ReadJump(void* in_pAddress);

    ///
    /// Writes a value in memory.
    ///
    /// \tparam T The type to write.
    ///
    /// \param in_pAddress The address to write to.
    /// \param in_rData    The data to write.
    ///
    template <typename T>
    inline bool Write(void* in_pAddress, const T& in_rData);

    ///
    /// Writes an array of values in memory.
    ///
    /// \tparam T The type to write.
    ///
    /// \param in_pAddress The address to write to.
    /// \param in_rData    The collection of values to write.
    ///
    template <typename T>
    inline bool Write(void* in_pAddress, const std::vector<T>& in_rData);

    ///
    /// Writes a call instruction in memory.
    ///
    /// \param in_pAddress     The address to write to.
    /// \param in_pDestination The address of the function to call.
    ///
    inline bool WriteCall(void* in_pAddress, void* in_pDestination);

    ///
    /// Writes a jump instruction in memory.
    ///
    /// \param in_pAddress     The address to write to.
    /// \param in_pDestination The address of the address to jump to.
    /// \param in_isCall       Determines whether the jump should be encoded as a call instruction.
    ///
    inline bool WriteJump(void* in_pAddress, void* in_pDestination, bool in_isCall = false);

    ///
    /// Writes a no-operation (NOP) instruction in memory.
    ///
    /// \param in_pAddress The address to write to.
    /// \param in_length   The total number of no-operation (NOP) instructions to write.
    ///
    inline bool WriteNop(void* in_pAddress, size_t in_length);

    ///
    /// Writes a string in memory.
    ///
    /// \param in_pAddress The address to write to.
    /// \param in_rStr     The string to write.
    ///
    template <hedgedev::csl::ut::expr::AnyString T>
    inline bool WriteString(void* in_pAddress, const T& in_rStr);

    ///
    /// Writes a string of fixed length in memory.
    ///
    /// \param in_pAddress The address to write to.
    /// \param in_rStr     The string to write.
    /// \param in_length   The length of the string to write.
    ///                    If zero, the length will be determined by the length of an existing
    ///                    string located at \ref in_pAddress.
    ///
    template <hedgedev::csl::ut::expr::AnyString T>
    inline bool WriteStringFixedLength(void* in_pAddress, const T& in_rStr, size_t in_length = 0);
}

#ifdef WIN32
#include "OS/Win32/MemoryWin32.inl"
#endif
