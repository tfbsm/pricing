#ifndef TFBSM_CRC32_H_
#define TFBSM_CRC32_H_

#include <cstdint>

uint32_t crc32(uint8_t const* data, size_t length) noexcept {
    uint32_t crc = 0xFFFFFFFFu;

    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc >> 1) ^ (0xEDB88320u & (~(crc & 1) + 1));
        }
    }

    return crc ^ 0xFFFFFFFFu;
}

#endif
