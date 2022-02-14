#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Converts a string to its binary representation.
 * 
 * @param s string to be converted
 * @return std::vector<bool>
 */
std::vector<bool> to_bits(const std::string &s)
{
    std::vector<bool> bits(8 * s.size());
    for (size_t i = 0; i < s.size(); i++)
        for (size_t j = 0; j < 8; j++)
            bits[i * 8 + j] = (s[i] >> (7 - j)) & 1;
    return bits;
}

/**
 * @brief Converts a binary sequence to its string representation.
 * 
 * @param bits binary representation to be converted
 * @return std::string
 */
std::string to_string(const std::vector<bool> &bits)
{
    std::string s((bits.size() + 7) / 8, '\0');
    for (size_t i = 0; i < s.size(); i++)
        for (size_t j = 0; j < 8; j++)
            s[i] |= bits[i * 8 + j] << (7 - j);
    return s;
}

/**
 * @brief Calculates ceil(log2(n+1))
 * 
 * @param n 
 * @return unsigned int 
 */
unsigned int inc_log_ceil(unsigned int n)
{
    unsigned int y = 1;

    while (n >>= 1)
        y++;

    return y;
}

/**
 * @brief Encodes a string using the Hamming code.
 * 
 * @param payload string to be encoded
 * @return std::string 
 */
std::string hamming_encode(const std::string &payload)
{
    const std::vector<bool> bits = to_bits(payload);

    // k: number of data bits
    // r: number of parity bits
    const size_t k = bits.size();
    size_t r = 2;
    while (1ULL << r < k + r + 1ULL)
        r++;

    std::vector<bool> parity_included(k + r);
    size_t j = 0;

    // Initialize coded-string bits, parity bits (power of 2 - 0 indexed) being set to 0
    for (size_t i = 0; i < k + r; i++)
        parity_included[i] = (i & (i + 1)) && bits[j++];

    // For every bit, see which parity bits it affects and flip them if the bit is 1
    for (size_t i = 0; i < k + r; i++)
        if (parity_included[i])
            for (size_t j = 0; j < r; j++)
                if (((i + 1) >> j) & 1)
                    parity_included[(1 << j) - 1].flip();

    return to_string(parity_included);
}

/**
 * @brief Decodes a Hamming code-encoded string, correcting one-bit error if exists.
 * 
 * @param payload string to be decoded
 * @return std::pair<int, std::string> 
 */
std::pair<int, std::string> hamming_decode(const std::string &payload)
{
    std::vector<bool> bits = to_bits(payload);

    // n: number of data + parity bits
    // r: number of parity bits
    size_t n = bits.size();
    size_t r = inc_log_ceil(n);

    // For every data bit, see which parity bits it affects and flip them if the bit is 1
    for (size_t i = 0; i < n; i++)
        if (bits[i] && (i & (i + 1)))
            for (size_t j = 0; j < r; j++)
                if (((i + 1) >> j) & 1)
                    bits[(1 << j) - 1].flip();

    std::vector<bool> message(n - r);
    int error_idx = -1;
    size_t j = 0;

    // Add data bits (non power of 2) to message, add set parity bits' 1-based indices to error_idx
    for (size_t i = 0; i < n; i++)
        if (i & (i + 1))
            message[j++] = bits[i];
        else if (bits[i])
            error_idx += i + 1;

    // If there is an errored data bit, flip it
    if (error_idx & (error_idx + 1))
        message[error_idx - inc_log_ceil(error_idx)].flip();

    return {error_idx, to_string(message)};
}
