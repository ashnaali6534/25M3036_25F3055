// ============================================================
//  heist.h  -  THE LOCKER HEIST (CL1004 OOP Semester Project)
//  Header file: declares all structs, enums, exceptions,
//  the DynamicArray template and every class of the game.
// ============================================================
#ifndef HEIST_H
#define HEIST_H

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <windows.h>   // only used for console colors, Sleep and Beep (graphics)
using namespace std;

// ------------------------------------------------------------
//  ENUMERATIONS
//  Enums give readable names to fixed sets of values, so the
//  code avoids "magic numbers" and stays easy to maintain.
// ------------------------------------------------------------

// Which Scrabble row a letter belongs to (Word 1 = locations, Word 2 = people)
enum WordRow { ROW_WORD1 = 0, ROW_WORD2 = 1 };

// State of the player inside the game
enum GameState { GAME_MENU, GAME_PLAYING, GAME_WON, GAME_LOST, GAME_EXIT };

// Options of the Main Menu
enum MenuChoice { CHOICE_NEW_GAME = 1, CHOICE_SCOREBOARD, CHOICE_ABOUT, CHOICE_RULES, CHOICE_EXIT };

// Console color codes (used by the graphics helper functions)
enum Tint {
    TINT_BLACK = 0, TINT_BLUE = 1, TINT_GREEN = 2, TINT_CYAN = 3, TINT_RED = 4,
    TINT_PURPLE = 5, TINT_YELLOW = 6, TINT_WHITE = 7, TINT_GRAY = 8, TINT_LBLUE = 9,
    TINT_LGREEN = 10, TINT_LCYAN = 11, TINT_LRED = 12, TINT_PINK = 13, TINT_GOLD = 14,
    TINT_BWHITE = 15
};

// ------------------------------------------------------------
//  STRUCTURES
//  Structs utilize simple public data holders, which improves
//  code reusability because many classes can share them.
// ------------------------------------------------------------

// Tile: one slot of the Scrabble board or one letter inside a rack
struct Tile {
    char tileLetter;   // the letter stored in the tile
    bool filled;       // true when a letter has been placed
    Tile() : tileLetter(' '), filled(false) {}
    Tile(char letter, bool isFilled) : tileLetter(letter), filled(isFilled) {}
    friend ostream& operator<<(ostream& out, const Tile& t);
    friend istream& operator>>(istream& in, Tile& t);
};

// Clue: a carrier that moves one letter + its position from a
// Location or Character to the Scrabble class
struct Clue {
    char clueLetter;
    int cluePosition;
    WordRow clueRow;
    Clue() : clueLetter(' '), cluePosition(0), clueRow(ROW_WORD1) {}
    Clue(char letter, int position, WordRow row) : clueLetter(letter), cluePosition(position), clueRow(row) {}
};

// Location: one place of the cottage which hides a letter of Word 1
struct Location {
    string locName;
    char letterClueLoc;
    int boardPosLoc;
    WordRow locRow;
    bool locSearchFlag;
    Location() : locName(""), letterClueLoc(' '), boardPosLoc(0), locRow(ROW_WORD1), locSearchFlag(false) {}
    friend ostream& operator<<(ostream& out, const Location& l);
    friend istream& operator>>(istream& in, Location& l);
};

// Comparison operators: compare letters coming from clue.txt
bool operator==(const Tile& a, const Tile& b);
bool operator==(const Clue& a, const Clue& b);
ostream& operator<<(ostream& out, const Tile& t);
istream& operator>>(istream& in, Tile& t);
ostream& operator<<(ostream& out, const Location& l);
istream& operator>>(istream& in, Location& l);

// ------------------------------------------------------------
//  EXCEPTIONS
//  GameException is the base class of every custom error.
//  All messages start with "HEIST ERROR".
// ------------------------------------------------------------
class GameException {
protected:
    string message;
public:
    GameException(const string& detail) : message("HEIST ERROR: " + detail) {}
    virtual ~GameException() {}
    virtual const string& getMessage() const { return message; }
    virtual void show() const;    // prints the message in red
};

class InvalidWordException : public GameException {
public:
    InvalidWordException(const string& d) : GameException("Invalid Word - " + d) {}
};
class RackFullException : public GameException {
public:
    RackFullException(const string& d) : GameException("Rack Full - " + d) {}
};
class RackEmptyException : public GameException {
public:
    RackEmptyException(const string& d) : GameException("Rack Empty - " + d) {}
};
class InvalidPositionException : public GameException {
public:
    InvalidPositionException(const string& d) : GameException("Invalid Position - " + d) {}
};
class SlotFilledException : public GameException {
public:
    SlotFilledException(const string& d) : GameException("Slot Already Filled - " + d) {}
};
class InvalidInputException : public GameException {
public:
    InvalidInputException(const string& d) : GameException("Invalid Input - " + d) {}
};
class FileException : public GameException {
public:
    FileException(const string& d) : GameException("File Problem - " + d) {}
};
class LockedCharacterException : public GameException {
public:
    LockedCharacterException(const string& d) : GameException("Interview Refused - " + d) {}
};
class WrongAccusationException : public GameException {
public:
    WrongAccusationException(const string& d) : GameException("Wrong Accusation - " + d) {}
};

// ------------------------------------------------------------
//  ConsoleUI
//  Static helper class for all graphics: colors, boxes, bars,
//  big banner text, animations, beeps and safe input reading.
// ------------------------------------------------------------
class ConsoleUI {
public:
    static void init();                               // sets console code page
    static void setColor(int color);                  // changes text color
    static void resetColor();                         // back to normal white
    static void clearScreen();                        // clears the console
    static void delay(int milliseconds);              // pause (animation timing)
    static void beep(int frequency, int milliseconds);// sound effect
    static void printChar(int code, int count = 1);   // prints extended box characters

    // Box drawing (double line borders)
    static void boxTop(int width, int color);
    static void boxDivider(int width, int color);
    static void boxBottom(int width, int color);
    static void boxLine(const string& text, int width, int borderColor, int textColor, bool center = true);

    // Effects
    static void typeText(const string& text, int delayMs, int color);   // typing animation
    static void bigText(const string& text, int color);                 // large block letters
    static void progressBar(int value, int maxValue, int width, int color);

    // Input helpers (all invalid input throws InvalidInputException)
    static string readLine();
    static int readChoice(const string& prompt, int low, int high);
    static bool askYesNo(const string& prompt);
    static void pause();

    // String helpers
    static string trim(const string& s);
    static string toUpper(const string& s);
    static string prettyName(const string& s);        // Bedroom_1 -> Bedroom 1
    static string pad(const string& s, int width);    // pads with spaces on the right
    static string intToStr(int value, int minDigits = 1);
    static int parseInt(const string& s, bool& ok);   // manual string -> int
    static int splitLine(const string& line, string parts[], int maxParts);
    static bool nextDataLine(ifstream& in, string& line);   // next non-empty line
};

// ------------------------------------------------------------
//  DynamicArray<T>  (Module 8: generic template)
//  A growable array that owns heap memory and performs deep copies.
// ------------------------------------------------------------
template <typename T>
class DynamicArray {
private:
    T* data;          // heap block
    int size;         // elements in use
    int capacity;     // allocated slots
    void grow(int newCapacity) {
        T* newData = new T[newCapacity];
        for (int i = 0; i < size; i++) newData[i] = data[i];
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }
public:
    DynamicArray(int cap = 0) : data(0), size(0), capacity(0) {
        if (cap > 0) { data = new T[cap]; capacity = cap; }
    }
    // Deep copy constructor
    DynamicArray(const DynamicArray& other) : data(0), size(other.size), capacity(other.capacity) {
        if (capacity > 0) {
            data = new T[capacity];
            for (int i = 0; i < size; i++) data[i] = other.data[i];
        }
    }
    // Assignment operator (deep copy, safe against self assignment)
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            T* newData = 0;
            if (other.capacity > 0) {
                newData = new T[other.capacity];
                for (int i = 0; i < other.size; i++) newData[i] = other.data[i];
            }
            delete[] data;
            data = newData;
            size = other.size;
            capacity = other.capacity;
        }
        return *this;
    }
    ~DynamicArray() { delete[] data; }

    void add(const T& item) {
        if (size == capacity) grow(capacity == 0 ? 4 : capacity * 2);
        data[size++] = item;
    }
    void resize(int newSize) {
        if (newSize > capacity) grow(newSize);
        for (int i = size; i < newSize; i++) data[i] = T();
        size = newSize;
    }
    void removeAt(int index) {
        if (index < 0 || index >= size)
            throw InvalidPositionException("index " + ConsoleUI::intToStr(index) + " is outside the array");
        for (int i = index; i < size - 1; i++) data[i] = data[i + 1];
        size--;
    }
    void clear() { size = 0; }
    int getSize() const { return size; }

    T& operator[](int index) {
        if (index < 0 || index >= size)
            throw InvalidPositionException("index " + ConsoleUI::intToStr(index) + " is outside the array");
        return data[index];
    }
    const T& operator[](int index) const {
        if (index < 0 || index >= size)
            throw InvalidPositionException("index " + ConsoleUI::intToStr(index) + " is outside the array");
        return data[index];
    }
};

// ------------------------------------------------------------
//  TileRack: stores the letters (with positions) the detective collected
// ------------------------------------------------------------
class TileRack {
private:
    DynamicArray<Tile> tiles;      // collected letters
    DynamicArray<int> positions;   // board position of each letter
    int maxSize;                   // rack capacity
    // --- earlier raw DMA logic (kept as comment, replaced by DynamicArray) ---
    // Tile* tilesRaw; int* posRaw; int count;
    // tilesRaw = new Tile[maxSize]; posRaw = new int[maxSize]; count = 0;
    // ~TileRack() { delete[] tilesRaw; delete[] posRaw; }
public:
    TileRack(int capacity = 8);
    TileRack(const TileRack& other);
    TileRack& operator=(const TileRack& other);
    ~TileRack();
    void addTile(const Clue& clue);        // throws RackFullException
    Tile removeTile(int index);            // throws RackEmptyException
    bool isFull() const;
    bool isEmpty() const;
    int getCount() const;
    void clear();
    void display() const;
    TileRack operator+(const TileRack& other) const;   // merges two racks with a space between
};

// ------------------------------------------------------------
//  Cottage: the 7 locations (aggregates a Location array)
// ------------------------------------------------------------
class Cottage {
private:
    DynamicArray<Location> locCache;   // LocCache
    // --- earlier raw DMA logic (kept as comment) ---
    // Location* locCacheRaw; int locCount;
    // locCacheRaw = new Location[7];  ... delete[] locCacheRaw;
public:
    static const int TOTAL_LOCATIONS = 7;
    Cottage();
    Cottage(const Cottage& other);
    Cottage& operator=(const Cottage& other);
    ~Cottage();
    void loadFromFile(const string& fileName, int culpritIndex);
    int getLocationCount() const;
    bool isSearched(int index) const;
    string getLocationName(int index) const;
    Clue getClue(int index) const;       // letter + position hidden in this location
    void markSearched(int index);        // sets the searched flag
    void displayMenu() const;
    void displayMap() const;
};

// ------------------------------------------------------------
//  Scrabble: jagged board, row 1 = 7 slots, row 2 = 8 slots
// ------------------------------------------------------------
class Scrabble {
private:
    DynamicArray<Tile> row1;
    DynamicArray<Tile> row2;
    string target1, target2;    // the two words loaded from clue.txt
    bool solved1, solved2;
    // --- earlier raw DMA logic (kept as comment) ---
    // Tile** grid = new Tile*[2]; grid[0] = new Tile[7]; grid[1] = new Tile[8];
    // delete[] grid[0]; delete[] grid[1]; delete[] grid;
    void displayRow(WordRow row) const;
public:
    static const int ROW1_SIZE = 7;
    static const int ROW2_SIZE = 8;
    Scrabble();
    Scrabble(const Scrabble& other);
    Scrabble& operator=(const Scrabble& other);
    ~Scrabble();
    void loadWords(const string& word1, const string& word2);
    void placeLetter(const Clue& clue);          // throws position / filled exceptions
    bool isRowComplete(WordRow row) const;
    bool isRowSolved(WordRow row) const;
    bool isBothSolved() const;
    bool checkWord(WordRow row, const string& typed);  // true when correct
    string getRowWord(WordRow row) const;
    void display() const;
    friend ostream& operator<<(ostream& out, const Scrabble& s);
    friend istream& operator>>(istream& in, Scrabble& s);
};
ostream& operator<<(ostream& out, const Scrabble& s);
istream& operator>>(istream& in, Scrabble& s);

// ------------------------------------------------------------
//  CHARACTER HIERARCHY
// ------------------------------------------------------------
class GameEngine;   // forward declaration (friend of Character)

// Abstract base class of every person in the game
class Character {
private:
    string characterName;
    string characterRole;
    string nationality;
    bool isCulprit;
    bool interviewed;
public:
    Character(const string& name, const string& role, const string& nation);
    virtual ~Character();
    virtual void display() const = 0;                  // pure virtual: every child overrides it
    virtual int getRequiredCredibility() const;        // credibility needed for an interview
    virtual Clue getClue() const;                      // letter + position this person gives
    virtual void setClue(char letter, int position, WordRow row);
    virtual bool isSuspectRole() const;                // true only for suspects

    string getName() const;
    string getRole() const;
    string getNationality() const;
    bool getIsCulprit() const;
    bool isInterviewed() const;
    void setName(const string& name);

    friend class GameEngine;   // the engine may touch private data directly
    friend ostream& operator<<(ostream& out, const Character& c);
    friend istream& operator>>(istream& in, Character& c);
protected:
    void showCard(int tint, const string& flavor) const;   // shared fancy display
};
ostream& operator<<(ostream& out, const Character& c);
istream& operator>>(istream& in, Character& c);

class Victim : public Character {
private:
    WordRow clueRowChar;
    char letterClueChar;
    int boardPosChar;
public:
    Victim(const string& name, const string& nation);
    virtual ~Victim();
    virtual void display() const;
    virtual int getRequiredCredibility() const;
    virtual Clue getClue() const;
    virtual void setClue(char letter, int position, WordRow row);
};

class Detective : public Character {
private:
    int credibility;
    int turns;
    TileRack rackWord1;
    TileRack rackWord2;
public:
    Detective(const string& name = "Agent", int startCredibility = 100);
    virtual ~Detective();
    virtual void display() const;
    void reset(const string& name, int startCredibility);
    int getCredibility() const;
    int getTurns() const;
    void loseCredibility(int amount);
    void useTurn();
    TileRack& getRack(WordRow row);
    const TileRack& getRack(WordRow row) const;
};

class Suspect : public Character {
protected:
    WordRow clueRowChar;
    char letterClueChar;
    int boardPosChar;
public:
    Suspect(const string& name, const string& role, const string& nation);
    virtual ~Suspect();
    virtual Clue getClue() const;
    virtual void setClue(char letter, int position, WordRow row);
    virtual bool isSuspectRole() const;
};

// virtual inheritance solves the diamond problem for PA
class BusinessRelations : virtual public Suspect {
public:
    BusinessRelations(const string& name, const string& role, const string& nation);
    virtual ~BusinessRelations();
};
class CloseRelations : virtual public Suspect {
public:
    CloseRelations(const string& name, const string& role, const string& nation);
    virtual ~CloseRelations();
};
class HouseHelp : public Suspect {
public:
    HouseHelp(const string& name, const string& role, const string& nation);
    virtual ~HouseHelp();
};

class Partner : public BusinessRelations {
public:
    Partner(const string& name, const string& role, const string& nation);
    virtual ~Partner();
    virtual void display() const;
    virtual int getRequiredCredibility() const;   // 50
};
class PA : public BusinessRelations, public CloseRelations {
public:
    PA(const string& name, const string& nation);
    virtual ~PA();
    virtual void display() const;
    virtual int getRequiredCredibility() const;   // 35
};
class Cook : public HouseHelp {
public:
    Cook(const string& name, const string& nation);
    virtual ~Cook();
    virtual void display() const;
};
class Guard : public HouseHelp {
public:
    Guard(const string& name, const string& nation);
    virtual ~Guard();
    virtual void display() const;
};
class Servant : public HouseHelp {
public:
    Servant(const string& name, const string& nation);
    virtual ~Servant();
    virtual void display() const;
};
class OwnersChild : public CloseRelations {
public:
    OwnersChild(const string& name, const string& nation);
    virtual ~OwnersChild();
    virtual void display() const;
    virtual int getRequiredCredibility() const;   // 75
};

// ------------------------------------------------------------
//  ScoreBoard: persistent leader/score board stored in a text file
// ------------------------------------------------------------
class ScoreBoard {
private:
    string fileName;
    static string todayDate();
public:
    ScoreBoard(const string& file);
    void addScore(const string& detectiveName, const string& result, int credibility, int turnsLeft, const string& culprit);
    void displayAll() const;
};

// ------------------------------------------------------------
//  GameEngine: Singleton that runs the whole game.
//  Composition : Detective, Cottage, Scrabble, TileRack
//  Association : ScoreBoard (used, not owned)
// ------------------------------------------------------------
class GameEngine {
private:
    static GameEngine* instance;
    static const int MAX_PEOPLE = 8;

    GameState state;
    Detective detective;      // composition
    Cottage cottage;          // composition
    Scrabble scrabble;        // composition
    TileRack clueRack;        // composition (merged clue rack)
    ScoreBoard* scoreBoard;   // association

    Character* people[MAX_PEOPLE];   // victim + 7 suspects (polymorphic)
    int peopleCount;

    int maxTurns, startCredibility, wrongTurnPenalty, wrongWordPenalty;
    string winCondition, loseCondition, culpritRole;

    GameEngine(ScoreBoard* board);
    GameEngine(const GameEngine&);              // copying is forbidden (singleton)
    GameEngine& operator=(const GameEngine&);

    // setup helpers
    void freePeople();
    Character* createCharacter(const string& name, const string& role, const string& nation);
    bool loadRules();
    void loadClueFile(int culpritIndex, string& role, string& word1, string& word2);
    void loadCharacters(int culpritIndex, const string& role);
    string readDetectiveName();
    void startNewGame();

    // menus and screens
    void menuStep();
    void playStep();
    void showMainMenu() const;
    void showGameMenu() const;
    void showHUD() const;
    void showAbout() const;
    void showRules();
    void showScoreBoardScreen() const;
    void showBoardScreen() const;
    void finishGame();

    // game actions
    void visitLocation();
    void interviewPerson();
    void formWord();
    void accuseSuspect();
    void quitGame();
    void spendTurn();
    void checkEnd();
    void flashLetter(const Clue& clue, const string& source);
    Character* findCulprit() const;
public:
    static GameEngine* getInstance(ScoreBoard* board = 0);
    static void destroyInstance();
    ~GameEngine();
    void showWelcome() const;
    void step();                 // runs one step of the state machine
    bool isRunning() const;
};

#endif
