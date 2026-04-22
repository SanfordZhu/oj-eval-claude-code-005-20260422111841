#include <iostream>
#include <cstdint>

int main() {
    // First few pixels from testcard.ppm
    uint8_t pixels[][3] = {
        {211, 211, 211},  // pixel 0
        {178, 178, 178},  // pixel 1
        {82, 82, 82},     // pixel 2
        {87, 87, 87},     // pixel 3
        {101, 101, 101},  // pixel 4
        {225, 225, 225},  // pixel 5
        {230, 230, 230},  // pixel 6
        {234, 234, 234},  // pixel 7
        {209, 209, 209},  // pixel 8
        {88, 88, 88},     // pixel 9
    };
    
    uint8_t pre_r = 0, pre_g = 0, pre_b = 0;
    
    for (int i = 0; i < 10; i++) {
        uint8_t r = pixels[i][0];
        uint8_t g = pixels[i][1];
        uint8_t b = pixels[i][2];
        
        int dr = (int)r - (int)pre_r;
        int dg = (int)g - (int)pre_g;
        int db = (int)b - (int)pre_b;
        
        std::cout << "Pixel " << i << ": (" << (int)r << "," << (int)g << "," << (int)b << ") ";
        std::cout << "dr=" << dr << " dg=" << dg << " db=" << db << " ";
        
        bool diff_ok = (dr >= -2 && dr <= 1 && dg >= -2 && dg <= 1 &&
                      db >= -2 && db <= 1);
        bool luma_ok = (dg >= -32 && dg <= 31 && dr >= -8 && dr <= 7 &&
                       db >= -8 && db <= 7);
        
        if (diff_ok) std::cout << "DIFF";
        else if (luma_ok) std::cout << "LUMA";
        else std::cout << "RGB";
        std::cout << std::endl;
        
        pre_r = r; pre_g = g; pre_b = b;
    }
    return 0;
}
