#pragma once
#include "BigInt.h"
#include "DynamicArray.h"
#include "RNG.h"
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

    /** 
     * @brief Mảng động lưu dãy byte (hệ cơ số 2^8 - ), làm cầu nối giữa file và BigInt
     * @details Mỗi phần tử là 1 byte (giá trị từ 0 tới 255) - tương ứng 1 char trong bảng ASCII.
     * Cơ chế:
     * 1. Khi biểu diễn 1 con số: dùng Big-Endian ( cách ghi số tự nhiên của con người )
     * 2. Khi là bộ đệm của file: bytes[0] là byte đầu tiên đọc được từ file
     * Example: bytes = [0x01, 0x00] <-> 0x0100 = 256
     * @note 
     * 1. Quy ước: 1 phần tử của mảng gọi là 1 byte
     * 2. Khác BigInt (Little-Endian, khối 32-bit), ByteArray dùng base 256 để dễ đọc/ghi file
    */
class ByteArray {
private:
    dynamicArray<unsigned char> bytes;      // Dãy byte toàn bộ dãy byte sẽ là văn bản cần phải ghi]

// ------------------------------------------------------------------- PRIVATE FUNCTION  -------------------------------------------------------------------
 
    /** 
     * @brief 1. Đổi 1 ký tự Hex sang giá trị số (0..15)
     * @details Tra theo bảng: '0'..'9' -> 0..9 ; 'a'..'f' hoặc 'A'..'F' -> 10..15
     * @param c: ký tự Hex cần đổi
     * @return Giá trị số tương ứng (0..15); trả về 0 nếu ký tự không hợp lệ
     * @note 
     * 1. Phụ trợ cho fromBigInt()
     * 2. O(1)
    */
    unsigned char hexToVal(char c) const {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        }
        if (c >= 'A' && c <= 'F') {
            return c - 'A' + 10;
        }
        return 0;
    }
public:
// ------------------------------------------------------------------- CONSTRUCTOR  -------------------------------------------------------------------

    /** 
     * @brief 1. Khởi tạo mặc định
     * @note Tạo mảng byte rỗng
    */
    ByteArray(){}

// ------------------------------------------------------------------- METHOD  -------------------------------------------------------------------

    // A. Đọc / Ghi file ( giao tiếp với file )

    /** 
     * @brief 1. Đọc toàn bộ file vào mảng byte
     * @details 
     * 1. Mở file nhị phân
     * 2. Xóa dữ liệu cũ cho tái sử dụng 
     * 3. Đọc lần lượt từng byte tới hết file, push_back vào mảng
     * @param path: đường dẫn file 
     * @note 
     * 1. Bắt buộc mở binary
     * 2. bytes[0] là byte đầu tiên của file
    */
    void readFile(string path){
        //1. 
        ifstream file(path, ios::binary);
    
        //1.1. Xử lý ngoại lệ 
        if (!file.is_open()) {
            cout << "Loi: Khong the mo file de doc!" << endl;
            return;
        }
        
        // 2. 
        bytes.clear();

        // 3. 
        char c;
        while (file.get(c)) {
            this->bytes.push_back((unsigned char)c);            // Ép kiểu sang unsigned char (0 - 255)
        }

        file.close();
    }

    /** 
     * @brief 2. Ghi toàn bộ mảng byte ra file
     * @details 
     * 1. Mở file nhị phân
     * 2. Ghi lần lượt từng byte trong mảng ra file
     * @param path: đường dẫn file 
     * @note Tương ứng với readFile()
    */
    void writeFile(string path){
        //1. 
        ofstream out(path, ios::binary);
        if (!out.is_open()) {
            cout << "Loi: Khong the mo file de ghi!" << endl;
            return;
        }
 
        //2. 
        for (int i = 0; i < bytes.getsize(); i++) {
            out.put((char)bytes[i]);
        }
 
        out.close();
    }


    // B. Chuyển đổi ( giao tiếp với BigInt )

    /** 
     * @brief 1. Chuyển dãy byte thành 1 số BigInt
     * @details Đi qua chuỗi Hex để dùng fromHex từ BigInt 
     * 1. Duyệt từng byte ở dạng Big-Endian, đổi mỗi byte thành đúng 2 ký tự Hex ( 1 byte ( 8 bit ) = 2 hex ( 4 bit))
     * 2. Ghép thành 1 chuỗi Hex hoàn chỉnh
     * 3. Gọi fromHex để ra kết quả
     * @return BigInt mang giá trị của dãy byte
     * @note 
     * 1. Cầu nối: byte -> BigInt để đưa vào encrypt() / decrypt()
     * 2. Mảng rỗng -> BigInt = 0
     * 3. 1 nibble = 1 hex = 4 bit
    */
    BigInt toBigInt() const{
        //1. 
        string hex = "";
        string hexChars = "0123456789ABCDEF";   // Bảng tra ký tự Hex
        
        //2.
        for (int i = 0; i < bytes.getsize(); i++) {
            unsigned char b = bytes[i];
            hex += hexChars[(b >> 4) & 0xF];     // nibble cao
            hex += hexChars[b & 0xF];            // nibble thấp
        }
 
        //3. 
        BigInt conv;                             
        return conv.fromHex(hex);
    }

    /** 
     * @brief 2. Nạp giá trị 1 số BigInt vào dãy byte
     * @details Đi qua chuỗi Hex để tái dùng toHex
     * 1. Gọi toHex() để lấy chuỗi Hex
     * 2. Nếu chuỗi Hex lẻ ký tự thì thêm '0' vào đầu cho đủ cặp
     * 3. Cắt từng cặp 2 ký tự Hex -> 1 byte, lưu vào mảng 
     * 4. Nếu width > 0 mà còn thiếu byte thì đệm thêm byte 0x00 vào ĐẦU mảng cho đủ width
     * @param 
     * 1. num: số nguyên không âm BigInt cần chuyển 
     * 2. width: số byte mong muốn 
     * @note 
     * 1. Cầu nối: BigInt -> byte để ghi ra file
     * 2. width dùng để ép bản mã đúng k byte, và khôi phục byte 0 ở đầu khối bản rõ
     * 3. Đệm 0x00 vào đầu bằng cách push_back các byte 0 TRƯỚC, rồi mới push phần nội dung
    */
    void fromBigInt(const BigInt& num, int width = 0){
        //0. 
        bytes.clear();
 
        //1. 
        string hex = num.toHex();
 
        //2. 
        if (hex.length() % 2 != 0) {
            hex = "0" + hex;
        }
 
        //3. 
        int contentBytes = hex.length() / 2;     // số byte thực tế
        for (int i = contentBytes; i < width; i++) {
            bytes.push_back(0);
        }
 
        //4. 
        for (int i = 0; i < (int)hex.length(); i += 2) {
            unsigned char hi = hexToVal(hex[i]);
            unsigned char lo = hexToVal(hex[i + 1]);
            bytes.push_back((unsigned char)((hi << 4) | lo));
        }
    }


    // C. Thao tác khối

    /** 
     * @brief 1. Trích 1 khối con từ mảng byte
     * @details Sao chép đoạn [start, start + len) ra 1 ByteArray mới
     * @param 
     * 1. start: vị trí byte bắt đầu
     * 2. len: số byte cần lấy
     * @return res: ByteArray chứa khối vừa cắt
     * @note 
     * 1. Dùng để tách file lớn thành các khối nhỏ trước khi mã hóa
     * 2. Cũng dùng để cắt bỏ phần đệm thừa
    */
    ByteArray getBlock(int start, int len) const{
        ByteArray res;
        for (int i = 0; i < len; i++) {
            int idx = start + i;
            if (idx >= 0 && idx < bytes.getsize()) {
                res.bytes.push_back(bytes[idx]);
            }
        }
        return res;
    }
    /** 
     * @brief 2. Nối thêm 1 dãy byte vào cuối mảng
     * @details Duyệt toàn bộ byte của other và push_back vào mảng hiện tại
     * @param other: ByteArray cần nối thêm
     * @note Dùng để gom các khối kết quả thành 1 mảng đầu ra
    */
    void append(const ByteArray& other) {
        for (int i = 0; i < other.bytes.getsize(); i++) {
            bytes.push_back(other.bytes[i]);
        }
    }

  // D. Getter
 
    /** 
     * @brief 1. Lấy số lượng byte
     * @return Số byte đang lưu
     * @note O(1)
    */
    int getSize() const {
        return bytes.getsize();
    }
 
    /** 
     * @brief 2. Truy cập byte thứ i ( chỉ đọc )
     * @param i: vị trí byte cần lấy
     * @return Giá trị byte tại vị trí i
     * @note O(1)
    */
    unsigned char operator[](int i) const {
        return bytes[i];
    }
};