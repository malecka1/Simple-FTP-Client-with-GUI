/** @File cgui.h
 *  Header for the CGUI class.
 *  @Author Kamil Maleček <malecka1@fit.cvut.cz> */

#ifndef CGUI_H
#define	CGUI_H

#ifndef CABSTRACTDIRECTORY_H
#include "cabstractDirectory.h"
#endif /* CABSTRACTDIRECTORY_H */
#ifndef CDIRECTORYFOLDER_H
#include "cdirectoryFolder.h"
#endif /* CDIRECTORYFOLDER_H */
#ifndef CDIRECTORYFILE_H
#include "cdirectoryFile.h"
#endif /* CDIRECTORYFILE_H */
#include <string>
#include <cstring>
#include <unistd.h>
#include <ncurses.h>
#include <menu.h>
#include <form.h>
#include <vector>
using namespace std;

/// Shows GUI and provides control of a program
/** This class uses ncurses and shows everything in GUI. Client looks like one-panel
 *  commander and has also standard controls. FTP commands uses F(n) keys.
 *  Class stores pointers to mainWin, pop-up and window-for-forms WINDOWS, MENU, ITEM,
 *  strings with connection informations, current path, currently selected
 *  file or folder name and vector with pointers to abstractDirectory instances
 *  which GUI shows in the MENU. */
class CGUI {
public:
    /// Destructor
    ~CGUI();
    /// Initialize curses
    /** Setting up colors, screen, keyboard etc. */
    void startCurses();
    /// Shows informations about current connection
    /** @param utfState Show server's UTF8 support (but ncurses cannot use UTF8)
     *  @param user Username of loged on user
     *  @param server Name of FTP server */
    void setInfo(const string& utfState, const string& user, const string& server);
    /// Updates current path
    /** @param path String with current path that is listed */
    void setPathInfo(const string& path);
    /// Initialize main window
    void initMainWin();
    /// Initialize pop-up window
    void initPopWin();
    /// Shows informations about connection and shortcuts with labels
    void initOtherComp() const;
    /// Ends curses mode
    void endCurses() const;
    /// Sets up form and gets server name
    /** @return string with server name */
    string setServerName();
    /// Sets up two forms and gets login and password
    /** @return pair<string,string> username and password */
    pair <string, string> setLogin();
    /// Asks user to fill a field
    /** @param call Is sentence that says what is expected from user
     *  @param showExitB True if we want to enable Exit button
     *  @return string With string that user have written */
    string askForString(const string& call, bool showExitB);
    /// Shows confirm windows, options Y/N
    /** @param label String with question
     *  @return true if user pressed Y -> confirmation */
    bool really(const string & label);
    /// Gets current position filename
    /** @return string Name of file or folder */
    string getCurrentPositionFilename() const;
    /// Refreshes screen
    void refreshScreen();
    /// Shows some information to user
    /** Method has two modes: Errors (red labels) and information (green labels)
     *  @param text Sentence that is displayed
     *  @param isError = false for information mode, true for error mode */
    void showInWindow(const string& text, bool isError = false);
    /// Makes and shows content of a current directory
    /** Creates fields for menu, according to actual window size and show them.
     *  @param a Pointers to abstractDirectory instances (files or folders) */
    void setDirectory(const vector<CAbstractDirectory*>& a);
    /// According to user's input calls FTP command
    /** Catches many buttons, important also current position.
     *  @return par<int, string> Number of command; Second parameter - for example:
     *  new name, path */
    pair <int, string> run();

private:
    WINDOW * m_myWin; // don't delete in destructor, ncurses still runs after exit
    WINDOW * m_popUpWin;
    WINDOW * m_innerWin;
    ITEM ** m_items = NULL;
    MENU * m_menu;
    string m_pathInfo;
    string m_servUserInfo;
    string m_currPosFileName;
    vector <char *> m_output;

};
#endif /* CGUI_H */
