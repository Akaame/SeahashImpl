#ifndef SEAHASH_H
#define SEAHASH_H

#include <cstdint> // fixed length integer types
#include <span>
#include <cstddef>

namespace seahash
{
    namespace v1
    {
        class Helper
        {
        public:
            static inline uint_fast64_t diffuse(uint_fast64_t val)
            {
                val = val * 0x6eed0e9da4d94a4f;
                auto a = val >> 32;
                auto b = val >> 60;
                val ^= a >> b;
                return val * 0x6eed0e9da4d94a4f;
            }

            static inline uint_fast64_t read_u64t(std::span<std::byte> bytes)
            {
                uint_fast64_t result = 0;
                auto size = bytes.size();
                for (int i = size - 1; i >= 0; --i)
                {
                    result <<= 8;
                    auto val = std::to_integer<uint8_t>(bytes[i]);
                    result |= val;
                }
                return result;
            }
        };

        class Seahash
        {
        public:
            class State;

            Seahash() {}

            uint_fast64_t hash(std::span<std::byte> bytes)
            {
                State state;
                auto size = bytes.size();
                for (int i = 0; i < size; i += 8)
                {
                    auto offset = ((int)size - i - 8) > 0 ? 8 : (size - i);
                    state.write_u64t(Helper::read_u64t(bytes.subspan(i, offset)));
                }
                return state.finish(size);
            }

            class State
            {
            public:
                uint_fast64_t a, b, c, d;

                State(uint_fast64_t a = 0x16f11fe89b0d677c,
                      uint_fast64_t b = 0xb480a793d8e6c86c,
                      uint_fast64_t c = 0x6fe2e5aaf078ebc9,
                      uint_fast64_t d = 0x14f994a4c5259381)
                    : a(a), b(b), c(c), d(d) {}

                inline void write_u64t(uint_fast64_t val)
                {
                    auto tmp = Helper::diffuse(a ^ val);
                    a = b;
                    b = c;
                    c = d;
                    d = tmp;
                }

                inline uint_fast64_t finish(uint_fast64_t val)
                {
                    return Helper::diffuse(a ^ b ^ c ^ d ^ val);
                }
            };
        };
    } // namespace v1
} // namespace seahash

#endif