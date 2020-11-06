#ifndef SEAHASH_H
#define SEAHASH_H

#include <cstdint> // fixed length integer types
#include <span>
#include <cstddef>

namespace seahash
{
    namespace v2
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
                // bytes.data() LE reinterpret TODO
                uint_fast64_t result = *reinterpret_cast<uint_fast64_t*>(bytes.data());
                // auto ull = result;
                // ull = (ull >> 56) |
                // ((ull<<40) & 0x00FF000000000000) |
                // ((ull<<24) & 0x0000FF0000000000) |
                // ((ull<<8) & 0x000000FF00000000) |
                // ((ull>>8) & 0x00000000FF000000) |
                // ((ull>>24) & 0x0000000000FF0000) |
                // ((ull>>40) & 0x000000000000FF00) |
                // (ull << 56);
                return result;
            } 

            static inline uint_fast64_t read_u64t_m(std::span<std::byte> bytes) {
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
                auto orig_size = bytes.size(); // check size control not necessary now
                auto excessive = orig_size % 32;
                if (orig_size / 32 > 0) {
                    for (int i = 0; i < orig_size; i += 32)
                    {
                        state.a ^= Helper::read_u64t(bytes.subspan(i, 8));
                        state.b ^= Helper::read_u64t(bytes.subspan(i+8, 8));
                        state.c ^= Helper::read_u64t(bytes.subspan(i+16, 8));
                        state.d ^= Helper::read_u64t(bytes.subspan(i+24, 8));

                        state.a = Helper::diffuse(state.a);
                        state.b = Helper::diffuse(state.b);
                        state.c = Helper::diffuse(state.c);
                        state.d = Helper::diffuse(state.d);
                    }
                }

                auto size = orig_size - excessive;
                // handle excessive
                if ((0<excessive) && (excessive < 8)) {
                    state.a ^= Helper::read_u64t_m(bytes.subspan(size, excessive));
                    state.a = Helper::diffuse(state.a);
                } 
                if (excessive == 8) {
                    state.a ^= Helper::read_u64t(bytes.subspan(size, excessive));
                    state.a = Helper::diffuse(state.a);
                }
                if ((8 < excessive) &&  (excessive < 16)) {
                    state.a ^= Helper::read_u64t(bytes.subspan(size, 8));
                    state.b ^= Helper::read_u64t_m(bytes.subspan(size+8, excessive-8));

                    state.a = Helper::diffuse(state.a);
                    state.b = Helper::diffuse(state.b);
                } 
                if (excessive == 16) {
                    state.a ^= Helper::read_u64t(bytes.subspan(size, 8));
                    state.b ^= Helper::read_u64t(bytes.subspan(size+8, 8));

                    state.a = Helper::diffuse(state.a);
                    state.b = Helper::diffuse(state.b);
                }
                if ((16 < excessive) && (excessive < 24)) {
                    state.a ^= Helper::read_u64t(bytes.subspan(size, 8));
                    state.b ^= Helper::read_u64t(bytes.subspan(size+8, 8));
                    state.c ^= Helper::read_u64t_m(bytes.subspan(size+16, excessive-16));

                    state.a = Helper::diffuse(state.a);
                    state.b = Helper::diffuse(state.b);
                    state.c = Helper::diffuse(state.c);
                } 
                if (excessive == 24) {
                    state.a ^= Helper::read_u64t(bytes.subspan(size, 8));
                    state.b ^= Helper::read_u64t(bytes.subspan(size+8, 8));
                    state.c ^= Helper::read_u64t(bytes.subspan(size+16, 8));

                    state.a = Helper::diffuse(state.a);
                    state.b = Helper::diffuse(state.b);
                    state.c = Helper::diffuse(state.c);
                }
                if ((24 < excessive) && (excessive < 32)) {
                    state.a ^= Helper::read_u64t(bytes.subspan(size, 8));
                    state.b ^= Helper::read_u64t(bytes.subspan(size+8, 8));
                    state.c ^= Helper::read_u64t(bytes.subspan(size+16, 8));
                    state.d ^= Helper::read_u64t_m(bytes.subspan(size+8, excessive-24));

                    state.a = Helper::diffuse(state.a);
                    state.b = Helper::diffuse(state.b);
                    state.c = Helper::diffuse(state.c);
                    state.d = Helper::diffuse(state.d);
                } 
                return state.finish(orig_size);
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