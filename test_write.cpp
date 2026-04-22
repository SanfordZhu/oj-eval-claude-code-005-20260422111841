#include <iostream>
#include <cstdint>

constexpr uint8_t QOI_OP_RGB_TAG = 0xfe;

inline void QoiWriteU8(uint8_t buf) {
    std::cout.write(reinterpret_cast<const char *>(&buf), sizeof(buf));
}

int main() {
    uint8_t r = 211;
    std::cout << "Writing RGB tag: " << std::hex << (int)QOI_OP_RGB_TAG << std::endl;
    QoiWriteU8(QOI_OP_RGB_TAG);
    std::cout << "Writing r: " << std::hex << (int)r << std::endl;
    QoiWriteU8(r);
    std::cout << std::endl;
    return 0;
}
