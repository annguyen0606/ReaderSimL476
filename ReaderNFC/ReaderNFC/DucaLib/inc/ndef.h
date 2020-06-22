/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NDEF_H
#define __NDEF_H

#include "nfcm1833_module.h"

#define MAX_NDEF_SIZE 247
#define MAX_TAG_SIZE  250
#define MAX_NDEF_RECORD 40
#define BLOCK_SIZE 		4U


typedef struct _NDEF_format
{
    union
    {
      uint8_t header;
      struct _ndef_header
      {
        uint8_t TNF:3;
        uint8_t IL:1;
        uint8_t SR:1;
        uint8_t CF:1;
        uint8_t ME:1;
        uint8_t MB:1;
      };
    };
    uint8_t type_len;
    union
    {
      struct _short_data_len
      {
      uint8_t data_short_len;
      uint8_t data_short[MAX_NDEF_SIZE];
      };
      struct _long_data_len
      {
        uint8_t data_long_len[4];
        uint8_t data_long[MAX_NDEF_SIZE -3];
      };
    };
} NDEF_t;

typedef struct _NDEF_TEXT_format
{
    uint8_t header;
    uint8_t type_len;
    uint8_t data_len;
    uint8_t record_type;
    uint8_t status_byte;
    uint8_t locale[2];
    uint8_t data[MAX_NDEF_SIZE - 4];
} NDEF_TEXT_t;

typedef enum _uri_type
{
    NA = 0x00,
    HTTP_WWW,
    HTTPS_WWW,
    HTTP,
    HTTPS,
    TEL,
    MAILTO,
    FTP_ANONYMOUS,
    FTP_FTP_DOT,
    FTPS,
    SFTP,
    SMB,
    NFS,
    FTP,
    DAV,
    NEWS,
    TELNET,
    IMAP,
    RTSP,
    URN,
    POP,
    SIP,
    SIPS,
    TFTP,
    BTSPP,
    BTL2CAP,
    BTGOEP,
    TCPOBEX,
    IDRAOBEX,
    FILE_
} uri_type;

typedef struct _NDEF_URI_format
{
    uint8_t header;
    uint8_t type_len;
    uint8_t data_len;
    uint8_t record_type;
    uri_type uri_type;
    uint8_t data[MAX_NDEF_SIZE - 2];
} NDEF_URI_t;

typedef struct _tag_data_format
{
    uint8_t nlen;
    uint8_t length;
    uint8_t data_raw[MAX_TAG_SIZE];
    uint8_t norecord;
    NDEF_t* record[MAX_NDEF_RECORD];
} tag_format_t;

typedef struct _version_access_field
{
    uint8_t major_version       : 2;
    uint8_t minorr_version      : 2;
    uint8_t read_access         : 2;
    uint8_t write_access        : 2;
} ver_acc_t;

typedef struct _CC_file
{
    uint8_t magic_number;
    union
    {
        uint8_t version_access;
        ver_acc_t version_access_info;
    };
    uint8_t MLEN_DIV8;
    uint8_t command_support;
} cc_file_data_t;

typedef enum _status_t
{
  STT_OK,
  STT_ERR
} status_t;

send_recv_status_t read_single_block (uint8_t block, uint8_t data[BLOCK_SIZE]);

send_recv_status_t read_cc_file_data (cc_file_data_t* cc_file_data);

send_recv_status_t read_all_tag_data (tag_format_t* tag_data);

send_recv_status_t write_singleblock (uint8_t block, uint8_t data[BLOCK_SIZE]);

send_recv_status_t write_all_tag_data (tag_format_t* tag_data);

send_recv_status_t clear_tag_fast (void);

send_recv_status_t clear_tag_full (void);

send_recv_status_t read_first_ndef (NDEF_t* ndef_data);

send_recv_status_t read_CustomerID1 (uint8_t data[BLOCK_SIZE * 2]);
send_recv_status_t write_CustomerID1 (uint8_t data[BLOCK_SIZE * 2]);

send_recv_status_t read_CustomerID2 (uint8_t data[BLOCK_SIZE * 2]);
send_recv_status_t write_CustomerID2 (uint8_t data[BLOCK_SIZE * 2]);

status_t add_ndef_to_tag_data (tag_format_t* tag_data, NDEF_t* ndef_data);
status_t convert_ndef_to_tag_data(tag_format_t* tag_data, NDEF_t* ndef_data);

#endif /* __NDEF_H */