/** @File cdirectory.h
 *  Header for the CDirectoryFile class.
 *  @Author Kamil Maleček <malecka1@fit.cvut.cz> */

#ifndef CDIRECTORYFILE_H
#define	CDIRECTORYFILE_H

#ifndef CABSTRACTDIRECTORY_H
#include "cabstractDirectory.h"
#endif /* CABSTRACTDIRECTORY_H */
using namespace std;

/// File representation
/** This class represents file of a directory on the remote server. Class
 *  stores size of the file with some unit. */
class CDirectoryFile : public CAbstractDirectory {
public:
    /// Constructor
    CDirectoryFile(const string& name, const string& size, const string& lastModified, const string& perm) : CAbstractDirectory(name, lastModified, perm) {
        this->m_size = size;
    }
    /// Gets size
    /** @return string with size of the file in correct unit */
    string getSize() const;
    /// Check the file is directory
    /** @return false, because file isn't folder */
    bool isDir() const;
    /// Cloner of file
    /** @return pointer to CAbstractdirectory instance */
    CAbstractDirectory* Clone() const;

private:
    string m_size;
};
#endif /* CDIRECTORYFILE_H */
