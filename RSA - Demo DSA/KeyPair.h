#pragma once
#include "BigInt.h"
#include "RNG.h"
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

    /** 
     * @brief Cấu trúc chứa khóa cho toàn bộ quá trình mã hóa và giải mã
     * @note 
     * 1. Phi(n) là hàm phi Euler
     * 2. Khóa công khai (n,e); Khóa bí mật (n,d)
    */
class KeyPair {
private:
    BigInt n;           //Modulus: n = p*q với p,q là số nguyên tố lớn
    BigInt e;           //Public Exponent: e - khóa công khai. Thỏa mãn gcd(e,phi(n)) = 1
    BigInt d;           //Private Exponent: d - khóa bí mật. Nghịch đảo modulo của e theo mod phi(n)


// ------------------------------------------------------------------- PRIVATE FUNCTION  -------------------------------------------------------------------

    /** 
     * @brief 1. Sinh 1 số nguyên tố ngẫu nhiên có độ dài bits
     * @details Lặp lại cho tới khi sinh ra số cần tìm
     * 1. Dùng rng.nextBigInt(bits) để sinh số ngẫu nhiên đủ độ dài
     * 2. Ép bit thấp nhất = 1 ( để số lẻ )
     * 3. Ép bit cao nhất = 1 để đảm bảo đủ số bit
     * @param
     * 1. bits: số bit của số nguyên tố sinh ra
     * 2. rng: bộ sinh số ngẫu nhiên
     * @return res: số nguyên tố có độ dài chỉ định
     * @note
     * 1. Phụ trợ cho hàm generate()
     * 2. Tôi đa 80 bit cho Prime để Miller Rabin còn đúng
    */
    BigInt generatePrime(int bits, RNG& rng){
        //0. Khởi tạo 
        BigInt one(1);
        BigInt highBit = BigInt(2) ^ (bits - 1);    // = 2^(bits-1), dùng để bật bit cao nhất
        BigInt res;

        while (true) {
            //1. 
            res = rng.nextBigInt(bits);

            //2. 
            if (res.getBit(0) == 0) {
                res = res + one;
            }

            //3. 
            if (res.getBit(bits - 1) == 0) {
                res = res + highBit;
            }

            //4. 
            if (res.isPrime()) {
                return res;
            }
        }
    }
public:
// ------------------------------------------------------------------- CONSTRUCTOR  -------------------------------------------------------------------
    
    /** 
     * @brief 1. Khởi tạo mặc định
     * @note n, e, d mặc định mang giá trị 0
    */
    KeyPair(){}

    /** 
     * @brief 2. Khởi tạo thủ công
    */
    KeyPair(const BigInt& n_val, const BigInt& e_val, const BigInt& d_val) {
        this->n = n_val;
        this->e = e_val;
        this->d = d_val;
    }

// ------------------------------------------------------------------- METHOD  -------------------------------------------------------------------

    // A. Sinh Khóa

    
    /** 
     * @brief 1. Sinh khóa từ p,q cho sẵn
     * @details 
     * 0. Khởi tạo và xử lý ngoại lệ
     * 1. Sinh khóa
     * @param 
     * 1. p: số nguyên tố thứ nhất
     * 2. q: số nguyên tố thứ hai
     * @note 
     * 1. 65537 là số mũ công khai phổ biến (số Fermat thứ 4)
     * 2. Nếu modInverse trả về 0 thì e và phi không nguyên tố cùng nhau
    */
    void generateKey(const BigInt& p, const BigInt& q) {
        //0.
        BigInt diff = p-q;
        if (!p.isPrime() || !q.isPrime() || diff.isZero()) {
            cout << "Loi dau vao" << endl;
            exit(1); 
        }

        
        //1.
        BigInt one(1);
        this->n = p * q;
        BigInt phi = (p - one) * (q - one);
        this->e = BigInt(65537); 
        this->d = this->e.modInverse(phi);
    }
    


    /** 
     * @brief 2. Sinh khóa tự động theo số bit
     * @details 
     * 1. Gọi generatePrime() 2 lần để sinh ra p và q
     * 2. Đảm bảo p khác q (nếu trùng thì sinh lại)
     * 3. Gọi generateKey(p, q) để tính ra n, e, d
     * 4. Nếu d = 0 (tức e không có nghịch đảo) thì sinh lại p, q
     * @param 
     * 1. bits: số bit mong muốn cho MỖI số nguyên tố p, q
     * 2. rng: bộ sinh số ngẫu nhiên
     * @note 
     * 1. Modulus n sinh ra sẽ có độ dài 2*bits bit
     * 2. p, q chỉ là biến tạm, không lưu lại làm thuộc tính
    */
    void generate(int bits, RNG& rng){
        
        //0. Khởi tạo
        BigInt p,q;

        while (true) {
            // 1.
            p = generatePrime(bits, rng);
            q = generatePrime(bits, rng);

            // 2. 
            BigInt diff = p - q;
            if (diff.isZero()) {
                continue; // Quay lại đầu vòng lặp để sinh lại p và q mới
            }

            // 3. 
            this->generateKey(p, q);

            // 4. 
            if (this->d.isZero()) {
                continue; // Quay lại đầu vòng lặp để tìm p, q khác phù hợp hơn
            }
            

            // Nếu tìm ra thi thoát khỏi loop
            break;
        }
    }


    // B. Mã hóa - GIải mã

    // Mã hóa [cite: 118]
    /** 
     * @brief 1. Mã hóa
     * @details Tính c = m^e mod n bằng modPow
     * @param m: plaintext dạng BigInt
     * @return c: ciphertext
     * @note 
     * 1. Dùng khóa công khai (n, e)
     * 2. Bắt buộc m < n, nếu không sẽ mất thông tin và không giải mã ngược được
    */
    BigInt encrypt(const BigInt& m) const{
        return m.modPow(this->e, this->n);
    }

    /** 
     * @brief 2. Giải mã
     * @details Tính m = c^d mod n bằng modPow
     * @param c: ciphertext dạng BigInt
     * @return m: plaintext 
     * @note 
     * 1. Dùng khóa bí mật (n, d)
    */
    BigInt decrypt(const BigInt& c) const{
        return c.modPow(this->d, this->n);
    }

    // C. Đọc / Ghi file khóa
 
    /** 
     * @brief 1. Ghi khóa công khai ra file
     * @details Ghi n và e dưới dạng chuỗi Hex (toHex), mỗi giá trị 1 dòng
     * @param path: đường dẫn file cần ghi
     * @note 
     * 1. Khóa công khai gồm (n, e)
     * 2. Đọc lại bằng loadPublic()
    */
    void savePublic( string path){
        //0. Mở file
        ofstream out(path);
        if (!out.is_open()) {
            cout << "Loi ghi Public Key!" << endl;
            return;
        }

        //1. Ghi n, e dưới dạng Hex, mỗi giá trị 1 dòng
        out << this->n.toHex() << "\n";
        out << this->e.toHex() << "\n";

        out.close();
    }
 
    /** 
     * @brief 2. Ghi khóa bí mật ra file
     * @details Ghi n và d dưới dạng chuỗi Hex (toHex), mỗi giá trị 1 dòng
     * @param path: đường dẫn file cần ghi
     * @note 
     * 1. Khóa bí mật gồm (n, d)
     * 2. Đọc lại bằng loadPrivate()
    */
    void savePrivate( string path){
        //0. Mở file
        ofstream out(path);
        if (!out.is_open()) {
            cout << "Loi ghi Private Key!" << endl;
            return;
        }

        //1. Ghi n, d dưới dạng Hex, mỗi giá trị 1 dòng
        out << this->n.toHex() << "\n";
        out << this->d.toHex() << "\n";

        out.close();
    } 


    /** 
     * @brief 3. Đọc khóa công khai từ file
     * @details Đọc 2 chuỗi Hex, chuyển thành n và e bằng fromHex
     * @param path: đường dẫn file cần đọc
     * @note 
     * 1. Tương ứng với file do savePublic() ghi ra
    */
    void loadPublic( string path){
        //0. Mở file
        ifstream in(path);
        if (!in.is_open()) {
            cout << "Loi: Khong the mo file Public Key!" << endl;
            return;
        }

        //1. Đọc 2 chuỗi Hex
        string n_hex, e_hex;
        in >> n_hex >> e_hex;

        //2. Chuyển Hex -> BigInt
        BigInt conv;                    // đối tượng tạm để gọi fromHex (do fromHex chưa static)
        this->n = conv.fromHex(n_hex);
        this->e = conv.fromHex(e_hex);

        in.close();
    }
    /** 
     * @brief 4. Đọc khóa bí mật từ file
     * @details Đọc 2 chuỗi Hex, chuyển thành n và d bằng fromHex
     * @param path: đường dẫn file cần đọc
     * @note 
     * 1. Tương ứng với file do savePrivate() ghi ra
    */
    void loadPrivate( string path){
        //0. Mở file
        ifstream in(path);
        if (!in.is_open()) {
            cout << "Loi: Khong the mo file Private Key!" << endl;
            return;
        }

        //1. Đọc 2 chuỗi Hex
        string n_hex, d_hex;
        in >> n_hex >> d_hex;

        //2. Chuyển Hex -> BigInt
        BigInt conv;                    
        this->n = conv.fromHex(n_hex);
        this->d = conv.fromHex(d_hex);

        in.close();
    } 
 
    // D. Getter
 
    /** 
     * @brief 1. Lấy modulus n
     * @return n
     * @note 
     * 1. Lớp ByteArray cần n để xác định kích thước khối dữ liệu (đảm bảo m < n)
     * 2. O(1)
    */
    BigInt getN() const{
        return this->n;
    }
 
    /** 
     * @brief 2. Lấy số mũ công khai e
     * @return e
     * @note O(1)
    */
    BigInt getE() const{
        return this->e;
    }
 
    /** 
     * @brief 3. Lấy số mũ bí mật d
     * @return d
     * @note O(1)
    */
    BigInt getD() const{
        return this->d;
    }
};

