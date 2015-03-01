/// Implementation of the CDirectoryFolder class.

#ifndef CDIRECTORYFOLDER_H
#include "cdirectoryFolder.h"
#endif	/* CDIRECTORYFOLDER_H */

string CDirectoryFolder::getSize() const {
    return "<DIR>";
}

bool CDirectoryFolder::isDir() const {
    return true;
}

CAbstractDirectory* CDirectoryFolder::Clone() const {
    CAbstractDirectory* x = new CDirectoryFolder(m_name, m_lastMod, m_perm);
    return x; // pointer to the new abstractDirectory instance
}
