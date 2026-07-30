#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "BigInt.h"
#include "RNG.h"
#include "KeyPair.h"
#include "ByteArray.h"

using namespace std;
using namespace std::chrono;

// GIAO DIEN MENU CHINH
void printMenu() {
    cout << "\n================= PHAN MEM MA HOA RSA =================\n";
    cout << "1. Doc cap khoa tu file (Load Keys)\n";
    cout << "2. Sinh cap khoa RSA ngau nhien & Luu file (Generate Keys and Save): \n";
    cout << "3. Ma hoa file thu cong (Encrypt)\n";
    cout << "4. Giai ma file thu cong (Decrypt)\n";
    cout << "0. Thoat chuong trinh (Exit) \n";
    cout << "=======================================================\n";
    cout << "Nhap lua chon cua ban (0-4): ";
}

int main() {
    int choice;
    RNG rng;
    KeyPair rsa; 
    bool hasKey = false; // Dung de kiem tra trang thai khoa truoc khi ma/giai ma

    while (true) {
        printMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 0) {
            cout << "Thoat chuong trinh. Tam biet!\n";
            break;
        }

        switch (choice) {
            case 1: {
                cout << "\n--- DOC CAP KHOA TU FILE ---\n";
                string pubPath, privPath;
                cout << "Nhap duong dan file Public Key: ";
                cin >> pubPath;
                cout << "Nhap duong dan file Private Key: ";
                cin >> privPath;
                
                // Goi ham load co san tu KeyPair
                rsa.loadPublic(pubPath);
                rsa.loadPrivate(privPath);
                
                cout << "=> Tinh trang: Da nap cap khoa tu file vao bo nho he thong.\n";
                hasKey = true;
                break;
            }
            case 2: {
                int bits;
                cout << "\n--- SINH KHOA RSA NGAU NHIEN ---\n";
                cout << "Nhap do dai khoa mong muon ( Toi da 80 bit de dam bao an toan ): ";
                cin >> bits;
                
                cout << "[Hethong] Dang xu ly thuat toan sinh cac so nguyen to lon...\n";
                
                // Do thoi gian sinh khoa
                auto start = high_resolution_clock::now();
                rsa.generate(bits / 2, rng); 
                auto end = high_resolution_clock::now();
                
                cout << "=> Sinh khoa thanh cong!\n";
                cout << "=> [THOI GIAN SINH KHOA]: " 
                     << duration_cast<milliseconds>(end - start).count() << " ms\n";
                     
                // Luu khoa luon ra file sau khi sinh
                string pubPath, privPath;
                cout << "Nhap ten file de ghi Public Key: ";
                cin >> pubPath;
                cout << "Nhap ten file de ghi Private Key: ";
                cin >> privPath;
                
                rsa.savePublic(pubPath);
                rsa.savePrivate(privPath);
                
                cout << "=> Tinh trang: Da xuat cap khoa ra file thanh cong.\n";
                hasKey = true;
                break;
            }
            case 3: {
                cout << "\n--- MA HOA FILE THU CONG ---\n";
                if (!hasKey) {
                    cout << "[Loi] Thieu khoa de lam viec! Hay sinh khoa (Option 2) hoan nap khoa (Option 1) truoc.\n";
                    break;
                }
                
                string inFile, outFile;
                cout << "Nhap ten file van ban ban da tu tao san ( vui long khong qua 9 byte de dam bao he thong ): ";
                cin >> inFile;
                cout << "Nhap ten file ban ma muon xuat ra: ";
                cin >> outFile;

                ByteArray inputData;
                inputData.readFile(inFile);
                
                if (inputData.getSize() == 0) {
                    cout << "[Loi] File nguon rong hoac duong dan khong ton tai!\n";
                    break;
                }
                
                BigInt m = inputData.toBigInt();
                
                if (rsa.getN() < m) {
                    cout << "[Loi] Du lieu tinh toan m dang lon hon Modulus N! Can dung so bit khoa cao hon.\n";
                    break;
                }

                // --- BAT DAU DO THOI GIAN MA HOA ---
                auto start = high_resolution_clock::now();
                BigInt c = rsa.encrypt(m);
                auto end = high_resolution_clock::now();
                // --- KET THUC DO THOI GIAN MA HOA ---

                ByteArray outputData;
                outputData.fromBigInt(c, 0);
                outputData.writeFile(outFile);

                cout << "=> Ma hoa file hoan tat, file ban ma duoc tao tai: " << outFile << "\n";
                cout << "=> [THOI GIAN MA HOA]: " 
                     << duration_cast<milliseconds>(end - start).count() << " ms\n";
                break;
            }
            case 4: {
                cout << "\n--- GIAI MA FILE THU CONG ---\n";
                if (!hasKey) {
                    cout << "[Loi] Thieu khoa de lam viec! Hay sinh khoa (Option 2) hoan nap khoa (Option 1) truoc.\n";
                    break;
                }
                
                string inFile, outFile;
                cout << "Nhap ten file ban ma can giai ma: ";
                cin >> inFile;
                cout << "Nhap ten file muon ghi ket qua van ban: ";
                cin >> outFile;

                ByteArray inputData;
                inputData.readFile(inFile);
                
                if (inputData.getSize() == 0) {
                    cout << "[Loi] File nguon rong hoac duong dan khong ton tai!\n";
                    break;
                }
                
                BigInt c = inputData.toBigInt();

                // --- BAT DAU DO THOI GIAN GIAI MA ---
                auto start = high_resolution_clock::now();
                BigInt m = rsa.decrypt(c);
                auto end = high_resolution_clock::now();
                // --- KET THUC DO THOI GIAN GIAI MA ---

                ByteArray outputData;
                outputData.fromBigInt(m, 0); 
                outputData.writeFile(outFile);

                cout << "=> Giai ma file hoan tat, file ket qua duoc tao tai: " << outFile << "\n";
                cout << "=> [THOI GIAN GIAI MA]: " 
                     << duration_cast<milliseconds>(end - start).count() << " ms\n";
                break;
            }
            default:
                cout << "Lua chon khong co trong danh muc! Nhap lai tu 0 den 4.\n";
        }
    }
    return 0;
}