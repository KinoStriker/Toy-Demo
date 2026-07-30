#pragma once
#include <iostream>

using namespace std;

template <typename T>
// Placeholder cho kiểu dữ liệu
// Quyết định kiểu dữ liệu gì cụ thể thì sẽ dùng sau. VD: dynamicArray<int> abcxyz;

    /** 
     * @brief 
     * @details 
     * @param
     * @return
     * @note
    */


/** 
 * @brief Cấu trúc mảng động
*/
class dynamicArray {
private:
    T* arr;             // Con trỏ quản lý mảng 
    int capacity;       // Sức chứa tối đa hiện tại
    int size;           // Số lượng phần tử thực tế 

// ------------------------------------------------------------------- PRIVATE FUNCTION  -------------------------------------------------------------------
    /** 
     * @brief Hàm tự đông hỗ trợ nhân đôi khi mảng đầy
     * @details 
     * 1. Cấp vùng mới rông gấp đôi mảng hiện tại
     * 2. Chép dữ liệu cũ sang vùng mới
     * 3. Xóa vùng nhớ cũ
     * 4. Trỏ sang bộ nhớ mới
     * @note O(n)
    */
    void grow() {
        capacity = (capacity == 0) ? 1 : capacity * 2;
        T* temp = new T[capacity];
        for (int i = 0; i < size; i++) {
            temp[i] = arr[i];
        }
        delete[] arr;
        arr = temp;
    }

public:
    // ------------------------------------------------------------------- CONSTRUCTOR -------------------------------------------------------------------
    // Constructor: Các hàm khởi tạo dữ liệu ban đầu cho một object ngay khi nó vừa ra đời.
    // Không return gì
    // Vòng đời: Sinh ra (Constructor / Copy Constructor) ➔ Hoạt động (Các phương thức, Toán tử gán =) ➔ Chết đi (Destructor).

    /** 
     * @brief 1. Constructor
     * @details Khởi tạo mảng ban đầu
     * @param cap: sức chứa ban đầu. Tham số là tùy chọn ( mặc định là 10 ) 
     * @note O(1)
    */
    dynamicArray(int cap = 10) {                    
        capacity = cap;                          
        size = 0;
        arr = new T[capacity];
    }

    /** 
     * @brief 2. Destructor
     * @details Xóa mảng khi không cần sử dụng
    */
    ~dynamicArray() {
        delete[] arr;
    }

    /** 
     * @brief 3. Copy Constructor ( Deep Copy )
     * @details 
     * 1. Cập nhật tham số "capacity" và "size" của mảng mới vào mảng hiện tại
     * 2. Duyệt vòng lặp để sao chép phần tử
     * @param other: địa chỉ mảng gốc dùng để sao chép dữ liệu 
     * @note O(n)
    */
    dynamicArray(const dynamicArray& other) {
        capacity = other.capacity;
        size = other.size;
        arr = new T[capacity];
        for (int i = 0; i < size; i++) {
            arr[i] = other.arr[i];
        }
    }

    // ------------------------------------------------------------------- METHOD  -------------------------------------------------------------------


    /** 
     * @brief 1. Nạp chồng toán tử gán "="
     * @details 
     * 1. Xóa mảng cũ
     * 2. Cập nhật tham số "capacity" và "size" của mảng mới vào mảng hiện tại
     * 3. Duyệt vòng lặp để gán dữ liệu ở mảng other sang mảng gốc
     * @param
     * @return
     * @note 
     * 1. Hỗ trợ chống lỗi tự gán. VD: a = a
     * 2. O(n)
    */
    dynamicArray& operator=(const dynamicArray& other) {
        if (this != &other) {           
            delete[] arr;               
            capacity = other.capacity;
            size = other.size;
            arr = new T[capacity];      
            for (int i = 0; i < size; i++) {
                arr[i] = other.arr[i];
            }
        }
        return *this;
    }


    /** 
     * @brief 2. Thêm phần tử vào cuối mảng
     * @details 
     * 1. Tự động nhân đôi mảng khi đầy
     * 2. Gán vào cuối mảng
     * @param data: phần tử cần thêm vào
     * @note O(n)
    */
    void push_back(T data) {
        if (size == capacity) {
            grow(); 
        }
        arr[size] = data;
        size++;
    }


    /** 
     * @brief 3. Xóa phần tử ở cuối mảng
     * @details 
     * 1. Giảm "size" đi 1 đơn vị --- Nhằm cấm truy cập vào phần tử cuối
     * @note O(1)
    */
    void pop_back() {
        if (size > 0) {
            size--;
        }
    }

    /** 
     * @brief 4. Lấy kích thước mảng
     * @return Trả về size
     * @note O(1)
    */
    int getsize() const {
        return size;
    }

    // Xóa sạch mảng (thực tế chỉ cần reset con trỏ size)
    /** 
     * @brief 5. Xóa mảng
     * @details Chỉ set size về 0
     * @note
     * 1. O(1)
     * 2. Không dùng delete[] vì tốn CPU và chuẩn bị dùng lại mảng đã khởi tạo
    */
    void clear() {
        size = 0; 
    }


    /** 
     * @brief 6. Nạp chồng toán tử truy cập "[]""
     * @details Thêm cú pháp chống lỗi khi truy cập ngoài mảng
     * @param index: index của phần tử cần truy cập
     * @return arr[index]: phần tử có thứ tư inde
     * @note O(1)
    */
    T& operator[](int index) {
        if (index < 0 || index >= size) {
            cout << "Loi: Truy cap vuot qua gioi han mang!" << endl;
            exit(1);
        }
        return arr[index];
    }

    // Toán tử [] dành cho các đối tượng hằng số (const)
    const T& operator[](int index) const {
        if (index < 0 || index >= size) {
            cout << "Loi: Truy cap vuot qua gioi han mang!" << endl;
            exit(1);
        }
        return arr[index];
    }
};