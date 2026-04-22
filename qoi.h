#ifndef QOI_FORMAT_CODEC_QOI_H_
#define QOI_FORMAT_CODEC_QOI_H_

#include "utils.h"

constexpr uint8_t QOI_OP_INDEX_TAG = 0x00;
constexpr uint8_t QOI_OP_DIFF_TAG  = 0x40;
constexpr uint8_t QOI_OP_LUMA_TAG  = 0x80;
constexpr uint8_t QOI_OP_RUN_TAG   = 0xc0;
constexpr uint8_t QOI_OP_RGB_TAG   = 0xfe;
constexpr uint8_t QOI_OP_RGBA_TAG  = 0xff;
constexpr uint8_t QOI_PADDING[8] = {0u, 0u, 0u, 0u, 0u, 1u};
constexpr uint8_t QOI_MASK_2 = 0xc0;

bool QoiEncode(uint32_t width, uint32_t height, uint8_t channels, uint8_t colorspace) {

    QoiWriteChar('q');
    QoiWriteChar('o');
    QoiWriteChar('i');
    QoiWriteChar('f');
    QoiWriteU32(width);
    QoiWriteU32(height);
    QoiWriteU8(channels);
    QoiWriteU8(colorspace);

    int run = 0;
    int px_num = width * height;

    uint8_t history[64][4];
    memset(history, 0, sizeof(history));

    uint8_t r, g, b, a;
    a = 255u;
    uint8_t pre_r, pre_g, pre_b, pre_a;
    pre_r = 0u;
    pre_g = 0u;
    pre_b = 0u;
    pre_a = 255u;

    for (int i = 0; i < px_num; ++i) {
        uint8_t curr_r = QoiReadU8();
        uint8_t curr_g = QoiReadU8();
        uint8_t curr_b = QoiReadU8();
        uint8_t curr_a = a;
        if (channels == 4) curr_a = QoiReadU8();

        if (curr_r == pre_r && curr_g == pre_g && curr_b == pre_b && curr_a == pre_a) {
            run++;
            if (run == 62 || i == px_num - 1) {
                QoiWriteU8(QOI_OP_RUN_TAG + (run - 1));
                run = 0;
            }
        } else {
            if (run > 0) {
                QoiWriteU8(QOI_OP_RUN_TAG + (run - 1));
                run = 0;
            }

            int idx = QoiColorHash(curr_r, curr_g, curr_b, curr_a);
            if (history[idx][0] == curr_r && history[idx][1] == curr_g &&
                history[idx][2] == curr_b && history[idx][3] == curr_a) {
                QoiWriteU8(QOI_OP_INDEX_TAG + idx);
            } else {
                int dr = (int)curr_r - (int)pre_r;
                int dg = (int)curr_g - (int)pre_g;
                int db = (int)curr_b - (int)pre_b;
                int da = (int)curr_a - (int)pre_a;

                if (dr >= -2 && dr <= 1 && dg >= -2 && dg <= 1 &&
                    db >= -2 && db <= 1 && da == 0) {
                    QoiWriteU8(QOI_OP_DIFF_TAG + ((dr + 2) << 4) + ((dg + 2) << 2) + (db + 2));
                } else {
                    int dr_dg = dr - dg;
                    int db_dg = db - dg;
                    if (dg >= -32 && dg <= 31 && dr_dg >= -8 && dr_dg <= 7 &&
                        db_dg >= -8 && db_dg <= 7 && da == 0) {
                        QoiWriteU8(QOI_OP_LUMA_TAG + (dg + 32));
                        QoiWriteU8(((dr_dg + 8) << 4) + (db_dg + 8));
                    } else if (da == 0) {
                        QoiWriteU8(QOI_OP_RGB_TAG);
                        QoiWriteU8(curr_r);
                        QoiWriteU8(curr_g);
                        QoiWriteU8(curr_b);
                    } else {
                        QoiWriteU8(QOI_OP_RGBA_TAG);
                        QoiWriteU8(curr_r);
                        QoiWriteU8(curr_g);
                        QoiWriteU8(curr_b);
                        QoiWriteU8(curr_a);
                    }
                }
            }

            if (i > 0) {
                int prev_idx = QoiColorHash(pre_r, pre_g, pre_b, pre_a);
                history[prev_idx][0] = pre_r;
                history[prev_idx][1] = pre_g;
                history[prev_idx][2] = pre_b;
                history[prev_idx][3] = pre_a;
            }

            pre_r = curr_r;
            pre_g = curr_g;
            pre_b = curr_b;
            pre_a = curr_a;
        }
    }

    for (int i = 0; i < sizeof(QOI_PADDING) / sizeof(QOI_PADDING[0]); ++i) {
        QoiWriteU8(QOI_PADDING[i]);
    }

    return true;
}

bool QoiDecode(uint32_t &width, uint32_t &height, uint8_t &channels, uint8_t &colorspace) {

    char c1 = QoiReadChar();
    char c2 = QoiReadChar();
    char c3 = QoiReadChar();
    char c4 = QoiReadChar();
    if (c1 != 'q' || c2 != 'o' || c3 != 'i' || c4 != 'f') {
        return false;
    }

    width = QoiReadU32();
    height = QoiReadU32();
    channels = QoiReadU8();
    colorspace = QoiReadU8();

    int run = 0;
    int px_num = width * height;

    uint8_t history[64][4];
    memset(history, 0, sizeof(history));

    uint8_t r, g, b, a;
    r = 0u;
    g = 0u;
    b = 0u;
    a = 255u;

    for (int i = 0; i < px_num; ++i) {
        if (run > 0) {
            run--;
        } else {
            uint8_t op = QoiReadU8();

            if (op == QOI_OP_RGB_TAG) {
                r = QoiReadU8();
                g = QoiReadU8();
                b = QoiReadU8();
            } else if (op == QOI_OP_RGBA_TAG) {
                r = QoiReadU8();
                g = QoiReadU8();
                b = QoiReadU8();
                a = QoiReadU8();
            } else if ((op & QOI_MASK_2) == QOI_OP_INDEX_TAG) {
                int idx = op;
                r = history[idx][0];
                g = history[idx][1];
                b = history[idx][2];
                a = history[idx][3];
            } else if ((op & QOI_MASK_2) == QOI_OP_DIFF_TAG) {
                int dr = ((op >> 4) & 0x03) - 2;
                int dg = ((op >> 2) & 0x03) - 2;
                int db = (op & 0x03) - 2;
                r = (r + dr) & 0xff;
                g = (g + dg) & 0xff;
                b = (b + db) & 0xff;
            } else if ((op & QOI_MASK_2) == QOI_OP_LUMA_TAG) {
                int dg = (op & 0x3f) - 32;
                uint8_t b2 = QoiReadU8();
                int dr_dg = ((b2 >> 4) & 0x0f) - 8;
                int db_dg = (b2 & 0x0f) - 8;
                r = (r + dg + dr_dg) & 0xff;
                g = (g + dg) & 0xff;
                b = (b + dg + db_dg) & 0xff;
            } else if ((op & QOI_MASK_2) == QOI_OP_RUN_TAG) {
                run = (op & 0x3f);
            }
        }

        int idx = QoiColorHash(r, g, b, a);
        history[idx][0] = r;
        history[idx][1] = g;
        history[idx][2] = b;
        history[idx][3] = a;

        QoiWriteU8(r);
        QoiWriteU8(g);
        QoiWriteU8(b);
        if (channels == 4) QoiWriteU8(a);
    }

    bool valid = true;
    for (int i = 0; i < sizeof(QOI_PADDING) / sizeof(QOI_PADDING[0]); ++i) {
        if (QoiReadU8() != QOI_PADDING[i]) valid = false;
    }

    return valid;
}

#endif // QOI_FORMAT_CODEC_QOI_H_
