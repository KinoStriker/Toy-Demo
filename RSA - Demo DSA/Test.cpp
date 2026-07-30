#include <iostream>
#include "BigInt.h"
#include "RNG.h"
#include "KeyPair.h"
#include "ByteArray.h"

/* =====================================================================
 *  KICH BAN 1: Test output
 * ===================================================================== */
bool testRSACycle(const BigInt& p, const BigInt& q, const BigInt& m) {
    // ========================================================
    //  1. SINH KHOA
    // ========================================================
    KeyPair rsa;
    rsa.generateKey(p, q);

    cout << "----- KHOA -----\n";
    cout << "n = " << rsa.getN().toHex() << "\n";
    cout << "e = " << rsa.getE().toHex() << "\n";
    cout << "d = " << rsa.getD().toHex() << "\n\n";

    // Kiểm tra điều kiện bắt buộc: m < n
    if (rsa.getN() < m) {
        cout << "[BO QUA] m >= n nen khong the ma hoa.\n";
        cout << "         Hay chon m nho hon, hoac p, q lon hon.\n";
        return false; // Test thất bại do lỗi đầu vào
    }

    // ========================================================
    //  2. MA HOA  ->  c = m^e mod n
    // ========================================================
    BigInt c = rsa.encrypt(m);

    cout << "----- MA HOA -----\n";
    cout << "Ban ro m = " << m.toHex() << "\n";
    cout << "Ban ma c = " << c.toHex() << "\n\n";

    // ========================================================
    //  3. GIAI MA  ->  m2 = c^d mod n
    // ========================================================
    BigInt m2 = rsa.decrypt(c);

    cout << "----- GIAI MA -----\n";
    cout << "Ket qua m2 = " << m2.toHex() << "\n\n";

    // ========================================================
    //  4. KIEM TRA  ->  m2 co bang m khong?
    // ========================================================
    BigInt diff = m - m2;   // bang 0 <=> m2 == m

    cout << "----- KET QUA KIEM THU -----\n";
    if (diff.isZero()) {
        cout << "PASS: Giai ma khop ban ro (decrypt(encrypt(m)) == m)\n";
        return true;
    } else {
        cout << "FAIL: Giai ma KHONG khop ban ro!\n";
        return false;
    }
}


/* =====================================================================
 *  KICH BAN 2: Edge case
 * ===================================================================== */
bool testEdgeMessages() {
    cout << "----- KICH BAN 2: THONG DIEP BIEN -----\n\n";
 
    // ----- SINH KHOA -----
    // Dung seed co dinh (42) de moi lan chay cho ra cung mot bo khoa.
    RNG rng(42ULL);
    KeyPair kp;
    kp.generate(24, rng);          // moi so nguyen to 24 bit -> n ~ 48 bit
 
    BigInt n = kp.getN();
    BigInt zero(0LL);
    BigInt one(1LL);
    BigInt nMinus1 = n - one;      // gia tri hop le lon nhat
 
    cout << "n = " << n.toHex() << "\n\n";
 
    // ----- 3 GIA TRI BIEN CAN KIEM TRA -----
    BigInt tests[3] = { zero,    one,     nMinus1   };
    string names[3] = { "m = 0", "m = 1", "m = n-1" };
 
    // ----- MA HOA -> GIAI MA -> SO SANH cho tung gia tri -----
    bool allOk = true;
    for (int i = 0; i < 3; i++) {
        BigInt c    = kp.encrypt(tests[i]);   // ma hoa
        BigInt back = kp.decrypt(c);          // giai ma
        BigInt diff = back - tests[i];        // bang 0 <=> giai ma dung
 
        if (diff.isZero()) {
            cout << "PASS: " << names[i] << "\n";
        } else {
            cout << "FAIL: " << names[i] << "\n";
            allOk = false;
        }
    }
 
    return allOk;
}
 
 
/* =====================================================================
 *  KICH BAN 3: ByteArray <-> BigInt 
 * ===================================================================== */
bool testByteArrayBridge() {
    cout << "----- KICH BAN 3: ByteArray <-> BigInt -----\n\n";
 
    bool allOk = true;
 
    // ----- PHAN 1: BigInt -> ByteArray -> BigInt phai giu nguyen gia tri -----
    cout << "----- PHAN 1: BigInt -> ByteArray -> BigInt -----\n";
 
    BigInt vals[7] = {
        BigInt(0LL),
        BigInt(1LL),
        BigInt(255LL),
        BigInt(256LL),
        BigInt(0x123456789ALL),     // 5 byte
        BigInt(0xAB00CD00EFLL),     // co byte 0x00 xen GIUA
        (BigInt(0x12345678LL) * (BigInt(2LL) ^ 32)) + BigInt(0x9ABCDEF0LL) // = 0x123456789ABCDEF0, 8 byte
    };
 
    for (int i = 0; i < 7; i++) {
        ByteArray ba;
        ba.fromBigInt(vals[i], 0);     // so   -> byte
        BigInt back = ba.toBigInt();   // byte -> so
        BigInt diff = back - vals[i];  // bang 0 <=> giu nguyen
 
        if (diff.isZero()) {
            cout << "PASS: " << vals[i].toHex() << "\n";
        } else {
            cout << "FAIL: " << vals[i].toHex()
                 << " -> back = " << back.toHex() << "\n";
            allOk = false;
        }
    }
    cout << "\n";
 
    // ----- PHAN 2: tham so 'width' dem byte 0x00 o dau, gia tri khong doi -----
    cout << "----- PHAN 2: Dem bo nho voi width = 5 -----\n";
 
    BigInt v(0xABCDLL);            // 2 byte noi dung
    ByteArray ba;
    ba.fromBigInt(v, 5);           // ep thanh 5 byte (them 3 byte 0x00 o dau)
 
    if (ba.getSize() == 5) {
        cout << "PASS: dung 5 byte (co dem 0x00)\n";
    } else {
        cout << "FAIL: so byte = " << ba.getSize() << " (mong doi 5)\n";
        allOk = false;
    }
 
    if (ba[0] == 0 && ba[1] == 0 && ba[2] == 0) {
        cout << "PASS: 3 byte dau la 0x00 (padding)\n";
    } else {
        cout << "FAIL: padding sai\n";
        allOk = false;
    }
 
    BigInt diffPad = ba.toBigInt() - v;   // bang 0 <=> padding khong doi gia tri
    if (diffPad.isZero()) {
        cout << "PASS: padding khong lam doi gia tri so\n";
    } else {
        cout << "FAIL: padding lam doi gia tri so\n";
        allOk = false;
    }
    cout << "\n";
 
    // ----- PHAN 3: byte -> BigInt -> ma hoa -> giai ma -> byte -----
    cout << "----- PHAN 3: ByteArray -> BigInt -> encrypt + decrypt -> ByteArray -----\n";
 
    RNG rng(7ULL);
    KeyPair kp;
    kp.generate(32, rng);          // n ~ 64 bit
 
    BigInt original(0x00C0FFEELL); // gia tri "ban ro" trong bo nho
    ByteArray src;
    src.fromBigInt(original, 0);
 
    BigInt m  = src.toBigInt();    // byte  -> so
    BigInt c  = kp.encrypt(m);     // ma hoa
    BigInt m2 = kp.decrypt(c);     // giai ma
 
    ByteArray dst;
    dst.fromBigInt(m2, 0);         // so    -> byte
 
    BigInt diffCycle = dst.toBigInt() - original;   // bang 0 <=> ve dung goc
    if (diffCycle.isZero()) {
        cout << "PASS: byte -> BigInt -> encrypt -> decrypt -> byte == goc\n";
    } else {
        cout << "FAIL: ket qua khac ban dau\n";
        allOk = false;
    }
 
    return allOk;
}
 
 
int main() {
    bool ok1 = testRSACycle(BigInt(1000000007LL), BigInt(1000000009LL), BigInt(123456789LL));
    cout << "\n";
    bool ok2 = testEdgeMessages();
    cout << "\n";
    bool ok3 = testByteArrayBridge();
 
    // ----- TONG KET -----
    cout << "\n========================================\n";
    cout << "----- TONG KET -----\n";
    cout << "Kich ban 1 (round-trip co ban):    " << (ok1 ? "PASS" : "FAIL") << "\n";
    cout << "Kich ban 2 (thong diep bien):      " << (ok2 ? "PASS" : "FAIL") << "\n";
    cout << "Kich ban 3 (ByteArray <-> BigInt): " << (ok3 ? "PASS" : "FAIL") << "\n";
 
    if (ok1 && ok2 && ok3) {
        cout << "\n=> TAT CA KICH BAN PASS\n";
    } else {
        cout << "\n=> CO KICH BAN FAIL\n";
    }
 
    return 0;
}