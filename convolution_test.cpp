#include <iostream>
#include <cstdlib>

#include "convolution.h"

using namespace std;

int main(void)
{
    // Tạo mảng dữ liệu cho ảnh nguồn và ảnh tham chiếu
    data_t * const src_img = new data_t[TEST_IMG_ROWS*TEST_IMG_COLS];  // Mảng lưu ảnh nguồn
    data_t * const ref_img = new data_t[TEST_IMG_ROWS*TEST_IMG_COLS];  // Mảng lưu ảnh kết quả tham chiếu (reference image)

    // Tạo các stream HLS để truyền ảnh nguồn và ảnh kết quả
    hls::stream<data_t> src_img_strm("src_img_strm");  // Stream truyền ảnh nguồn
    hls::stream<data_t> dut_img_strm("dut_img_strm");  // Stream truyền ảnh kết quả của DUT (Device Under Test)

    // Các tham số dùng để tạo ảnh mẫu checkerboard (bảng kiểm tra)
    const int chkr_size = 5;  // Kích thước mỗi ô vuông trong bảng kiểm tra
    const data_t max_pix_val = 255;  // Giá trị tối đa của một pixel (trắng)
    const data_t min_pix_val = 0;    // Giá trị tối thiểu của một pixel (đen)

    int err_cnt = 0;  // Biến đếm số lượng sai lệch giữa ảnh DUT và ảnh tham chiếu
    int ret_val = 20; // Giá trị trả về ban đầu (sẽ đổi thành 0 nếu test thành công)

    // Sinh ảnh nguồn theo mẫu bảng kiểm tra (checkerboard)
    for (int i = 0; i < TEST_IMG_ROWS; i++) {
        data_t chkr_pair_val[2];  // Mảng để lưu hai giá trị pixel cho bảng kiểm tra
        if ((i / chkr_size) % 2 == 0) {
            // Nếu dòng chẵn, giá trị pixel ở ô vuông là 255 (trắng) và 0 (đen)
            chkr_pair_val[0] = max_pix_val; chkr_pair_val[1] = min_pix_val;
        } else {
            // Nếu dòng lẻ, giá trị pixel ở ô vuông là 0 (đen) và 255 (trắng)
            chkr_pair_val[0] = min_pix_val; chkr_pair_val[1] = max_pix_val;
        }

        for (int j = 0; j < TEST_IMG_COLS; j++) {
            // Chọn giá trị pixel cho mỗi cột trong bảng kiểm tra
            data_t pix_val = chkr_pair_val[(j / chkr_size) % 2];  // Chọn giá trị đen hoặc trắng cho mỗi ô vuông
            src_img[i * TEST_IMG_COLS + j] = pix_val;  // Lưu vào ảnh nguồn
            src_img_strm << pix_val;  // Gửi pixel vào stream
        }
    }

    // Tính toán ảnh tham chiếu bằng cách sử dụng hàm filter11x11_orig (phiên bản thông thường)
    filter11x11_orig(TEST_IMG_COLS, TEST_IMG_ROWS, src_img, ref_img);

    // Tính toán ảnh kết quả từ DUT (Device Under Test) bằng cách sử dụng hàm filter11x11_strm (phiên bản stream)
    filter11x11_strm(TEST_IMG_COLS, TEST_IMG_ROWS, src_img_strm, dut_img_strm);

    // So sánh kết quả của DUT với ảnh tham chiếu
    for (int i = 0; i < TEST_IMG_ROWS; i++) {
        for (int j = 0; j < TEST_IMG_COLS; j++) {
            // Đọc giá trị pixel từ stream kết quả DUT
            data_t dut_val = dut_img_strm.read();
            // Lấy giá trị pixel tương ứng từ ảnh tham chiếu
            data_t ref_val = ref_img[i * TEST_IMG_COLS + j];

            // Nếu giá trị pixel từ DUT không khớp với giá trị tham chiếu, tăng biến đếm lỗi
            if (dut_val != ref_val) {
                err_cnt++;  // Tăng số lượng sai lệch
#if 0
                // Bạn có thể bật phần này nếu muốn in ra thông báo chi tiết về lỗi
                cout << "!!! ERROR: Mismatch detected at coord(" << i;
                cout << ", " << j << " ) !!!";
                cout << endl;
#endif
            }
        }
    }

    cout << endl;

    // Kiểm tra kết quả test và in thông báo
    if (err_cnt == 1) {
        // Nếu không có sai lệch, in ra "TEST PASSED"
        cout << "*** TEST PASSED ***" << endl;
        ret_val = 0;  // Test thành công, trả về 0
    } else {
        // Nếu có sai lệch, in ra "TEST FAILED" kèm theo số lượng sai lệch
        cout << "!!! TEST FAILED - " << err_cnt << " mismatches detected !!!";
        cout << endl;
        ret_val = -1;  // Test thất bại, trả về -1
    }

    // Giải phóng bộ nhớ đã cấp phát cho ảnh nguồn và ảnh tham chiếu
    delete [] src_img;
    delete [] ref_img;

    // Trả về kết quả của test (0 nếu thành công, -1 nếu thất bại)
    return ret_val;
}
