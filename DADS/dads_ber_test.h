#ifndef DADS_BER_TEST_H
#define DADS_BER_TEST_H
#include <itpp/itcomm.h>
#include <itpp/itstat.h>
using namespace std;
using namespace itpp;
#include "dads_modem.h"

class DADS_BER_Test
{
public:
    DADS_BER_Test();

private:
    DADS_Modem *modem;
};

#endif // DADS_BER_TEST_H
