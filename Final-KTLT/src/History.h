#pragma once
#include "MyString.h"
#include "DynamicArray.h"

/** 
 * @brief Một nút trong danh sách lịch sử
 * @details Sử dụng doubly linked list 
 * Lưu bản sao từ đã tra và 2 con trỏ tới nút liền trước / liền sau
*/
struct HistoryNode {
    MyString     word;      // bản sao từ
    HistoryNode* prev;      // nút mới hơn
    HistoryNode* next;      // nút cũ hơn

    /** 
     * @brief Khởi tạo một nút lịch sử
     * @param w: từ cần lưu
    */
    HistoryNode(const MyString& w){
        word = w;
        prev = nullptr;
        next = nullptr;
    }
};


/** 
 * @brief Danh sách lịch sử tra từ theo cơ chế LRU 
 * @details Bản chất là doubly-linked list

*/
class HistoryList {
private:
    HistoryNode* head;          // mới nhất
    HistoryNode* tail;          // cũ nhất
    int          size;          // số nút
    int          capacity;      // sức chứa tối đa


    /** 
     * @brief 1. Tìm nút theo từ
     * @details Duyệt từ head tới tail, so sánh chính xác bằng compare()
     * @param w: từ cần tìm
     * @return Con trỏ tới nút khớp, hoặc nullptr nếu không tìm thấy
    */
    HistoryNode* find(const MyString& w) const{
        HistoryNode* cur = head;
        while (cur != nullptr) {
            if (cur->word.compare(w) == 0) {
                return cur;
            }
            cur = cur->next;
        }
        return nullptr;
    }


    /** 
     * @brief 2. Gỡ một nút ra khỏi danh sách
     * @details 
     * 1. Nối nút trước và nút sau của node lại với nhau 
     * 1.1. cập nhật head nếu node đang là đầu 
     * 1.2. Cập nhật tail nếu node đang là cuối
     * 2. Cô lập node và giảm size
     * @param n: nút cần gỡ
    */
    void detach(HistoryNode* node){
        if ( node == nullptr) return ;

        // 1. + 1.1.
        if (node->prev != nullptr) {
            node->prev->next = node->next;
        } else{
            head = node->next;
        }

        // 1. + 1.2. 
        if (node->next != nullptr) {
            node->next->prev = node->prev; 
        } else {
            tail = node->prev;
        }

        // 2.
        node -> prev = nullptr;
        node -> next = nullptr;
        size --;

    }

    /** 
     * @brief 3. Gắn một nút lên đầu danh sách
     * @details 
     * 1. Trỏ next của n vào head cũ
     * 2. Nối head cũ ngược lại n 
     * 3. Cập nhật head và tăng size
     * @param n: nút cần gắn lên đầu
     * @note O(1)
    */
    void pushFront(HistoryNode* node){
        if ( node == nullptr ) return ;

        // 1.
        node->prev = nullptr;
        node->next = head;

        // 2.
        if (head != nullptr) {
            head->prev = node;
        } else { 
            tail = node;            //List trống
        }
 
        // 3.
        head = node;
        size++;
    }


    /** 
     * @brief 4. Xóa nút cũ nhất ( ở cuối list )
     * @details Gỡ tail ra khỏi danh sách rồi giải phóng bộ nhớ
    */
    void deleteTail(){
        if (tail == nullptr) return;
 
        HistoryNode* old = tail;
        detach(old);
        delete old;
    }  

public:

    // ------------------------------------------------------------------- CONSTRUCTOR -------------------------------------------------------------------
    
    /** 
     * @brief 1. Constructor
     * @details Khởi tạo danh sách rỗng
     * @param cap: sức chứa tối đa, mắc định là 10 
    */
    HistoryList(int cap = 10){
        head = nullptr;
        tail = nullptr;
        size = 0;
        capacity = (cap < 1) ? 1 : cap;
    }

    /** 
     * @brief 2. Copy Constructor 
     * @details 
     * 1. Khởi tạo danh sách rỗng + sức chứa
     * 2. Duyệt other từ tail -> head rồi pushFront 
     * @param other: danh sách gốc dùng để sao chép
    */
    HistoryList(const HistoryList& other){
        // 1.
        head = nullptr;
        tail = nullptr;
        size = 0;
        capacity = other.capacity;
 
        // 2.
        HistoryNode* cur = other.tail;
        while (cur != nullptr) {
            HistoryNode* node = new HistoryNode(cur->word);
            pushFront(node);
            cur = cur->prev;
        }
    }

    
    /** 
     * @brief 3. Nạp chồng toán tử gán "="
     * @details 
     * 1. Chống tự gán ( như a = a )
     * 2. Xóa list hiện tại
     * 3. Chép sức chứa + từng nút từ other ( cũ -> mới )
     * @param other: danh sách gốc dùng để gán
     * @return Tham chiếu tới chính nó
     * @note 
     * 1. Hỗ trợ chống lỗi tự gán. VD: a = a
     * 2. O(n)
    */
    HistoryList& operator=(const HistoryList& other){
        if (this != &other) {
            // 2.
            clear();
 
            // 3.
            capacity = other.capacity;
            HistoryNode* cur = other.tail;
            while (cur != nullptr) {
                HistoryNode* node = new HistoryNode(cur->word);
                pushFront(node);
                cur = cur->prev;
            }
        }
        return *this;
    }

    /** 
     * @brief 4. Destructor
     * @details Giải phóng toàn bộ nút trong danh sách
    */
    ~HistoryList(){
        clear();
    }


    // ------------------------------------------------------------------- METHOD  -------------------------------------------------------------------


    /** 
     * @brief 1. Thêm một từ vào lịch sử theo cơ chế LRU 
     * @details 
     * 1. Nếu từ đã có => đẩy nút đó lên đầu
     * 2. Nếu là từ mới => tạo nút mới rồi gắn lên đầu
     * 3. Nếu vượt sức chứa => xóa nút cũ nhất ở cuối
     * @param w: từ vừa được tra
    */
    void add(const MyString& w){
        // 1.
        HistoryNode* found = find(w);
        if (found != nullptr) {
            detach(found);
            pushFront(found);
            return;
        }
 
        // 2.
        HistoryNode* node = new HistoryNode(w);
        pushFront(node);
 
        // 3.
        if (size > capacity) {
            deleteTail();
        }
    }
    
    /** 
     * @brief 2. Lấy danh sách từ theo thứ tự mới đến cũ
     * @details 
     * 1. Xóa sạch mảng kết quả 
     * 2. Duyệt từ head tới tail, thêm từng từ vào mảng
     * @param out: mảng động dùng để chứa kết quả
    */
    void getRecent(dynamicArray<MyString>& out) const{
        // 1.
        out.clear();
 
        // 2.
        HistoryNode* cur = head;
        while (cur != nullptr) {
            out.push_back(cur->word);
            cur = cur->next;
        }
    }

    /** 
     * @brief 3. Xóa sạch lịch sử
     * @details Duyệt và giải phóng từng nút, sau đó reset tham số
    */
    void clear(){
        HistoryNode* cur = head;
        while (cur != nullptr) {
            HistoryNode* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        head = nullptr;
        tail = nullptr;
        size = 0;
    }



    /** 
     * @brief 4. Lấy số từ đang lưu
     * @return Trả về size
    */
    int getsize() const {
        return size;
    }
};