namespace hedgedev::csl::mem
{
    enum class BranchType
    {
        Conditional,
        Jump,
        Call
    };

    enum class BranchDistance
    {
        Short,
        Near,
        Far
    };

    enum class BranchCondition
    {
        Overflow,
        NotOverflow,
        Below,
        NotBelow,
        Equal,
        NotEqual,
        BelowOrEqual,
        Above,
        Sign,
        NotSign,
        Parity,
        NotParity,
        Less,
        NotLess,
        NotGreater,
        Greater
    };

    inline bool IsNop(void* in_pAddress)
    {
        return in_pAddress && Read<uint8_t>(in_pAddress) == 0x90;
    }

    inline BranchInfo GetBranchInfo(void* in_pAddress)
    {
        BranchInfo result{};

        if (!in_pAddress)
            return result;
        
        const auto b0 = Read<uint8_t>(in_pAddress);
        const auto b1 = Read<uint8_t>((uint8_t*)in_pAddress + 1);

        if ((b0 & 0xF0) == 0x70 || b0 == 0x0F)
        {
            result.Type = BranchType::Conditional;

            if (b0 == 0x0F)
            {
                result.Distance = BranchDistance::Near;
                result.Condition = BranchCondition(b1 & ~0x80);
                result.OpcodeLength = 2;
                result.InstrLength = 6;
            }
            else
            {
                result.Distance = BranchDistance::Short;
                result.Condition = BranchCondition(b0 & ~0x70);
                result.OpcodeLength = 1;
                result.InstrLength = 2;
            }
        }
        else
        {
            if (b0 == 0xFF)
            {
                result.Type = b1 == 0x15 ? BranchType::Call : BranchType::Jump;
                result.Distance = BranchDistance::Far;
#ifdef CMNLIB_X64
                result.OpcodeLength = 6;
                result.InstrLength = 14;
#else
                result.OpcodeLength = 2;
                result.InstrLength = 6;
#endif
            }
            else if (b0 == 0xEB)
            {
                result.Type = BranchType::Jump;
                result.Distance = BranchDistance::Short;
                result.OpcodeLength = 1;
                result.InstrLength = 2;
            }
            else
            {
                result.Type = b0 == 0xE8 ? BranchType::Call : BranchType::Jump;
                result.Distance = BranchDistance::Near;
                result.OpcodeLength = 1;
                result.InstrLength = 5;
            }
        }

        const auto immOffset = uintptr_t(in_pAddress) + result.OpcodeLength;
        ptrdiff_t rva{};

        switch (result.Distance)
        {
            case BranchDistance::Short:
                rva = *(int8_t*)immOffset;
                break;

            case BranchDistance::Near:
                rva = *(int32_t*)immOffset;
                break;

            case BranchDistance::Far:
                result.pDestination = (void*)(*(uintptr_t*)immOffset);
                return result;
        }

        result.pDestination = (void*)(uintptr_t(in_pAddress) + rva + result.InstrLength);

        return result;
    }

    template <typename T>
    inline void* ReadInstructionAddress(void* in_pAddress, ptrdiff_t in_offset, size_t in_stride)
    {
        if (!in_pAddress)
            return nullptr;

        return (void*)((uintptr_t(in_pAddress) + *(T*)(uintptr_t(in_pAddress) + in_offset)) + in_stride);
    }

    inline void* ReadCall(void* in_pAddress)
    {
        return ReadJump(in_pAddress);
    }

    inline void* ReadJump(void* in_pAddress)
    {
        return GetBranchInfo(in_pAddress).pDestination;
    }

    inline bool WriteCall(void* in_pAddress, void* in_pDestination)
    {
        return WriteJump(in_pAddress, in_pDestination, true);
    }

    inline bool WriteJump(void* in_pAddress, void* in_pDestination, bool in_isCall)
    {
        if (!in_pAddress)
            return false;

        const auto rva = ptrdiff_t(in_pDestination) - ptrdiff_t(in_pAddress);

        if (rva - 2 <= 0x7F && !in_isCall)
        {
            ASSERT_RETURN_FALSE(Write<uint8_t>(in_pAddress, 0xEB));
            ASSERT_RETURN_FALSE(Write<int8_t>((uint8_t*)in_pAddress + 1, int8_t(rva - 2)));
        }
        else
        {
            if (rva - 5 <= 0x7FFFFFFF)
            {
                ASSERT_RETURN_FALSE(Write<uint8_t>(in_pAddress, in_isCall ? 0xE8 : 0xE9));
                ASSERT_RETURN_FALSE(Write<int32_t>((uint8_t*)in_pAddress + 1, int32_t(rva - 5)));
            }
            else
            {
                ASSERT_RETURN_FALSE(Write<uint8_t>(in_pAddress, 0xFF));
                ASSERT_RETURN_FALSE(Write<uint8_t>((uint8_t*)in_pAddress + 1, in_isCall ? 0x15 : 0x25));
#ifdef CMNLIB_X64
                ASSERT_RETURN_FALSE(Write<uint32_t>((uint8_t*)in_pAddress + 2, 0));

                const auto immOffset = 6;
#else
                const auto immOffset = 2;
#endif
                ASSERT_RETURN_FALSE(Write<uintptr_t>((uint8_t*)in_pAddress + immOffset, uintptr_t(in_pDestination)));
            }
        }

        return true;
    }

    inline bool WriteNop(void* in_pAddress, size_t in_count)
    {
        return Write<uint8_t>(in_pAddress, 0x90, in_count);
    }
}
