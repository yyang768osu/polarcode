#include "polar_encoder.hpp"
#include "utilities.hpp"
#include <vector>
#include <assert.h>

void polar_encoder_helper(std::vector<int> &bit_sequence, int start, int len)
{
    int half_len = len / 2;
    for (int i = 0; i < half_len; i++)
    {
        bit_sequence[i + start] = bit_sequence[i + start] ^ bit_sequence[start + i + half_len];
    }
    if (half_len)
    {
        polar_encoder_helper(bit_sequence, start, half_len);
        polar_encoder_helper(bit_sequence, start + half_len, half_len);
    }
}

void polar_encoder(std::vector<int> &bit_sequence)
{
    int len = bit_sequence.size();
    assert((len & (len - 1)) == 0);
    polar_encoder_helper(bit_sequence, 0, len);
}
