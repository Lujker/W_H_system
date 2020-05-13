#ifndef PACKET_H
#define PACKET_H
#include <QTime>


class Packet{
public:
    int number_of_packet;
    QTime time_of_accept;
    //сделать чтоб продуктов в пакете могло быть несколько (а можно не усложнять и приянть условие что в одном пакете один товар)
    int artic, number_of_product, number_of_table;
    QString name_of_product;

    Packet(int n_o_p, QTime timer, int art, int n_o_prod, int n_o_tab, QString name);
    Packet(){}
};

#endif // PACKET_H
