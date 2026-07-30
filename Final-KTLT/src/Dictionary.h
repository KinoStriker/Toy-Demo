#pragma once
#include <fstream>
#include <cstdlib>
#include "MyString.h"
#include "DynamicArray.h"
#include "History.h"


/** 
 * @brief Cấu trúc lưu trữ toàn bộ thông tin của một từ
 * @details Gồm từ gốc, danh sách nghĩa, ví dụ, từ đồng nghĩa và flag yêu thích vào một bản ghi duy nhất
*/
struct WordEntry {
    MyString                word;       // Từ gốc
    dynamicArray<MyString>  meanings;   // Nghĩa tiếng việt
    dynamicArray<MyString>  examples;   // Ví Dụ
    dynamicArray<MyString>  synonyms;   // Từ đồng nghĩa
    bool                    isFavorite; // flag yêu thích


    // ------------------------------------------------------------------- CONSTRUCTOR -------------------------------------------------------------------

    /** 
     * @brief 1. Khởi tạo mặc định
     * @details word, meanings, examples, synonyms tự động gọi constructor mặc định của chúng 
     * @return Một từ rỗng, chưa được đánh dấu yêu thích
     * @note 
    */
    WordEntry(){
        isFavorite = false;
    }                    
    
    /** 
     * @brief 2. Khởi tạo thủ công
     * @details 
     * 1. Gán word = w 
     * 2. Các đặc trưng còn lại để ở mặc định
     * @param w: từ gốc dùng để khởi tạo
    */
    WordEntry(const MyString& w){
        word = w;
        isFavorite = false;
    }


    // ------------------------------------------------------------------- METHOD  -------------------------------------------------------------------

    /** 
     * @brief 1. Thêm một nghĩa mới
     * @param m: nghĩa tiếng Việt cần thêm
    */
    void addMeaning(const MyString& m){
        meanings.push_back(m);    
    }

    /** 
     * @brief 2. Thêm một ví dụ mới
     * @param e: câu ví dụ cần thêm
    */
    void addExample(const MyString& e) {
        examples.push_back(e);
    }
 
    /** 
     * @brief 3. Thêm một từ đồng nghĩa mới
     * @param s: từ đồng nghĩa cần thêm
    */
    void addSynonym(const MyString& s) {
        synonyms.push_back(s);
    }
 
    /** 
     * @brief 4. Đặt trạng thái yêu thích
     * @param f: true nếu đánh dấu yêu thích, false nếu bỏ đánh dấu
    */
    void setFavorite(bool f) {
        isFavorite = f;
    }

};


/** 
 * @brief Từ điển: quản lý tập các WordEntry
*/
class Dictionary {
private:
    dynamicArray<WordEntry> entries;  
    HistoryList history;

    
    /** 
     * @brief 1. Tìm chỉ số của một từ (
     * @details Sử dụng binary search + so sánh bằng compare()
     * @param w: từ cần tìm
     * @return Chỉ số của từ trong mảng, hoặc -1 nếu không tồn tại
    */
    int findIndex(const MyString& w) const{
        int lo = 0;
        int hi = entries.getsize() - 1;
 
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            int cmp = entries[mid].word.compare(w);
 
            if (cmp == 0) {
                return mid;
            } else if (cmp < 0) {   
                lo = mid + 1;
            } else {                
                hi = mid - 1;
            }
        }
        return -1;
        }


    /** 
     * @brief 2. Tách một chuỗi theo kí tự phân cách
     * @details Duyệt từng kí tự, gom thành từng mẩu, gặp dấu phân cách thì cắt
     * @param 
     * 1. str: chuỗi cần tách
     * 2. delim: kí tự dùng để cắt
     * 3. out: mảng động chứa các mẩu thu được
     * @note Hỗ trợ đọc file và in từ trên terminal 
     * Example: Trong file sẽ lưu: book \t 1.(...)|2.(...) \t không \t Hey... \t Publication
     * thì split sẽ tách thành
     * book
     * 1.(...)|2.(...)
     * Không ( yêu thích 
     * ....
    */
    void split(const MyString& str, char delim, dynamicArray<MyString>& out) const {
        out.clear();
        const char* s = str.c_str();
        char piece[4096];
        int p = 0;
 
        for (int i = 0; ; i++) {
            char c = s[i];
            if (c == delim || c == '\0') {
                piece[p] = '\0';
                MyString item(piece);
                out.push_back(item);
                p = 0;
                if (c == '\0') break;
            } else {
                if (p < 4095) piece[p++] = c;
            }
        }
    }
 
    /** 
     * @brief 3. Bỏ kí tự '\r' ở cuối dòng
     * @details Phòng trường hợp file lưu kiểu Windows ( xuống dòng là "\r\n" )
     * @param s: bộ đệm dòng vừa đọc
     * @note Hỗ trợ đọc file
    */
    void strip(char* s) const {
        int n = 0;
        while (s[n] != '\0') n++;
        if (n > 0 && s[n - 1] == '\r') s[n - 1] = '\0';
    }

public:

    // ------------------------------------------------------------------- METHOD  -------------------------------------------------------------------

    // A. Chức Năng
    /** 
     * @brief 1. Thêm một từ vào từ điển
     * @details 
     * 1. Nếu từ đã tồn tại => ghi đè bản ghi cũ
     * 2. Nếu là từ mới => push_back rồi sắp xếp lại mảng cho 1 phần tử đó ( insertion sort )
     * @param e: bản ghi từ cần thêm
    */
    void addWord(const WordEntry& e){
                // 1.
        int idx = findIndex(e.word);
        if (idx >= 0) {
            entries[idx] = e;
            return;
        }
 
        // 2.
        entries.push_back(e);
        int i = entries.getsize() - 1;
        while (i > 0 && entries[i - 1].word.compare(e.word) > 0) {
            WordEntry tmp = entries[i];
            entries[i]     = entries[i - 1];
            entries[i - 1] = tmp;
            i--;
        }
    }

    /** 
     * @brief 2. Tra cứu chính xác
     * @details Dùng findIndex() để định vị từ
     * @param w: từ cần tra
     * @return Con trỏ tới bản ghi khớp, hoặc nullptr nếu không có
     * @note 
    */
    WordEntry* lookup(const MyString& w){
        int idx = findIndex(w);
        if (idx < 0) {
            return nullptr;
        }
        history.add(w);
        return &entries[idx];
    }

    /** 
     * @brief 3. Tìm kiếm gần đúng
     * @details 
     * 1. Xóa sạch mảng kết quả
     * 2. Duyệt mọi từ, tính khoảng cách Levenshtein tới w
     * 3. Thu nhận những từ có khoảng cách <= ngưỡng
     * @param 
     * 1. w: từ cần tìm gần đúng
     * 2. max: ngưỡng, số kí tự sai khác tối đa cho phép
     * 3. out: mảng động chứa các từ gợi ý
    */
    void approxSearch(const MyString& w, int max, dynamicArray<MyString>& out){
 
        // 1.
        out.clear();
 
        // 2. + 3.
        for (int i = 0; i < entries.getsize(); i++) {
            int dist = entries[i].word.levenshtein(w);
            if (dist <= max) {
                out.push_back(entries[i].word);
            }
        }
    }

    /** 
     * @brief 4. Xóa một từ
     * @details 
     * 1. Tìm vị trí từ
     * 2. Dịch trái các phần tử phía sau
     * 3. pop_back để bỏ phần tử dư ở cuối
     * @param w: từ cần xóa
     * @return true nếu xóa được, false nếu không tìm thấy từ
    */
    bool removeWord(const MyString& w){
         // 1.
        int idx = findIndex(w);
        if (idx < 0) {
            return false;
        }
 
        // 2.
        int n = entries.getsize();
        for (int i = idx; i < n - 1; i++) {
            entries[i] = entries[i + 1];
        }
 
        // 3.
        entries.pop_back();
        return true;
    }

    /** 
     * @brief 5. Đảo trạng thái yêu thích
     * @details Tìm từ rồi đảo isFavorite
     * no fav - ( toggle ) -> fav và ngược lại
     * @param w: từ cần đổi trạng thái
    */
    void toggleFavorite(const MyString& w){
        int idx = findIndex(w);
        if (idx < 0) {
            return;
        }
        entries[idx].setFavorite(!entries[idx].isFavorite);
    }

    /** 
     * @brief 6. Lấy danh sách từ đang được yêu thích
     * @details 
     * 1. Xóa sạch mảng kết quả
     * 2. Duyệt toàn bộ, thu các từ có isFavorite = true
     * @param out: mảng động chứa kết quả ( vẫn theo thứ tự alphabet )
     * @note O(n)
    */
    void getFavorites(dynamicArray<MyString>& out) const{
        // 1.
        out.clear();
 
        // 2.
        for (int i = 0; i < entries.getsize(); i++) {
            if (entries[i].isFavorite) {
                out.push_back(entries[i].word);
            }
        }
    }


    /** 
     * @brief 7. Gọi HistoryList
     * @note Hỗ trợ hàm control.cpp
    */
    void getHistory(dynamicArray<MyString>& out) const {
        history.getRecent(out);    
    }


    /** 
     * @brief 8. Lấy danh sách toàn bộ từ đã lưu
     * @details 
     * 1. Xóa sạch mảng kết quả
     * 2. Duyệt toàn bộ, thu mọi từ
     * @param out: mảng động chứa kết quả theo thứ tự alphabet
    */
    void getAllWords(dynamicArray<MyString>& out) const {
        // 1.
        out.clear();
 
        // 2.
        for (int i = 0; i < entries.getsize(); i++) {
            out.push_back(entries[i].word);
        }
    }

    
    // B. File
 
    /** 
     * @brief 1. Nạp từ điển từ file
     * @details 
     * 1. Mở file
     * 2. Bỏ qua dòng tiêu đề
     * 3. Mỗi dòng còn lại: tách 5 cột theo Tab (\t) rồi dựng lại WordEntry
     * 4. addWord để vừa nạp vừa giữ thứ tự sắp xếp
     * @param path: đường dẫn file
     * @return true nếu nạp thành công, false nếu không mở được file
     * @note 
     * 1. Định dạng lưới ( như excel ): mỗi từ 1 dòng, 5 cột ngăn bởi Tab, dòng đầu là tiêu đề
     * 2. Trong 1 ô, nhiều nghĩa / ví dụ / đồng nghĩa ngăn nhau bởi '|'
     * 3. Không hỗ trợ Tab / '|' / xuống dòng nằm trong một ô; mỗi dòng tối đa 4096 kí tự
    */
    bool loadFromFile(const char* path){
        // 1.
        ifstream file(path);
        if (!file.is_open()) {
            return false;
        }
        entries.clear();
 
        char buf[4096];
        dynamicArray<MyString> cols;    // 5 cột của 1 dòng: từ / nghĩa / yêu thích ? / ví dụ / đồng nghĩa
        dynamicArray<MyString> items;   // các phần tử bên trong 1 ô
 
        // 2.
        file.getline(buf, sizeof(buf));   // bỏ qua tiêu đề
 
        // 3.
        while (file.getline(buf, sizeof(buf))) {
            strip(buf);
            if (buf[0] == '\0') continue;   // bỏ dòng trống
 
            MyString line(buf);
            split(line, '\t', cols);
 
            // cột 0: từ ( bắt buộc có )
            MyString word = (cols.getsize() > 0) ? cols[0] : MyString("");
            if (word.empty()) continue;
            WordEntry e(word);
 
            // cột 2: yêu thích ( "Có" -> true, còn lại -> false )
            if (cols.getsize() > 2) {
                e.setFavorite( cols[2].compare(MyString("Có")) == 0 );
            }
 
            // cột 1: nghĩa ( nhiều, ngăn bởi '|' )
            if (cols.getsize() > 1 && !cols[1].empty()) {
                split(cols[1], '|', items);
                for (int k = 0; k < items.getsize(); k++) e.addMeaning(items[k]);
            }
 
            // cột 3: ví dụ
            if (cols.getsize() > 3 && !cols[3].empty()) {
                split(cols[3], '|', items);
                for (int k = 0; k < items.getsize(); k++) e.addExample(items[k]);
            }
 
            // cột 4: đồng nghĩa
            if (cols.getsize() > 4 && !cols[4].empty()) {
                split(cols[4], '|', items);
                for (int k = 0; k < items.getsize(); k++) e.addSynonym(items[k]);
            }
 
            // 4.
            addWord(e);
        }
 
        file.close();
        return true;
    }
 
    /** 
     * @brief 2. Lưu từ điển ra file 
     * @details 
     * 1. Mở file
     * 2. Ghi dòng tiêu đề
     * 3. Mỗi từ ghi 1 dòng: 5 cột ngăn bởi Tab; nhiều phần tử trong 1 ô nối bằng '|'
     * @param path: đường dẫn file
     * @return true nếu lưu thành công, false nếu không mở được file
     * @note Cột theo thứ tự: Từ | Nghĩa | Yêu thích | Câu ví dụ | đồng nghĩa
    */
    bool saveToFile(const char* path) const {
        // 1.
        ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
 
        // 2.
        file << "Từ\tNghĩa\tYêu thích\tCâu ví dụ\tđồng nghĩa\n";
 
        // 3.
        for (int i = 0; i < entries.getsize(); i++) {
            const WordEntry& e = entries[i];
 
            // cột 0: từ
            file << e.word.c_str() << "\t";
 
            // cột 1: nghĩa
            for (int j = 0; j < e.meanings.getsize(); j++) {
                if (j > 0) file << "|";
                file << e.meanings[j].c_str();
            }
            file << "\t";
 
            // cột 2: yêu thích
            file << (e.isFavorite ? "Có" : "không") << "\t";
 
            // cột 3: ví dụ
            for (int j = 0; j < e.examples.getsize(); j++) {
                if (j > 0) file << "|";
                file << e.examples[j].c_str();
            }
            file << "\t";
 
            // cột 4: đồng nghĩa
            for (int j = 0; j < e.synonyms.getsize(); j++) {
                if (j > 0) file << "|";
                file << e.synonyms[j].c_str();
            }
            file << "\n";
        }
 
        file.close();
        return true;
    }
};