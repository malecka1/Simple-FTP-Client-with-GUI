/** @File cftp.h
 *  Header for the CFTP class.
 *  @Author Kamil Maleček <malecka1@fit.cvut.cz> */

#ifndef CFTP_H
#define	CFTP_H

#ifndef CCONNECTION_H
#include "cconnection.h"
#endif	/* CCONNECTION_H */
#ifndef CABSTRACTDIRECTORY_H
#include "cabstractDirectory.h"
#endif /* CABSTRACTDIRECTORY_H */
#ifndef CDIRECTORYFOLDER_H
#include "directoryFolder.h"
#endif /* CDIRECTORYFOLDER_H */
#ifndef CDIRECTORYFILE_H
#include "directoryFile.h"
#endif /* CDIRECTORYFILE_H */
#include <vector>
#include <algorithm>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
using namespace std;

/// Makes all FTP commands
/** This class makes all FTP commands and stores buffer for getting data from
 *  a socket, user login, path of current directory on the server, whether server
 *  supports UTF8 or not and pointers to all items in the current directory. */
class CFTP : public CConnection {
public:
    /// Constructor

    CFTP(const string& name, CGUI * gui) : CConnection(name, gui) {
    }
    /// Destructor
    ~CFTP();
    /// Ensure user's login
    /** @param user Name of the user
     *  @param pass Password of the user
     *  @return bool=true if login was successful */
    bool login(const string& user, const string& pass);
    /// Sets PWD
    /** Get current path from a server and set it here. */
    void setPWD();
    /// Changes dir
    /** @param name Name of the currently chosen item of abstractDirectory */
    void changeDir(const string& name);
    /// Goes to the parent folder
    /** Simply goes up to the parent folder, if the current path is "/" then
     *  just make refresh. */
    void changeDirParent();
    /// Just refreshes items of the current directory
    /** @return true if everything was ok */
    bool refreshList();
    /// Asks server where to wait for data
    /** This method asks server for address and port, where will server send or
     *  receive data. Method also parses server's response and gets IP and port
     *  in a correct format.
     *  @return pair<string, int> string is IP and int is a port, pair will be used
     *  to open new socket. When server didn't send these data, pair will be "-1", -1 */
    pair <string, int> pasv();
    /// Gets list of files and folders of the current directory
    /** In fact, from newly created socket method get list and save these files
     *  into vector.
     *  @param print Is true when method has to print, true if hasn't
     *  @return true if listing was ok, false if there was an error, which will
     *  be shown on the screen */
    bool listDirectory(bool print = true);
    /// Adds item into vector
    /** @param a Push back this pointer to AbstractDirectory vector */
    void addEntry(const CAbstractDirectory& a);
    /// Cleans buffer and waits
    void flushBufferWait();
    /// Parse line
    /** Because of the LIST command standardization, every line has the same
     *  format so this method just get selected column.
     *  @param line Is string to parse
     *  @param column Number of column we want to get
     *  @return cut off string */
    string parseLine(const string& line, const int column) const;
    /// Creates new folder
    /** When creating isn't possible, error message with response will be shown.
     *  @param name Name of the new folder
     *  @param outputs = false means no pop-up windows, it's for recursion
     *  @return true if creating was ok */
    bool createDir(const string& name, bool outputs = false);
    /// Removes folder
    /** @param name Name of the folder from the current directory to delete
     *  @return true if deleting was successful, or false when it wasn't */
    bool rmDir(const string& name);
    /// Removes file
    /** @param name Name of the file to be deleted
     *  @return true if deleting was successful, or false if it wasn't */
    bool rmFile(const string& name);
    /// Calls suitable method for recursive deleting
    /** In case that currently selected item is folder, method will call rmDir,
     *  in other case will call rmFile. Also recursive and showing confirms.
     *  @param name Name of the item to be deleted */
    void removeMy(const string& name);
    /// Renames the item
    /** @param old Currently selected item to be renamed
     *  @param name New name for selected item */
    void rename(const string& old, const string& name);
    /// Recursive download
    /** Because of recursion, we need to download folders. So method creates
     *  folders with permissions 0755 and then downloads all the files into these
     *  folders. Files have permissions 0644.
     *  @param name Name of the item to be downloaded
     *  @param path Is inserted path which will be used for storing files
     *  @return true if everything was ok */
    bool download(const string& name, const string& path);
    /// Recursive upload
    /** Method can upload files and folders, recursively.
     *  @param path Is path on a local drive which leads to the file
     *  @return true if everything was ok */
    bool upload(const string& path);
    /// Reads local directory
    /** Gets all file or folder names located in the current local directory and
     *  initialize their upload.
     *  @param path is a local directory path */
    void lsAndUp(const string& path);

private:
    char m_buffer[200];
    string m_user;
    string m_pwd;
    string m_utf8Status;
    vector<CAbstractDirectory*> m_directory;
};
#endif	/* CFTP_H */
