#include <iostream>
#include <cstdlib>
#include "Dictionary.h"
#include "DynamicArray.h"
#include "History.h"
#include "MyString.h"

using namespace std;


/**
 * @brief Hàm chính: giao diện menu điều khiển từ điển Anh - Việt
 * @details
 * 1. Nạp dữ liệu từ file lúc khởi động
 * 2. Lặp hiển thị menu và xử lý lựa chọn của người dùng:
 *    1  - Thêm / sửa từ
 *    2  - Tra cứu chính xác
 *    3  - Tra cứu gần đúng
 *    4  - Xóa từ
 *    5  - Đánh dấu / bỏ yêu thích
 *    6  - Xem tất cả từ đã lưu
 *    7  - Xem danh sách yêu thích
 *    8  - Xem lịch sử tra cứu
 *    9  - Lưu ra file
 *    10 - Nạp từ file
 *    0  - Thoát
 * 3. Lưu dữ liệu ra file trước khi kết thúc
 * @return 0 khi chương trình kết thúc bình thường
 */


int main() {
    Dictionary dict;
    const char* FILE_PATH = "TuDien.txt";  

    // 1. Nạp dữ liệu 
    if (dict.loadFromFile(FILE_PATH)) {
        cout << "Da nap du lieu tu file: " << FILE_PATH << endl;
    } else {
        cout << "Chua co file du lieu. Bat dau voi tu dien rong." << endl;
    }

    int choice = -1;

    while (choice != 0) {

        // 2.1. Menu
        cout << "\n========== TU DIEN ANH - VIET ==========\n";
        cout << "1. Them / sua tu\n";
        cout << "2. Tra cuu chinh xac\n";
        cout << "3. Tra cuu gan dung\n";
        cout << "4. Xoa tu\n";
        cout << "5. Danh dau / bo yeu thich\n";
        cout << "6. Xem tat ca tu da luu\n";
        cout << "7. Xem danh sach yeu thich\n";
        cout << "8. Xem lich su tra cuu\n";
        cout << "9. Luu ra file\n";
        cout << "10. Nap tu file\n";
        cout << "0. Thoat\n";
        cout << "-----------------------------------------------------------------\n";
        cout << "Chon chuc nang: ";


        // 2.2. Đọc lựa chọn
        char choiceBuf[256];
        cin.getline(choiceBuf, 256);
        MyString choiceStr(choiceBuf);
        
        // Ép kiểu chuỗi về số nguyên 
        choice = atoi(choiceStr.c_str());

        // Kiểm tra choice
        if (choice == 0 && choiceStr.compare(MyString("0")) != 0) {
            choice = -1; 
        }


        // 2.3. Chức năng chính

        // I. Thêm / sửa từ
        if (choice == 1) {
            char buf[4096];

            cout << "Nhap tu (tieng Anh): ";
            cin.getline(buf, 4096);
            MyString w(buf);

            if (w.empty()) {
                cout << "Tu khong duoc rong!" << endl;
            } else {
                WordEntry e(w);

                // a. Nhập nhiều nghĩa
                cout << "Nhap cac nghia ( go Enter de dung ):" << endl;
                while (true) {
                    cout << "  Nghia: ";
                    cin.getline(buf, 4096);
                    MyString m(buf);
                    if (m.empty()) break;
                    e.addMeaning(m);
                }

                // b. Nhập nhiều ví dụ
                cout << "Nhap cac vi du ( go Enter de dung ):" << endl;
                while (true) {
                    cout << "  Vi du: ";
                    cin.getline(buf, 4096);
                    MyString ex(buf);
                    if (ex.empty()) break;
                    e.addExample(ex);
                }

                // c. Nhập nhiều từ đồng nghĩa
                cout << "Nhap cac tu dong nghia ( go Enter de dung ):" << endl;
                while (true) {
                    cout << "  Dong nghia: ";
                    cin.getline(buf, 4096);
                    MyString sy(buf);
                    if (sy.empty()) break;
                    e.addSynonym(sy);
                }

                dict.addWord(e);
                cout << "Da luu tu \"" << w.c_str() << "\" vao tu dien." << endl;
            }
        }

        // II. Tra cứu chính xác
        else if (choice == 2) {
            char buf[4096];

            // lặp đến khi tra được từ, hoặc gõ Enter trống để quay lại menu
            while (true) {
                cout << "Nhap tu can tra ( de trong de quay lai ): ";
                cin.getline(buf, 4096);
                MyString w(buf);

                // a. để trống => quay lại menu
                if (w.empty()) break;

                // b. tra cứu
                WordEntry* e = dict.lookup(w);

                // c. không tìm thấy => báo lỗi và cho nhập lại
                if (e == nullptr) {
                    cout << "Khong co tu nhu tu da nhap. Vui long nhap lai." << endl;
                    continue;
                }

                // d. tìm thấy => in toàn bộ thông tin của từ rồi dừng
                cout << "Tu: " << e->word.c_str();
                if (e->isFavorite) cout << "  [YEU THICH]";
                cout << endl;

                cout << "  Nghia:" << endl;
                for (int i = 0; i < e->meanings.getsize(); i++) {
                    cout << "    " << i + 1 << ". " << e->meanings[i].c_str() << endl;
                }

                cout << "  Vi du:" << endl;
                for (int i = 0; i < e->examples.getsize(); i++) {
                    cout << "    " << i + 1 << ". " << e->examples[i].c_str() << endl;
                }

                cout << "  Dong nghia:" << endl;
                for (int i = 0; i < e->synonyms.getsize(); i++) {
                    cout << "    " << i + 1 << ". " << e->synonyms[i].c_str() << endl;
                }

                break;
            }
        }

        // III. Tra cứu gần đúng
        else if (choice == 3) {
            char buf[4096];

            // a. nhập ngưỡng ( số ký tự sai tối đa cho phép )
            int max = -1;
            while (true) {
                cout << "Nhap so ky tu sai khac toi da: ";
                cin.getline(buf, 4096);
                MyString maxStr(buf);

                // Nếu nhấn Enter  -> back lại menu
                if (maxStr.empty()) {
                    break; 
                }

                // Kiểm tra xem chuỗi nhập vào có phải toàn là số không
                bool isValid = true;
                for (int i = 0; i < maxStr.length(); i++) {
                    if (maxStr[i] < '0' || maxStr[i] > '9') {
                        isValid = false;
                        break;
                    }
                }

                // Nếu nhập sai định dạng -> Yêu cầu nhập lại
                if (!isValid) {
                    cout << "Loi: Vui long chi nhap so nguyen!" << endl;
                    continue; 
                }

                // Ép kiểu chuỗi thành số nguyên
                max = 0;
                for (int i = 0; i < maxStr.length(); i++) {
                    max = max * 10 + (maxStr[i] - '0');
                }
                
                break; 
            }

            if (max == -1) {
                continue; 
            }

            // b. nhập từ cần tra
            cout << "Nhap tu can tra gan dung: ";
            cin.getline(buf, 4096);
            MyString w(buf);

            // c. tra cứu gần đúng theo ngưỡng vừa nhập
            dynamicArray<MyString> result;
            dict.approxSearch(w, max, result);

            // d. hiển thị kết quả
            if (result.getsize() == 0) {
                cout << "Khong co tu nao gan giong." << endl;
            } else {
                cout << "Cac tu gan giong ( " << result.getsize() << " ):" << endl;
                for (int i = 0; i < result.getsize(); i++) {
                    cout << "  - " << result[i].c_str() << endl;
                }
            }
        }

        // IV. Xóa từ 
        else if (choice == 4) {
            char buf[4096];
            cout << "Nhap tu can xoa: ";
            cin.getline(buf, 4096);
            MyString w(buf);

            if (dict.removeWord(w)) {
                cout << "Da xoa tu \"" << w.c_str() << "\"." << endl;
            } else {
                cout << "Khong tim thay tu \"" << w.c_str() << "\"." << endl;
            }
        }

        // V. Đánh dấu/Bỏ yêu thích 
        else if (choice == 5) {
            char buf[4096];
            cout << "Nhap tu can danh dau / bo yeu thich: ";
            cin.getline(buf, 4096);
            MyString w(buf);
            

            WordEntry* e = dict.lookup(w);


            if (e == nullptr) {
                cout << "Khong tim thay tu \"" << w.c_str() << "\" trong tu dien." << endl;
            } else {
                // Đảo trạng thái
                dict.toggleFavorite(w);

                if (e->isFavorite == true) {
                    cout << "Da danh dau yeu thich cho tu \"" << w.c_str() << "\"." << endl;
                } else {
                    cout << "Da loai bo yeu thich cho tu \"" << w.c_str() << "\"." << endl;
                }
            }
        }

        // VI. Xem tất cả từ đã lưu
        else if (choice == 6) {
            dynamicArray<MyString> words;
            dict.getAllWords(words);

            if (words.getsize() == 0) {
                cout << "Tu dien dang rong." << endl;
            } else {
                cout << "Danh sach tat ca tu da luu ( " << words.getsize() << " ):" << endl;
                for (int i = 0; i < words.getsize(); i++) {
                    cout << "  - " << words[i].c_str() << endl;
                }
            }
        }

        // VII. Xem danh sách yêu thích 
        else if (choice == 7) {
            dynamicArray<MyString> favs;
            dict.getFavorites(favs);

            if (favs.getsize() == 0) {
                cout << "Chua co tu yeu thich nao." << endl;
            } else {
                cout << "Danh sach tu yeu thich ( " << favs.getsize() << " ):" << endl;
                for (int i = 0; i < favs.getsize(); i++) {
                    cout << "  - " << favs[i].c_str() << endl;
                }
            }
        }

        // VIII. Xem lịch sử tra cứu 
        else if (choice == 8) {
            dynamicArray<MyString> hist;
            dict.getHistory(hist);

            if (hist.getsize() == 0) {
                cout << "Chua co lich su tra cuu." << endl;
            } else {
                cout << "Lich su tra cuu ( moi nhat truoc ):" << endl;
                for (int i = 0; i < hist.getsize(); i++) {
                    cout << "  " << i + 1 << ". " << hist[i].c_str() << endl;
                }
            }
        }

        // IX. Lưu ra file 
        else if (choice == 9) {
            if (dict.saveToFile(FILE_PATH)) {
                cout << "Da luu tu dien ra file: " << FILE_PATH << endl;
            } else {
                cout << "Loi: khong the mo file de luu!" << endl;
            }
        }

        // X. Nạp từ file 
        else if (choice == 10) {
            if (dict.loadFromFile(FILE_PATH)) {
                cout << "Da nap tu dien tu file: " << FILE_PATH << endl;
            } else {
                cout << "Loi: khong the mo file de nap!" << endl;
            }
        }

        // XI. Ngoại lệ 
        else {
            cout << "Lua chon khong hop le! Vui long chon lai." << endl;
        }
    }

    // 3. Lưu dữ liệu trước khi thoát
    if (dict.saveToFile(FILE_PATH)) {
        cout << "Da luu du lieu ra file. Tam biet!" << endl;
    } else {
        cout << "Khong the luu du lieu ra file. Tam biet!" << endl;
    }

    return 0;
}