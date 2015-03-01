/** @File cabstractDirectory.h
 *  Header for the CAbstractDirectory class.
 *  @Author Kamil Maleček <malecka1@fit.cvut.cz> */

#ifndef CABSTRACTDIRECTORY_H
#define	CABSTRACTDIRECTORY_H

#include <string>
using namespace std;

/// Represents content of a directory on a FTP server
/** This abstract class represents directory on a FTP server. It stores it's
 *  name, time of last modification and permissions. */
class CAbstractDirectory {
public:
    /// Constructor
    /** @param name Name of an item from directory
     *  @param lastMod Time of last modification of an item
     *  @param perm String with permissions in the classic unix format */
    CAbstractDirectory(const string& name, const string& lastMod, const string& perm);
    /// Destructor
    virtual ~CAbstractDirectory() {
    }
    /// Gets name of the item
    /** @return name of the item from directory */
    virtual string getName() const;
    /// Gets last modification of the item
    /** @return string with time */
    virtual string getLastMod() const;
    /// Gets permissions of the item from directory
    /** @return string with permissions */
    virtual string getPermissions() const;
    /// Gets some string for size column
    /** @return size for files, DIR for folders */
    virtual string getSize() const = 0;
    /// Checks if the item is a directory or isn't
    /** @return true if the item is directory */
    virtual bool isDir() const = 0;
    /// Cloner
    /** @return pointer to deep copy of the item */
    virtual CAbstractDirectory* Clone() const = 0;

protected:
    string m_name; //!< name of file/folder
    string m_lastMod; //!< date of last modification
    string m_perm; //!< permissions
};
#endif	/* CABSTRACTDIRECTORY_H */
