#include "ut/preprocessor.h"

namespace hedgedev::csl::mem
{
    enum class branch_type
    {
        conditional,
        jump,
        call
    };

    enum class branch_distance
    {
        short_branch,
        near_branch,
        far_branch
    };

    enum class branch_condition
    {
        overflow,
        not_overflow,
        below,
        not_below,
        equal,
        not_equal,
        below_or_equal,
        above,
        sign,
        not_sign,
        parity,
        not_parity,
        less,
        not_less,
        not_greater,
        greater
    };

    inline bool is_nop(void* in_address)
    {
        return in_address && read<uint8_t>(in_address) == 0x90;
    }

    inline branch_info get_branch_info(void* in_address)
    {
        branch_info result{};

        if (!in_address)
            return result;
        
        const auto b0 = read<uint8_t>(in_address);
        const auto b1 = read<uint8_t>(reinterpret_cast<uint8_t*>(in_address) + 1);

        if ((b0 & 0xF0) == 0x70 || b0 == 0x0F)
        {
            result.type = branch_type::conditional;

            if (b0 == 0x0F)
            {
                result.distance = branch_distance::near_branch;
                result.condition = branch_condition(b1 & ~0x80);
                result.opcode_length = 2;
                result.instr_length = 6;
            }
            else
            {
                result.distance = branch_distance::short_branch;
                result.condition = branch_condition(b0 & ~0x70);
                result.opcode_length = 1;
                result.instr_length = 2;
            }
        }
        else
        {
            if (b0 == 0xFF)
            {
                result.type = b1 == 0x15 ? branch_type::call : branch_type::jump;
                result.distance = branch_distance::far_branch;
#ifdef CMNLIB_X64
                result.opcode_length = 6;
                result.instr_length = 14;
#else
                result.opcode_length = 2;
                result.instr_length = 6;
#endif
            }
            else if (b0 == 0xEB)
            {
                result.type = branch_type::jump;
                result.distance = branch_distance::short_branch;
                result.opcode_length = 1;
                result.instr_length = 2;
            }
            else
            {
                result.type = b0 == 0xE8 ? branch_type::call : branch_type::jump;
                result.distance = branch_distance::near_branch;
                result.opcode_length = 1;
                result.instr_length = 5;
            }
        }

        const auto imm_offset = uintptr_t(in_address) + result.opcode_length;

        ptrdiff_t rva{};

        switch (result.distance)
        {
            case branch_distance::short_branch:
                rva = *reinterpret_cast<int8_t*>(imm_offset);
                break;

            case branch_distance::near_branch:
                rva = *reinterpret_cast<int32_t*>(imm_offset);
                break;

            case branch_distance::far_branch:
                result.destination = reinterpret_cast<void*>(*reinterpret_cast<uintptr_t*>(imm_offset));
                return result;
        }

        result.destination = reinterpret_cast<void*>(uintptr_t(in_address) + rva + result.instr_length);

        return result;
    }

    template <typename T>
    inline void* read_instr_address(void* in_address, ptrdiff_t in_offset, size_t in_stride)
    {
        if (!in_address)
            return nullptr;

        return reinterpret_cast<void*>((uintptr_t(in_address) + *reinterpret_cast<T*>(uintptr_t(in_address) + in_offset)) + in_stride);
    }

    inline void* read_call(void* in_address)
    {
        return read_jump(in_address);
    }

    inline void* read_jump(void* in_address)
    {
        return get_branch_info(in_address).destination;
    }

    inline bool write_call(void* in_address, void* in_destination)
    {
        return write_jump(in_address, in_destination, true);
    }

    inline bool write_jump(void* in_address, void* in_destination, bool in_isCall)
    {
        if (!in_address)
            return false;

        const auto rva = ptrdiff_t(in_destination) - ptrdiff_t(in_address);

        if (rva - 2 <= 0x7F && !in_isCall)
        {
            ASSERT_RETURN_FALSE(write<uint8_t>(in_address, 0xEB));
            ASSERT_RETURN_FALSE(write<int8_t>(reinterpret_cast<uint8_t*>(in_address) + 1, int8_t(rva - 2)));
        }
        else
        {
            if (rva - 5 <= 0x7FFFFFFF)
            {
                ASSERT_RETURN_FALSE(write<uint8_t>(in_address, in_isCall ? 0xE8 : 0xE9));
                ASSERT_RETURN_FALSE(write<int32_t>(reinterpret_cast<uint8_t*>(in_address) + 1, int32_t(rva - 5)));
            }
            else
            {
                ASSERT_RETURN_FALSE(write<uint8_t>(in_address, 0xFF));
                ASSERT_RETURN_FALSE(write<uint8_t>(reinterpret_cast<uint8_t*>(in_address) + 1, in_isCall ? 0x15 : 0x25));
#ifdef CMNLIB_X64
                ASSERT_RETURN_FALSE(write<uint32_t>(reinterpret_cast<uint8_t*>(in_address) + 2, 0));

                const ptrdiff_t imm_offset = 6;
#else
                const ptrdiff_t imm_offset = 2;
#endif
                ASSERT_RETURN_FALSE(write<uintptr_t>(reinterpret_cast<uint8_t*>(in_address) + imm_offset, uintptr_t(in_destination)));
            }
        }

        return true;
    }

    inline bool write_nop(void* in_address, size_t in_count)
    {
        return write<uint8_t>(in_address, 0x90, in_count);
    }
}
