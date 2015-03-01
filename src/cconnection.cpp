/// Implementation of the CConnection class.

#ifndef CCONNECTION_H
#include "cconnection.h"
#endif	/* CCONNECTION_H */

CConnection::CConnection(const string& name, CGUI * gui) {
    this->m_ftpAddr = name;
    this->m_gui = gui;
}

int CConnection::openCliSocket(const string& name, const int port) {
    struct addrinfo * addrInfo;
    char portStr[10];
    // according to the name it will determine a type of the address (IPv4/6) and binary representation
    snprintf(portStr, sizeof ( portStr), "%d", port);
    if (getaddrinfo(name.c_str(), portStr, NULL, &addrInfo)) {
        m_gui->showInWindow("Incorrect server address.", true);
        return -1;
    }
    // opening socket, socket type (family) according to the return value of getaddrinfo, stream = TCP
    int fd = socket(addrInfo -> ai_family, SOCK_STREAM, 0);
    if (fd == -1) {
        freeaddrinfo(addrInfo);
        m_gui->showInWindow("Error while opening socket.", true);
        return -1;
    }
    // requests for connection with server, in fact here starts the communication
    if (connect(fd, addrInfo -> ai_addr, addrInfo -> ai_addrlen) == -1) {
        close(fd);
        freeaddrinfo(addrInfo);
        m_gui->showInWindow("Connection error.", true);
        return -1;
    }
    freeaddrinfo(addrInfo);
    return fd;
}

void CConnection::closeCliSocket() {
    if (m_fd != 0) {
        close(m_fd); // closes socket
    }
}
