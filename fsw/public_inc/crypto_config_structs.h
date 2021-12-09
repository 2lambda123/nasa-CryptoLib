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
#ifndef _crypto_config_structs_h_
#define _crypto_config_structs_h_

#include "crypto_config.h"

#ifdef NOS3 //NOS3/cFS build is ready
#include "common_types.h"
#else //Assume build outside of NOS3/cFS infrastructure
#include "common_types_minimum.h"
#endif

//main config enums
typedef enum { SADB_TYPE_INMEMORY, SADB_TYPE_MARIADB } SadbType;
//gvcid managed parameter enums
typedef enum { TC_NO_FECF, TC_HAS_FECF } TcFecfPresent;
typedef enum { TC_NO_SEGMENT_HDRS, TC_HAS_SEGMENT_HDRS } TcSegmentHdrsPresent;
typedef enum { CRYPTO_TC_CREATE_FECF_FALSE, CRYPTO_TC_CREATE_FECF_TRUE } TcCreateFecfBool;
typedef enum { TC_PROCESS_SDLS_PDUS_FALSE, TC_PROCESS_SDLS_PDUS_TRUE } TcProcessSdlsPdus;
typedef enum { TC_NO_PUS_HDR, TC_HAS_PUS_HDR } TcPusHdrPresent;
typedef enum { TC_IGNORE_SA_STATE_FALSE, TC_IGNORE_SA_STATE_TRUE } TcIgnoreSaState;
typedef enum { TC_IGNORE_ANTI_REPLAY_FALSE, TC_IGNORE_ANTI_REPLAY_TRUE } TcIgnoreAntiReplay;
typedef enum { TC_UNIQUE_SA_PER_MAP_ID_FALSE, TC_UNIQUE_SA_PER_MAP_ID_TRUE } TcUniqueSaPerMapId;

/*
** Main Crypto Configuration Block
*/
typedef struct
{
    SadbType sadb_type;
    TcCreateFecfBool crypto_create_fecf; //Whether or not CryptoLib is expected to calculate TC FECFs and return payloads with the FECF
    TcProcessSdlsPdus process_sdls_pdus; //Config to process SDLS extended procedure PDUs in CryptoLib
    TcPusHdrPresent has_pus_hdr;
    TcIgnoreSaState ignore_sa_state; //TODO - add logic that uses this configuration
    TcIgnoreAntiReplay ignore_anti_replay; //TODO - add logic that uses this configuration
    TcUniqueSaPerMapId unique_sa_per_mapid;
    uint8 vcid_bitmask;
} CryptoConfig_t;
#define CRYPTO_CONFIG_SIZE (sizeof(CryptoConfig_t))

typedef struct _GvcidManagedParameters_t GvcidManagedParameters_t;
struct _GvcidManagedParameters_t{
    uint8   tfvn     :4;  // Transfer Frame Version Number
    uint16  scid    :10; //SpacecraftID
    uint8   vcid    :6; //Virtual Channel ID
    TcFecfPresent has_fecf;
    TcSegmentHdrsPresent has_segmentation_hdr;
    GvcidManagedParameters_t* next; //Will be a list of managed parameters!
};
#define GVCID_MANAGED_PARAMETERS_SIZE (sizeof(GvcidManagedParameters_t))

/*
** SaDB MariaDB Configuration Block
*/
typedef struct
{
    char*   mysql_username;
    char*   mysql_password;
    char*   mysql_hostname;
    char*   mysql_database;
    uint16  mysql_port;
} SadbMariaDBConfig_t;
#define SADB_MARIADB_CONFIG_SIZE (sizeof(SadbMariaDBConfig_t))

#endif