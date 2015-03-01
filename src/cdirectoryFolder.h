/** @File cdirectoryFolder.h
 *  Header for the CDirectoryFolder class.
 *  @Author Kamil Maleček <malecka1@fit.cvut.cz> */

#ifndef CDIRECTORYFOLDER_H
#define	CDIRECTORYFOLDER_H

#ifndef CABSTRACTDIRECTORY_H
#include "cabstractDirectory.h"
#endif /* CABSTRACTDIRECTORY_H */
using namespace std;

/// Folder representation
/** This class represents folder of a directory on the remote server. */
class CDirectoryFolder : public CAbstractDirectory {
public:
    /// Constructor
    CDirectoryFolder(const string& name, const string& lastModified, const string& perm) : CAbstractDirectory(name, lastModified, perm) {
    }
    /// Gets size
    /** @return "<DIR>" */
    string getSize() const;
    /// Check if folder is folder
    /** @return true */
    bool isDir() const;
    /// Cloner
    /** @return pointer to CAbstractdirectory instance */
    CAbstractDirectory* Clone() const;
};
#endif	/* CDIRECTORYFOLDER_H */
