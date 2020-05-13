#ifndef QR_H
#define QR_H

struct QR {
    int qr_state;//qr-метка на полу
    int qr_post;//qr-метка на стелаже
    int numb_post;//номер стелажа

    bool operator==(QR other);
    bool operator!=(QR other);
};

#endif // QR_H
