#pragma once

struct alignas(4) ObjectGUID
{
public:
	static ObjectGUID NewGUID();

public:
	uint32 mA;
	uint32 mB;
	uint32 mC;
	uint32 mD;
};

inline bool operator==(const ObjectGUID& a, const ObjectGUID& b)
{
    return a.mA == b.mA && 
        a.mB == b.mB &&
        a.mC == b.mC &&
        a.mD == b.mD;
}

inline size_t HashCombine(size_t seed, size_t value)
{
    // 마법의 숫자 루트 5 : 0x9e3779b97f4a7c15ULL
    return seed ^ (value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2));
}

template<>
struct std::hash<ObjectGUID>
{
    size_t operator()(const ObjectGUID& id) const noexcept
    {
        size_t hash = 0;
        hash = HashCombine(hash, id.mA);
        hash = HashCombine(hash, id.mB);
        hash = HashCombine(hash, id.mC);
        hash = HashCombine(hash, id.mD);
        return hash;
    }
};
