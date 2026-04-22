#include <iostream>
#include <cstdint>

constexpr uint8_t QOI_OP_DIFF_TAG  = 0x40;
constexpr uint8_t QOI_OP_LUMA_TAG  = 0x80;

int main() {
    // Test case: prev=(234,234,234), curr=(209,209,209)
    uint8_t pre_r = 234, pre_g = 234, pre_b = 234;
    uint8_t r = 209, g = 209, b = 209;
    
    int dr = (int)r - (int)pre_r;
    int dg = (int)g - (int)pre_g;
    int db = (int)b - (int)pre_b;
    
    std::cout << "dr=" << dr << " dg=" << dg << " db=" << db << std::endl;
    
    // DIFF check
    bool diff_ok = (dr >= -2 && dr <= 1 && dg >= -2 && dg <= 1 &&
                  db >= -2 && db <= 1);
    std::cout << "DIFF ok: " << diff_ok << std::endl;
    
    // LUMA check
    bool luma_ok = (dg >= -32 && dg <= 31 && dr >= -8 && dr <= 7 &&
                   db >= -8 && db <= 7);
    std::cout << "LUMA ok: " << luma_ok << std::endl;
    
    return 0;
}
