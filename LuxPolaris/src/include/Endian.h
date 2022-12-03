/**
 * @file Endian.h
 * @brief
 *
 * @version 1.0
 * @author Tian-en Lu (tianenlu957@gmail.com)
 * @date 2022-11
 */

#ifndef ENDIAN_H
#define ENDIAN_H

#include <cstdint>
#include <endian.h>

namespace Lux {
namespace Polaris {
    namespace sockets {
// the inline assembler code makes type blur,
// so we disable warnings for a while.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"


        /// @brief 64字节主机转网络
        inline uint64_t
        hostToNetwork64(uint64_t host64) {
            return htobe64(host64);
        }

        /// @brief 32字节主机转网络
        inline uint32_t
        hostToNetwork32(uint32_t host32) {
            return htobe32(host32);
        }


        /// @brief 16字节主机转网络
        inline uint16_t
        hostToNetwork16(uint16_t host16) {
            return htobe16(host16);
        }


        /// @brief 64字节网络转主机
        inline uint64_t
        networkToHost64(uint64_t net64) {
            return be64toh(net64);
        }

        /// @brief 32字节网络转主机
        inline uint32_t
        networkToHost32(uint32_t net32) {
            return be32toh(net32);
        }

        /// @brief 16字节网络转主机
        inline uint16_t
        networkToHost16(uint16_t net16) {
            return be16toh(net16);
        }

#pragma GCC diagnostic pop


    } // namespace sockets
} // namespace Polaris
} // namespace Lux
#endif // ENDIAN_H