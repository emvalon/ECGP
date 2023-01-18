#include "ECGP_session.h"
#include "ECGP_presentation.h"

static SLIST_HEAD(, ECGP_socket) g_session_sockets_hdr;

/**
 * @brief This is called to initialize sessioin layer.
 * 
 */
void ECGP_sessionInit(void)
{
    SLIST_INIT(&g_session_sockets_hdr);
}

/**
 * @brief This is called to initialze socket.
 * 
 * @param socket Pointer of new socket
 * @return ECGP_error 
 *  -ECGP_EPARAM,   socket has been initialized
 */
ECGP_error ECGP_sessionSocketInit(struct ECGP_socket *socket)
{
    struct ECGP_socket *tempSocket;

    /* Check if this socket has been inserted into list */
    SLIST_FOREACH(tempSocket, &g_session_sockets_hdr, entry) {
        if(tempSocket == socket) {
            return -ECGP_EPARAM;
        }
    }

    SLIST_INSERT_HEAD(&g_session_sockets_hdr, socket, entry);

    return  ECGP_ENONE;
}

void ECGP_sessionAccept(struct ECGP_socket *socket, u8 device, u8 port)
{

}

void ECGP_sessionConnect(struct ECGP_socket *socket, u8 device, u8 port)
{

}

u32 ECGP_sessionWrite(struct ECGP_socket *socket, u8 *data, u32 len)
{

}

u32 ECGP_sessionRead(struct ECGP_socket *socket, u8 *buffer, u32 len)
{

}