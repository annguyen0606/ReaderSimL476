#include "ndef.h"
#include "string.h"

send_recv_status_t read_single_block (uint8_t block, uint8_t data[BLOCK_SIZE])
{
    uint8_t command[] = {0x04, 0x03, 0x02, 0x20, 0x00};
    command[4] = block;
    cr95hf_recv_format_t data_result;
    send_recv_status_t status =  sendrecv (command, sizeof (command), &data_result);
    memcpy (data, data_result.data_recv + 1, BLOCK_SIZE);
    return status;
}

send_recv_status_t read_cc_file_data (cc_file_data_t* cc_file_data)
{
    return read_single_block (0, (uint8_t*) cc_file_data);
}

send_recv_status_t read_all_tag_data (tag_format_t* tag_data)
{
    uint8_t data[BLOCK_SIZE];
    uint8_t counter;
    uint8_t retry_time = 0;
    send_recv_status_t status = read_single_block (1, data);
    
    if (status == SEND_RECV_OK)
        {
            tag_data->nlen              = data[0];
            tag_data->length            = data[1];
            tag_data->data_raw[0]       = data[2];
            tag_data->data_raw[1]       = data[3];
            
            if (tag_data->length > 0)
                {
                    for (counter = 0; counter < (tag_data->length + 1) / 4; counter ++)
                        {
                            do
                                {
                                    status = read_single_block (counter + 2, tag_data->data_raw + 2 + counter * 4);
                                    retry_time++;
                                }
                            while ((status != SEND_RECV_OK)&&(retry_time<=10));
                            if(retry_time >= 10) 
                            {
                              return COMM_ERR;
                            }
                            retry_time = 0;
                        }
                        
                    tag_data->record[0] = (NDEF_t*) & (tag_data->data_raw[0]);
                    tag_data->norecord = 1;
                    
                    while ( (tag_data->record[tag_data->norecord - 1]->header & 0x40) == 0x00) //ME is zero -> not ending messenge
                        {
                            tag_data->record[tag_data->norecord] = (NDEF_t*) & (tag_data->record[tag_data->norecord - 1]->data_short[tag_data->record[tag_data->norecord - 1]->data_short_len + tag_data->record[tag_data->norecord - 1]->type_len]);
                            tag_data->norecord ++;
                        }
                }
        }
        
    return status;
}

send_recv_status_t write_singleblock (uint8_t block, uint8_t data[BLOCK_SIZE])
{
    uint8_t write_cmd[] = {0x04, 0x07, 0x02, 0x21, 0x01, 0x03, 0x00, 0xD1, 0x01};
    cr95hf_recv_format_t data_result;
    write_cmd[4] = block;
    write_cmd[5] = data[0];
    write_cmd[6] = data[1];
    write_cmd[7] = data[2];
    write_cmd[8] = data[3];
    send_recv_status_t status = sendrecv (write_cmd, sizeof (write_cmd), &data_result);
    return status;
}

send_recv_status_t write_all_tag_data (tag_format_t* tag_data)
{
    uint8_t data[BLOCK_SIZE];
    uint8_t counter;
    data[0] = tag_data->nlen;
    data[1] = tag_data->length;
    data[2] = tag_data->data_raw[0];
    data[3] = tag_data->data_raw[1];
    tag_data->data_raw[tag_data->length] = 0xFE;
    tag_data->data_raw[tag_data->length + 1] = 0xFF;
    
    if (tag_data->nlen == 0)
        tag_data->nlen = 0x03;
        
    send_recv_status_t status = write_singleblock (1, data);
    
    for (counter = 0; counter < (tag_data->length + 2) / 4; counter ++)
        {
            status |= write_singleblock (counter + 2, tag_data->data_raw + 2 + counter * 4);
            
            if (status != SEND_RECV_OK)
                break;
        }
        
    return status;
}

send_recv_status_t clear_tag_fast (void)
{
    uint8_t data_block1[BLOCK_SIZE] = {0x03, 0x00, 0xD0, 0x00};
    uint8_t data_block2[BLOCK_SIZE] = {0x00, 0xFE, 0x00, 0x00};
    send_recv_status_t status = write_singleblock (1, data_block1);
    
    if (status == SEND_RECV_OK)
        status |= write_singleblock (2, data_block2);
        
    if (status == SEND_RECV_OK)
        status |= write_singleblock (1, data_block1);
        
    return status;
}

send_recv_status_t clear_tag_full (void)
{
    cc_file_data_t cc_data;
    uint8_t counter;
    uint8_t data_block[BLOCK_SIZE] = {0x00, 0x00, 0x00, 0x00};
    read_cc_file_data (&cc_data);
    send_recv_status_t status = clear_tag_fast();
    
    if (status == SEND_RECV_OK)
        {
            for (counter = 2; counter < cc_data.MLEN_DIV8 * 2 ; counter ++)
                {
                    status |= write_singleblock (counter, data_block);
                    
                    if (status != SEND_RECV_OK)
                        break;
                }
        }
        
    return status;
}

status_t add_ndef_to_tag_data (tag_format_t* tag_data, NDEF_t* ndef_data)
{
    uint8_t counter = 0;
    
    if ( (tag_data->length +  ndef_data->data_short_len + ndef_data->type_len + 3) > MAX_TAG_SIZE)
        {
            return STT_ERR;
        }
    else
        {
            tag_data->data_raw[tag_data->length + 0] = ndef_data->header;
            tag_data->data_raw[tag_data->length + 1] = ndef_data->type_len;
            tag_data->data_raw[tag_data->length + 2] = ndef_data->data_short_len;
            
            for (counter = 0; counter < ndef_data->type_len + ndef_data->data_short_len; counter ++)
                {
                    tag_data->data_raw[tag_data->length + 3 + counter] = ndef_data->data_short[counter];
                }
                
            tag_data->record[tag_data->norecord] = (NDEF_t*) & (tag_data->data_raw[tag_data->length]);
            tag_data->length += ndef_data->data_short_len + ndef_data->type_len + 3; //3 byte ndef header,nlen,typelen
            tag_data->norecord++;
            
            if (tag_data->norecord > 1)
                {
                    tag_data->record[tag_data->norecord - 2]->header &= 0xBF; //ME = 0;
                    tag_data->record[tag_data->norecord - 1]->header &= 0x7F;     //MB = 0;
                    tag_data->record[tag_data->norecord - 1]->header |= 0x40;     //ME = 1; // Make sure it's last ndef message
                }
                
            return STT_OK;
        }
}


status_t convert_ndef_to_tag_data (tag_format_t* tag_data, NDEF_t* ndef_data)
{
    uint8_t counter = 0;
    
    if ( (ndef_data->data_short_len + ndef_data->type_len + 3) > MAX_TAG_SIZE)
        {
            return STT_ERR;
        }
    else
        {
            tag_data->length = 0;
            tag_data->norecord = 0;
            tag_data->nlen = 0x03;
            tag_data->data_raw[0] = ndef_data->header;
            tag_data->data_raw[1] = ndef_data->type_len;
            tag_data->data_raw[2] = ndef_data->data_short_len;
            
            for (counter = 0; counter < ndef_data->type_len + ndef_data->data_short_len; counter ++)
                {
                    tag_data->data_raw[3 + counter] = ndef_data->data_short[counter];
                }
                
            tag_data->record[tag_data->norecord] = (NDEF_t*) & (tag_data->data_raw[0]);
            tag_data->length = ndef_data->data_short_len + ndef_data->type_len + 3; //3 byte ndef header,nlen,typelen
            tag_data->norecord++;
            return STT_OK;
        }
}

send_recv_status_t read_CustomerID1 (uint8_t data[BLOCK_SIZE * 2])
{
    send_recv_status_t status = read_single_block (0x3E, data);
    status |= read_single_block (0x3F, data + BLOCK_SIZE);
    return status;
}

send_recv_status_t write_CustomerID1 (uint8_t data[BLOCK_SIZE * 2])
{
    send_recv_status_t status = write_singleblock (0x3E, data);
    status |= write_singleblock (0x3F, data + BLOCK_SIZE);
    return status;
}

send_recv_status_t read_CustomerID2 (uint8_t data[BLOCK_SIZE * 2])
{
    send_recv_status_t status = read_single_block (0x3C, data);
    status |= read_single_block (0x3D, data + BLOCK_SIZE);
    return status;
}

send_recv_status_t write_CustomerID2 (uint8_t data[BLOCK_SIZE * 2])
{
    send_recv_status_t status = write_singleblock (0x3C, data);
    status |= write_singleblock (0x3D, data + BLOCK_SIZE);
    return status;
}