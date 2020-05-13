#include "packet.h"

Packet::Packet(int n_o_p, QTime timer, int art, int n_o_prod, int n_o_tab, QString name)
{
            number_of_packet=n_o_p;
            time_of_accept=timer;
            artic=art;
            number_of_product=n_o_prod;
            number_of_table=n_o_tab;
            name_of_product=name;
}

Packet::Packet()
{
    number_of_packet=0;
    time_of_accept=QTime::currentTime();
    artic=0;
    number_of_product=0;
    number_of_table=0;
    name_of_product="";
}

bool Packet::operator ==(Packet pc)
{
    return this->number_of_packet==pc.number_of_packet;
}

