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
