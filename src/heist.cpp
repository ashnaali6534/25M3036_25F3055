// ============================================================
//  heist.cpp  -  THE LOCKER HEIST (CL1004 OOP Semester Project)
//  Implementation file: defines every class declared in heist.h
// ============================================================
#include "heist.h"

// ============================================================
//  OPERATORS FOR STRUCTS
// ============================================================

// Two tiles are equal when they carry the same letter
bool operator==(const Tile& a, const Tile& b) {
    return a.tileLetter == b.tileLetter;
}
// Two clues are equal when letter, position and row are all the same
bool operator==(const Clue& a, const Clue& b) {
    return a.clueLetter == b.clueLetter && a.cluePosition == b.cluePosition && a.clueRow == b.clueRow;
}
// Insertion: prints a tile as [A] or [ ]
ostream& operator<<(ostream& out, const Tile& t) {
    out << "[" << (t.filled ? t.tileLetter : ' ') << "]";
    return out;
}
// Extraction: reads one letter and marks the tile filled
istream& operator>>(istream& in, Tile& t) {
    in >> t.tileLetter;
    t.filled = true;
    return in;
}
ostream& operator<<(ostream& out, const Location& l) {
    out << l.locName << " [" << l.letterClueLoc << " at " << l.boardPosLoc << "]";
    return out;
}
istream& operator>>(istream& in, Location& l) {
    in >> l.locName >> l.letterClueLoc >> l.boardPosLoc;
    return in;
}

// ============================================================
//  GameException
// ============================================================

// Prints the error message in a red color and beeps once
void GameException::show() const {
    ConsoleUI::beep(300, 200);
    ConsoleUI::setColor(TINT_LRED);
    cout << "\n  " << message << endl;
    ConsoleUI::resetColor();
}

// ============================================================
//  ConsoleUI  (graphics + safe input)
// ============================================================

// Big block letters used by the banner (# = block, . = empty)
static const char* FONT_LETTERS = "THELOCKRISWN ";
static const char* FONT_GLYPHS[13][5] = {
    {"#####", "..#..", "..#..", "..#..", "..#.."},   // T
    {"#...#", "#...#", "#####", "#...#", "#...#"},   // H
    {"#####", "#....", "####.", "#....", "#####"},   // E
    {"#....", "#....", "#....", "#....", "#####"},   // L
    {".###.", "#...#", "#...#", "#...#", ".###."},   // O
    {".####", "#....", "#....", "#....", ".####"},   // C
    {"#...#", "#..#.", "###..", "#..#.", "#...#"},   // K
    {"####.", "#...#", "####.", "#..#.", "#...#"},   // R
    {"#####", "..#..", "..#..", "..#..", "#####"},   // I
    {".####", "#....", ".###.", "....#", "####."},   // S
    {"#...#", "#...#", "#.#.#", "##.##", "#...#"},   // W
    {"#...#", "##..#", "#.#.#", "#..##", "#...#"},   // N
    {".....", ".....", ".....", ".....", "....."}    // space
};

// Sets the console to the code page that has box drawing characters
void ConsoleUI::init() {
    SetConsoleOutputCP(437);
}
// Changes the text color of the console
void ConsoleUI::setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
}
// Restores the default white text color
void ConsoleUI::resetColor() {
    setColor(TINT_WHITE);
}
// Clears the whole console window
void ConsoleUI::clearScreen() {
    system("cls");
}
// Waits for the given milliseconds (used by animations)
void ConsoleUI::delay(int milliseconds) {
    Sleep(milliseconds);
}
// Plays a short sound effect
void ConsoleUI::beep(int frequency, int milliseconds) {
    Beep(frequency, milliseconds);
}
// Prints an extended ASCII character (box lines, blocks) several times
void ConsoleUI::printChar(int code, int count) {
    for (int i = 0; i < count; i++) cout << (char)code;
}
// Top border of a double line box
void ConsoleUI::boxTop(int width, int color) {
    setColor(color);
    cout << "  ";
    printChar(201); printChar(205, width); printChar(187);
    cout << endl;
    resetColor();
}
// Middle divider of a double line box
void ConsoleUI::boxDivider(int width, int color) {
    setColor(color);
    cout << "  ";
    printChar(204); printChar(205, width); printChar(185);
    cout << endl;
    resetColor();
}
// Bottom border of a double line box
void ConsoleUI::boxBottom(int width, int color) {
    setColor(color);
    cout << "  ";
    printChar(200); printChar(205, width); printChar(188);
    cout << endl;
    resetColor();
}
// One text line inside a box (centered or left aligned)
void ConsoleUI::boxLine(const string& text, int width, int borderColor, int textColor, bool center) {
    string t = text;
    if ((int)t.length() > width) t = t.substr(0, width);
    int len = (int)t.length();
    int left = center ? (width - len) / 2 : 1;
    if (left + len > width) left = width - len;
    int right = width - len - left;
    setColor(borderColor);
    cout << "  ";
    printChar(186);
    cout << string(left, ' ');
    setColor(textColor);
    cout << t;
    cout << string(right, ' ');
    setColor(borderColor);
    printChar(186);
    cout << endl;
    resetColor();
}
// Prints text one character at a time (typing animation)
void ConsoleUI::typeText(const string& text, int delayMs, int color) {
    setColor(color);
    for (size_t i = 0; i < text.length(); i++) {
        cout << text[i] << flush;
        delay(delayMs);
    }
    resetColor();
}
// Prints a large 5 row banner made of block characters
void ConsoleUI::bigText(const string& text, int color) {
    setColor(color);
    for (int row = 0; row < 5; row++) {
        cout << "    ";
        for (size_t i = 0; i < text.length(); i++) {
            int index = 12;   // space glyph by default
            for (int k = 0; FONT_LETTERS[k] != '\0'; k++)
                if (FONT_LETTERS[k] == text[i]) { index = k; break; }
            const char* g = FONT_GLYPHS[index][row];
            for (int c = 0; c < 5; c++) cout << (g[c] == '#' ? (char)219 : ' ');
            cout << " ";
        }
        cout << endl;
    }
    resetColor();
}
// Draws a bar like [██████░░░░] for credibility / turns / loading
void ConsoleUI::progressBar(int value, int maxValue, int width, int color) {
    if (maxValue < 1) maxValue = 1;
    if (value < 0) value = 0;
    if (value > maxValue) value = maxValue;
    int filled = value * width / maxValue;
    cout << "[";
    setColor(color);
    printChar(219, filled);
    setColor(TINT_GRAY);
    printChar(176, width - filled);
    resetColor();
    cout << "]";
}
// Reads one full line; quits cleanly if the input stream is closed
string ConsoleUI::readLine() {
    string s;
    if (!getline(cin, s)) {
        resetColor();
        exit(0);
    }
    return s;
}
// Reads a number between low and high, otherwise throws
int ConsoleUI::readChoice(const string& prompt, int low, int high) {
    setColor(TINT_GOLD);
    cout << "  " << prompt << ": ";
    resetColor();
    string text = trim(readLine());
    bool ok = false;
    int value = parseInt(text, ok);
    if (!ok) throw InvalidInputException("please enter a number, not '" + text + "'.");
    if (value < low || value > high)
        throw InvalidInputException("choose a number from " + intToStr(low) + " to " + intToStr(high) + ".");
    return value;
}
// Asks a Y/N question, anything else throws
bool ConsoleUI::askYesNo(const string& prompt) {
    setColor(TINT_GOLD);
    cout << prompt << " (Y/N): ";
    resetColor();
    string answer = toUpper(trim(readLine()));
    if (answer == "Y" || answer == "YES") return true;
    if (answer == "N" || answer == "NO") return false;
    throw InvalidInputException("please type Y or N.");
}
// Waits for ENTER so the player can read the screen
void ConsoleUI::pause() {
    setColor(TINT_GRAY);
    cout << "\n  Press ENTER to continue...";
    resetColor();
    readLine();
}
// Removes spaces, tabs and line endings from both sides
string ConsoleUI::trim(const string& s) {
    size_t start = 0, end = s.length();
    while (start < end && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n')) start++;
    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n')) end--;
    return s.substr(start, end - start);
}
// Converts a string to capital letters
string ConsoleUI::toUpper(const string& s) {
    string r = s;
    for (size_t i = 0; i < r.length(); i++)
        if (r[i] >= 'a' && r[i] <= 'z') r[i] = (char)(r[i] - 32);
    return r;
}
// Replaces underscores by spaces for nicer display
string ConsoleUI::prettyName(const string& s) {
    string r = s;
    for (size_t i = 0; i < r.length(); i++)
        if (r[i] == '_') r[i] = ' ';
    return r;
}
// Adds spaces on the right so columns line up
string ConsoleUI::pad(const string& s, int width) {
    if ((int)s.length() >= width) return s;
    return s + string(width - (int)s.length(), ' ');
}
// Converts a number to text (minDigits adds leading zeros, e.g. 05)
string ConsoleUI::intToStr(int value, int minDigits) {
    bool negative = value < 0;
    if (negative) value = -value;
    string digits = "";
    if (value == 0) digits = "0";
    while (value > 0) {
        digits = (char)('0' + value % 10) + digits;
        value /= 10;
    }
    while ((int)digits.length() < minDigits) digits = "0" + digits;
    return negative ? "-" + digits : digits;
}
// Converts text to a number without library functions
int ConsoleUI::parseInt(const string& s, bool& ok) {
    string t = trim(s);
    ok = false;
    if (t.empty() || t.length() > 9) return 0;
    size_t i = 0;
    bool negative = false;
    if (t[0] == '-') { negative = true; i = 1; }
    if (i >= t.length()) return 0;
    int value = 0;
    for (; i < t.length(); i++) {
        if (t[i] < '0' || t[i] > '9') return 0;
        value = value * 10 + (t[i] - '0');
    }
    ok = true;
    return negative ? -value : value;
}
// Splits a line like "a|b|c" into parts[]; returns how many parts exist
int ConsoleUI::splitLine(const string& line, string parts[], int maxParts) {
    string t = trim(line);
    if (!t.empty() && t[0] == '|') t = t.substr(1);
    if (!t.empty() && t[t.length() - 1] == '|') t = t.substr(0, t.length() - 1);
    int count = 0;
    string current = "";
    for (size_t i = 0; i <= t.length(); i++) {
        if (i == t.length() || t[i] == '|') {
            if (count < maxParts) parts[count] = trim(current);
            count++;
            current = "";
        } else {
            current += t[i];
        }
    }
    return count;
}
// Reads the next line that is not empty; false at end of file
bool ConsoleUI::nextDataLine(ifstream& in, string& line) {
    while (getline(in, line)) {
        line = trim(line);
        if (!line.empty()) return true;
    }
    return false;
}

// ============================================================
//  TileRack
// ============================================================

TileRack::TileRack(int capacity) : tiles(), positions(), maxSize(capacity) {}
// Deep copy constructor
TileRack::TileRack(const TileRack& other) : tiles(other.tiles), positions(other.positions), maxSize(other.maxSize) {}
// Assignment operator (deep copy)
TileRack& TileRack::operator=(const TileRack& other) {
    if (this != &other) {
        tiles = other.tiles;
        positions = other.positions;
        maxSize = other.maxSize;
    }
    return *this;
}
TileRack::~TileRack() {}   // DynamicArray frees its own heap memory

// Stores the letter of a clue and its board position
void TileRack::addTile(const Clue& clue) {
    if (isFull()) throw RackFullException("the rack already holds " + ConsoleUI::intToStr(maxSize) + " letters.");
    tiles.add(Tile(clue.clueLetter, true));
    positions.add(clue.cluePosition);
}
// Removes and returns the letter at index
Tile TileRack::removeTile(int index) {
    if (isEmpty()) throw RackEmptyException("there is no letter to remove.");
    Tile t = tiles[index];
    tiles.removeAt(index);
    positions.removeAt(index);
    return t;
}
bool TileRack::isFull() const { return tiles.getSize() >= maxSize; }
bool TileRack::isEmpty() const { return tiles.getSize() == 0; }
int TileRack::getCount() const { return tiles.getSize(); }
void TileRack::clear() { tiles.clear(); positions.clear(); }

// Shows each letter with its board position, like A(3)
void TileRack::display() const {
    if (isEmpty()) {
        ConsoleUI::setColor(TINT_GRAY);
        cout << "(empty)" << endl;
        ConsoleUI::resetColor();
        return;
    }
    for (int i = 0; i < tiles.getSize(); i++) {
        if (tiles[i].tileLetter == ' ') {
            cout << "  /  ";
        } else {
            ConsoleUI::setColor(TINT_LGREEN);
            cout << tiles[i].tileLetter;
            ConsoleUI::setColor(TINT_GRAY);
            cout << "(" << positions[i] << ") ";
        }
    }
    ConsoleUI::resetColor();
    cout << endl;
}
// operator+ merges two racks into one clue with a space between them
TileRack TileRack::operator+(const TileRack& other) const {
    TileRack merged(maxSize + other.maxSize + 1);
    for (int i = 0; i < tiles.getSize(); i++) {
        merged.tiles.add(tiles[i]);
        merged.positions.add(positions[i]);
    }
    merged.tiles.add(Tile(' ', false));
    merged.positions.add(0);
    for (int i = 0; i < other.tiles.getSize(); i++) {
        merged.tiles.add(other.tiles[i]);
        merged.positions.add(other.positions[i]);
    }
    return merged;
}

// ============================================================
//  Cottage
// ============================================================

Cottage::Cottage() : locCache() {}
Cottage::Cottage(const Cottage& other) : locCache(other.locCache) {}   // deep copy
Cottage& Cottage::operator=(const Cottage& other) {
    if (this != &other) locCache = other.locCache;
    return *this;
}
Cottage::~Cottage() {}   // DynamicArray deletes the Location array

// Reads locations.txt and picks the letter column of the culprit profile
void Cottage::loadFromFile(const string& fileName, int culpritIndex) {
    ifstream in(fileName.c_str());
    if (!in) throw FileException(fileName + " was not found.");
    string line;
    if (!ConsoleUI::nextDataLine(in, line)) throw FileException(fileName + " is empty.");   // header
    DynamicArray<Location> temp;
    while (ConsoleUI::nextDataLine(in, line)) {
        string parts[12];
        if (ConsoleUI::splitLine(line, parts, 12) != 9)
            throw FileException(fileName + " is corrupted (9 columns expected).");
        bool ok = false;
        int pos = ConsoleUI::parseInt(parts[1], ok);
        string letterText = parts[2 + culpritIndex];
        if (!ok || pos < 1 || pos > TOTAL_LOCATIONS || letterText.empty())
            throw FileException(fileName + " is corrupted (bad position or letter).");
        Location loc;
        loc.locName = parts[0];
        loc.boardPosLoc = pos;
        loc.letterClueLoc = ConsoleUI::toUpper(letterText)[0];
        loc.locRow = ROW_WORD1;
        loc.locSearchFlag = false;
        temp.add(loc);
    }
    if (temp.getSize() != TOTAL_LOCATIONS)
        throw FileException(fileName + " must contain exactly 7 locations.");
    locCache = temp;
}
int Cottage::getLocationCount() const { return locCache.getSize(); }
bool Cottage::isSearched(int index) const { return locCache[index].locSearchFlag; }
string Cottage::getLocationName(int index) const { return locCache[index].locName; }
// Returns the clue hidden in a location
Clue Cottage::getClue(int index) const {
    return Clue(locCache[index].letterClueLoc, locCache[index].boardPosLoc, locCache[index].locRow);
}
// Marks the location as searched
void Cottage::markSearched(int index) { locCache[index].locSearchFlag = true; }

// Prints the numbered location list with Searched / NOT Searched status
void Cottage::displayMenu() const {
    ConsoleUI::setColor(TINT_LCYAN);
    cout << "\n  ====== Location Menu ======" << endl;
    for (int i = 0; i < locCache.getSize(); i++) {
        ConsoleUI::setColor(TINT_WHITE);
        cout << "  [" << (i + 1) << "] " << ConsoleUI::pad(ConsoleUI::prettyName(locCache[i].locName), 14);
        if (locCache[i].locSearchFlag) {
            ConsoleUI::setColor(TINT_LGREEN);
            cout << " - Searched ";
            ConsoleUI::printChar(251);
        } else {
            ConsoleUI::setColor(TINT_LRED);
            cout << " - NOT Searched";
        }
        cout << endl;
    }
    ConsoleUI::setColor(TINT_GRAY);
    cout << "  [0] Back to Game Menu" << endl;
    ConsoleUI::setColor(TINT_LCYAN);
    cout << "  ==========================" << endl;
    ConsoleUI::resetColor();
}
// Draws a small three floor ASCII map of the cottage
void Cottage::displayMap() const {
    if (locCache.getSize() != TOTAL_LOCATIONS) return;
    // layout by file order: 0,1,2 upstairs | 3 garden | 4,5,6 downstairs
    int floors[3][3] = { {3, -1, -1}, {0, 1, 2}, {4, 5, 6} };
    const char* labels[3] = { "OUTSIDE   ", "UPSTAIRS  ", "DOWNSTAIRS" };
    ConsoleUI::setColor(TINT_GOLD);
    cout << "\n  ------------ COTTAGE MAP ------------" << endl;
    for (int f = 0; f < 3; f++) {
        ConsoleUI::setColor(TINT_LBLUE);
        cout << "  " << labels[f] << ": ";
        for (int k = 0; k < 3; k++) {
            int idx = floors[f][k];
            if (idx < 0) continue;
            bool done = locCache[idx].locSearchFlag;
            ConsoleUI::setColor(done ? TINT_LGREEN : TINT_GRAY);
            cout << "[" << ConsoleUI::prettyName(locCache[idx].locName);
            if (done) { cout << " "; ConsoleUI::printChar(251); }
            cout << "] ";
        }
        cout << endl;
    }
    ConsoleUI::setColor(TINT_GOLD);
    cout << "  -------------------------------------" << endl;
    ConsoleUI::resetColor();
}

// ============================================================
//  Scrabble
// ============================================================

Scrabble::Scrabble() : row1(ROW1_SIZE), row2(ROW2_SIZE), target1(""), target2(""), solved1(false), solved2(false) {
    row1.resize(ROW1_SIZE);
    row2.resize(ROW2_SIZE);
}
Scrabble::Scrabble(const Scrabble& other)
    : row1(other.row1), row2(other.row2), target1(other.target1), target2(other.target2),
      solved1(other.solved1), solved2(other.solved2) {}
Scrabble& Scrabble::operator=(const Scrabble& other) {
    if (this != &other) {
        row1 = other.row1; row2 = other.row2;
        target1 = other.target1; target2 = other.target2;
        solved1 = other.solved1; solved2 = other.solved2;
    }
    return *this;
}
Scrabble::~Scrabble() {}

// Loads the two target words and empties the board
void Scrabble::loadWords(const string& word1, const string& word2) {
    if ((int)word1.length() != ROW1_SIZE || (int)word2.length() != ROW2_SIZE)
        throw FileException("Word_1 must have 7 letters and Word_2 must have 8 letters.");
    target1 = ConsoleUI::toUpper(word1);
    target2 = ConsoleUI::toUpper(word2);
    row1.clear(); row1.resize(ROW1_SIZE);
    row2.clear(); row2.resize(ROW2_SIZE);
    solved1 = false;
    solved2 = false;
}
// Places a letter at its board position (position = index + 1)
void Scrabble::placeLetter(const Clue& clue) {
    DynamicArray<Tile>& row = (clue.clueRow == ROW_WORD1) ? row1 : row2;
    int index = clue.cluePosition - 1;
    if (index < 0 || index >= row.getSize())
        throw InvalidPositionException("position " + ConsoleUI::intToStr(clue.cluePosition) + " does not exist on this row.");
    if (row[index].filled)
        throw SlotFilledException("position " + ConsoleUI::intToStr(clue.cluePosition) + " already has a letter.");
    row[index].tileLetter = clue.clueLetter;
    row[index].filled = true;
}
// True when every slot of the row has a letter
bool Scrabble::isRowComplete(WordRow row) const {
    const DynamicArray<Tile>& r = (row == ROW_WORD1) ? row1 : row2;
    for (int i = 0; i < r.getSize(); i++)
        if (!r[i].filled) return false;
    return true;
}
bool Scrabble::isRowSolved(WordRow row) const { return row == ROW_WORD1 ? solved1 : solved2; }
bool Scrabble::isBothSolved() const { return solved1 && solved2; }

// Compares the typed word with the board letters (uses operator== of Tile)
bool Scrabble::checkWord(WordRow row, const string& typed) {
    if (!isRowComplete(row))
        throw InvalidWordException("the row is not complete yet.");
    const DynamicArray<Tile>& r = (row == ROW_WORD1) ? row1 : row2;
    const string& target = (row == ROW_WORD1) ? target1 : target2;
    if ((int)typed.length() != r.getSize() || typed != target) return false;
    for (int i = 0; i < r.getSize(); i++) {
        Tile t(typed[i], true);
        if (!(t == r[i])) return false;
    }
    if (row == ROW_WORD1) solved1 = true; else solved2 = true;
    return true;
}
// Returns the letters currently on the row as text
string Scrabble::getRowWord(WordRow row) const {
    const DynamicArray<Tile>& r = (row == ROW_WORD1) ? row1 : row2;
    string w = "";
    for (int i = 0; i < r.getSize(); i++) w += (r[i].filled ? r[i].tileLetter : '_');
    return w;
}
// Draws one row of colored tiles with a status tag
void Scrabble::displayRow(WordRow row) const {
    const DynamicArray<Tile>& r = (row == ROW_WORD1) ? row1 : row2;
    bool solved = isRowSolved(row);
    ConsoleUI::setColor(TINT_GRAY);
    cout << "           ";
    for (int i = 0; i < r.getSize(); i++) cout << " " << (i + 1) << "  ";
    cout << endl;
    ConsoleUI::setColor(TINT_GOLD);
    cout << "  Word " << (row == ROW_WORD1 ? 1 : 2) << ":  ";
    for (int i = 0; i < r.getSize(); i++) {
        if (r[i].filled) {
            ConsoleUI::setColor(solved ? TINT_GOLD : TINT_LGREEN);
            cout << "[" << r[i].tileLetter << "] ";
        } else {
            ConsoleUI::setColor(TINT_GRAY);
            cout << "[ ] ";
        }
    }
    if (solved) {
        ConsoleUI::setColor(TINT_GOLD);
        cout << " SOLVED";
    } else if (isRowComplete(row)) {
        ConsoleUI::setColor(TINT_LCYAN);
        cout << " COMPLETE";
    }
    ConsoleUI::resetColor();
    cout << endl;
}
// Shows the whole Scrabble board
void Scrabble::display() const {
    ConsoleUI::setColor(TINT_LCYAN);
    cout << "  Scrabble Board:" << endl;
    ConsoleUI::resetColor();
    displayRow(ROW_WORD1);
    displayRow(ROW_WORD2);
}
ostream& operator<<(ostream& out, const Scrabble& s) {
    out << "Word 1: " << s.getRowWord(ROW_WORD1) << "  Word 2: " << s.getRowWord(ROW_WORD2);
    return out;
}
// Extraction: reads two target words from a stream
istream& operator>>(istream& in, Scrabble& s) {
    string a, b;
    in >> a >> b;
    s.loadWords(a, b);
    return in;
}

// ============================================================
//  Character hierarchy
// ============================================================

Character::Character(const string& name, const string& role, const string& nation)
    : characterName(name), characterRole(role), nationality(nation), isCulprit(false), interviewed(false) {}
Character::~Character() {}
int Character::getRequiredCredibility() const { return 0; }
Clue Character::getClue() const { return Clue(); }
void Character::setClue(char, int, WordRow) {}
bool Character::isSuspectRole() const { return false; }
string Character::getName() const { return characterName; }
string Character::getRole() const { return characterRole; }
string Character::getNationality() const { return nationality; }
bool Character::getIsCulprit() const { return isCulprit; }
bool Character::isInterviewed() const { return interviewed; }
void Character::setName(const string& name) { characterName = name; }

// Shared card style used by all display() overrides
void Character::showCard(int tint, const string& flavor) const {
    cout << "  ";
    ConsoleUI::setColor(tint);
    cout << characterRole << ": ";
    ConsoleUI::setColor(TINT_BWHITE);
    cout << characterName;
    ConsoleUI::setColor(TINT_GRAY);
    cout << " (" << nationality << ")" << endl;
    if (!flavor.empty()) cout << "    " << flavor << endl;
    ConsoleUI::resetColor();
}
ostream& operator<<(ostream& out, const Character& c) {
    out << c.characterName << " (" << c.characterRole << ", " << c.nationality << ")";
    return out;
}
istream& operator>>(istream& in, Character& c) {
    in >> c.characterName >> c.characterRole >> c.nationality;
    return in;
}

// ---- Victim ----
Victim::Victim(const string& name, const string& nation)
    : Character(name, "Victim", nation), clueRowChar(ROW_WORD2), letterClueChar(' '), boardPosChar(0) {}
Victim::~Victim() {}
void Victim::display() const { showCard(TINT_LRED, "The wealthy owner whose locker was robbed. Trusts only a credible detective."); }
int Victim::getRequiredCredibility() const { return 80; }
Clue Victim::getClue() const { return Clue(letterClueChar, boardPosChar, clueRowChar); }
void Victim::setClue(char letter, int position, WordRow row) {
    letterClueChar = letter; boardPosChar = position; clueRowChar = row;
}

// ---- Detective ----
Detective::Detective(const string& name, int startCredibility)
    : Character(name, "Detective", "Malay"), credibility(startCredibility), turns(0),
      rackWord1(Scrabble::ROW1_SIZE), rackWord2(Scrabble::ROW2_SIZE) {}
Detective::~Detective() {}
void Detective::display() const {
    showCard(TINT_LCYAN, "Credibility: " + ConsoleUI::intToStr(credibility) + " | Turns used: " + ConsoleUI::intToStr(turns));
}
// Prepares the detective for a fresh game
void Detective::reset(const string& name, int startCredibility) {
    setName(name);
    credibility = startCredibility;
    turns = 0;
    rackWord1.clear();
    rackWord2.clear();
}
int Detective::getCredibility() const { return credibility; }
int Detective::getTurns() const { return turns; }
// Credibility never goes below zero
void Detective::loseCredibility(int amount) {
    credibility -= amount;
    if (credibility < 0) credibility = 0;
}
// The engine increments turns, the detective only stores the count
void Detective::useTurn() { turns++; }
TileRack& Detective::getRack(WordRow row) { return row == ROW_WORD1 ? rackWord1 : rackWord2; }
const TileRack& Detective::getRack(WordRow row) const { return row == ROW_WORD1 ? rackWord1 : rackWord2; }

// ---- Suspect and its branches ----
Suspect::Suspect(const string& name, const string& role, const string& nation)
    : Character(name, role, nation), clueRowChar(ROW_WORD2), letterClueChar(' '), boardPosChar(0) {}
Suspect::~Suspect() {}
Clue Suspect::getClue() const { return Clue(letterClueChar, boardPosChar, clueRowChar); }
void Suspect::setClue(char letter, int position, WordRow row) {
    letterClueChar = letter; boardPosChar = position; clueRowChar = row;
}
bool Suspect::isSuspectRole() const { return true; }

BusinessRelations::BusinessRelations(const string& name, const string& role, const string& nation)
    : Suspect(name, role, nation) {}
BusinessRelations::~BusinessRelations() {}
CloseRelations::CloseRelations(const string& name, const string& role, const string& nation)
    : Suspect(name, role, nation) {}
CloseRelations::~CloseRelations() {}
HouseHelp::HouseHelp(const string& name, const string& role, const string& nation)
    : Suspect(name, role, nation) {}
HouseHelp::~HouseHelp() {}

// With virtual inheritance the most derived class builds the shared Suspect part
Partner::Partner(const string& name, const string& role, const string& nation)
    : Suspect(name, role, nation), BusinessRelations(name, role, nation) {}
Partner::~Partner() {}
void Partner::display() const { showCard(TINT_PURPLE, "Business partner with a grudge about profits."); }
int Partner::getRequiredCredibility() const { return 50; }

PA::PA(const string& name, const string& nation)
    : Suspect(name, "PA", nation), BusinessRelations(name, "PA", nation), CloseRelations(name, "PA", nation) {}
PA::~PA() {}
void PA::display() const { showCard(TINT_GOLD, "Right hand of the owner. Knows every secret."); }
int PA::getRequiredCredibility() const { return 35; }

Cook::Cook(const string& name, const string& nation) : HouseHelp(name, "Cook", nation) {}
Cook::~Cook() {}
void Cook::display() const { showCard(TINT_LGREEN, "Spent the party in the kitchen near the knives and spices."); }

Guard::Guard(const string& name, const string& nation) : HouseHelp(name, "Guard", nation) {}
Guard::~Guard() {}
void Guard::display() const { showCard(TINT_LBLUE, "Watched the gate and the garden all night."); }

Servant::Servant(const string& name, const string& nation) : HouseHelp(name, "Servant", nation) {}
Servant::~Servant() {}
void Servant::display() const { showCard(TINT_CYAN, "Carried trays between rooms and heard every whisper."); }

OwnersChild::OwnersChild(const string& name, const string& nation)
    : Suspect(name, "Child", nation), CloseRelations(name, "Child", nation) {}
OwnersChild::~OwnersChild() {}
void OwnersChild::display() const { showCard(TINT_PINK, "Panicked teenager (15-19) hiding behind the sofa."); }
int OwnersChild::getRequiredCredibility() const { return 75; }

// ============================================================
//  ScoreBoard
// ============================================================

// Makes sure the leader folder and file exist
ScoreBoard::ScoreBoard(const string& file) : fileName(file) {
    CreateDirectoryA("leader", NULL);
    ifstream test(fileName.c_str());
    if (!test) {
        ofstream create(fileName.c_str());
        if (create) create << "Detective|Result|Credibility|Turns_Left|Culprit|Date" << endl;
    }
}
// Today's date as dd-mm-yy
string ScoreBoard::todayDate() {
    time_t now = time(0);
    tm* local = localtime(&now);
    char buffer[16];
    strftime(buffer, sizeof(buffer), "%d-%m-%y", local);
    return string(buffer);
}
// Appends one finished game to the score file
void ScoreBoard::addScore(const string& detectiveName, const string& result, int credibility, int turnsLeft, const string& culprit) {
    ofstream out(fileName.c_str(), ios::app);
    if (!out) throw FileException(fileName + " could not be opened for writing.");
    string culpritText = culprit;
    for (size_t i = 0; i < culpritText.length(); i++)
        if (culpritText[i] == ' ') culpritText[i] = '_';
    out << detectiveName << "|" << result << "|" << credibility << "|"
        << ConsoleUI::intToStr(turnsLeft, 2) << "|" << culpritText << "|" << todayDate() << endl;
}
// Prints every saved score as a colored table
void ScoreBoard::displayAll() const {
    ifstream in(fileName.c_str());
    if (!in) throw FileException(fileName + " was not found.");
    string line;
    if (!ConsoleUI::nextDataLine(in, line)) throw FileException(fileName + " is empty or corrupted.");
    ConsoleUI::setColor(TINT_LCYAN);
    cout << "  " << ConsoleUI::pad("Detective", 16) << ConsoleUI::pad("Result", 8) << ConsoleUI::pad("Cred", 6)
         << ConsoleUI::pad("Left", 6) << ConsoleUI::pad("Culprit", 12) << "Date" << endl;
    ConsoleUI::setColor(TINT_GRAY);
    cout << "  ";
    ConsoleUI::printChar(196, 56);
    cout << endl;
    int rows = 0;
    while (ConsoleUI::nextDataLine(in, line)) {
        string parts[8];
        if (ConsoleUI::splitLine(line, parts, 8) != 6) throw FileException(fileName + " is corrupted (6 columns expected).");
        ConsoleUI::setColor(TINT_WHITE);
        cout << "  " << ConsoleUI::pad(parts[0], 16);
        ConsoleUI::setColor(parts[1] == "WIN" ? TINT_LGREEN : TINT_LRED);
        cout << ConsoleUI::pad(parts[1], 8);
        ConsoleUI::setColor(TINT_WHITE);
        cout << ConsoleUI::pad(parts[2], 6) << ConsoleUI::pad(parts[3], 6)
             << ConsoleUI::pad(ConsoleUI::prettyName(parts[4]), 12) << parts[5] << endl;
        rows++;
    }
    if (rows == 0) {
        ConsoleUI::setColor(TINT_GRAY);
        cout << "  No games played yet." << endl;
    }
    ConsoleUI::resetColor();
}

// ============================================================
//  GameEngine  (Singleton)
// ============================================================

GameEngine* GameEngine::instance = 0;

// Private constructor: nobody else can create an engine
GameEngine::GameEngine(ScoreBoard* board)
    : state(GAME_MENU), detective(), cottage(), scrabble(), clueRack(Scrabble::ROW1_SIZE + Scrabble::ROW2_SIZE + 1),
      scoreBoard(board), peopleCount(0), maxTurns(25), startCredibility(100), wrongTurnPenalty(5),
      wrongWordPenalty(10), winCondition("Accuse_Correct_Culprit"),
      loseCondition("Credibility_Zero_Or_Turns_Exhausted"), culpritRole("") {
    for (int i = 0; i < MAX_PEOPLE; i++) people[i] = 0;
}
// Returns the one and only engine (creates it on first call)
GameEngine* GameEngine::getInstance(ScoreBoard* board) {
    if (instance == 0) instance = new GameEngine(board);
    return instance;
}
void GameEngine::destroyInstance() {
    delete instance;
    instance = 0;
}
GameEngine::~GameEngine() { freePeople(); }

// Deletes every character (virtual destructors clean the right class)
void GameEngine::freePeople() {
    for (int i = 0; i < MAX_PEOPLE; i++) {
        delete people[i];
        people[i] = 0;
    }
    peopleCount = 0;
}
bool GameEngine::isRunning() const { return state != GAME_EXIT; }

// Small helper: draws a titled box
static void showHeader(const string& title, int color) {
    ConsoleUI::boxTop(60, color);
    ConsoleUI::boxLine(title, 60, color, TINT_BWHITE);
    ConsoleUI::boxBottom(60, color);
}

// ---------------- Welcome screen ----------------
// First page: title, locker drawing and a loading bar
void GameEngine::showWelcome() const {
    ConsoleUI::clearScreen();
    cout << endl;
    ConsoleUI::boxTop(60, TINT_GOLD);
    ConsoleUI::boxLine("W E L C O M E   T O", 60, TINT_GOLD, TINT_BWHITE);
    ConsoleUI::boxBottom(60, TINT_GOLD);
    cout << endl;
    ConsoleUI::bigText("THE", TINT_GOLD);
    cout << endl;
    ConsoleUI::bigText("LOCKER HEIST", TINT_LCYAN);
    cout << endl;
    ConsoleUI::setColor(TINT_GRAY);
    cout << "              _______________" << endl;
    cout << "             |  _____  _____ |" << endl;
    cout << "             | |     ||     ||" << endl;
    cout << "             | |  O  ||  O  ||" << endl;
    cout << "             | |_____||_____||" << endl;
    cout << "             |_______________|" << endl << endl;
    ConsoleUI::resetColor();
    cout << "      ";
    ConsoleUI::typeText("A Detective Game backed by Principles of OOP", 25, TINT_LGREEN);
    cout << endl << endl;
    ConsoleUI::beep(523, 120);
    ConsoleUI::beep(659, 120);
    ConsoleUI::beep(784, 200);
    for (int p = 0; p <= 100; p += 4) {
        cout << "\r      Loading ";
        ConsoleUI::progressBar(p, 100, 30, TINT_LGREEN);
        cout << " " << p << "%  " << flush;
        ConsoleUI::delay(70);
    }
    cout << endl;
    ConsoleUI::delay(800);
}

// ---------------- Menus ----------------
void GameEngine::showMainMenu() const {
    ConsoleUI::clearScreen();
    cout << endl;
    ConsoleUI::boxTop(60, TINT_GOLD);
    ConsoleUI::boxLine("THE LOCKER HEIST - MAIN MENU", 60, TINT_GOLD, TINT_BWHITE);
    ConsoleUI::boxDivider(60, TINT_GOLD);
    ConsoleUI::boxLine("", 60, TINT_GOLD, TINT_WHITE);
    ConsoleUI::boxLine("      [1] New Game", 60, TINT_GOLD, TINT_LGREEN, false);
    ConsoleUI::boxLine("      [2] View Leader/Score Board", 60, TINT_GOLD, TINT_LCYAN, false);
    ConsoleUI::boxLine("      [3] About", 60, TINT_GOLD, TINT_LBLUE, false);
    ConsoleUI::boxLine("      [4] Rules", 60, TINT_GOLD, TINT_PINK, false);
    ConsoleUI::boxLine("      [5] Exit", 60, TINT_GOLD, TINT_LRED, false);
    ConsoleUI::boxLine("", 60, TINT_GOLD, TINT_WHITE);
    ConsoleUI::boxBottom(60, TINT_GOLD);
}
void GameEngine::showGameMenu() const {
    ConsoleUI::boxTop(60, TINT_LCYAN);
    ConsoleUI::boxLine("GAME MENU", 60, TINT_LCYAN, TINT_BWHITE);
    ConsoleUI::boxDivider(60, TINT_LCYAN);
    ConsoleUI::boxLine("      [1] Visit a Location", 60, TINT_LCYAN, TINT_WHITE, false);
    ConsoleUI::boxLine("      [2] Interview a Person", 60, TINT_LCYAN, TINT_WHITE, false);
    ConsoleUI::boxLine("      [3] View Scrabble Board (free - no turn)", 60, TINT_LCYAN, TINT_LGREEN, false);
    ConsoleUI::boxLine("      [4] Form a Word", 60, TINT_LCYAN, TINT_WHITE, false);
    ConsoleUI::boxLine("      [5] Accuse a Suspect", 60, TINT_LCYAN, TINT_GOLD, false);
    ConsoleUI::boxLine("      [6] Quit", 60, TINT_LCYAN, TINT_LRED, false);
    ConsoleUI::boxBottom(60, TINT_LCYAN);
}

// Status screen: credibility bar, turn bar, racks and board
void GameEngine::showHUD() const {
    ConsoleUI::clearScreen();
    showHeader("THE LOCKER HEIST - Detective " + detective.getName(), TINT_GOLD);
    int cred = detective.getCredibility();
    int color = cred > 50 ? TINT_LGREEN : (cred > 25 ? TINT_GOLD : TINT_LRED);
    cout << "  Credibility: ";
    ConsoleUI::progressBar(cred, startCredibility, 20, color);
    cout << " " << cred << "/" << startCredibility << endl;
    cout << "  Turns used:  ";
    ConsoleUI::progressBar(detective.getTurns(), maxTurns, 20, TINT_LCYAN);
    cout << " " << detective.getTurns() << "/" << maxTurns << endl;
    cout << "  Rack W1: ";
    detective.getRack(ROW_WORD1).display();
    cout << "  Rack W2: ";
    detective.getRack(ROW_WORD2).display();
    cout << endl;
    scrabble.display();
    cout << endl;
}

// ---------------- Text file screens ----------------
void GameEngine::showAbout() const {
    ifstream in("about.txt");
    if (!in) throw FileException("about.txt was not found.");
    ConsoleUI::clearScreen();
    string line;
    while (getline(in, line)) {
        line = ConsoleUI::trim(line);
        if (line.find("====") != string::npos) ConsoleUI::setColor(TINT_GOLD);
        else if (line.find("----") != string::npos) ConsoleUI::setColor(TINT_LCYAN);
        else ConsoleUI::setColor(TINT_WHITE);
        cout << "  " << line << endl;
    }
    ConsoleUI::resetColor();
    ConsoleUI::pause();
}
// Reads rules.txt; falls back to default values if it is missing or corrupt
bool GameEngine::loadRules() {
    int mt = 25, sc = 100, wt = 5, ww = 10;
    string win = "Accuse_Correct_Culprit", lose = "Credibility_Zero_Or_Turns_Exhausted";
    bool ok = true;
    ifstream in("rules.txt");
    if (!in) {
        ok = false;
    } else {
        string line;
        while (getline(in, line)) {
            line = ConsoleUI::trim(line);
            if (line.empty()) continue;
            size_t eq = line.find('=');
            if (eq == string::npos) { ok = false; break; }
            string key = ConsoleUI::trim(line.substr(0, eq));
            string val = ConsoleUI::trim(line.substr(eq + 1));
            bool numOk = true;
            if (key == "Max_Turns") mt = ConsoleUI::parseInt(val, numOk);
            else if (key == "Starting_Credibility") sc = ConsoleUI::parseInt(val, numOk);
            else if (key == "Every_Wrong_Turn") { wt = ConsoleUI::parseInt(val, numOk); if (wt < 0) wt = -wt; }
            else if (key == "Every_Wrong_Word") { ww = ConsoleUI::parseInt(val, numOk); if (ww < 0) ww = -ww; }
            else if (key == "Win_Condition") win = val;
            else if (key == "Lose_Condition") lose = val;
            if (!numOk) { ok = false; break; }
        }
        if (mt < 1 || sc < 1) ok = false;
    }
    if (!ok) {
        mt = 25; sc = 100; wt = 5; ww = 10;
        win = "Accuse_Correct_Culprit";
        lose = "Credibility_Zero_Or_Turns_Exhausted";
    }
    maxTurns = mt; startCredibility = sc; wrongTurnPenalty = wt; wrongWordPenalty = ww;
    winCondition = win; loseCondition = lose;
    return ok;
}
void GameEngine::showRules() {
    bool ok = loadRules();
    ConsoleUI::clearScreen();
    cout << endl;
    showHeader("THE LOCKER HEIST - RULES", TINT_PINK);
    if (!ok) {
        ConsoleUI::setColor(TINT_GOLD);
        cout << "\n  rules.txt is missing or corrupt - default rules are shown." << endl;
    }
    ConsoleUI::setColor(TINT_WHITE);
    cout << "\n  Maximum turns         : " << maxTurns << endl;
    cout << "  Starting credibility  : " << startCredibility << endl;
    cout << "  Every wrong turn      : -" << wrongTurnPenalty << " credibility" << endl;
    cout << "  Every wrong word      : -" << wrongWordPenalty << " credibility" << endl;
    ConsoleUI::setColor(TINT_LGREEN);
    cout << "  Win condition         : " << ConsoleUI::prettyName(winCondition) << endl;
    ConsoleUI::setColor(TINT_LRED);
    cout << "  Lose condition        : " << ConsoleUI::prettyName(loseCondition) << endl;
    ConsoleUI::setColor(TINT_GRAY);
    cout << "\n  Viewing the Scrabble board is free (no turn)." << endl;
    cout << "  Interview limits: Victim 80+, Child 75+, Partners 50+, PA 35+." << endl;
    cout << "  Re-visiting a place or person costs a turn." << endl;
    ConsoleUI::resetColor();
    ConsoleUI::pause();
}
void GameEngine::showScoreBoardScreen() const {
    ConsoleUI::clearScreen();
    cout << endl;
    showHeader("LEADER / SCORE BOARD", TINT_LCYAN);
    cout << endl;
    scoreBoard->displayAll();
    ConsoleUI::pause();
}

// ---------------- Loading game data ----------------
// Creates the right class for a role found in characters.txt
Character* GameEngine::createCharacter(const string& name, const string& role, const string& nation) {
    string r = ConsoleUI::toUpper(ConsoleUI::prettyName(role));
    if (r == "VICTIM") return new Victim(name, nation);
    if (r == "COOK") return new Cook(name, nation);
    if (r == "GUARD") return new Guard(name, nation);
    if (r == "SERVANT") return new Servant(name, nation);
    if (r == "CHILD") return new OwnersChild(name, nation);
    if (r == "PA") return new PA(name, nation);
    if (r == "PARTNER 1") return new Partner(name, "Partner 1", nation);
    if (r == "PARTNER 2") return new Partner(name, "Partner 2", nation);
    throw FileException("unknown role '" + role + "' in characters.txt.");
}
// Reads clue.txt and returns the role and two words of profile number culpritIndex
void GameEngine::loadClueFile(int culpritIndex, string& role, string& word1, string& word2) {
    ifstream in("clue.txt");
    if (!in) throw FileException("clue.txt was not found.");
    string line;
    if (!ConsoleUI::nextDataLine(in, line)) throw FileException("clue.txt is empty.");   // header
    string roles[7], w1[7], w2[7];
    int count = 0;
    while (ConsoleUI::nextDataLine(in, line)) {
        string parts[5];
        if (ConsoleUI::splitLine(line, parts, 5) != 3 || count >= 7)
            throw FileException("clue.txt is corrupted (7 lines with 3 columns expected).");
        roles[count] = ConsoleUI::prettyName(parts[0]);
        w1[count] = ConsoleUI::toUpper(parts[1]);
        w2[count] = ConsoleUI::toUpper(parts[2]);
        count++;
    }
    if (count != 7) throw FileException("clue.txt must contain exactly 7 profiles.");
    role = roles[culpritIndex];
    word1 = w1[culpritIndex];
    word2 = w2[culpritIndex];
}
// Reads characters.txt, builds the 8 objects and marks the culprit
void GameEngine::loadCharacters(int culpritIndex, const string& role) {
    ifstream in("characters.txt");
    if (!in) throw FileException("characters.txt was not found.");
    freePeople();
    string line;
    if (!ConsoleUI::nextDataLine(in, line)) throw FileException("characters.txt is empty.");   // header
    bool culpritFound = false;
    int suspects = 0;
    while (ConsoleUI::nextDataLine(in, line)) {
        string parts[16];
        if (ConsoleUI::splitLine(line, parts, 16) != 12 || peopleCount >= MAX_PEOPLE)
            throw FileException("characters.txt is corrupted (8 lines with 12 columns expected).");
        bool ok = false;
        int pos = ConsoleUI::parseInt(parts[3], ok);
        string letterText = parts[4 + culpritIndex];
        if (!ok || pos < 1 || pos > Scrabble::ROW2_SIZE || letterText.empty())
            throw FileException("characters.txt is corrupted (bad position or letter).");
        Character* person = createCharacter(parts[0], parts[1], parts[2]);
        people[peopleCount++] = person;                      // stored at once so it is always freed
        person->setClue(ConsoleUI::toUpper(letterText)[0], pos, ROW_WORD2);
        if (person->isSuspectRole()) suspects++;
        if (ConsoleUI::toUpper(ConsoleUI::prettyName(parts[1])) == ConsoleUI::toUpper(role)) {
            person->isCulprit = true;                        // friend access for GameEngine
            culpritFound = true;
        }
    }
    if (peopleCount != MAX_PEOPLE || suspects != 7 || !culpritFound)
        throw FileException("characters.txt must have 1 victim, 7 suspects and the culprit role.");
}

// Asks for the detective name (no spaces allowed, _ or . can replace them)
string GameEngine::readDetectiveName() {
    ConsoleUI::setColor(TINT_GOLD);
    cout << "\n  Enter detective name: ";
    ConsoleUI::resetColor();
    string name = ConsoleUI::trim(ConsoleUI::readLine());
    if (name.empty()) throw InvalidInputException("the name cannot be empty.");
    if (name.length() > 20) throw InvalidInputException("the name can have at most 20 characters.");
    if (name.find(' ') != string::npos) {
        ConsoleUI::setColor(TINT_GOLD);
        cout << "  Spaces are not allowed. Replace spaces with underscore (_) or dot (.)? Type _ or . : ";
        ConsoleUI::resetColor();
        string symbol = ConsoleUI::trim(ConsoleUI::readLine());
        if (symbol != "_" && symbol != ".") throw InvalidInputException("please type _ or . only.");
        for (size_t i = 0; i < name.length(); i++)
            if (name[i] == ' ') name[i] = symbol[0];
    }
    return name;
}

// New Game: loads everything first, then starts so a file error never leaves a half game
void GameEngine::startNewGame() {
    ConsoleUI::clearScreen();
    cout << endl;
    showHeader("THE LOCKER HEIST - NEW GAME", TINT_LGREEN);
    string name = readDetectiveName();
    bool rulesOk = loadRules();
    int culpritIndex = rand() % 7;                       // random culprit profile
    string role, word1, word2;
    loadClueFile(culpritIndex, role, word1, word2);
    Cottage freshCottage;
    freshCottage.loadFromFile("locations.txt", culpritIndex);
    loadCharacters(culpritIndex, role);
    scrabble.loadWords(word1, word2);
    cottage = freshCottage;                              // deep copy assignment
    detective.reset(name, startCredibility);
    clueRack.clear();
    culpritRole = role;
    state = GAME_PLAYING;

    cout << endl;
    if (!rulesOk) {
        ConsoleUI::setColor(TINT_GOLD);
        cout << "  rules.txt is missing or corrupt - default rules are used." << endl;
        ConsoleUI::resetColor();
    }
    cout << "  ";
    ConsoleUI::typeText("A culprit has been secretly chosen. The game begins!", 25, TINT_LGREEN);
    cout << endl << endl;
    ConsoleUI::setColor(TINT_WHITE);
    cout << "  Max turns: " << maxTurns << " | Starting credibility: " << startCredibility
         << " | Wrong turn: -" << wrongTurnPenalty << " | Wrong word: -" << wrongWordPenalty << endl;
    ConsoleUI::resetColor();
    ConsoleUI::beep(600, 150);
    ConsoleUI::beep(900, 200);
    ConsoleUI::pause();
}

// ---------------- State machine ----------------
// Runs one step; exceptions travel up to main() and the state stays safe
void GameEngine::step() {
    switch (state) {
    case GAME_MENU:    menuStep(); break;
    case GAME_PLAYING: playStep(); break;
    case GAME_WON:
    case GAME_LOST:    finishGame(); break;
    case GAME_EXIT:    break;
    }
}
void GameEngine::menuStep() {
    showMainMenu();
    int choice = ConsoleUI::readChoice("Choice", 1, 5);
    switch ((MenuChoice)choice) {
    case CHOICE_NEW_GAME:   startNewGame(); break;
    case CHOICE_SCOREBOARD: showScoreBoardScreen(); break;
    case CHOICE_ABOUT:      showAbout(); break;
    case CHOICE_RULES:      showRules(); break;
    case CHOICE_EXIT:       state = GAME_EXIT; break;
    }
}
void GameEngine::playStep() {
    checkEnd();
    if (state != GAME_PLAYING) return;
    showHUD();
    showGameMenu();
    cout << "\n  --- TURN " << (detective.getTurns() + 1) << " ---" << endl;
    int choice = ConsoleUI::readChoice("Choice", 1, 6);
    switch (choice) {
    case 1: visitLocation(); break;
    case 2: interviewPerson(); break;
    case 3: showBoardScreen(); break;
    case 4: formWord(); break;
    case 5: accuseSuspect(); break;
    case 6: quitGame(); break;
    }
    checkEnd();
}
// Turn counter is owned by the engine
void GameEngine::spendTurn() { detective.useTurn(); }
// Lose conditions: credibility zero or turns exhausted
void GameEngine::checkEnd() {
    if (state != GAME_PLAYING) return;
    if (detective.getCredibility() <= 0 || detective.getTurns() >= maxTurns) state = GAME_LOST;
}
Character* GameEngine::findCulprit() const {
    for (int i = 0; i < peopleCount; i++)
        if (people[i]->isCulprit) return people[i];
    return 0;
}

// Letter collected animation
void GameEngine::flashLetter(const Clue& clue, const string& source) {
    cout << endl;
    for (int i = 0; i < 6; i++) {
        ConsoleUI::setColor(i % 2 == 0 ? TINT_GOLD : TINT_LGREEN);
        cout << "\r      >>>  [ " << clue.clueLetter << " ]  <<<   " << flush;
        ConsoleUI::delay(120);
    }
    cout << endl;
    ConsoleUI::beep(880, 120);
    ConsoleUI::setColor(TINT_LGREEN);
    cout << "\n  " << source << " gave you a clue!" << endl;
    cout << "  Letter " << clue.clueLetter << " collected at position " << clue.cluePosition
         << " of Word " << (clue.clueRow == ROW_WORD1 ? 1 : 2) << "." << endl;
    ConsoleUI::resetColor();
}

// ---------------- Actions ----------------
void GameEngine::showBoardScreen() const {
    showHUD();
    ConsoleUI::pause();
}

void GameEngine::visitLocation() {
    while (true) {
        showHUD();
        cottage.displayMap();
        cottage.displayMenu();
        int pick = ConsoleUI::readChoice("Pick", 0, cottage.getLocationCount());
        if (pick == 0) return;
        int index = pick - 1;
        bool before = cottage.isSearched(index);
        string name = ConsoleUI::prettyName(cottage.getLocationName(index));
        bool yes;
        if (before) {
            ConsoleUI::setColor(TINT_GOLD);
            cout << "\n  You have ALREADY searched the " << name << "." << endl;
            yes = ConsoleUI::askYesNo("  Search it again?");
        } else {
            ConsoleUI::setColor(TINT_GOLD);
            cout << "\n  You have NOT searched the " << name << " yet." << endl;
            yes = ConsoleUI::askYesNo("  Search?");
        }
        if (!yes) continue;                              // NO: back to the location menu
        cout << "  ";
        ConsoleUI::typeText("Searching the " + name + "...", 30, TINT_GRAY);
        Clue clue = cottage.getClue(index);
        if (!before) {                                   // first visit: store and place the letter
            detective.getRack(ROW_WORD1).addTile(clue);
            scrabble.placeLetter(clue);
            cottage.markSearched(index);
        }
        flashLetter(clue, name);
        if (!before) cout << "  Letter has been placed." << endl;
        spendTurn();
        ConsoleUI::pause();
        return;
    }
}

void GameEngine::interviewPerson() {
    while (true) {
        showHUD();
        ConsoleUI::setColor(TINT_LCYAN);
        cout << "  ====== Character Menu ======" << endl;
        for (int i = 0; i < peopleCount; i++) {
            Character* p = people[i];
            ConsoleUI::setColor(TINT_WHITE);
            cout << "  [" << (i + 1) << "] " << ConsoleUI::pad(p->getName() + " (" + p->getRole() + ")", 26);
            if (p->isInterviewed()) {
                ConsoleUI::setColor(TINT_LGREEN);
                cout << " - Interviewed ";
                ConsoleUI::printChar(251);
            } else {
                ConsoleUI::setColor(TINT_LRED);
                cout << " - NOT Interviewed";
                if (p->getRequiredCredibility() > 0) {
                    ConsoleUI::setColor(TINT_GRAY);
                    cout << " (needs " << p->getRequiredCredibility() << "+)";
                }
            }
            cout << endl;
        }
        ConsoleUI::setColor(TINT_GRAY);
        cout << "  [0] Back to Game Menu" << endl;
        ConsoleUI::resetColor();
        int pick = ConsoleUI::readChoice("Pick", 0, peopleCount);
        if (pick == 0) return;
        Character* p = people[pick - 1];
        bool before = p->isInterviewed();
        cout << endl;
        p->display();                                    // polymorphic display
        if (!before && detective.getCredibility() < p->getRequiredCredibility()) {
            detective.loseCredibility(wrongTurnPenalty); // wrong step
            spendTurn();
            throw LockedCharacterException(p->getName() + " refuses to talk (needs credibility "
                + ConsoleUI::intToStr(p->getRequiredCredibility()) + "+). -"
                + ConsoleUI::intToStr(wrongTurnPenalty) + " credibility, turn used.");
        }
        bool yes;
        if (before) {
            ConsoleUI::setColor(TINT_GOLD);
            cout << "\n  You have ALREADY interviewed " << p->getName() << "." << endl;
            yes = ConsoleUI::askYesNo("  Interview again?");
        } else {
            ConsoleUI::setColor(TINT_GOLD);
            cout << "\n  You have NOT interviewed " << p->getName() << " yet." << endl;
            yes = ConsoleUI::askYesNo("  Interview?");
        }
        if (!yes) continue;                              // NO: back to the character menu
        cout << "  ";
        ConsoleUI::typeText("Interviewing " + p->getName() + "...", 30, TINT_GRAY);
        Clue clue = p->getClue();
        if (!before) {
            detective.getRack(ROW_WORD2).addTile(clue);
            scrabble.placeLetter(clue);
            p->interviewed = true;                       // friend access for GameEngine
        }
        flashLetter(clue, p->getName());
        if (!before) cout << "  Letter has been placed." << endl;
        spendTurn();
        ConsoleUI::pause();
        return;
    }
}

void GameEngine::formWord() {
    showHUD();
    ConsoleUI::setColor(TINT_LCYAN);
    cout << "  ====== Form a Word ======" << endl;
    for (int r = 0; r < 2; r++) {
        WordRow row = (r == 0) ? ROW_WORD1 : ROW_WORD2;
        ConsoleUI::setColor(TINT_WHITE);
        cout << "  [" << (r + 1) << "] Word " << (r + 1) << " - ";
        if (scrabble.isRowSolved(row)) { ConsoleUI::setColor(TINT_GOLD); cout << "SOLVED"; }
        else if (scrabble.isRowComplete(row)) { ConsoleUI::setColor(TINT_LCYAN); cout << "COMPLETE (ready)"; }
        else { ConsoleUI::setColor(TINT_GRAY); cout << "not complete"; }
        cout << endl;
    }
    ConsoleUI::setColor(TINT_GRAY);
    cout << "  [0] Back to Game Menu" << endl;
    ConsoleUI::resetColor();
    int pick = ConsoleUI::readChoice("Pick", 0, 2);
    if (pick == 0) return;
    WordRow row = (pick == 1) ? ROW_WORD1 : ROW_WORD2;
    if (scrabble.isRowSolved(row)) {
        ConsoleUI::setColor(TINT_GOLD);
        cout << "\n  This word is already solved." << endl;
        ConsoleUI::resetColor();
        ConsoleUI::pause();
        return;
    }
    if (!scrabble.isRowComplete(row))
        throw InvalidWordException("Word " + ConsoleUI::intToStr(pick) + " is not complete. Collect all its letters first.");
    ConsoleUI::setColor(TINT_GOLD);
    cout << "\n  Type the word made by the letters: ";
    ConsoleUI::resetColor();
    string typed = ConsoleUI::toUpper(ConsoleUI::trim(ConsoleUI::readLine()));
    if (typed.empty()) throw InvalidInputException("the word cannot be empty.");
    spendTurn();
    if (!scrabble.checkWord(row, typed)) {
        detective.loseCredibility(wrongWordPenalty);
        throw InvalidWordException("'" + typed + "' is wrong. -" + ConsoleUI::intToStr(wrongWordPenalty) + " credibility.");
    }
    ConsoleUI::beep(784, 150);
    ConsoleUI::beep(1047, 250);
    ConsoleUI::setColor(TINT_LGREEN);
    cout << "\n  Correct! '" << typed << "' is right." << endl;
    ConsoleUI::resetColor();
    if (scrabble.isBothSolved()) {
        clueRack = detective.getRack(ROW_WORD1) + detective.getRack(ROW_WORD2);   // operator+
        Character* culprit = findCulprit();
        ConsoleUI::setColor(TINT_GOLD);
        cout << "\n  Both words solved:" << endl;
        cout << "  " << scrabble.getRowWord(ROW_WORD1) << " + " << scrabble.getRowWord(ROW_WORD2);
        if (culprit != 0)
            cout << " = " << culprit->getNationality() << " -> " << culprit->getRole() << " is the culprit!";
        cout << endl;
        ConsoleUI::setColor(TINT_GRAY);
        cout << "  Clue rack: ";
        clueRack.display();
        ConsoleUI::setColor(TINT_LCYAN);
        cout << "\n  Now choose [5] Accuse a Suspect to name them." << endl;
        ConsoleUI::resetColor();
    }
    ConsoleUI::pause();
}

void GameEngine::accuseSuspect() {
    showHUD();
    ConsoleUI::setColor(TINT_GOLD);
    cout << "  ====== Accuse a Suspect ======" << endl;
    int indexOf[MAX_PEOPLE];
    int listed = 0;
    for (int i = 0; i < peopleCount; i++) {
        if (!people[i]->isSuspectRole()) continue;       // the victim is not a suspect
        indexOf[listed++] = i;
        ConsoleUI::setColor(TINT_WHITE);
        cout << "  [" << listed << "] " << people[i]->getName() << " (" << people[i]->getRole() << ")" << endl;
    }
    ConsoleUI::setColor(TINT_GRAY);
    cout << "  [0] Back to Game Menu" << endl;
    ConsoleUI::resetColor();
    int pick = ConsoleUI::readChoice("Accuse", 0, listed);
    if (pick == 0) return;
    Character* accused = people[indexOf[pick - 1]];
    cout << "\n  Accuse: " << accused->getName() << " (" << accused->getRole() << ")" << endl;
    if (!ConsoleUI::askYesNo("  Are you sure?")) return;
    spendTurn();
    if (accused->isCulprit) {
        cout << "\n  ";
        ConsoleUI::typeText(accused->getName() + " is the culprit!", 40, TINT_LGREEN);
        cout << endl;
        ConsoleUI::delay(600);
        state = GAME_WON;
    } else {
        detective.loseCredibility(wrongTurnPenalty);
        throw WrongAccusationException(accused->getName() + " is innocent! -"
            + ConsoleUI::intToStr(wrongTurnPenalty) + " credibility.");
    }
}

void GameEngine::quitGame() {
    if (ConsoleUI::askYesNo("\n  Quit this game? (progress will be lost)")) state = GAME_MENU;
}

// WIN / LOSE screen, saves the score and returns to the main menu
void GameEngine::finishGame() {
    bool won = (state == GAME_WON);
    Character* culprit = findCulprit();
    string culpritText = culprit ? (culprit->getRole() + " " + culprit->getName()) : culpritRole;
    int cred = detective.getCredibility();
    int used = detective.getTurns();
    int left = maxTurns - used;
    if (left < 0) left = 0;
    string who = detective.getName();
    state = GAME_MENU;                                   // set first: an error can never repeat this screen

    ConsoleUI::clearScreen();
    cout << endl;
    ConsoleUI::bigText(won ? "WIN" : "LOSE", won ? TINT_LGREEN : TINT_LRED);
    cout << endl;
    int color = won ? TINT_LGREEN : TINT_LRED;
    ConsoleUI::boxTop(60, color);
    ConsoleUI::boxLine(who + (won ? " WIN!" : " LOSE!"), 60, color, TINT_BWHITE);
    ConsoleUI::boxLine("Final credibility: " + ConsoleUI::intToStr(cred) + " | Turns used: " + ConsoleUI::intToStr(used),
                       60, color, TINT_WHITE);
    ConsoleUI::boxLine("Culprit: " + culpritText, 60, color, TINT_GOLD);
    ConsoleUI::boxBottom(60, color);
    if (!won) {
        ConsoleUI::setColor(TINT_GRAY);
        cout << "\n  " << (cred <= 0 ? "Your credibility ran out." : "You ran out of turns.") << endl;
        ConsoleUI::resetColor();
    }
    if (won) {
        ConsoleUI::beep(523, 150); ConsoleUI::beep(659, 150); ConsoleUI::beep(784, 150); ConsoleUI::beep(1047, 350);
    } else {
        ConsoleUI::beep(400, 250); ConsoleUI::beep(300, 250); ConsoleUI::beep(200, 500);
    }
    scoreBoard->addScore(who, won ? "WIN" : "LOSE", cred, left, culprit ? culprit->getRole() : culpritRole);
    ConsoleUI::pause();
}
