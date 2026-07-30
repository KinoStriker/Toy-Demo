#pragma once
#include <iostream>
#include <ctime>
#include "BigInt.h" 

using namespace std;

/** 
 * @brief Cấu trúc sinh mã pseudorandom
 * @details Dựa trên thuật toán Linear Congruential Generator ( LCG )
*/
class RNG {
private:
    unsigned long long state; 

    // Hằng số cho LCG
    const unsigned long long a = 6364136223846793005ULL;
    const unsigned long long c = 1442695040888963407ULL;


// ------------------------------------------------------------------- PRIVATE FUNCTION  -------------------------------------------------------------------

    /** 
     * @brief Hàm chạy một lượt của LCG
     * @note 
     * 1. Máy tính sẽ tự động mod 2^64 do tính chất overflow của unsigned long long 
     * 2. O(1)
    */    
   void step() {
        state = a * state + c; 
    }

public:
// ------------------------------------------------------------------- CONSTRUCTOR  -------------------------------------------------------------------
    /** 
     * @brief 1. Constructor khởi tạo mặc định
     * @details Tạo seed dựa trên thời gian thực
     * @note O(1)
    */  
    RNG() {
        state = (unsigned long long)time(0);
    }

    /** 
     * @brief 2. Constructor khởi tạo thủ công
     * @details Tạo seed do người dùng nhập vào
     * @param s: seed do người dùng nhập vào
     * @note O(1)
    */      
   RNG(unsigned long long s) {
        state = s;
    }
// ------------------------------------------------------------------- METHOD  -------------------------------------------------------------------

    /** 
     * @brief 1. Thiết lập lại seed mới 
     * @details Thay thế seed cũ ban đầu
     * @param s: seed do người dùng nhập vào
     * @note O(1)
    */     
   void seed(unsigned long long s) {
        state = s;
    }

    /** 
     * @brief 2. Tạo mã ngẫu nhiên 32 bit
     * @details Lấy 32 bit cuối của biến "state"
     * @note O(1)
    */        
   unsigned int next32() {
        step();
        return (unsigned int)(state & 0xFFFFFFFF);
    }

    /** 
     * @brief 2. Tạo mã ngẫu nhiên 64 bit
     * @details Lấy toàn bộ biến "state"
     * @note O(1)
    */      
   unsigned long long next64() {
        step();
        return state;
    }

    /** 
     * @brief Hàm sinh số BigInt ngẫu nhiên theo số bit chỉ định
     * @details Quá trình sinh diễn ra bằng cách ghép các khối 32-bit lại với nhau:
     * 1. Tính toán số lượng khối 32-bit trọn vẹn cần sinh (chunks = bits / 32).
     * 2. Lặp để sinh từng khối: Dịch bit và cộng khối mới vào.
     * 3. Tính toán số lượng bit lẻ còn dư (remaining = bits % 32).
     * 4. Nếu có bit dư, sinh thêm 1 khối, cắt bỏ các bit thừa, dịch trái tương ứng và ghép nốt vào kết quả.
     * @param bits Số lượng bit mong muốn của số BigInt cần sinh ra
     * @return Trả về một đối tượng BigInt chứa giá trị ngẫu nhiên
     * @note 
     * 1. O(n^2) với n là số lượng khối
    */    
   BigInt nextBigInt(int bits) {

        //0. Khởi tạo và xử lý ngoại lệ
        BigInt res(0); 
        
        if (bits <= 0) return res;
        //0.1. Khởi tạo hằng số 2^32 --- Nhân hằng số tương đương với phép dịch trái 32 bit
        BigInt two(2LL);
        BigInt shift_factor = two ^ 32;

        //1. 
        int chunks = bits / 32;

        //2.
        for (int i = 0; i < chunks; ++i) {
            long long safe_block = (long long)next32(); 
            BigInt block(safe_block);
            res = (res * shift_factor) + block;                
        }

        //3.
        int remaining = bits % 32;

        //4.
        if (remaining > 0) {
            // Cắt bỏ các bit dư thừa bằng phép dịch phải
            long long safe_mask = (long long)(next32() >> (32 - remaining));
            
            BigInt shift_rem(1LL << remaining); // 2^remaining
            BigInt block_rem(safe_mask);
            
            res = (res * shift_rem) + block_rem;           
        }

        return res;
    }
};