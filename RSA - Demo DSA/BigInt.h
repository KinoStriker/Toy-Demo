#pragma once
#include <iostream>
#include <string>
#include "DynamicArray.h" 

using namespace std;

    /** 
     * @brief 
     * @details 
     * @param
     * @return
     * @note
    */

    /** 
     * @brief Mảng động lưu số nguyên lớn, mỗi 1 khối chứa 32 bit
     * @details Mảng chứa các chữ số hệ cơ số 2^32. 
     * Cơ chế: Lưu theo chuẩn Little Endian ( từ thấp tới cao ) --- Sử dụng cho việc cộng/trừ/nhân theo từng đơn vị
     * Example: digits = [2,1,3] <-> 2 + 1*2^32 + 3*2^64
     * @note Quy ước: 1 phần tử của mảng dynamic array gọi là 1 khối
    */
class BigInt {
private:
    dynamicArray<unsigned int> digits;      // Mảng chứa các chữ số hệ cơ số 2^32
    int sign;                               // Quy tắc: +1 khi dương, 0 khi bằng không, -1 nếu âm

// ------------------------------------------------------------------- PRIVATE FUNCTION  -------------------------------------------------------------------
    
    /** 
     * @brief 1. Chuẩn hóa dữ liệu 
     * @details 
     * 1. Xóa các khối 32 bit chỉ chứa 0
     * 2. Đưa sign về 0 nếu toàn bộ số có giá trị bằng 0 
     * @note 
    */
    void normalize() {
    while (digits.getsize() > 0 && digits[digits.getsize() - 1] == 0) {
        digits.pop_back(); 
    }
    
    if (digits.getsize() == 0) {
        sign = 0;
    }
} 

    /** 
     * @brief 2. Cộng giá trị tuyệt đối của 2 số BigInt ( bỏ qua dấu )
     * @details Dùng vòng lặp cộng từng khối 32-bit từ đuôi lên đầu
     * 1. Tìm chiều dài lớn nhất của 2 số
     * 2. Dùng vòng lặp cộng từng khối 32-bit từ đuôi lên đầu
     * 3. Xử lý carry cuối cùng 
     * 4. Chuẩn hóa
     * @param b: số BigInt cần cộng
     * @return res: res = |A| + |B|
     * @note 
     * 1. Phụ trợ cho nạp chồng toán tử "+"
     * 2. x& 0xFFFFFFFF: Lấy đúng 32 bit phần đuôi của x
    */
    BigInt addAbs( const BigInt& B) const {
        //0. Khởi tạo                       
        BigInt res;                         // Biến kết quả     
        res.sign = 1;
        unsigned long long carry = 0;       // Biến nhớ
        
        //1. 
        int lenA = this->digits.getsize();
        int lenB = B.digits.getsize();
        int maxSize = (lenA > lenB) ? lenA : lenB;

        //2. 
        for (int i = 0; i < maxSize; i++) {
            //2.1. Cộng
            unsigned long long sum = carry; 
            
            if (i < lenA) {
                sum += this->digits[i]; 
            }
            
            if (i < lenB) {
                sum += B.digits[i];
            }

            //2.2. Nhập kết quả
            res.digits.push_back((unsigned int)(sum & 0xFFFFFFFF));

            //2.3. Phần dư theo 32 bit cho phần tiếp theo
            carry = sum >> 32;
    }
        //3. 
        if (carry > 0) {
                res.digits.push_back((unsigned int)carry);
            }
        
        //4. Chuẩn hóa
        res.normalize();

        return res;
}

    /** 
     * @brief 3. So sánh giá trị tuyệt đối |A| và |B|
     * @details Dùng vòng lặp trừ từng khối 32-bit từ đuôi lên đầu
     * 1. So sánh theo độ dài
     * 2. Nếu fail thì tiếp tục so sánh theo phần tử tại block cao nhất
     * 3. Nếu fail tiếp thì trả về false
     * @return res: bool xác định bởi
     * 1. Nếu |A| < |B| thì true
     * 2. Nếu |A| >= |B| thì false
     * @note 
     * 1. Phụ trợ cho nạp chồng toán tử "<"
    */
    bool isAbsoluteLess(const BigInt& B) const {
        // 1.
        if (this->digits.getsize() < B.digits.getsize()) {
            return true;
        } 
        if (this->digits.getsize() > B.digits.getsize()) {
            return false;
        }
        
        // 2. 
        for (int i = this->digits.getsize() - 1; i >= 0; i--) {
            if (this->digits[i] < B.digits[i]) {
                return true;  // |this| < |B|
            } else if (this->digits[i] > B.digits[i]) {
                return false; // |this| > |B|
            }
    }
    
    //3.
    return false;
    }

    /** 
     * @brief 4. Trừ giá trị tuyệt đối ( chênh lệch ) của 2 số BigInt ( bỏ qua dấu )
     * @details 
     * 1. So sánh theo giá trị tuyệt đối của A,B
     * 2. Tìm chiều dài lớn nhất của 2 số
     * 3. Dùng vòng lặp trừ từng khối 32-bit từ đuôi lên đầu. Nếu không đủ thì mượn 1 borrow từ khối đằng sau
     * 4. Chuẩn hóa
     * @param b: số BigInt cần trừ đi
     * @return res: res = | |A| - |B| |
     * @note 
     * 1. Phụ trợ cho nạp chồng toán tử "-"
     * 2. 0x100000000LL: Mượn 1 đơn vị từ block kể tiếp
     * Example: Cơ số 10 thì 4-7 < 0 ---> 4-7+10 > 0. Khi này, diff += 10 và borrow = 1 
    */
    BigInt subAbs(const BigInt& B) const {
        //0. Khởi tạo                       
        BigInt res;                             // Biến kết quả     
        res.sign = 1;
        unsigned long long borrow = 0;          // Biến nhớ
        
        //1.
        const BigInt* larger = this;
        const BigInt* smaller = &B;

        bool isLess = false;
        //1.
        if (this->isAbsoluteLess(B)) {
            larger = &B;
            smaller = this;
        }

        //2. 
        int lenLarger = larger->digits.getsize();
        int lenSmaller = smaller->digits.getsize();

        //3. 
        for (int i = 0; i < lenLarger; i++) {            
            //3.1. Trừ
            long long diff = -borrow; 
            
            diff += larger->digits[i];              //Luôn tìm được chữ số của số dài nhất
            
            
            if (i < lenSmaller) {
                diff -= smaller->digits[i];
            }

            //3.2. Xử lý mượn 
            if (diff < 0) {
                diff += 0x100000000LL;              // Mượn 1 khối kế tiếp 
                borrow = 1;
            } else {
                borrow = 0;
            }

            //3.3. Nhập kết quả 
            res.digits.push_back((unsigned int)diff);
        }
        
        //4. Chuẩn hóa
        res.normalize();

        return res;
    }

    /** * @brief 5. Nhân giá trị tuyệt đối của 2 số BigInt ( bỏ qua dấu )
     * @details Nhân từng khối 32-bit.
     * 1. Khởi tạo BigInt có độ dài lớn nhất của tích là lenA + lenB. Khởi tạo mảng kết quả với toàn số 0.
     * 2. 2 vòng lặp lồng nhau nhân từng khối và cộng dồn vào ô [i + j]
     * 3. Chuẩn hóa kết quả
     * @param B: số BigInt cần nhân
     * @return res: res = |A| * |B|
     * @note 
     * 1. Phụ trợ cho nạp chồng toán tử "*"
     * 2. Ép kiểu (unsigned long long) để không bị tràn khi nhân 2 số 32-bit 
     * 3. x& 0xFFFFFFFF: Lấy 32 bit ở phần đuôi kết quả
     */
    BigInt mulAbs(const BigInt& B) const {    
        // 0. Khởi tạo 
        BigInt res;
        res.sign = 1;
        
        int lenA = this->digits.getsize();
        int lenB = B.digits.getsize();

        // 1. 
        for (int i = 0; i < lenA + lenB; i++) {
            res.digits.push_back(0); 
        }

        // 2. 
        for (int i = 0; i < lenA; i++) {
            unsigned long long carry = 0; // Biến nhớ cho từng hàng
            
            for (int j = 0; j < lenB; j++) {
                // 2.1. Tính tổng: Tích 2 khối + Giá trị đang có sẵn ở cột hiện tại + Biến nhớ
                unsigned long long product = (unsigned long long)this->digits[i] * B.digits[j] + res.digits[i + j] + carry;
                
                // 2.2. 
                res.digits[i + j] = (unsigned int)(product & 0xFFFFFFFF);
                
                // 2.3. Lấy phần còn dư
                carry = product >> 32;
            }
            
            // 2.4. Xử lý phần dư
            if (carry > 0) {
                res.digits[i + lenB] += (unsigned int)carry;
            }
        }

        // 3. 
        res.normalize();

        return res;    
    }


    /** 
     * @brief 6. Chia lấy dư giá trị tuyệt đối của 2 số BigInt (bỏ qua dấu)
     * @details Dùng thuật toán chia nhị phân (Long Division).
     * 1. Duyệt từng bit của số bị chia (this) từ cao xuống thấp.
     * 2. Dịch trái số dư R thêm 1 bit, hạ bit tiếp theo của this xuống.
     * 3. Nếu số dư R >= B, trừ R cho B (R = R - B).
     * 4. Chuẩn hóa
     * @param B: số chia (BigInt)
     * @return res: res = |A| % |B|
     * @note 
     * Phụ trợ cho nạp chồng toán tử "%"
     * R.addAbs(R) tương đương với R * 2 (dịch trái 1 bit)
     */
    BigInt modAbs(const BigInt& B) const {
        //0. Khởi tạo và xử lý ngoại lệ

        BigInt R; // Biến lưu số dư
        R.sign = 1;
        
        // Chặn lỗi chia cho 0
        if (B.digits.getsize() == 0 || (B.digits.getsize() == 1 && B.digits[0] == 0)) {
            return R; 
        }

        for (int i = this->digits.getsize() - 1; i >= 0; i--) {
            // Duyệt từ bit 31 lùi về bit 0
            for (int j = 31; j >= 0; j--) {
                
                // 1. Dịch R sang trái 1 bit 
                R = R.addAbs(R); 

                // 2. Hạ bit thứ j của khối hiện tại xuống
                unsigned int bit = (this->digits[i] >> j) & 1;
                
                if (bit == 1) {
                    BigInt one; 
                    one.sign = 1; 
                    one.digits.push_back(1);
                    R = R.addAbs(one); 
                }

                // 3. Nếu số dư R >= B, thì trừ B ra khỏi R
                if (R.isAbsoluteLess(B) == false) {
                    R = R.subAbs(B);
                }
            }
        }
        
        // 4. 
        R.normalize();
        
        return R;
    }

    /** 
     * @brief 7. Chia nguyên giá trị tuyệt đối của 2 số BigInt (bỏ qua dấu)
     * @details Dùng thuật toán chia nhị phân (Long Division). Tương tự modAbs
     * 1. Duyệt từng bit của số bị chia (this) từ cao xuống thấp.
     * 2. Dịch trái R (số dư) và Q (thương số) thêm 1 bit. Hạ bit tiếp theo của this xuống R.
     * 3. Nếu số dư R >= B: trừ R cho B và cộng 1 vào thương số Q.
     * 4. Chuẩn hóa
     * @param B: số chia (BigInt)
     * @return Q: res = |A| / |B|
     * @note 
     * Phụ trợ cho nạp chồng toán tử "/"
     */
    BigInt divAbs(const BigInt& B) const {
        //0. Khởi tạo và xử lý ngoại lệ
        BigInt Q;               // Thương số
        Q.sign = 1;
        BigInt R;               // Số dư
        R.sign = 1;
        
        // 0. Chặn lỗi chia cho 0
        if (B.digits.getsize() == 0 || (B.digits.getsize() == 1 && B.digits[0] == 0)) {
            return Q; 
        }

        BigInt one;
        one.sign = 1;
        one.digits.push_back(1);

        //1. Duyệt từng khối 32-bit từ đuôi mảng lùi về đầu
        for (int i = this->digits.getsize() - 1; i >= 0; i--) {
            
            // Duyệt từ bit 31 lùi về bit 0
            for (int j = 31; j >= 0; j--) {
                
                // 1.1. Dịch trái phần dư R và thương số Q thêm 1 bit bằng cách nhân 2
                R = R.addAbs(R); 
                Q = Q.addAbs(Q);

                // 2. 
                unsigned int bit = (this->digits[i] >> j) & 1;
                if (bit == 1) {
                    R = R.addAbs(one);
                }

                // 3.
                if (R.isAbsoluteLess(B) == false) {
                    R = R.subAbs(B);   
                    Q = Q.addAbs(one); 
                }
            }
        }
        
        // 4. 
        Q.normalize();
        
        return Q;
    }
public:
// ------------------------------------------------------------------- CONSTRUCTOR -------------------------------------------------------------------
    
    /** 
     * @brief 1. Khởi tạo mặc định
     * @details Gán sign = 0
    */
    BigInt(){
        this->sign = 0;
    }

    /** 
     * @brief 2. Khởi tạo thủ công
     * @details Chuyển từ long long sang BigInt
     * 0. Xử lý ban đầu cho biến sign và các ngoại lệ
     * 1. Lặp lại cho đến hết lần lượt thao tác: Cắt 32 bit cuối của x  -> Cắt 32 bit cuối của x  -> ...
     * @param x: Số nhập vào
     * @note
     * 1. x& 0xFFFFFFFF: Lấy đúng 32 bit phần đuôi của x
    */
    BigInt(long long x){
        //0.

        //0.1. Xử lý biến sign
        if (x < 0){
            this->sign = -1;
        }
        else if (x == 0){
            this->sign = 0;
            return;
        }
        else{
            this->sign = 1;
        }

        //0.2. Ép kiểu để lấy trị tuyệt đối
        unsigned long long absX = (x < 0) ? -(unsigned long long)x : (unsigned long long)x; 
        
        //1.
        while (absX > 0){
            this->digits.push_back(absX& 0xFFFFFFFF);
            absX = absX >> 32;
        }
    }




    
// ------------------------------------------------------------------- METHOD -------------------------------------------------------------------

    //A. Nạp chồng toán tử

    /** 
     * @brief 1. Nạp chồng toán tử cộng "+"
     * @details Xét dấu các toán hạng và gọi các hàm phụ trợ phía trên
     * 1. A, B cùng dấu: Gọi addAbs và sign giữ nguyên
     * 2. A, B khác dấu: Xét 2 trường hợp cho A > B hoặc A < B
     * @param b: số BigInt cần cộng
     * @return res: res = A + B
    */
    BigInt operator+(const BigInt& b) const {
        BigInt res;
       //1. 
       if ( this->sign == b.sign){
        res = this->addAbs(b);
        res.sign = this->sign;
        return res;
        }

        //2. 
        res = this->subAbs(b);

        if( this->isAbsoluteLess(b)){       //2.1. |A| < |B|
            res.sign = b.sign;
        }
        else{
            res.sign = this->sign;          //2.2 |A| >= |B|
        }

        res.normalize();

        return res;
    }


    /** 
     * @brief 2. Nạp chồng toán tử cộng "-"
     * @details Xét dấu các toán hạng và gọi các hàm phụ trợ phía trên
     * 1. A, B khác dấu: Gọi addAbs và sign giữ nguyên. Đồng thời 
     * 2. A, B cùng dấu: Xét 2 trường hợp cho A > B hoặc A < B
     * @param b: số BigInt cần trừ
     * @return res: res = A - B
    */
    BigInt operator-(const BigInt& b) const {
        BigInt res;

        //1. 
        if (this->sign != b.sign) {
            res = this->addAbs(b);
            res.sign = (this->sign != 0) ? this->sign : -b.sign;  // 0 - b = -b
            res.normalize();
            return res;
        }

        // 2. 
        res = this->subAbs(b);

        if (this->isAbsoluteLess(b)) {      // 2.1. |A| < |B|
            res.sign = -b.sign;            
        } else {
            res.sign = this->sign;          // 2.2. |A| >= |B| 
        }

        res.normalize();

        return res;
    }


    /** 
     * @brief 3. Nạp chồng toán tử so sánh "<"
     * @details Xét dấu các toán hạng và gọi các hàm phụ trợ phía trên
     * 1. A, B khác dấu: Chọn số nào dương
     * 2. A, B cùng dấu: Gọi isAbsoluteLess 
     * @param b: số BigInt cần so sánh
     * @return 
     * 1. True nếu so sánh đúng
     * 2. False nếu so sánh sai
    */
    bool operator<(const BigInt&b ) const {
        //1.
        if (this->sign != b.sign) {
            return this->sign < b.sign;
        }

        //2.

        //2.1. Cùng dấu dương
        if (this->sign == 1) {
            return this->isAbsoluteLess(b);      
        }

        //2.2. Cùng dấu âm
        if (this->sign == -1) {
            return b.isAbsoluteLess(*this);     
        }

        //3.
        return false;
        }

    /** 
     * @brief 4. Nạp chồng toán tử nhân "*"
     * @details 
     * 1. Dùng hàm mulAbs để nhân giá trị tuyệt đối
     * 2. Chỉ cần xác định dấu của tích 
     * 3. Chuẩn hóa
     * @param b: số BigInt cần nhân
     * @return res: Tích của 2 số bao gồm cả dấu chuẩn xác
     */
    BigInt operator*(const BigInt& b) const {
        // 1. Nhân giá trị tuyệt đối của 2 số
        BigInt res = this->mulAbs(b);

        // 2. Xác định dấu 
        res.sign = this->sign * b.sign;

        res.normalize();

        return res;
    }
    
    /** 
     * @brief 5. Nạp chồng toán tử lũy thừa "^"
     * @details Dựa trên binary exp - đệ quy
     * 1. e = 0 thì res = 1
     * 2. e chẵn thì (x^(e/2))^2
     * 3. e lẻ thì x*(x^((e-1)/2))^2
     * @param e: lũy thừa
     * @return res: res = this^e
     */
    BigInt operator^(unsigned long long e) const {
        // 1. Base case
        
        //1.1
        if (e == 0) {
            BigInt res;
            res.digits.push_back(1); // Trả về số 1 dương
            res.sign = 1;
            return res;
        }
        
        //1.2
        if (e == 1) {
            return *this; 
        }

        // 2. Đệ quy

        //2.1. e chẵn
        BigInt half = (*this) ^ (e / 2);
        BigInt squared = half * half;

        //3. Phân nhánh
        if (e % 2 == 0) {
            return squared;
        }
        else{
            return (*this)*squared;
        }
    }

    /** 
     * @brief 6. Nạp chồng toán tử chia lấy dư "%"
     * @details Dùng hàm modAbs để tính phần dư tuyệt đối. 
     * Quy tắc C++: Dấu của phần dư luôn đi theo dấu của số bị chia (this).
     * @param b: số chia (BigInt)
     * @return res: res = this % b
     */
    BigInt operator%(const BigInt& b) const {
        // 1. Tính phần dư của giá trị tuyệt đối
        BigInt res = this->modAbs(b);

        // 2. Xét dấu cho kết quả
        if (res.sign != 0) {
            res.sign = this->sign;
        }

        // 3. Chuẩn hóa
        res.normalize();

        return res;
    }

    /**
     * @brief Nạp chồng toán tử chia lấy nguyên "/"
     * @details Dùng hàm divAbs để tính thương số tuyệt đối. 
     * Quy tắc xét dấu y hệt như phép nhân: 2 số cùng dấu ra dương, trái dấu ra âm.
     * @param b: số chia (BigInt)
     * @return res: res = this / b
     */
    BigInt operator/(const BigInt& b) const {
        // 1. Tính thương số của giá trị tuyệt đối
        BigInt res = this->divAbs(b);

        // 2. Xét dấu cho kết quả
        if (res.sign != 0) {
            res.sign = this->sign * b.sign;
        }

        // 3. Chuẩn hóa 
        res.normalize();

        return res;
    }

    // B. Hàm hỗ trợ
    /** 
     * @brief 1. Kiểm tra zero
     * @return
     * 1. True nếu this == 0
     * 2. False nếu this != 0
     */
    bool isZero() const{
        if ( this->sign == 0 ){
            return true;
        } else{
            return false;
        }
    }

    /** 
     * @brief 2. Lấy tổng số bit của số 
     * @details 
     * 1. Số 0 quy ước có độ dài 0 bit
     * 2. Xét block cao nhất (cuối mảng theo Little Endian)
     * 3. Đếm số bit thực sự dùng trong block cao nhất
     * 4. Tổng bit = (các block thấp đầy đủ) * 32 + (số bit của block cao nhất)
     * @return Số lượng bit
     * @note 
     * 1. Phụ trợ cho getBit và các thuật toán cần duyệt bit
     */
    int bitLength() const {
        //1. 
        if (this->sign == 0 || this->digits.getsize() == 0) {
            return 0;
        }

        //2. 
        int lastIdx = this->digits.getsize() - 1;
        unsigned int highBlock = this->digits[lastIdx];

        //3. 
        int bitsInHigh = 0;
        while (highBlock > 0) {
            bitsInHigh++;
            highBlock = highBlock >> 1;
        }

        //4. 
        return lastIdx * 32 + bitsInHigh;
    }


     /** 
     * @brief 3. Lấy giá trị bit thứ i
     * @details 
     * 1. Xử lý ngoại lệ
     * 2. Xác định block chứa bit i và vị trí của bit i trong block đó
     * 3. Dịch phải block về đúng vị trí rồi lấy bit thấp nhất
     * @param i: vị trí bit cần lấy (bit 0 là bit thấp nhất)
     * @return 
     * 1. 0 nếu bit thứ i bằng 0 (hoặc i nằm ngoài phạm vi)
     * 2. 1 nếu bit thứ i bằng 1
     * @note 
     * 1. blockIdx = i / 32 do mỗi block chứa 32 bit
     * 2. bitIdx = i % 32 tương ứng vị trí bit bên trong block
     */
    int getBit(int i) const {
        //1. Vị trí không hợp lệ -> bit = 0
        if (i < 0 || i >= this->bitLength()) {
            return 0;
        }

        //2. Xác định block và vị trí bit trong block
        int blockIdx = i / 32;
        int bitIdx = i % 32;

        //3. Dịch phải rồi lấy bit thấp nhất
        return (this->digits[blockIdx] >> bitIdx) & 1;
    }



    // C. RSA

    /** * @brief 1. Extended Euclidean Algorithm
     * @details Tìm ƯCLN của a và b, đồng thời tìm x, y thỏa mãn: a*x + b*y = gcd(a, b)
     * Triển khai theo đệ quy
     * 1. Base case: b = 0 thì gcd(a,b) = a và x = 1, y = 0
     * 2. Đệ quy: Dựa trên tính chất toán học, gcd(a,b) = gcd(b,a%b)
     * @param 
     * 1. a: Số BigInt thứ nhất
     * 2. b: Số BigInt thứ hai
     * 3. x: Biến tham chiếu để lưu hệ số x
     * 4. y: Biến tham chiếu để lưu hệ số y
     * @return gcd: Ước chung lớn nhất của a và b
     * @note Do C++ không hỗ trợ trả về multiple values nên cần thêm cả x,y vào để lưu kết quả
     */
    BigInt gcdExtended(const BigInt& a, const BigInt& b, BigInt& x, BigInt& y) const {
        // 1. Base case

        if (b.sign == 0) {
            BigInt one, zero;
            one.sign = 1; 
            one.digits.push_back(1);
            zero.sign = 0; 

            x = one;
            y = zero;
            return a;
        }

        // 2. Gọi đệ quy
        BigInt x1, y1;
        BigInt gcd = gcdExtended(b, a % b, x1, y1);

        // 3. Cập nhật lại x và y theo kết quả của bước đệ quy phía dưới
        x = y1;
        y = x1 - (a / b) * y1;

        return gcd;
    }
    

    /** 
     * @brief 2. Lũy thừa module (Modular Exponentiation)
     * @details Tính (this ^ e) % m
     * Sử dụng Left-to-Right Square-and-Multiply và bitwise
     * 1. Khởi tạo: r = 1 
     * 2. Lặp
     * 2.1. Bước square: r & m = r^2
     * 2.2. Bước multiply: e_i = 1 -> r % m = (r*x)
     * @param 
     * 1. e: Số mũ
     * 2. m: Số module
     * @return res: res = (this^e)%m
     * @note 
     * 1. Dùng trực tiếp để mã hóa và giải mã trong RSA.
     * 2. Tính thông qua quy tắc Horner e = (e_{k-1}e_{k-2}...e_0)_2 = (...((e_{k-1}*2+e_{k-2})*2 + e_{k-3})...)*2 +e_0 
     * 3. Khi chuyển qua x^e, phép nhân với số mũ là phép bình phương r <- r^2, phép cộng với e_i ( = 1 ) là nhân thêm x: r <- r*x 
     */
    BigInt modPow(const BigInt& e, const BigInt& m) const {
        // 1. Khởi tạo
        BigInt res;
        res.sign = 1;
        res.digits.push_back(1);

        BigInt base = (*this) % m;

        // 2. Xử lý ngoại lệ 
        if (e.isZero()) return res;

        // 4. Duyệt qua từng bit của số mũ e
        int len = e.bitLength();
        for (int i = len - 1; i >= 0; i--) {
            // 4.1. Bước Square 
            res = (res * res) % m;

            // 4.2. Bước Multiply 
            if (e.getBit(i) == 1) {
                res = (res * base) % m;
            }
        }

        return res;
    }

    /**
     * @brief 3. Tìm nghịch đảo module (Modular Multiplicative Inverse)
     * @details Tìm x sao cho (this * x) % m = 1
     * Dùng Extended Euclidean Algorithm:
     * 1. Gọi gcdExtended để tìm x, y và ước chung lớn nhất (GCD).
     * 2. Kiểm tra GCD. Nếu GCD != 1, nghịch đảo không tồn tại (trả về 0).
     * 3. Nếu tồn tại, xử lý x âm bằng công thức: res = (x % m + m) % m.
     * @param m: Số module
     * @return res: res = x
     * @note Phụ trợ để tính Private Key trong RSA.
     */
    BigInt modInverse(const BigInt& m) const {
        BigInt x, y;
        
        // 1. Gọi thuật toán Euclid mở rộng cho A (this) và m
        BigInt gcd = gcdExtended(*this, m, x, y);

        // Tạo BigInt mang giá trị 1 để so sánh
        BigInt one;
        one.sign = 1;
        one.digits.push_back(1);

        // 2. Nếu GCD != 1 thì Không có nghịch đảo
        if ((gcd - one).sign != 0) {
            BigInt zero; 
            zero.sign = 0;
            return zero; 
        }

        // 3. Chuẩn hóa x 
        BigInt res = ((x % m) + m) % m;

        return res;
    }    

    /** 
     * @brief 4. Kiểm tra số nguyên tố 
     * @details Dùng thuật toán Miller-Rabin với bases cố định - là các số nguyên tố nhỏ.
     * 1. Xử lý các trường hợp ban đầu:
     * 2. Phân tích n - 1 = 2^r * d  với d lẻ
     * 3. Với mỗi base a, tính x = a^d mod n:
     *    3.1. Nếu x = 1 hoặc x = n - 1 thì a "pass" -> xét base tiếp theo
     *    3.2. Bình phương x tối đa r - 1 lần, nếu gặp x = n - 1 thì a "pass"
     *    3.3. Nếu không gặp -> n là hợp số
     * 4. Pass tất cả base -> n (almost) là số nguyên tố
     * @return 
     * 1. True nếu this là số nguyên tố
     * 2. False nếu this là hợp số
     * @note 
     * 1. Dùng để sinh số nguyên tố p, q trong RSA
     * 2. Bases là các số nguyên tố nhỏ cố định 
     * 3. Dựa trên mệnh đề: n là số nguyên tố thì với mọi a ( 1 < a < n), một trong hai điều kiện sau phải đúng
     * 3.1. a^d % n = 1
     * 3.2. Tồn tại 0 <= i < r sao cho a^(d*2^i) % n = -1 ( or n-1)
     * 4. Giới hạn đúng ( n < 3*10^24 - 81 bit )
     */
    bool isPrime() const {
        //0. Khởi tạo các hằng số hay dùng
        BigInt one(1);
        BigInt two(2);
        BigInt three(3);

        //1. Xử lý các trường hợp nhỏ

        //1.1. n < 2 
        if ((*this - two).sign < 0) {
            return false;
        }

        //1.2. n = 2 hoặc n = 3 
        if ((*this - two).isZero() || (*this - three).isZero()) {
            return true;
        }

        //1.3. n chẵn 
        if ((this->digits[0] & 1) == 0) {
            return false;
        }

        //2. Phân tích n - 1 = 2^r * d  
        BigInt d = *this - one;             // d = n - 1
        int r = 0;
        while ((d.digits[0] & 1) == 0) {   
            d = d / two;
            r++;
        }

        //3. Lần lượt kiểm tra với từng base a
        BigInt nMinus1 = *this - one;       

        long long bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
                             31, 37, 41, 43, 47, 53, 59, 61, 67, 71};
        int numBases = 20;

        for (int k = 0; k < numBases; k++) {
            BigInt a(bases[k]);

            //3.0. Bỏ qua nếu a >= n 
            if ((a - *this).sign >= 0) {
                continue;
            }

            //3.1. x = a^d mod n
            BigInt x = a.modPow(d, *this);

            //3.2. Nếu x = 1 hoặc x = n - 1 -> base này pass, xét base tiếp theo
            if ((x - one).isZero() || (x - nMinus1).isZero()) {
                continue;
            }

            //3.3. Bình phương x tối đa r - 1 lần để tìm n - 1
            bool isComposite = true;
            for (int t = 0; t < r - 1; t++) {
                x = (x * x) % (*this);          

                if ((x - nMinus1).isZero()) {   // gặp n - 1 -> a pass
                    isComposite = false;
                    break;
                }
            }

            //3.4. Không tìm được n - 1 thì n chắc chắn là hợp số
            if (isComposite) {
                return false;
            }
        }

        //4. Vượt qua tất cả base thì n là số nguyên tố
        return true;
    }


    // D. Bộ Chuyển Đổi 

    /** 
     * @brief 1. Hàm chuyển đổi từ Hex sang số BigInt
     * @details 
     * 1. Xử lý ban đầu: dấu, tiền tố, các số 0
     * 2. Lặp từ cuối chuỗi lên đầu, cắt từng khối tối đa 8 kí tự Hex ( 32 bit )
     * 3. Chuyển Hex thành unsigned int thông qua dịch bit
     * 4. Chuẩn hóa
     * @param s: chuỗi Hex
     * @return res: BigInt mang giá trị của chuỗi Hex
     * @note
     * 1. 8 ký tự Hex (16^8) tương đương đúng 1 khối 32-bit (2^32)
     * 2. O(n)
    */
    BigInt fromHex ( string& s ) {
        //0. Khởi tạo                       
        BigInt res;                             // Biến kết quả     
        res.sign = 1;
        
        if (s.empty()) {
            res.sign = 0;
            return res;
        }

        int startIdx = 0;

        //1. 
        if (s[0] == '-') {
            res.sign = -1;
            startIdx = 1;
        } else if (s[0] == '+') {
            startIdx = 1;
        }

        //1.1. Bỏ qua 0x
        if (startIdx + 1 < s.length() && s[startIdx] == '0' && 
           (s[startIdx + 1] == 'x' || s[startIdx + 1] == 'X')) {
            startIdx += 2;
        }

        //1.2. Bỏ qua các số 0 vô nghĩa ở đầu
        while (startIdx < s.length() && s[startIdx] == '0') {
            startIdx++;
        }

        //1.3. Nếu chuỗi chỉ toàn số 0
        if (startIdx == s.length()) {
            res.sign = 0;                       
            return res; 
        }

        //2. Cắt khối 32-bit từ đuôi lên đầu
        for (int i = s.length() - 1; i >= startIdx; i -= 8) {
            int len = 8;
            int firstCharIdx = i - 7;
            
            // Xử lý block cuối cùng (nằm ở đầu chuỗi) nếu không đủ 8 ký tự
            if (firstCharIdx < startIdx) {
                firstCharIdx = startIdx;
                len = i - startIdx + 1;
            }

            //3.
            unsigned int blockVal = 0;                      // Giá trị của 1 phần tử trong mảng BigInt
            for (int j = 0; j < len; ++j) {
                char c = s[firstCharIdx + j];
                unsigned int digit = 0;
                
                if (c >= '0' && c <= '9') {
                    digit = c - '0';
                } else if (c >= 'a' && c <= 'f') {
                    digit = c - 'a' + 10;
                } else if (c >= 'A' && c <= 'F') {
                    digit = c - 'A' + 10;
                }
                
                // res = res * 16 + val
                blockVal = (blockVal << 4) | digit; 
            }
            
            // Nhập kết quả vào mảng (Little-endian: khối nhỏ nhất ở index 0)
            res.digits.push_back(blockVal);
        }

        //4. Chuẩn hóa
        res.normalize();

        return res;
    }
    

    /** 
     * @brief 3. Hàm chuyển đổi từ BigInt sang chuỗi Hex
     * @details 
     * 1. Xử lý ngoại lệ
     * 2. Thêm dấu âm nếu cần
     * 3. Duyệt các khối từ cao xuống thấp ( duyệt từ hàng đơn vị do sử dụng Little Endian ):
     *    3.1. Khối cao nhất: in từ nibble khác 0 đầu tiên 
     *    3.2. Các khối còn lại: luôn in đủ 8 kí tự Hex (đệm số 0 ở đầu nếu thiếu)
     * @return res: chuỗi Hex biểu diễn số
     * @note 
     * 1. 1 khối 32-bit = đúng 8 kí tự Hex (mỗi 4 bit = 1 nibble = 1 kí tự Hex)
     * 2. nibble = (block >> (j*4)) & 0xF: lấy 4 bit ở vị trí thứ j
     * 3. Có thể đọc ngược lại bằng hàm fromHex
    */
    string toHex() const {
        //1. 
        if (this->sign == 0) {
            return "0";
        }

        string res = "";
        string hexChars = "0123456789ABCDEF";   // Bảng tra kí tự Hex

        //2. Thêm dấu âm nếu cần
        if (this->sign == -1) {
            res += '-';
        }

        int highIdx = this->digits.getsize() - 1;

        //3. Duyệt các khối từ cao xuống thấp
        for (int i = highIdx; i >= 0; i--) {
            unsigned int block = this->digits[i];

            //3.1. Khối cao nhất
            if (i == highIdx) {
                int startNibble = 7;
                while (startNibble > 0 && ((block >> (startNibble * 4)) & 0xF) == 0) {
                    startNibble--;
                }

                for (int j = startNibble; j >= 0; j--) {
                    res += hexChars[(block >> (j * 4)) & 0xF];
                }
            }
            //4.2. Các khối còn lại
            else {
                for (int j = 7; j >= 0; j--) {
                    res += hexChars[(block >> (j * 4)) & 0xF];
                }
            }
        }

        return res;
    }
};
