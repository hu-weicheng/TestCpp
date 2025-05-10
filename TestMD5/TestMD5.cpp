#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
//'sprintf': This function or variable may be unsafe.Consider using sprintf_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.See online help for details.
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

inline uint32_t rotate_left(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

inline uint32_t F(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) | (~x & z);
}

inline uint32_t G(uint32_t x, uint32_t y, uint32_t z) {
    return (x & z) | (y & ~z);
}

inline uint32_t H(uint32_t x, uint32_t y, uint32_t z) {
    return x ^ y ^ z;
}

inline uint32_t I(uint32_t x, uint32_t y, uint32_t z) {
    return y ^ (x | ~z);
}

static const uint32_t T[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

const int param_indices[4][16] = {
    {0,3,2,1,0,3,2,1,0,3,2,1,0,3,2,1},
    {3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2},
    {2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1},
    {1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0}
};

const int m_indices[4][16] = {
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
    {1,6,11,0,5,10,15,4,9,14,3,8,13,2,7,12},
    {5,8,11,14,1,4,7,10,13,0,3,6,9,12,15,2},
    {0,3,6,9,12,15,2,5,8,11,14,1,4,7,10,13}
};

void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) {
    a = b + rotate_left((a + F(b, c, d) + x + t), s);
}

void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) {
    a = b + rotate_left((a + G(b, c, d) + x + t), s);
}

void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) {
    a = b + rotate_left((a + H(b, c, d) + x + t), s);
}

void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) {
    a = b + rotate_left((a + I(b, c, d) + x + t), s);
}

vector<uint8_t> pad_data(const uint8_t* input, size_t nBytes) {
    size_t originalLengthBits = nBytes * 8;
    size_t newLength = nBytes + 1;
    while (newLength % 64 != 56) {
        newLength++;
    }
    vector<uint8_t> data(newLength + 8);
    memcpy(data.data(), input, nBytes);
    data[nBytes] = 0x80;
    for (int i = 0; i < 8; ++i) {
        data[newLength + i] = (originalLengthBits >> ((7 - i) * 8)) & 0xFF;
    }
    return data;
}

void process_block(const uint8_t* block, uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    uint32_t M[16];
    for (int i = 0; i < 16; ++i) {
        M[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16) | (block[i * 4 + 2] << 8) | block[i * 4 + 3];
    }

    for (int round = 0; round < 4; ++round) {
        for (int step = 0; step < 16; ++step) {
            int s = param_indices[round][step];
            int m_index = m_indices[round][step];
            uint32_t x = M[m_index];
            switch (round) {
            case 0: FF(a, b, c, d, x, s, T[step]); break;
            case 1: GG(d, a, b, c, x, s, T[16 + step]); break;
            case 2: HH(c, d, a, b, x, s, T[32 + step]); break;
            case 3: II(b, c, d, a, x, s, T[48 + step]); break;
            }
        }
    }
}

string md5(const void* input, size_t nBytes) {
    uint32_t a = 0x67452301;
    uint32_t b = 0xEFCDAB89;
    uint32_t c = 0x98BADCFE;
    uint32_t d = 0x10325476;

    vector<uint8_t> data = pad_data(static_cast<const uint8_t*>(input), nBytes);

    for (size_t i = 0; i < data.size(); i += 64) {
        process_block(data.data() + i, a, b, c, d);
    }

    uint8_t digest[16];
    memcpy(digest, &a, 4);
    memcpy(digest + 4, &b, 4);
    memcpy(digest + 8, &c, 4);
    memcpy(digest + 12, &d, 4);

    char hex[33];
    for (int i = 0; i < 16; ++i) {
        sprintf(hex + i * 2, "%02x", digest[i]);
    }
    return string(hex);
}

int main() 
{
    const char* test_str = "hello world";
    cout << "MD5(\""<<test_str<<"\") = " << md5(test_str, strlen(test_str)) << endl;
    return 0;
}