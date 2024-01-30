
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "send_and_receive.h"
#include "pollLib.h"
#include "safeUtil.h"

#define NULL_TERMINATOR '\0'

int sendPDU(int clientSocket, uint8_t *dataBuffer, int lengthOfData)
{
    //header buff (NEEDS TO BE 2 BYTES)
    uint16_t header_buffer = htons((uint16_t)  lengthOfData);

    //allocating memory for the data and the header 
    uint8_t *message = malloc(lengthOfData + __SIZEOF_SHORT__);

    //putting in header
    memcpy(message, &header_buffer, sizeof(header_buffer));
    //putting actual data into the message array
    memcpy(message + __SIZEOF_SHORT__, dataBuffer, lengthOfData);

    int num_bytes_to_send = safeSend(clientSocket, message, lengthOfData + __SIZEOF_SHORT__, 0);

    return num_bytes_to_send;
}

int recvPDU(int socketNumber, uint8_t *dataBuffer, int bufferSize)
{
    //payload length
    uint16_t payload_len = 0;

    //getting expected bytes
    int bytes_from_header = safeRecv(socketNumber, (uint8_t*) &payload_len, __SIZEOF_SHORT__, MSG_WAITALL);

    //if bytes < 0 then client is dead, need to remove from polling set
    if (bytes_from_header <= 0) 
    {
        removeFromPollSet(socketNumber);
        close(socketNumber);

        return 0;
    }
    //otherwise return 0
    else if (bufferSize < bytes_from_header)
    {
        return 0;
    }

    payload_len = ntohs(payload_len); //sneaky idea from Jr - no need for an extra variable

    //recieving pdu from client
    int actual_bytes_received = safeRecv(socketNumber, dataBuffer, payload_len, MSG_WAITALL);

    //if not sent properly, value is <= 0 and client should be removed
    if (actual_bytes_received <= 0) 
    {
        removeFromPollSet(socketNumber);
        close(socketNumber);

        return 0;
    }

    return actual_bytes_received;
}