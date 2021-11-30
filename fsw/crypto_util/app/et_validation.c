/* Copyright (C) 2009 - 2017 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

   This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
   limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
   for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
   any warranty that the software will be error free.

   In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
   arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
   contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
   documentation or services provided hereunder

   ITC Team
   NASA IV&V
   ivv-itc@lists.nasa.gov
*/

/*
 *  Unit Tests that macke use of TC_ApplySecurity function on the data.
 */

#include "et_validation.h"
#include "utest.h"
#include <python3.8/Python.h>

#include "sadb_routine.h"
#include "crypto_error.h"


// Setup for some Unit Tests using a Python Script to Verify validiy of frames
PyObject *pName, *pModule, *pDict, *pFunc, *pValue, *pArgs, *pClass, *pInstance;
int EndPython()
{
    Py_XDECREF(pInstance);
    Py_XDECREF(pValue);
    Py_XDECREF(pModule);
    Py_XDECREF(pName);
    Py_Finalize();
}

void python_auth_encryption(char* data, char* key, char* iv, char* header, char* bitmask, uint8** expected, long* expected_length)
{
    printf("IN PYTHON\n");
    Py_Initialize();
    PyRun_SimpleString("import sys\nsys.path.append('../../python')");
    printf("\tLine %d\n", __LINE__);
    pName = PyUnicode_FromString("encryption_test");
    pModule = PyImport_Import(pName);
    if(pModule == NULL)
    {
        printf("ERROR, NO MODULE FOUND\n");
        EndPython();
        return;
    }
    printf("\tLine %d\n", __LINE__);
    pDict = PyModule_GetDict(pModule);
    pClass = PyDict_GetItemString(pDict, "Encryption");

    if (PyCallable_Check(pClass))
    {
        pInstance = PyObject_CallObject(pClass, NULL);
    }
    else
    {
        printf("ERROR, NO CLASS INSTANCE FOUND\n");
        EndPython();
        return;
    }
    printf("\tLine %d\n", __LINE__);
    pValue = PyObject_CallMethod(pInstance, "encrypt", "sssss", data, key, iv, header, bitmask);
    printf("\tLine %d\n", __LINE__);
    pValue = PyObject_CallMethod(pInstance, "get_len", NULL);
    printf("\tLine %d\n", __LINE__);
    long temp_length = PyLong_AsLong(pValue);
    printf("\tLine %d\n", __LINE__);
    *expected_length = temp_length;
    printf("\tLine %d\n", __LINE__);
    pValue = PyObject_CallMethod(pInstance, "get_results", NULL);
    printf("\tLine %d\n", __LINE__);
    char* temp_expected = PyBytes_AsString(pValue);
    printf("\tLine %d\n", __LINE__);
    *expected= (uint8*)malloc(sizeof(uint8) * (int)*expected_length);
    printf("\tLine %d\n", __LINE__);
    memcpy(*expected, temp_expected, (int)*expected_length);
    printf("\tLine %d\n", __LINE__);
    return;
}

UTEST(ET_VALIDATION, ENCRYPTION_TEST)
{
    //Setup & Initialize CryptoLib
    Crypto_Init();
    Py_Initialize();

    uint8* expected = NULL;
    long expected_length = 0;
    long buffer_size = 0;
    long buffer2_size = 0;
    long buffer3_size = 0;

    char *buffer = c_read_file("../../fsw/crypto_tests/data/encryption_test_ping.dat", &buffer_size);
    char *buffer2 = c_read_file("../../fsw/crypto_tests/data/activate_sa4.dat", &buffer2_size);

    SecurityAssociation_t* test_association = NULL;
    test_association = malloc(sizeof(SecurityAssociation_t) * sizeof(unsigned char));

    uint16 buffer_size_i = (uint16) buffer_size;
    int buffer2_size_i = (int) buffer2_size;

    uint8 *ptr_enc_frame = NULL;
    uint16 enc_frame_len = 0;
    int32 return_val = -1;
    TC_t *tc_sdls_processed_frame;

    tc_sdls_processed_frame = malloc(sizeof(uint8) * TC_SIZE);
    
    Crypto_TC_ProcessSecurity(buffer2, &buffer2_size_i, tc_sdls_processed_frame);
    
    expose_sadb_get_sa_from_spi(1,&test_association);

    test_association->sa_state = SA_NONE;

    expose_sadb_get_sa_from_spi(4, &test_association);
    test_association->arc_len = 0;
    test_association->gvcid_tc_blk.vcid=1;
    
    return_val = Crypto_TC_ApplySecurity(buffer, buffer_size_i, &ptr_enc_frame, &enc_frame_len);

    python_auth_encryption("1880d2ca0008197f0b0031000039c5", "FEDCBA9876543210FEDCBA9876543210FEDCBA9876543210FEDCBA9876543210", "00000000000000000000000", "2003043400FF0004", "00", &expected, &expected_length);
                            
    printf("\nGot: \n");
    for (int i = 0; i < expected_length; i++)
    {
        printf("0x%02x ", ptr_enc_frame[i]);
    }
    printf("\n");
    printf("\nExpected:\n");
    for (int i = 0; i < expected_length; i++)
    {
        printf("0x%02x ", expected[i]);
    }    
    printf("\n");
    for( int i = 0; i < expected_length; i++)
    {
        printf("EXPECTED: 0x%02x, GOT: 0x%02x\n", expected[i], ptr_enc_frame[i]);
        ASSERT_EQ(expected[i], ptr_enc_frame[i]);
    }
    for( int i = 0; i < expected_length; i++)
    {
        //printf("EXPECTED: 0x%02x, GOT: 0x%02x\n", expected[i], ptr_enc_frame[i]);
        ASSERT_EQ(expected[i], ptr_enc_frame[i]);
    }
    
    free(buffer);
    free(ptr_enc_frame); 
    free(expected);
    free(tc_sdls_processed_frame);
    //free(tc_sdls_processed_frame2);
    EndPython();
}

int convert_hexstring_to_byte_array(char* source_str, uint8* dest_buffer)
{
    char *line = source_str;
    char *data = line;
    int offset;
    int read_byte;
    int data_len = 0;

    while (sscanf(data, " %02x%n", &read_byte, &offset) == 1) 
    {
        dest_buffer[data_len++] = read_byte;
        data += offset;
    }
    return data_len;
}

// AES-GCM 256 Test Vectors
// Reference: https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Algorithm-Validation-Program/documents/mac/gcmtestvectors.zip
UTEST(ET_VALIDATION, VALIDATION_TEST)
{
    int32 status = CRYPTO_LIB_SUCCESS;
    // Setup & Initialize CryptoLib
    Crypto_Init();  

    // NIST supplied vectors
    // NOTE: Added Transfer Frame header to this
    char *buffer_nist_key_h = "ef9f9284cf599eac3b119905a7d18851e7e374cf63aea04358586b0f757670f8";
    char *buffer_nist_pt_h = "2003001100722ee47da4b77424733546c2d400c4e51069";
    char *buffer_nist_iv_h = "b6ac8e4963f49207ffd6374c";
    char *buffer_nist_ct_h = "1224dfefb72a20d49e09256908874979";

    // Setup SAs
    SecurityAssociation_t* test_association = NULL;
    test_association = malloc(sizeof(SecurityAssociation_t) * sizeof(unsigned char));
    // Deactivate SA 1
    expose_sadb_get_sa_from_spi(1,&test_association);
    test_association->sa_state = SA_NONE;
    // Get SA 9
    expose_sadb_get_sa_from_spi(9, &test_association);
    // Set supplied key
    // uint8 *buffer_nist_key_b = malloc((sizeof(buffer_nist_key_h) / 2) * sizeof(uint8));
    // printf('TEST %d\n', *buffer_nist_key_b);
    // int buffer_nist_key_b_length = convert_hexstring_to_byte_array(buffer_nist_key_h, buffer_nist_key_b);
    // memcpy(&ek_ring[test_association->ekid].value[0], buffer_nist_key_b, buffer_nist_key_b_length);
    test_association->arc_len = 0;
    // Activate SA 9
    test_association->sa_state = SA_OPERATIONAL;
    expose_sadb_get_sa_from_spi(9, &test_association);

    // Convert input plaintext
    uint8 *buffer_nist_pt_b = malloc((sizeof(buffer_nist_pt_h) / 2) * sizeof(uint8));
    // TODO: Account for length of header and FECF (5+2)
    int buffer_nist_pt_b_length = convert_hexstring_to_byte_array(buffer_nist_pt_h, buffer_nist_pt_b);

    // Convert/Set input IV
    uint8 *buffer_nist_iv_b = malloc((sizeof(buffer_nist_iv_h) / 2) * sizeof(uint8));
    int buffer_nist_iv_b_length = convert_hexstring_to_byte_array(buffer_nist_iv_h, buffer_nist_iv_b);
    memcpy(&test_association->iv[0], buffer_nist_iv_b, buffer_nist_iv_b_length);

    //Convert input ciphertext
    uint8 *buffer_nist_ct_b = malloc((sizeof(buffer_nist_ct_h) / 2) * sizeof(uint8));
    int buffer_nist_ct_b_length = convert_hexstring_to_byte_array(buffer_nist_ct_h, buffer_nist_ct_b);


    uint8 *ptr_enc_frame = NULL;
    uint16 enc_frame_len = 0;
    int32 return_val = -1;  
    #undef INCREMENT
    return_val = Crypto_TC_ApplySecurity(buffer_nist_pt_b, buffer_nist_pt_b_length, &ptr_enc_frame, &enc_frame_len);
    #define INCREMENT
    // Note: For comparison, interested in the TF payload (exclude headers and FECF if present)
    // Calc payload index: total length - pt length
    uint16 enc_data_idx = enc_frame_len - buffer_nist_ct_b_length - 2;
    #ifdef DEBUG
        printf("\t enc_frame_len: %d,  -2 for fecf\n", enc_frame_len, buffer_nist_pt_b_length );
        printf("\t buffer_nist_pt_b_length: %d\n", buffer_nist_pt_b_length);
        printf("\t buffer_nist_iv_b_length: %d\n", buffer_nist_iv_b_length);
        printf("\t buffer_nist_ct_b_length: %d\n", buffer_nist_ct_b_length);
    #endif
    // Account for our headers and FECF
    for (int i=0; i<buffer_nist_pt_b_length-7; i++)
    {
        if (*(ptr_enc_frame+enc_data_idx) != buffer_nist_ct_b[i])
        {
            status = CRYPTO_LIB_ERR_UT_BYTE_MISMATCH;
        }

        #ifdef DEBUG
            printf("\tidx: %d, EXPECTED: 0x%02x, GOT: 0x%02x\n", enc_data_idx, buffer_nist_ct_b[i], ptr_enc_frame[enc_data_idx] );
        #endif

        enc_data_idx++;
    }
    
    ASSERT_EQ(status, CRYPTO_LIB_SUCCESS);

    free(ptr_enc_frame);
}

UTEST_MAIN();
