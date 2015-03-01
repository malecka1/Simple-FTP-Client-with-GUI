/// Implementation of the CGUI class.

#ifndef CGUI_H
#include "cgui.h"
#endif /* CGUI_H */

CGUI::~CGUI() {
    for (int i = 0; i < (signed)m_output.size(); i++) {
        delete [] m_output[i];
    }
}

void CGUI::startCurses() {
    initscr(); // initialize screen
    cbreak(); // enable bash shortcuts
    noecho();
    curs_set(0); // disable cursor
    intrflush(stdscr, false);
    start_color(); // set up color mode
    init_pair(1, COLOR_RED, COLOR_WHITE);
    init_pair(2, COLOR_GREEN, COLOR_WHITE);
    init_pair(3, COLOR_WHITE, COLOR_BLUE);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    init_pair(5, COLOR_CYAN, COLOR_BLUE);
    init_pair(6, COLOR_MAGENTA, COLOR_WHITE);
    bkgd(' ' | COLOR_PAIR(3)); // set background color
    initMainWin();
}

void CGUI::setInfo(const string& utfState, const string& user, const string& server) {
    this->m_servUserInfo = "FTP: ";
    this->m_servUserInfo += server;
    this->m_servUserInfo += " (";
    this->m_servUserInfo += utfState;
    this->m_servUserInfo += "), user: ";
    this->m_servUserInfo += user;
    refreshScreen();
}

void CGUI::setPathInfo(const string& path) {
    this->m_pathInfo = "Current directory: \"";
    this->m_pathInfo += path;
    if (path[path.size() - 1] != '/') {
        this->m_pathInfo += "/";
    }
    this->m_pathInfo += "\"";
}

void CGUI::initMainWin() {
    refresh(); // must be here and first, relative dimensions
    m_myWin = newwin(LINES * 8 / 10, COLS * 30 / 31, (LINES - (LINES * 6 / 7)) / 2, (COLS - (COLS * 30 / 31)) / 2);
    box(m_myWin, 0, 0); // 0, 0 gives default characters for the vertical and horizontal line
    wbkgd(m_myWin, ' ' | COLOR_PAIR(3));
    wrefresh(m_myWin); // Show that box
}

void CGUI::initPopWin() {
    m_popUpWin = newwin(LINES / 4, COLS / 2, LINES / 4, COLS / 4); // relative dimensions
    wbkgd(m_popUpWin, ' ' | COLOR_PAIR(4));
    wattron(m_popUpWin, A_STANDOUT); // set up font highlight mode
}

void CGUI::initOtherComp() const {
    // connection informations
    attron(A_BOLD);
    move((LINES * 1 / 20) - 1, 0 + (COLS * 1 / 31));
    printw(m_servUserInfo.c_str());
    move((LINES * 1 / 20), 0 + (COLS * 1 / 31));
    printw(m_pathInfo.c_str());
    // bottom menu
    attron(A_UNDERLINE | COLOR_PAIR(5));
    move((LINES * 9 / 10), 0 + (COLS * 1 / 31) + 2);
    printw("F2");
    move((LINES * 9 / 10), 0 + (COLS * 5 / 31) + 2);
    printw("F3");
    move((LINES * 9 / 10), 0 + (COLS * 9 / 31) + 3);
    printw("F4");
    move((LINES * 9 / 10), 0 + (COLS * 13 / 31) + 2);
    printw("F5");
    move((LINES * 9 / 10), 0 + (COLS * 17 / 31) + 2);
    printw("F7");
    move((LINES * 9 / 10), 0 + (COLS * 21 / 31) + 2);
    printw("F8");
    move((LINES * 9 / 10), 0 + (COLS * 25 / 31) + 1);
    printw("F10");
    move((LINES * 9 / 10), 0 + (COLS * 29 / 31));
    printw("F12");
    attroff(A_UNDERLINE | COLOR_PAIR(5));
    move((LINES * 9 / 10) + 1, 0 + (COLS * 1 / 31));
    printw("Rename");
    move((LINES * 9 / 10) + 1, 0 + (COLS * 5 / 31));
    printw("Upload");
    move((LINES * 9 / 10) + 1, 0 + (COLS * 9 / 31));
    printw("Download");
    move((LINES * 9 / 10) + 1, 0 + (COLS * 13 / 31));
    printw("Refresh");
    move((LINES * 9 / 10) + 1, 0 + (COLS * 17 / 31));
    printw("MakeDir");
    move((LINES * 9 / 10) + 1, 0 + (COLS * 21 / 31));
    printw("Delete");
    move((LINES * 9 / 10) + 1, 0 + (COLS * 25 / 31));
    printw("Logout");
    move((LINES * 9 / 10) + 1, 0 + (COLS * 29 / 31));
    printw("Exit");
    attroff(A_BOLD);
}

void CGUI::endCurses() const {
    unpost_menu(m_menu);
    free_menu(m_menu);
    // because ITEM ** -> so free all memory
    for (int i = 0; i < (signed)m_output.size(); i++) {
        free_item(m_items[i]);
    }
    free(m_items);
    endwin(); // ends curses mode
}

string CGUI::setServerName() {
    clear(); // deletes all components, just main window will be there
    initMainWin();
    string out = askForString("Enter the FTP server address:", true);
    return out;
}

pair<string, string> CGUI::setLogin() {
    pair<string, string> out;
    out.first = askForString("Enter username:", true);
    if (out.first == "-1") {
        return out;
    }
    out.second = askForString("Enter password:", true);
    return out;
}

string CGUI::askForString(const string& call, bool showExitB) {
    initPopWin(); // initialize windows where form will be shown
    wattroff(m_popUpWin, A_STANDOUT);
    wmove(m_popUpWin, (LINES / 8) + 3, (COLS / 4) - 4);
    wattron(m_popUpWin, A_UNDERLINE | A_BOLD);
    wprintw(m_popUpWin, "F12");
    wattroff(m_popUpWin, A_UNDERLINE | A_BOLD);
    if (showExitB) { // show exit or cancel button option
        wprintw(m_popUpWin, " Exit");
        wattron(m_popUpWin, A_STANDOUT);
    } else {
        wprintw(m_popUpWin, " Cancel");
        wattron(m_popUpWin, A_STANDOUT);
    }
    wmove(m_popUpWin, (LINES / 8) - 1, (COLS / 4) - (call.length() / 2)); // centered label
    wprintw(m_popUpWin, call.c_str());
    FIELD * field[2];
    field[0] = new_field(1, 50, 0, 0, 0, 0); // field dimensions and position
    field[1] = NULL;
    set_field_back(field[0], A_UNDERLINE); // underline
    field_opts_off(field[0], O_STATIC);
    set_max_field(field[0], 100); // max length is 100 chars, vertically scroll able
    FORM * form = new_form(field); // field is part of form
    m_innerWin = newwin(1, 50, (LINES / 4) + (LINES / 8) + 1, (COLS / 4) + (COLS / 4) - 25);
    set_form_sub(form, m_innerWin); // set form into new win
    post_form(form);
    curs_set(1); // show cursor in field
    wrefresh(m_popUpWin);
    wrefresh(m_innerWin);
    keypad(m_innerWin, true); // catch keyboard

    int ch, size = 0;
    string out;
    char bufferA [101];
    char * buffer = bufferA;
    while ((ch = wgetch(m_innerWin))) {
        switch (ch) {
            case 10: // enter
                form_driver(form, REQ_VALIDATION);
                buffer = field_buffer(field[0], 0);
                out.clear();
                for (int i = 0; i < size; i++) { // saving field content
                    out += buffer[i];
                }
                out = out.substr(0, out.find_last_not_of(" ") + 1); // cut off spaces at the end
                ch = -1;
                break;
            case KEY_F(12): // exit
                if (showExitB) {
                    out = "-1";
                } else {
                    out = "..";
                }
                ch = -1;
                break;
            case 127: // backspace for Linux Mint
                form_driver(form, REQ_DEL_PREV);
                if (size > 0) {
                    size--;
                }
                break;
            case KEY_BACKSPACE: // backspace for PA2 Ubuntu image
                form_driver(form, REQ_DEL_PREV);
                if (size > 0) {
                    size--;
                }
                break;
            case KEY_HOME:
                form_driver(form, REQ_BEG_LINE);
                break;
            case KEY_END:
                form_driver(form, REQ_END_LINE);
                break;
            case KEY_RIGHT:
                form_driver(form, REQ_NEXT_CHAR);
                break;
            case KEY_LEFT:
                form_driver(form, REQ_PREV_CHAR);
                break;
            case KEY_DC:
                form_driver(form, REQ_DEL_CHAR);
                break;
            default:
                form_driver(form, ch);
                if (size == 100) {
                    size -= 100;
                }
                size++;
        }
        wrefresh(m_popUpWin);
        if (ch == -1) {
            break;
        }
    }
    curs_set(0); // disable cursor
    unpost_form(form); // unset form
    free_form(form);
    free_field(field[0]); // delete field
    return out;
}

bool CGUI::really(const string& label) {
    initPopWin();
    wattron(m_popUpWin, COLOR_PAIR(6));
    wmove(m_popUpWin, (LINES / 8), (COLS / 4) - (label.size() / 2));
    wprintw(m_popUpWin, label.c_str());
    wattroff(m_popUpWin, COLOR_PAIR(6));
    wrefresh(m_popUpWin);
    keypad(m_popUpWin, true); // m_popupwin catches now -> just Y/N
    keypad(m_innerWin, false);
    int ch = 0;
    while (ch != 121) { // Y
        ch = wgetch(m_popUpWin);
        switch (ch) {
            case 110: // N -> no
                keypad(m_popUpWin, false);
                keypad(m_innerWin, true);
                return false;
        }
    }
    keypad(m_popUpWin, false);
    keypad(m_innerWin, true); // sets catching back
    return true;
}

string CGUI::getCurrentPositionFilename() const {
    return m_currPosFileName;
}

void CGUI::refreshScreen() {
    clear();
    initOtherComp();
    initMainWin();
}

void CGUI::showInWindow(const string& text, bool isError) {
    initPopWin();
    if (!isError) { // green highlight
        wattron(m_popUpWin, COLOR_PAIR(2));
        wmove(m_popUpWin, (LINES / 8), (COLS / 4) - (text.length() / 2));
        wprintw(m_popUpWin, text.c_str());
        wattroff(m_popUpWin, COLOR_PAIR(2));
        wrefresh(m_popUpWin);
    } else { // error with red highlight
        wattron(m_popUpWin, COLOR_PAIR(1));
        wmove(m_popUpWin, (LINES / 8) - 1, (COLS / 4) - 2);
        wprintw(m_popUpWin, "ERROR");
        wmove(m_popUpWin, (LINES / 8) + 1, (COLS / 4) - (text.length() / 2));
        wprintw(m_popUpWin, text.c_str());
        wattroff(m_popUpWin, COLOR_PAIR(1));
        wrefresh(m_popUpWin);
        usleep(1500000);
    }
}

void CGUI::setDirectory(const vector<CAbstractDirectory*>& a) {
    if (m_items != NULL) {
        unpost_menu(m_menu);
        free_menu(m_menu);
        for (int i = 0; i < (signed)m_output.size(); i++) {
            free_item(m_items[i]);
            delete [] m_output[i];
        }
        free(m_items);
    }
    m_output.clear();
    // storing menu items
    int len = COLS * 30 / 31;
    string tmpItem;
    char * tmpCh;
    tmpItem.clear();
    tmpItem.append(".."); // creates .. directory field in the list
    for (int j = tmpItem.size(); j < len - 35; j++) {
        tmpItem.append(" ");
    }
    tmpItem.append("<DIR>");
    for (int j = tmpItem.size(); j < len; j++) {
        tmpItem.append(" ");
    }
    tmpCh = new char[tmpItem.size() + 1];
    strcpy(tmpCh, tmpItem.c_str());
    m_output.push_back(tmpCh);
    for (int i = 0; i < (signed)a.size(); i++) { // creates other fields
        tmpItem.clear();
        tmpItem.append(a[i]->getName());
        for (int j = tmpItem.size(); j < len - 38; j++) {
            tmpItem.append(" ");
        }
        tmpItem = tmpItem.substr(0, len - 38);
        for (int j = 8; j > (signed)a[i]->getSize().size(); j--) {
            tmpItem.append(" ");
        }
        tmpItem.append(a[i]->getSize());
        tmpItem.append("  ");
        tmpItem.append(a[i]->getLastMod());
        tmpItem.append("  ");
        tmpItem.append(a[i]->getPermissions());
        tmpCh = new char[tmpItem.size() + 1]; // from string to char -> field
        strcpy(tmpCh, tmpItem.c_str());
        m_output.push_back(tmpCh);
    }
    tmpCh = NULL; // there must be last field NULL
    m_output.push_back(tmpCh);
    // setting up menu
    m_items = (ITEM **) calloc(m_output.size(), sizeof (ITEM *)); // allocs ITEMS
    for (int i = 0; i < (signed)m_output.size(); i++) {
        m_items[i] = new_item(m_output[i], m_output[i]);
    }
    m_innerWin = newwin((LINES * 8 / 10) - 2, (COLS * 30 / 31) - 4, ((LINES - (LINES * 6 / 7)) / 2) + 1, ((COLS - (COLS * 30 / 31)) / 2) + 2);
    wbkgd(m_innerWin, ' ' | COLOR_PAIR(3)); // setting up new window with menu
    m_menu = new_menu((ITEM **) m_items);
    set_menu_win(m_menu, m_innerWin);
    set_menu_fore(m_menu, COLOR_PAIR(4));
    set_menu_back(m_menu, COLOR_PAIR(3));
    set_menu_mark(m_menu, "");
    set_menu_format(m_menu, (LINES * 8 / 10) - 2, 1);
    menu_opts_on(m_menu, O_ONEVALUE);
    post_menu(m_menu);
    // setting current filename
    const char * p = item_name((const ITEM *) current_item(m_menu));
    string ps = p;
    size_t pos = ps.find_first_of(" ");
    ps = ps.substr(0, pos);
    m_currPosFileName = ps;
    keypad(m_innerWin, true);
    wrefresh(m_myWin);
    wrefresh(m_innerWin);
}

pair <int, string> CGUI::run() {
    int ch;
    string ps;
    size_t pos;
    pair <int, string> out;
    while (1) {
        ch = wgetch(m_innerWin);
        switch (ch) {
            case 10: /* user pressed up enter key - change(parent)dir*/
            {
                const char * p;
                p = item_name((const ITEM *) current_item(m_menu));
                ps = p;
                if (ps.find("<DIR>") != string::npos) {
                    if (m_currPosFileName == ".." || m_currPosFileName == ".") {
                        out.first = 1;
                    } else {
                        out.first = 0;
                    }
                    return out;
                }
            }
                break;
            case 127: // backspace (for linux mint) - parent dir
                out.first = 1;
                return out;
            case KEY_BACKSPACE: // backspace for ubuntu
                out.first = 1;
                return out;
            case KEY_UP: /* user pressed up arrow key */
            {
                menu_driver(m_menu, REQ_UP_ITEM); // select +1 item
                const char * p;
                p = item_name((const ITEM *) current_item(m_menu)); // get new currFileName
                ps = p;
                pos = ps.find_first_of(" "); // select just begin -> just name
                ps = ps.substr(0, pos);
                m_currPosFileName = ps;
            }
                break;
            case KEY_DOWN: /* user pressed down arrow key */
            {
                menu_driver(m_menu, REQ_DOWN_ITEM); // select -1 item
                const char * p;
                p = item_name((const ITEM *) current_item(m_menu)); // get new currFileName
                ps = p;
                pos = ps.find_first_of(" ");
                ps = ps.substr(0, pos);
                m_currPosFileName = ps;
            }
                break;
            case KEY_PPAGE: /* user pressed down page up key */
            {
                menu_driver(m_menu, REQ_SCR_UPAGE);
                const char * p;
                p = item_name((const ITEM *) current_item(m_menu)); // get new currFileName
                ps = p;
                pos = ps.find_first_of(" ");
                ps = ps.substr(0, pos);
                m_currPosFileName = ps;
            }
                break;
            case KEY_NPAGE: /* user pressed down page down key */
            {
                menu_driver(m_menu, REQ_SCR_DPAGE);
                const char * p;
                p = item_name((const ITEM *) current_item(m_menu)); // get new currFileName
                ps = p;
                pos = ps.find_first_of(" ");
                ps = ps.substr(0, pos);
                m_currPosFileName = ps;
            }
                break;
            case KEY_HOME: /* user pressed down home key */
            {
                menu_driver(m_menu, REQ_FIRST_ITEM);
                const char * p;
                p = item_name((const ITEM *) current_item(m_menu)); // get new currFileName
                ps = p;
                pos = ps.find_first_of(" ");
                ps = ps.substr(0, pos);
                m_currPosFileName = ps;
            }
                break;
            case KEY_END: /* user pressed down end key */
            {
                menu_driver(m_menu, REQ_LAST_ITEM);
                const char * p;
                p = item_name((const ITEM *) current_item(m_menu)); // get new currFileName
                ps = p;
                pos = ps.find_first_of(" ");
                ps = ps.substr(0, pos);
                m_currPosFileName = ps;
            }
                break;
            case KEY_F(2): /* user pressed key F2 rename -> 2 */
                if (m_currPosFileName == ".." || m_currPosFileName == ".") { // cannot rename parent folder
                    break;
                }
                out.second = askForString("Enter the new name:", false);
                if (out.second == "..") {
                    out.first = 5;
                    return out;
                }
                out.first = 2;
                return out;
            case KEY_F(3): /* user pressed key F3 upload file -> 3 */
                out.second = askForString("Enter the path to the file you want to upload:", false);
                if (out.second == "..") {
                    out.first = 5;
                    return out;
                }
                out.first = 3;
                return out;
            case KEY_F(4): /* user pressed key F4 download -> 4 */
                if (m_currPosFileName == ".." || m_currPosFileName == ".") { // cannot download parent folder
                    break;
                }
                out.second = askForString("Enter the path where you want to save the file:", false);
                if (out.second == "..") {
                    out.first = 5;
                    return out;
                }
                out.first = 4;
                return out;
            case KEY_F(5): /* user pressed key F5 refresh -> 5 */
                out.first = 5;
                return out;
            case KEY_F(7): /* user pressed key F7 mkdir -> 7 */
                out.second = askForString("Enter the folder name:", false);
                if (out.second == "..") {
                    out.first = 5;
                    return out;
                }
                out.first = 7;
                return out;
            case KEY_F(8): /* user pressed key F8 delete -> 8 */
                if (m_currPosFileName == ".." || m_currPosFileName == ".") { // cannot delete parent folder
                    break;
                }
                if (!really("Are you sure? Y/N")) { // if NO just refresh
                    out.first = 5;
                    return out;
                }
                out.first = 8;
                return out;
            case KEY_F(10): /* user pressed key F10 -> 10 (logout) */
                out.first = 10;
                return out;
            case KEY_F(12): /* user pressed key F12 -> 12 (exit) */
                if (!really("Do you want to quit? Y/N")) {
                    out.first = 5;
                    return out;
                }
                out.first = 12;
                return out;
            case KEY_RESIZE: /* user resized terminal = like refresh */
                refreshScreen();
                showInWindow("Please make refresh (F5) to show files.", false);
                wrefresh(m_myWin);
                wrefresh(m_innerWin);
                break;
        }
    }
    out.first = -1;
    return out;
}
