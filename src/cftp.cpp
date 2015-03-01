/// Implementation of the CFTP class.

#ifndef CFTP_H
#include "cftp.h"
#endif /* CFTP_H */

CFTP::~CFTP() {
    for (int i = 0; i < (signed)m_directory.size(); i++) {
        delete m_directory[i];
    }
}

bool CFTP::login(const string& user, const string& pass) {
    int fd = openCliSocket(m_ftpAddr, m_port); // creating main socket
    if (fd == -1) {
        return false;
    }
    this->m_user = user;
    this->m_fd = fd;
    flushBufferWait();
    read(m_fd, m_buffer, sizeof (m_buffer) - 1); // reads from m_fd into buffer up to sizeof(buffer)
    string command = m_buffer;
    flushBufferWait();
    if (command.find("220") == string::npos || command.find("421") != string::npos) {
        size_t ss = command.find_first_of("\n", 4);
        command = command.substr(4, ss - 4);
        m_gui->showInWindow(command, true); // errors will be shown this way
        return false;
    }
    command = "USER "; // USER command
    command += user;
    command += "\r\n";
    snprintf(m_buffer, sizeof (m_buffer), "%s", command.c_str()); // print command to buffer
    write(m_fd, m_buffer, strlen(m_buffer)); // send buffer to socket
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    string command2 = m_buffer;
    flushBufferWait();
    if (command2.find("530") != string::npos) {
        size_t ss = command2.find_first_of("\n", 4);
        command2 = command2.substr(4, ss - 4);
        m_gui->showInWindow(command2, true);
        return false;
    }
    command = "PASS "; // PASS command
    command += pass;
    command += "\r\n";
    snprintf(m_buffer, sizeof (m_buffer), "%s", command.c_str());
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    command = m_buffer;
    flushBufferWait();
    if (command.find("530") != string::npos || command.find("421") != string::npos) {
        size_t ss = command.find_first_of("\n", 4);
        command = command.substr(4, ss - 4);
        m_gui->showInWindow(command, true);
        return false;
    }
    // set utf8 on if it's possible
    snprintf(m_buffer, sizeof (m_buffer), "%s", "OPTS UTF8 ON\r\n");
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    command = m_buffer;
    flushBufferWait();
    if (command.find("200") != string::npos || command.find("421") != string::npos) {
        m_utf8Status = "UTF8 enabled";
    } else {
        m_utf8Status = "UTF8 disabled";
    }
    // set binary transfer mode
    snprintf(m_buffer, sizeof (m_buffer), "%s", "TYPE I\r\n");
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    command2 = m_buffer;
    flushBufferWait();
    if (command2.find("200") == string::npos) {
        size_t ss = command2.find_first_of("\n", 4);
        command2 = command2.substr(4, ss - 4);
        m_gui->showInWindow(command2, true);
        return false;
    }
    m_gui->setInfo(m_utf8Status, m_user, m_ftpAddr); // show connection data on the screen
    // set initial path for listing
    setPWD();
    return true;
}

void CFTP::setPWD() {
    snprintf(m_buffer, sizeof (m_buffer), "%s", "PWD\r\n");
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    string command = m_buffer;
    flushBufferWait();
    if (command.find("257") == string::npos || command.find("421") != string::npos) {
        size_t ss = command.find_first_of("\n", 4);
        command = command.substr(4, ss - 4);
        m_gui->showInWindow(command, true);
        return;
    }
    size_t pos1 = command.find_first_of("\"");
    size_t pos2 = command.find_first_of("\"", ++pos1);
    m_pwd = command.substr(pos1, pos2 - pos1); // gets path from " "
    m_gui->setPathInfo(m_pwd); // show it in the gui
    return;
}

void CFTP::changeDir(const string& name) {
    string command = "CWD ";
    command += name;
    command += "\r\n";
    snprintf(m_buffer, sizeof (m_buffer), "%s", command.c_str());
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    string command2 = m_buffer;
    flushBufferWait();
    if (command2.find("550") != string::npos) {
        size_t ss = command2.find_first_of("\n", 4);
        command2 = command2.substr(4, ss - 4);
        m_gui->showInWindow(command2, true);
        return;
    }
    // it's important to delete items of previous directory
    for (int i = 0; i < (signed)m_directory.size(); i++) {
        delete m_directory[i];
    }
    m_directory.clear();
    setPWD();
    return;
}

void CFTP::changeDirParent() {
    snprintf(m_buffer, sizeof (m_buffer), "%s", "CDUP\r\n");
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    string command = m_buffer;
    flushBufferWait();
    if (command.find("550") != string::npos || command.find("421") != string::npos) {
        size_t ss = command.find_first_of("\n", 4);
        command = command.substr(4, ss - 4);
        m_gui->showInWindow(command, true);
        return;
    }
    for (int i = 0; i < (signed)m_directory.size(); i++) {
        delete m_directory[i];
    }
    m_directory.clear();
    setPWD();
    return;
}

bool CFTP::refreshList() {
    for (int i = 0; i < (signed)m_directory.size(); i++) {
        delete m_directory[i];
    }
    m_directory.clear();
    flushBufferWait();
    if (!listDirectory()) {
        m_gui->showInWindow("No connection.", true);
        return false;
    }
    return true;
}

pair <string, int> CFTP::pasv() {
    snprintf(m_buffer, sizeof (m_buffer), "%s", "PASV\r\n");
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    string command = m_buffer;
    flushBufferWait();
    if (command.find("227") == string::npos || command.find("421") != string::npos) {
        size_t ss = command.find_first_of("\n", 4);
        command = command.substr(4, ss - 4);
        m_gui->showInWindow(command, true);
        return make_pair("-1", -1);
    }
    size_t pos1 = command.find_first_of("(");
    size_t pos2 = command.find_first_of(")", ++pos1);
    string tmp = command.substr(pos1, pos2 - pos1);
    pos1 = tmp.find_first_of("0123456789", 0);
    pos2 = tmp.find_first_not_of("0123456789", pos1);
    string ip = tmp.substr(pos1, pos2 - pos1) + "."; // parsing IP and port
    pos1 = tmp.find_first_of("0123456789", ++pos2);
    pos2 = tmp.find_first_not_of("0123456789", pos1);
    ip += tmp.substr(pos1, pos2 - pos1) + ".";
    pos1 = tmp.find_first_of("0123456789", ++pos2);
    pos2 = tmp.find_first_not_of("0123456789", pos1);
    ip += tmp.substr(pos1, pos2 - pos1) + ".";
    pos1 = tmp.find_first_of("0123456789", ++pos2);
    pos2 = tmp.find_first_not_of("0123456789", pos1);
    ip += tmp.substr(pos1, pos2 - pos1);
    pos1 = tmp.find_first_of("0123456789", ++pos2);
    pos2 = tmp.find_first_not_of("0123456789", pos1);
    int port = stoi(tmp.substr(pos1, pos2 - pos1))*256;
    pos1 = tmp.find_first_of("0123456789", ++pos2);
    pos2 = tmp.find_first_not_of("0123456789", pos1);
    port += stoi(tmp.substr(pos1, pos2 - pos1));
    return make_pair(ip, port);
}

bool CFTP::listDirectory(bool print) {
    pair <string, int> tmp = pasv();
    if (tmp.first == "-1") {
        return false;
    }
    // opens new socket where will server send data
    int fd = openCliSocket(tmp.first, tmp.second);
    if (fd == -1) {
        return false;
    }
    snprintf(m_buffer, sizeof (m_buffer), "%s", "LIST\r\n");
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    string command2 = m_buffer;
    flushBufferWait();
    if (command2.find("150") == string::npos) { // just another error
        size_t ss = command2.find_first_of("\n", 4);
        command2 = command2.substr(4, ss - 4);
        m_gui->showInWindow(command2, true);
        close(fd);
        return false;
    }
    size_t pos;
    string line, command;
    while (read(fd, m_buffer, sizeof (m_buffer) - 1)) {
        command.append(m_buffer);
        command.erase(remove(command.begin(), command.end(), '\u0001'), command.end()); // removes SOH(0x01) chars
        pos = command.find_first_of("\n");
        while (pos != string::npos) {
            line = command.substr(0, ++pos);
            if (line[0] == 'd') { // if the first char in permissions is 'd', then it's folder
                string name = parseLine(line, 0);
                string date = parseLine(line, 2);
                string perm = parseLine(line, 3);
                addEntry(CDirectoryFolder(name, date, perm)); // adds folder into vector
            } else {
                string name = parseLine(line, 0);
                string size = parseLine(line, 1);
                string date = parseLine(line, 2);
                string perm = parseLine(line, 3);
                addEntry(CDirectoryFile(name, size, date, perm)); // adds file into vector
            }
            command.erase(0, pos);
            pos = command.find_first_of("\n");
        }
        flushBufferWait();
    }
    close(fd);
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    command2 = m_buffer;
    flushBufferWait();
    if (command2.find("226") == string::npos) {
        size_t ss = command2.find_first_of("\n", 4);
        command2 = command2.substr(4, ss - 4);
        m_gui->showInWindow(command2, true);
        return false;
    }
    if (print) {
        m_gui->setDirectory(m_directory); // prints current directory
    }
    return true;
}

void CFTP::addEntry(const CAbstractDirectory& a) {
    m_directory.push_back(a.Clone()); // clone return pointer to abstractdirectory
}

void CFTP::flushBufferWait() {
    memset(m_buffer, 0, sizeof (m_buffer)); // clears buffer with 0
    usleep(m_delay * 1000);
}

string CFTP::parseLine(const string& line, const int column) const {
    size_t pos1, pos2;
    string out;
    if (column == 0) { // filename
        pos1 = line.find_last_of(" ");
        out = line.substr(++pos1);
        out.erase(remove(out.begin(), out.end(), '\n'), out.end());
        out.erase(remove(out.begin(), out.end(), '\r'), out.end());
    } else if (column == 2) { // date
        pos1 = 0;
        for (int i = 0; i < 5; i++) {
            pos1 = line.find_first_of(" \n\r\t", ++pos1);
            pos1 = line.find_first_not_of(" \n\r\t", ++pos1);
        }
        pos2 = pos1 + 1;
        for (int i = 0; i < 3; i++) {
            pos2 = line.find_first_not_of(" \n\r\t", ++pos2);
            pos2 = line.find_first_of(" \n\r\t", ++pos2);
        }
        out = line.substr(pos1, pos2 - pos1);
    } else if (column == 1) { // size
        pos1 = 0;
        for (int i = 0; i < 4; i++) {
            pos1 = line.find_first_of(" \n\r\t", pos1);
            pos1 = line.find_first_not_of(" \n\r\t", ++pos1);
        }
        pos2 = line.find_first_of(" \n\r\t", pos1 + 1);
        out = line.substr(pos1, pos2 - pos1);
        unsigned long size = stoul(out);
        unsigned int i;
        for (i = 0; size >= pow(1000, i); i++) {
            if (i == 5) {
                break;
            }
        }
        if (i <= 1) { // selects right unit
            out += " B";
        } else if (i == 2) {
            out = to_string(size / 1000.0);
            pos1 = out.find_first_of(".", 0);
            out = out.substr(0, pos1 + 2);
            out += " KB";
        } else if (i == 3) {
            out = to_string(size / 1000000.0);
            pos1 = out.find_first_of(".", 0);
            out = out.substr(0, pos1 + 2);
            out += " MB";
        } else if (i == 4) {
            out = to_string(size / 1000000000.0);
            pos1 = out.find_first_of(".", 0);
            out = out.substr(0, pos1 + 2);
            out += " GB";
        } else {
            out = to_string(size / 1000000000.0);
            pos1 = out.find_first_of(".", 0);
            out = out.substr(0, pos1 + 2);
            out += " TB";
        }
    } else if (column == 3) { // permissions
        pos1 = line.find_first_of(" ");
        out = line.substr(0, pos1);
    }
    return out;
}

bool CFTP::createDir(const string& name, bool outputs) {
    if (outputs) {
        m_gui->showInWindow("Creating directory...");
    }
    string command = "MKD ";
    command += name;
    command += "\r\n";
    snprintf(m_buffer, sizeof (m_buffer), "%s", command.c_str());
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    command = m_buffer;
    flushBufferWait();
    if (command.find("550") != string::npos || command.find("421") != string::npos) {
        size_t ss = command.find_first_of("\n", 4);
        command = command.substr(4, ss - 4);
        m_gui->showInWindow(command, true);
        return false;
    }
    if (outputs) {
        command = "Folder ";
        command += name;
        command += " was created.";
        m_gui->showInWindow(command); // show confirm window
        usleep(600000);
    }
    return true;
}

bool CFTP::rmDir(const string& name) {
    string command = "RMD ";
    command += name;
    command += "\r\n";
    snprintf(m_buffer, sizeof (m_buffer), "%s", command.c_str());
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    command = m_buffer;
    flushBufferWait();
    if (command.find("550") != string::npos || command.find("421") != string::npos) {
        if (command.find("Permission denied") != string::npos) {
            size_t ss = command.find_first_of("\n", 4);
            command = command.substr(4, ss - 4);
            m_gui->showInWindow(command, true);
            return false;
        }
        changeDir(name); // go to non-empty folder
        listDirectory(false); // because of recursion we don't want to see every step of deleting on the screen
        for (int i = 0; i < (signed)m_directory.size(); i++) {
            if (m_directory[i]->isDir()) { // decides which method will call
                rmDir(m_directory[i]->getName());
            } else {
                rmFile(m_directory[i]->getName());
            }
        }
        changeDirParent(); // because of recursion we are going back
        rmDir(name); // delete empty folder
    }
    return true;
}

bool CFTP::rmFile(const string& name) {
    string command = "DELE ";
    command += name;
    command += "\r\n";
    snprintf(m_buffer, sizeof (m_buffer), "%s", command.c_str());
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    command = m_buffer;
    flushBufferWait();
    if (command.find("550") != string::npos || command.find("421") != string::npos) {
        size_t ss = command.find_first_of("\n", 4);
        command = command.substr(4, ss - 4);
        m_gui->showInWindow(command, true);
        return false;
    }
    return true;
}

void CFTP::removeMy(const string& name) {
    m_gui->showInWindow("Deleting..."); // deleting can be slow -> some waiting banner
    string command = name;
    for (int i = 0; i < (signed)m_directory.size(); i++) {
        if (m_directory[i]->getName() == command) {
            if (m_directory[i]->isDir()) {
                if (!rmDir(command)) {
                    return;
                }
            } else {
                if (!rmFile(command)) {
                    return;
                }
            }
        }
    }
    command = name;
    command += " was deleted.";
    m_gui->showInWindow(command);
    usleep(600000);
    return;
}

void CFTP::rename(const string& old, const string& name) {
    m_gui->showInWindow("Renaming...");
    string command = name;
    for (int i = 0; i < (signed)m_directory.size(); i++) {
        if (m_directory[i]->getName() == name) {
            command = name;
            command += ": filename already exist."; // filename exists
            m_gui->showInWindow(command, true);
            return;
        }
    }
    command = "RNFR "; // command says rename old
    command += old;
    command += "\r\n";
    snprintf(m_buffer, sizeof (m_buffer), "%s", command.c_str());
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    command = m_buffer;
    flushBufferWait();
    if (command.find("350") == string::npos || command.find("421") != string::npos) {
        command = name;
        command += ": cannot rename.";
        m_gui->showInWindow(command, true);
        return;
    }
    command = "RNTO "; // to name
    command += name;
    command += "\r\n";
    snprintf(m_buffer, sizeof (m_buffer), "%s", command.c_str());
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    command = m_buffer;
    flushBufferWait();
    if (command.find("550") != string::npos || command.find("421") != string::npos) {
        size_t ss = command.find_first_of("\n", 4);
        command = command.substr(4, ss - 4);
        m_gui->showInWindow(command, true);
        return;
    }
    command = old;
    command += " was renamed to ";
    command += name;
    command += ".";
    m_gui->showInWindow(command); // user is still informed
    usleep(600000);
    return;
}

bool CFTP::download(const string& name, const string& locPath) {
    string locDir = locPath;
    if (locDir[locDir.size() - 1] != '/') {
        locDir += "/"; // path needs / at the end
    }
    locDir += name;
    for (int i = 0; i < (signed)m_directory.size(); i++) {
        if (m_directory[i]->getName() == name) {
            if (m_directory[i]->isDir()) {
                if (mkdir(locDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {// 0777
                    m_gui->showInWindow("Cannot create directory.", true);
                    return false;
                }
                changeDir(m_directory[i]->getName());
                listDirectory(false); // we don't want to see steps of a recursion
                for (int j = 0; j < (signed)m_directory.size(); j++) { // at first create all directories
                    if (m_directory[j]->isDir()) {
                        if (m_directory[j]->getName() == ".." || m_directory[j]->getName() == ".") { // don't download .. or . directory
                            continue;
                        }
                        download(m_directory[j]->getName(), locDir);
                    }
                }
                for (int j = 0; j < (signed)m_directory.size(); j++) { // then just download all files inside
                    if (!m_directory[j]->isDir()) {
                        download(m_directory[j]->getName(), locDir);
                    }
                }
                changeDirParent(); // go back = level up
                listDirectory(false);
                return true;
            } else {
                ofstream file(locDir.c_str(), ios::out | ios::binary); // created file
                if (file.is_open() && !file.fail() && !file.eof()) {
                    pair <string, int> tmp = pasv();
                    if (tmp.first == "-1") {
                        file.close();
                        m_gui->showInWindow("Connection error.", true);
                        return false;
                    }
                    int fd = openCliSocket(tmp.first, tmp.second);
                    if (fd == -1) {
                        file.close();
                        return false;
                    }
                    string command = "RETR ";
                    command += name;
                    command += "\r\n";
                    snprintf(m_buffer, sizeof (m_buffer), "%s", command.c_str());
                    write(m_fd, m_buffer, strlen(m_buffer));
                    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
                    command = m_buffer;
                    flushBufferWait();
                    if (command.find("150") == string::npos || command.find("421") != string::npos) {
                        file.close();
                        close(fd);
                        size_t ss = command.find_first_of("\n", 4);
                        command = command.substr(4, ss - 4);
                        m_gui->showInWindow(command, true);
                        return false;
                    }
                    int tmpSize;
                    while ((tmpSize = read(fd, m_buffer, sizeof (m_buffer) - 1))) {
                        file.write(m_buffer, tmpSize);
                        flushBufferWait();
                    }
                    file.close();
                    close(fd); // closes socket
                    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
                    command = m_buffer;
                    flushBufferWait();
                    if (command.find("226") == string::npos || command.find("421") != string::npos) {
                        size_t ss = command.find_first_of("\n", 4);
                        command = command.substr(4, ss - 4);
                        m_gui->showInWindow(command, true);
                        return false;
                    }
                } else {
                    file.close();
                    m_gui->showInWindow("Invalid path.", true);
                    return false;
                }
            }
        }
    }
    return true;
}

bool CFTP::upload(const string& path) {
    pair <string, int> tmp = pasv();
    if (tmp.first == "-1") {
        return false;
    }
    if (!chdir(path.c_str())) { // check if path leads to a folder (it's possible to enter it)
        string folder;
        string newpath = path;
        size_t f1, f2;
        if (newpath[newpath.size() - 1] == '/') { // last char is /
            f1 = newpath.find_last_of("/", newpath.size());
            f1 = newpath.find_last_of("/", --f1);
            f2 = newpath.size();
        } else {
            f1 = newpath.find_last_of("/");
            newpath += "/";
            f2 = newpath.size();
        }
        if (f1 == string::npos || f2 == string::npos) {
            m_gui->showInWindow("Wrong path.", true);
            return false;
        }
        f1++;
        folder = newpath.substr(f1, f2 - f1);
        if (!createDir(folder)) { // creates new folder
            return false;
        }
        listDirectory(false);
        changeDir(folder);
        listDirectory(false);
        lsAndUp(newpath);
        changeDirParent();
        listDirectory(false);
        return true;
    }
    string name = path;
    size_t pos1 = name.find_last_of("/");
    name = name.substr(++pos1);
    int fd = openCliSocket(tmp.first, tmp.second);
    if (fd == -1) {
        return false;
    }
    string command = "STOR ";
    command += name;
    command += "\r\n";
    snprintf(m_buffer, sizeof (m_buffer), "%s", command.c_str());
    write(m_fd, m_buffer, strlen(m_buffer));
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    command = m_buffer;
    flushBufferWait();
    if (command.find("150") == string::npos || command.find("421") != string::npos) {
        size_t ss = command.find_first_of("\n", 4);
        command = command.substr(4, ss - 4);
        m_gui->showInWindow(command, true);
        close(fd);
        return false;
    }
    ifstream file(path, ios::in | ios::binary);
    if (file.is_open() && !file.fail()) {
        while (!file.eof()) {
            file.read(m_buffer, sizeof (m_buffer) - 1);
            write(fd, m_buffer, sizeof (m_buffer[0]) * file.gcount()); // number of read chars in last operation
            flushBufferWait();
        }
        file.close();
    } else {
        file.close();
        m_gui->showInWindow("File doesn't exist.", true);
        close(fd);
        read(m_fd, m_buffer, sizeof (m_buffer) - 1);
        command = m_buffer;
        flushBufferWait();
        rmFile(name.c_str());
        return false;
    }
    close(fd);
    read(m_fd, m_buffer, sizeof (m_buffer) - 1);
    command = m_buffer;
    flushBufferWait();
    if (command.find("226") == string::npos || command.find("421") != string::npos) {
        size_t ss = command.find_first_of("\n", 4);
        command = command.substr(4, ss - 4);
        m_gui->showInWindow(command, true);
        return false;
    }
    return true;
}

void CFTP::lsAndUp(const string& path) {
    string out;
    DIR * dir;
    struct dirent * dirp; // structure representing directory entry
    dir = opendir("."); // opens current directory
    while ((dirp = readdir(dir)) != NULL) { // gets filenames
        if (string(dirp->d_name) == ".." || string(dirp->d_name) == ".") {
            continue;
        }
        out = path;
        out += dirp->d_name;
        upload(out); // upload file/folder
    }
    closedir(dir);
    return;
}
