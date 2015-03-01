/// Implementation of the CDirectoryFile class.

#ifndef CDIRECTORYFILE_H
#include "cdirectoryFile.h"
#endif /* CDIRECTORYFILE_H */

string CDirectoryFile::getSize() const {
    return m_size;
}

bool CDirectoryFile::isDir() const {
    return false;
}

CAbstractDirectory* CDirectoryFile::Clone() const {
    CAbstractDirectory* x = new CDirectoryFile(m_name, m_size, m_lastMod, m_perm);
    return x; // pointer to the new abstractDirectory instance
}
