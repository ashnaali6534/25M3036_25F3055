// ============================================================
//  main.cpp  -  THE LOCKER HEIST (CL1004 OOP Semester Project)
//  Driver file: creates the GameEngine (Singleton) and runs it.
//  Every GameException thrown anywhere in the game is caught here.
// ============================================================
#include "heist.h"

int main() {
    srand((unsigned int)time(NULL));          // seed for the random culprit
    ConsoleUI::init();                        // box drawing code page

    ScoreBoard board("leader/scoreBoard.txt");               // persistent score file
    GameEngine* engine = GameEngine::getInstance(&board);    // the one and only engine

    try {
        engine->showWelcome();                // welcome page, then the main menu opens
    } catch (GameException& e) {
        e.show();
    }

    // The engine keeps its own state, so after an error the game simply continues
    while (engine->isRunning()) {
        try {
            engine->step();
        } catch (GameException& e) {          // all custom HEIST errors end up here
            e.show();
            ConsoleUI::pause();
        } catch (...) {                       // anything unexpected: stop safely
            ConsoleUI::setColor(TINT_LRED);
            cout << "\n  HEIST ERROR: Unexpected problem. The game will close." << endl;
            ConsoleUI::resetColor();
            break;
        }
    }

    ConsoleUI::clearScreen();
    cout << endl;
    ConsoleUI::boxTop(60, TINT_GOLD);
    ConsoleUI::boxLine("Thanks for playing THE LOCKER HEIST!", 60, TINT_GOLD, TINT_BWHITE);
    ConsoleUI::boxLine("Goodbye, Detective.", 60, TINT_GOLD, TINT_LCYAN);
    ConsoleUI::boxBottom(60, TINT_GOLD);
    cout << endl;

    GameEngine::destroyInstance();            // frees all heap memory
    return 0;
}
