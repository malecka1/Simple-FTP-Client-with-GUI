/// Implementation of the CAbstractDirectory class.

#ifndef CABSTRACTDIRECTORY_H
#include "cabstractDirectory.h"
#endif /* CABSTRACTDIRECTORY_H */

CAbstractDirectory::CAbstractDirectory(const string& name, const string& lastMod, const string& perm) {
    this->m_lastMod = lastMod;
    this->m_name = name;
    this->m_perm = perm;
}

string CAbstractDirectory::getLastMod() const {
    return m_lastMod;
}

string CAbstractDirectory::getName() const {
    return m_name;
}

string CAbstractDirectory::getPermissions() const {
    return m_perm;
}
