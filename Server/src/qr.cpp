#include "qr.h"


bool QR::operator==(QR other)
{
    return this->numb_post==other.numb_post;
}

bool QR::operator!=(QR other)
{
    return !(*this==other);
}
