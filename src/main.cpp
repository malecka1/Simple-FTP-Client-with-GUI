/** @mainpage
 *  This project implements FTP client with GUI. Client can do following commands:
 *  browsing directories on a FTP server, renaming files and folders,
 *  recursive uploading from a local directory, recursive downloading from a server,
 *  making new directories and recursive deleting on a server.
 *  
 *  Kamil Maleček, <malecka1@fit.cvut.cz>, May 2014 */

#ifndef CFTP_H
#include "cftp.h"
#endif /* CFTP_H */

int main() {

    bool isEnd = false;
    CGUI * gui = new CGUI;
    CFTP * ftp;
    gui->startCurses();

    while (!isEnd) { // do till the end
        // get server and login
        string serverName = gui->setServerName();
        if (serverName == "-1") { // -1 means exit
            isEnd = true;
            continue;
        }
        pair<string, string> login = gui->setLogin();
        if (serverName == "-1" || login.first == "-1" || login.second == "-1") {
            isEnd = true;
            continue;
        }
        ftp = new CFTP(serverName, gui);
        // requesting for correct data, or exit
        while (!ftp->login(login.first, login.second)) {
            delete ftp;
            serverName = gui->setServerName();
            if (serverName == "-1") {
                isEnd = true;
                break;
            }
            login = gui->setLogin();
            if (login.first == "-1" || login.second == "-1") {
                isEnd = true;
                break;
            }
            ftp = new CFTP(serverName, gui);
        }
        if (isEnd) {
            continue;
        }
        // logged in, "/" directory listed
        gui->showInWindow("Login successful.", false);
        usleep(600000);
        gui->refreshScreen();
        ftp->refreshList();
        pair<int, string> action(-1, "");
        bool success;
        while (action.first != 10 && action.first != 12) {
            // FTP commands
            action = gui->run();
            switch (action.first) {
                case 0: // go to folder
                    ftp->changeDir(gui->getCurrentPositionFilename());
                    gui->refreshScreen();
                    ftp->refreshList();
                    break;
                case 1: // go to parent folder
                    ftp->changeDirParent();
                    gui->refreshScreen();
                    ftp->refreshList();
                    break;
                case 2: // rename
                    ftp->rename(gui->getCurrentPositionFilename(), action.second);
                    gui->refreshScreen();
                    ftp->refreshList();
                    break;
                case 3: // recursive upload
                    gui->showInWindow("Uploading...", false);
                    success = ftp->upload(action.second);
                    gui->refreshScreen();
                    if (success) {
                        gui->showInWindow("Upload complete.");
                        usleep(600000);
                    }
                    ftp->refreshList();
                    break;
                case 4: // recursive download
                    gui->showInWindow("Downloading...", false);
                    success = ftp->download(gui->getCurrentPositionFilename(), action.second);
                    gui->refreshScreen();
                    if (success) {
                        gui->showInWindow("Download complete.", false);
                        usleep(600000);
                    }
                    ftp->refreshList();
                    break;
                case 5: // refresh
                    gui->refreshScreen();
                    gui->showInWindow("Refreshing...", false);
                    ftp->refreshList();
                    break;
                case 7: // make directory
                    ftp->createDir(action.second);
                    gui->refreshScreen();
                    ftp->refreshList();
                    break;
                case 8: // recursive delete
                    ftp->removeMy(gui->getCurrentPositionFilename());
                    gui->refreshScreen();
                    ftp->refreshList();
                    break;
                case 10: // logout
                    break;
                case 12: // logout & quit
                    isEnd = true;
                    break;
            }
        }
        // closing connection
        ftp->closeCliSocket();
        delete ftp;
        gui->showInWindow("Logout successful.", false);
        usleep(600000);
    }
    // quit client
    gui->showInWindow("Bye.", false);
    usleep(400000);
    gui->endCurses();
    delete gui;
    return 0;
}
