#ifndef _BYTE_CONST_H_
#define _BYTE_CONST_H_

//------------------------------------------------------------------------
//------------------------------------------------------------------------
#include <sal/types.h>

//------------------------------------------------------------------------
//------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------
//------------------------------------------------------------------------
const sal_Int32 kTestByteCount1 = 7;
const sal_Int32 kTestByteCount2 = 0;
const sal_Int32 kTestByteCount3 = 45;
const sal_Int32 kTestByteCount4 = 100;
const sal_Int32 kTestByteCount5 = 23;
const sal_Int32 kTestByteCount6 = 90;


sal_Int8 kTestByte = 100;
sal_Int8 kTestByte1 = 0;
sal_Int8 kTestByte2 = 1;
sal_Int8 kTestByte3 = 2;
sal_Int8 kTestByte4 = -98;

sal_Int8 kTestByte5[] = {kTestByte, kTestByte1, kTestByte2, kTestByte3, kTestByte4};

sal_Int8 kTestByte60 = 56;
sal_Int8 kTestByte61 = -1;
sal_Int8 kTestByte62 = -23;
sal_Int8 kTestByte63 = 21;
sal_Int8 kTestByte64 = -128;
sal_Int8 kTestByte65 = 127;
sal_Int8 kTestByte6[] = {kTestByte60, kTestByte61, kTestByte62, kTestByte63, kTestByte64, kTestByte65};

//------------------------------------------------------------------------

char kTestChar = 45;
char kTestChar0 = 0;
    char kTestChar1 = (char)((500 & 0xff) - 256); 
char kTestChar2 = 78;
    char kTestChar3 = (char)(-155 & 0xff);

sal_Int32 kTestSeqLen0 = 0;
sal_Int32 kTestSeqLen1 = 5;
sal_Int32 kTestSeqLen2 = 34;
sal_Int32 kTestSeqLen3 = 270;

sal_Sequence kTestEmptyByteSeq =
{
    1,      		/* sal_Int32    refCount;   */
    kTestSeqLen0,      	/* sal_Int32    length;     */
    { kTestChar0 }   	/* sal_Unicode  buffer[1];  */
};

sal_Sequence kTestByteSeq1 =
{
    1,      		/* sal_Int32    refCount;   */
    kTestSeqLen1,      	/* sal_Int32    length;     */
    { kTestChar1 }   	/* sal_Unicode  buffer[1];  */
};

sal_Sequence kTestByteSeq2 =
{
    3,      		/* sal_Int32    refCount;   */
    kTestSeqLen2,      	/* sal_Int32    length;     */
    { kTestChar2 }   	/* sal_Unicode  buffer[1];  */
};

sal_Sequence kTestByteSeq3 =
{
    2,      		/* sal_Int32    refCount;   */
    kTestSeqLen3,      	/* sal_Int32    length;     */
    { kTestChar3 }   	/* sal_Unicode  buffer[1];  */
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

//------------------------------------------------------------------------
//------------------------------------------------------------------------

#endif /* _BYTE_CONST_H_ */


