#include <rtl/ustring>
#include <stdio.h>
#include <typeinfo.h>
#include "samplelibrtti.hxx"


//using namespace salhelper;
using namespace rtl;



int main( int argc, char *argv[ ], char *envp[ ] )
{
    MyClassB b;
    MyClassA* pA= &b;
    // test the virtual function
    pA->funcA();

    const type_info& i1= typeid ( b);
    const type_info& i2= typeid( pA);

    if( typeid( b) == typeid( pA))
        printf("\nsame types");

    
    MyClassB* pB= dynamic_cast<MyClassB* >( pA);
        pB->funcA();
    return 0;
}
