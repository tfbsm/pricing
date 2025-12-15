#ifndef TFBSM_FEEDER_PROTOCOL_H_
#define TFBSM_FEEDER_PROTOCOL_H_

#include "stdint.h"

#include "crc32.h"

#include <algorithm>
#include <cstdint>
#include <array>

namespace tfbsm
{

constexpr uint32_t to_be32(uint32_t v) {
    return ((v & 0x000000FFu) << 24) |
           ((v & 0x0000FF00u) << 8)  |
           ((v & 0x00FF0000u) >> 8)  |
           ((v & 0xFF000000u) >> 24);
}

constexpr uint64_t to_be64(uint64_t v) {
    return ((v & 0x00000000000000FFULL) << 56) |
           ((v & 0x000000000000FF00ULL) << 40) |
           ((v & 0x0000000000FF0000ULL) << 24) |
           ((v & 0x00000000FF000000ULL) << 8)  |
           ((v & 0x000000FF00000000ULL) >> 8)  |
           ((v & 0x0000FF0000000000ULL) >> 24) |
           ((v & 0x00FF000000000000ULL) >> 40) |
           ((v & 0xFF00000000000000ULL) >> 56);
}

constexpr uint32_t from_be32(uint32_t v) { return to_be32(v); }
constexpr uint64_t from_be64(uint64_t v) { return to_be64(v); }

class Protocol {
public:
    #pragma pack(push, 1)
    struct PriceEstimation {
        uint32_t option_instrument_code;
        uint32_t spot_instrument_code;
        uint32_t price_exponent;
        uint64_t market_price;
        uint64_t predicted_price;
        uint64_t ts;
        uint32_t crc32;
    };
    #pragma pack(pop)

    // Verify no padding
    static_assert(sizeof(PriceEstimation) == 
        sizeof(uint32_t) * 4 + sizeof(uint64_t) * 3,
        "Price estimation has padding!");

    static std::array<uint8_t, sizeof(PriceEstimation)>
    serialize(const PriceEstimation& p) {
        std::array<uint8_t, sizeof(PriceEstimation)> buf{};
        size_t off = 0;

        auto write32 = [&](uint32_t v) {
            v = to_be32(v);
            std::memcpy(buf.data() + off, &v, 4);
            off += 4;
        };

        auto write64 = [&](uint64_t v) {
            v = to_be64(v);
            std::memcpy(buf.data() + off, &v, 8);
            off += 8;
        };

        write32(p.option_instrument_code);
        write32(p.spot_instrument_code);
        write32(p.price_exponent);
        write64(p.market_price);
        write64(p.predicted_price);
        write64(p.ts);

        // FIXME: CRC32 may be incorrect here :(
        if (p.crc32 == 0) {
            write32(xcrc32(buf.data(), sizeof(PriceEstimation) - sizeof(uint32_t), 0));
        } else {
            write32(p.crc32);
        }

        return buf;
    }

    static PriceEstimation deserialize(const uint8_t* data, size_t size) {
        if (size < sizeof(PriceEstimation)) {
            throw std::runtime_error("buffer too small");
        }

        PriceEstimation p{};
        size_t off = 0;

        auto read32 = [&](uint32_t& v) {
            std::memcpy(&v, data + off, 4);
            v = from_be32(v);
            off += 4;
        };

        auto read64 = [&](uint64_t& v) {
            std::memcpy(&v, data + off, 8);
            v = from_be64(v);
            off += 8;
        };

        read32(p.option_instrument_code);
        read32(p.spot_instrument_code);
        read32(p.price_exponent);
        read64(p.market_price);
        read64(p.predicted_price);
        read64(p.ts);
        read32(p.crc32);

        return p;
    }
    
};
    
} // namespace tfbsm


#endif
