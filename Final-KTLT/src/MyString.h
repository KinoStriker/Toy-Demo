#pragma once
#include <iostream>

using namespace std;


    /** 
     * @brief 
     * @details 
     * @param
     * @return
     * @note
    */

class MyString {
private:
    char* data;   // chuỗi ký tự, kết thúc bằng '\0'
    int   len;    // số lượng ký tự (không tính '\0')
    int   cap;    // dung lượng tối đa (đã gồm ô cho '\0')
 


    /** 
     * @brief 1. Hàm hỗ trợ đếm chuỗi
     * @details Đếm cho đến khi gặp '\0'
     * @note Mục đích duy nhất là hỗ trợ constructor từ string mặc định sang  MyString
    */
    int countString(const char* s) const {
        int n = 0;
        while (s[n] != '\0') n++;
        return n;
    }

    /** 
     * @brief 1. Hàm tìm min 3 số
     * @note Hỗ trợ cho Levenshtein
    */
    int Min(int a, int b, int c) const {
        int lowest = a;           
        if (b < lowest) {
            lowest = b;           
        }
        if (c < lowest) {
            lowest = c;           
        }
    return lowest;
}

public:
    // ------------------------------------------------------------------- CONSTRUCTOR -------------------------------------------------------------------

    
    /** 
     * @brief 1. Khởi tạo mặc định
     * @return Khởi tạo chuỗi rỗng
    */
    MyString() {
        cap = 1;                
        len = 0;                
        data = new char[cap];   
        data[0] = '\0';         
    }


    /** 
     * @brief 2. Khởi tạo thủ công
     * @details Nhúng chuỗi nhập thủ công và MyString()
     * 1. Kiểm tra ngoại lệ
     * 2. Đếm độ dài
     * 3. Khởi tạo
     * @param s: chuỗi được nhập thủ công
    */
    MyString(const char* s){
        // 1.
        if (s == nullptr) {
            cap = 1;
            len = 0;
            data = new char[cap];
            data[0] = '\0';
            return; 
        }
        
        // 2.
        this->len = countString(s);

        // 3.
        // 3.1. Cấp phát
        cap = len + 1;
        data = new char[cap];

        //3.2. Sao chép dữ liệu 
        for (int i = 0; i < len; i++) {
            data[i] = s[i];
        }

        //3.3. Gán đuôi
        data[len] = '\0';
    }

    /** 
     * @brief 3. Copy Constructor ( Deep Copy )
     * @details 
     * 1. Cập nhật tham số của chuỗi mới vào chuỗi hiện tại
     * 2. Duyệt vòng lặp để sao chép phần tử
     * 3. Gán đuôi '\0'
     * @param other: địa chỉ chuỗi gốc dùng để sao chép dữ liệu 
    */
    MyString(const MyString& other){
        // 1.
        this->cap = other.cap;
        this->len = other.len;

        // 2. 
        data = new char[cap];
        for ( int i = 0; i < len; i ++){
            this->data[i] = other.data[i];
        }
        // 3.
        this->data[len] = '\0';
    }
    

    /** 
     * @brief 4. Nạp chồng toán tử gán "="
     * @details 
     * 1. Xóa chuỗi cũ
     * 2. Cập nhật tham số của chuỗi mới vào chuỗi hiện tại
     * 3. Duyệt vòng lặp để gán dữ liệu ở chuỗi other sang chuỗi gốc
     * 4. Gán đuôi '\0'
     * @param
     * @return
     * @note Hỗ trợ chống lỗi tự gán. VD: a = a
    */
    MyString& operator=(const MyString& other){
        if ( this != &other){

            // 1. 
            delete[] data;

            // 2.
            cap = other.cap;
            len = other.len;

            // 3.
            data = new char[cap];
            for ( int i = 0; i < len; i++){
                data[i] = other.data[i];
            }

            // 4.
            data[len] = '\0';
        }

        return *this;    
    }

    /** 
     * @brief 5. Destructor
     * @details Xóa chuỗi khi không cần sử dụng
    */
    ~MyString(){
        delete[] data;
    }                            

    // ------------------------------------------------------------------- METHOD  -------------------------------------------------------------------
    
    // A. Truy xuất thông tin

    /** 
     * @brief 1. Lấy kích thước chuỗi
     * @return Trả về len
    */
    int  length() const{
        return len;
    }

    /** 
     * @brief 2. Kiểm tra rỗng
     * @return Trả về true nếu rỗng, false nếu ngược lại
    */
    bool empty()  const{
        return len == 0;
    }

    /** 
     * @brief 3. Lấy con trỏ chuỗi C - style
     * @details Trả về con trỏ "data" để in ra màn hình 
     * @return const char*: con trỏ chuỗi kết thúc bằng '\0'
     * @note Dùng để in cho MyString: cout << s.c_str();
    */
    const char* c_str() const{
        return data;
    }              

    /** 
     * @brief 4. Nạp chồng toán tử truy cập []
     * @details Thêm cú pháp chống lỗi khi truy cập ngoài chuỗi
     * @param i: chỉ số của kí tự cần đọc
     * @return data[i]: kí tự thứ i
    */
    // 4.1. Truy cập tổng quát ( để gán từ or else )
    char& operator[](int i){
        if ( i < 0 || i >= len ) {
            cout << "Loi truy cap" << endl;
            exit(1);
        }    
        return data[i];
    }

    // 4.2. Truy cập cho hằng số ( chỉ để đọc )
    const char& operator[](int i) const {
        if (i < 0 || i >= len) {
            cout << "Loi truy cap" << endl;
            exit(1);
        }
        return data[i];
    }



   // B. Nâng Cao

    /** 
     * @brief 1. So sánh theo thứ tự alphabet
     * @details So sánh từ trái tới phải, theo từng kí tự và thứ tự alphabet
     * 1. Duyệt từng kí tự ở 2 chuỗi cùng lúc đến khi gặp kí tự khác nhau hoặc hết một chuỗi
     * 2. Nếu kí tự khác nhau thì so sánh theo mã ASCII
     * 3. Nếu duyệt hết thì chuỗi nào dài hơn sẽ lớn hơn
     * @param s: chuỗi dùng để so sánh
     * @return 
     * 1 nếu ">"
     * 0 nếu "="
     * -1 nếu "<"
    */
    int compare( const MyString& s) const {
        // 1.
        int i = 0;
        while ( i < len && i < s.len ){
            // 2.
            if ( data[i] != s.data[i]){
                return (data[i] > s.data[i]) ? 1 : -1;
            }
            i ++;
        }

        // 3.
        if ( len == s.len ){
            return 0;
        }
        return ( len > s.len) ? 1: -1;
    }




    /** 
     * @brief 2. Tính khoảng cách Levenshtein 
     * @details Số phép biến đổi tối thiểu ( thêm / xóa / thay ) để biến chuỗi hiện tại thành chuỗi s
     * 1. Xử lý ngoại lệ
     * 2. Tạo ma trận dp
     * 3. Khởi tạo cột 0 và hàng 0
     * 4. dp[i][j] = min( xóa, thêm, thay ) dựa trên các ô đã tính
     * 5. Lấy kết quả ở góc dp[m][n] rồi giải phóng bộ nhớ
     * @param s: chuỗi dùng để so sánh
     * @return int: số phép biến đổi tối thiểu giữa 2 chuỗi
     * @note example: A = "HAT", B = "CATS"
     * dp[i][j]         j = 0(empty)        j = 1(C)            j = 2(A)        j = 3(T)        j=4 (S)
     * i = 0 (empty)        0                   1                   2               3               4               
     * i = 1 (H)            1                   1                   2               3               4
     * i = 2 (A)            2                   2                   1               2               3
     * i = 3 (T)            3                   3                   2               1               2
     * 
     * 2. Tại dp[i][j]
     * Xóa: dp[i-1][j] + 1
     * Thêm: dp[i][j-1] + 1
     * Thay: dp[i-1][j-1] + cost
     * ---> Mỗi bước chọn phương pháp tối thiểu để thế kí tự
    */
    int levenshtein(const MyString& s) const {
        int m = len;
        int n = s.len;

        // 1.
        if (m == 0) return n;
        if (n == 0) return m;

        // 2.
        int** dp = new int*[m + 1];
        for (int i = 0; i <= m; i++) {
            dp[i] = new int[n + 1];
        }

        // 3.
        for (int i = 0; i <= m; i++) {
            dp[i][0] = i;
        }
        for (int j = 0; j <= n; j++) {
            dp[0][j] = j;
        }

        // 4.
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                // chi phí thay: 0 nếu trùng kí tự, 1 nếu khác
                int cost = (data[i - 1] == s.data[j - 1]) ? 0 : 1;

                int del = dp[i - 1][j] + 1;          // Xóa
                int ins = dp[i][j - 1] + 1;          // Thêm
                int sub = dp[i - 1][j - 1] + cost;   // Thay

                dp [i][j] = Min(del,ins, sub);
            }
        }

        // 5.
        int result = dp[m][n];
        for (int i = 0; i <= m; i++) {
            delete[] dp[i];
        }
        delete[] dp;

        return result;
    }
};