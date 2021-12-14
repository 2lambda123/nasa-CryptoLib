/* Copyright (C) 2009 - 2022 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

   This software is provided "as is" without any warranty of any kind, either expressed, implied, or statutory, including, but not
   limited to, any warranty that the software will conform to specifications, any implied warranties of merchantability, fitness
   for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
   any warranty that the software will be error free.

   In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
   arising out of, resulting from, or in any way connected with the software or its documentation, whether or not based upon warranty,
   contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
   documentation or services provided hereunder.

   ITC Team
   NASA IV&V
   jstar-development-team@mail.nasa.gov
*/

#ifndef _crypto_h_
#define _crypto_h_

/*
** Crypto Includes
*/

#ifdef NOS3 //NOS3/cFS build is ready
#include "cfe.h"
#else //Assume build outside of NOS3/cFS infrastructure
#include "cfe_minimum.h"
#endif

#include "crypto_structs.h"
#include "crypto_config_structs.h"

#define CRYPTO_LIB_MAJOR_VERSION    1
#define CRYPTO_LIB_MINOR_VERSION    2
#define CRYPTO_LIB_REVISION         0
#define CRYPTO_LIB_MISSION_REV      0

/*
** Prototypes
*/

// Crypto Library Configuration functions
extern int32 Crypto_Config_CryptoLib(uint8 sadb_type, uint8 crypto_create_fecf, uint8 process_sdls_pdus, uint8 has_pus_hdr, uint8 ignore_sa_state, uint8 ignore_anti_replay, uint8 unique_sa_per_mapid, uint8 vcid_bitmask);
extern int32 Crypto_Config_MariaDB(char* mysql_username, char* mysql_password, char* mysql_hostname, char* mysql_database, uint16 mysql_port);
extern int32 Crypto_Config_Add_Gvcid_Managed_Parameter(uint8 tfvn, uint16 scid, uint8 vcid, uint8 has_fecf, uint8 has_segmentation_hdr);

// Initialization
extern int32 Crypto_Init(void); // Initialize CryptoLib After Configuration Calls
extern int32 Crypto_Init_With_Configs(CryptoConfig_t* crypto_config_p,GvcidManagedParameters_t* gvcid_managed_parameters_p,SadbMariaDBConfig_t* sadb_mariadb_config_p); // Initialize CryptoLib With Application Defined Configuration
extern int32 Crypto_Init_Unit_Test(void); // Initialize CryptoLib with unit test default Configurations

// Cleanup
extern int32 Crypto_Shutdown(void); // Free all allocated memory

// Telecommand (TC)
extern int32 Crypto_TC_ApplySecurity(const uint8* p_in_frame, const uint16 in_frame_length, \
                                      uint8 **pp_enc_frame, uint16 *p_enc_frame_len);
extern int32 Crypto_TC_ProcessSecurity(char* ingest, int*  len_ingest, TC_t* tc_sdls_processed_frame);
// Telemetry (TM)
extern int32 Crypto_TM_ApplySecurity(char* ingest, int* len_ingest);
extern int32 Crypto_TM_ProcessSecurity(char* ingest, int* len_ingest);
// Advanced Orbiting Systems (AOS)
extern int32 Crypto_AOS_ApplySecurity(char* ingest, int* len_ingest);
extern int32 Crypto_AOS_ProcessSecurity(char* ingest, int* len_ingest);
// Security Functions
extern int32 Crypto_ApplySecurity(char* ingest, int* len_ingest);
extern int32 Crypto_ProcessSecurity(char* ingest, int* len_ingest);

// Data stores used in multiple components
extern CCSDS_t sdls_frame;
extern TM_t tm_frame;
extern crypto_key_t ek_ring[NUM_KEYS];
// Assisting functions used in multiple components
extern uint8 Crypto_Prep_Reply(char* ingest, uint8 appID);
extern int32 Crypto_increment(uint8 *num, int length);

//Global configuration structs
extern CryptoConfig_t* crypto_config;
extern SadbMariaDBConfig_t* sadb_mariadb_config;
extern GvcidManagedParameters_t* gvcid_managed_parameters;
extern GvcidManagedParameters_t* current_managed_parameters;

#endif