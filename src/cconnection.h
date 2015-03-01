/** @File cconnection.h
 *  Header for the CConnection class.
 *  @Author Kamil Maleček <malecka1@fit.cvut.cz> */

#ifndef CCONNECTION_H
#define	CCONNECTION_H

#ifndef CGUI_H
#include "cgui.h"
#endif /* CGUI_H */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
using namespace std;

/// Setting up and closing down network connection
/** This class opens client's socket and closes it when it is needed. Class stores
 *  pointer to GUI for showing error messages, FTP server address, FTP port number
 *  constant, delay constant and main socket for main communication. */
class CConnection {
public:
    /// Constructor
    /** @param name FTP server address
     *  @param gui GUI pointer for this connection */
    CConnection(const string& name, CGUI * gui);
    /// Opens client's socket
    /** Author: Ing. Ladislav Vagner, Ph.D. <xvagner@fit.cvut.cz>
     *  Code is from extra seminary about networks & C++
     *  April 2014, availability: <Edux.fit.cvut.cz>
     *  @param name Address for the new socket 
     *  @param port Port for the new socket
     *  @return number of created socket */
    int openCliSocket(const string& name, const int port);
    /// Closes main socket
    void closeCliSocket();

protected:
    CGUI * m_gui; //!< pointer to GUI for showing messages
    string m_ftpAddr; //!< server address
    const int m_port = 21; //!< FTP port is 21
    const int m_delay = 2; //!< constant of delay length
    int m_fd; //!< port for main comunication
};
#endif	/* CCONNECTION_H */
