#include<iostream>
// #include<windows.h> // Removed for Linux compatibility
#include <curl/curl.h> // Added for libcurl
#include <string> // Added for std::string
#include <sstream> // Added for stringstream (string to number)
#include <vector> // Added for std::vector (potentially for board representation)
#include <algorithm> // Added for std::remove_if
using namespace std;

// Placeholders for Gemini API
// IMPORTANT: REPLACE these placeholder values with your actual Gemini API endpoint and key.
const std::string GEMINI_API_ENDPOINT = "YOUR_GEMINI_API_ENDPOINT_HERE"; // Replace with your Gemini API endpoint
const std::string GEMINI_API_KEY = "YOUR_GEMINI_API_KEY_HERE";          // Replace with your Gemini API key

// Callback function for libcurl to write received data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
    } catch(std::bad_alloc &e) {
        //handle memory problem
        return 0;
    }
    return newLength;
}

void testHttpLibrary() {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://jsonplaceholder.typicode.com/todos/1");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        // Adding a user-agent header
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "User-Agent: TicTacToe-Client/1.0");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            cout << "HTTP Request Successful!" << endl;
            cout << "Response:" << endl;
            cout << readBuffer << endl;
        }
        curl_slist_free_all(headers); // free the header list
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

void draw(char board[][3] ) ;
void logic(char board[][3]);
int isgameover(char board[][3]);
int getAIMove(char board[][3]); // Declaration of getAIMove

bool isvalid=true;
int player=1;
int g_moveCount=0; // Renamed from 'count' to avoid ambiguity
int gameMode; // Made gameMode global
// HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE); // Removed for Linux compatibility

int main(){
    char board[3][3]={{'1' , '2' , '3'} , {'4' , '5' , '6'} , {'7' ,'8' , '9'}};
    // int gameMode; // Removed local declaration

    // cout << "Choose game mode:" << endl;
    // cout << "1. Player vs Player" << endl;
    // cout << "2. Player vs AI" << endl;
    // cout << "Enter your choice (1 or 2): ";
    // cin >> gameMode;

    // gameMode = 2; // Directly set to AI mode for testing getAIMove
    // cout << "Temporarily set gameMode = 2 for testing AI." << endl;

    // Restore original game mode selection
    cout << "Choose game mode:" << endl;
    cout << "1. Player vs Player" << endl;
    cout << "2. Player vs AI" << endl;
    cout << "Enter your choice (1 or 2): ";
    cin >> gameMode;

    while (cin.fail() || (gameMode != 1 && gameMode != 2)) {
        cout << "Invalid input. Please enter 1 for Player vs Player or 2 for Player vs AI: ";
        cin.clear();
        cin.ignore(10000, '\n');
        cin >> gameMode;
    }


    if (gameMode == 1) {
        cout << "You chose Player vs Player mode." << endl;
    } else if (gameMode == 2) {
        cout << "You chose Player vs AI mode." << endl;
    } else {
        cout << "Invalid choice. Exiting." << endl;
        return 1; // Indicate an error
    }
    
    // Restore the main game loop
    draw(board);
    while(isgameover(board) == 0) {
        logic(board);
        draw(board);
    }
    
    // testHttpLibrary(); // Call the test function - move this to where AI logic will be if needed

    // --- Temporary test for getAIMove ---
    // Removed the temporary test block for getAIMove
    // if (gameMode == 2) { 
    //     cout << "Attempting to get AI move (testing getAIMove)..." << endl;
    //     char testBoard[3][3]={{'1', 'O', 'X'}, {'X', '5', 'O'}, {'7', '8', 'X'}};
    //     cout << "Test board state:" << endl;
    //     draw(testBoard);
    //     int aiMove = getAIMove(testBoard);
    //     if (aiMove != -1) {
    //         cout << "AI suggested move: " << aiMove << endl;
    //     } else {
    //         cout << "AI move function returned an error or invalid move." << endl;
    //     }
    // // --- End of temporary test --- 


    // system("pause"); // Removed for Linux compatibility
    cout << "Press Enter to exit..." << endl; // Alternative to system("pause")
    cin.ignore(); // Wait for Enter key
    cin.get();    // Wait for Enter key
    return 0; // Added return 0 for main
} // Correct closing brace for main


// Function to get AI's move using Gemini API
int getAIMove(char board[][3]) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    std::string jsonPayload;

    // 1. Construct the prompt for Gemini API
    jsonPayload = "{\n";
    jsonPayload += "  \"contents\": [\n";
    jsonPayload += "    {\n";
    jsonPayload += "      \"parts\": [\n";
    jsonPayload += "        {\n";
    jsonPayload += "          \"text\": \"You are a Tic-Tac-Toe player. The current board is:\\n";
    
    std::string boardStr = "";
    std::vector<int> emptyCells;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardStr += board[i][j];
            if (j < 2) boardStr += " | ";
            if (board[i][j] != 'X' && board[i][j] != 'O') {
                emptyCells.push_back((i * 3 + j) + 1);
            }
        }
        if (i < 2) boardStr += "\\n--|---|--\\n"; else boardStr += "\\n";
    }
    jsonPayload += boardStr;
    jsonPayload += "Player is 'O'. Choose a number for an empty cell from the list: [";
    for(size_t i = 0; i < emptyCells.size(); ++i) {
        jsonPayload += std::to_string(emptyCells[i]);
        if (i < emptyCells.size() - 1) jsonPayload += ", ";
    }
    jsonPayload += "]. Return only the number of the cell you choose. For example, if you choose cell 5, return {\\\"move\\\": 5}.\"\n";
    jsonPayload += "        }\n";
    jsonPayload += "      ]\n";
    jsonPayload += "    }\n";
    jsonPayload += "  ]\n";
    jsonPayload += "}\n";

    cout << "DEBUG: JSON Payload for AI:" << endl << jsonPayload << endl;

    int aiMove = -1; // Initialize aiMove to -1 (error/no move)

    // 2. Make HTTP POST request (or simulate if placeholders are used)
    if (GEMINI_API_ENDPOINT == "YOUR_GEMINI_API_ENDPOINT_HERE" || GEMINI_API_KEY == "YOUR_GEMINI_API_KEY_HERE") {
        cout << "INFO: Using placeholder API endpoint/key. Simulating API response." << endl;
        // Simulate various responses for testing error handling:
        // readBuffer = "{\"move\":5}";           // Valid, no space
        // readBuffer = "{\"move\": 5}";          // Valid, with space
        // readBuffer = "{\"move\":10}";         // Invalid (out of range)
        // readBuffer = "{\"foo\":\"bar\"}";       // Malformed (unexpected JSON)
        // readBuffer = "{\"move\": }";          // Malformed (empty value)
        // readBuffer = "";                     // Empty response
        readBuffer = "{\"move\":5}"; // Default simulation
        cout << "DEBUG: Using simulated API response: " << readBuffer << endl;
        res = CURLE_OK; // Simulate success for API call itself
    } else {
        cout << "INFO: Attempting actual API call to: " << GEMINI_API_ENDPOINT << endl;
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if(curl) {
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            std::string apiKeyHeader = "x-goog-api-key: " + GEMINI_API_KEY;
            headers = curl_slist_append(headers, apiKeyHeader.c_str());

            curl_easy_setopt(curl, CURLOPT_URL, GEMINI_API_ENDPOINT.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "TicTacToe-Client/1.0");
            
            res = curl_easy_perform(curl);
            
            if(res != CURLE_OK) {
                cerr << "ERROR: curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
                curl_slist_free_all(headers);
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                cout << "INFO: API call failed. Using fallback move." << endl;
                return getFallbackMove(board, emptyCells);
            }
            cout << "DEBUG: Raw API Response:" << endl << readBuffer << endl;
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        } else {
            cerr << "ERROR: curl_easy_init() failed." << endl;
            curl_global_cleanup(); // Potentially redundant if curl_easy_init failed, but safe.
            cout << "INFO: Curl initialization failed. Using fallback move." << endl;
            return getFallbackMove(board, emptyCells);
        }
        curl_global_cleanup();
    }

    // 3. Process API Response (real or simulated)
    if (readBuffer.empty() && res == CURLE_OK) { // res == CURLE_OK implies it's not an API call failure handled above
        cerr << "ERROR: API response buffer is empty." << endl;
        cout << "INFO: Empty API response. Using fallback move." << endl;
        return getFallbackMove(board, emptyCells);
    }

    // --- Start of Parsing Logic ---
    std::string parsedValueStr; 

    // Attempt 1: Simplified parsing for "{\"move\": N}"
    size_t simpleMovePos = readBuffer.find("\"move\":");
    if (simpleMovePos != std::string::npos) {
        size_t valueStartPos = readBuffer.find(":", simpleMovePos) + 1;
        size_t valueEndPos = readBuffer.find_first_of("}, \n\r", valueStartPos);
        if (valueEndPos != std::string::npos) {
            parsedValueStr = readBuffer.substr(valueStartPos, valueEndPos - valueStartPos);
        } else {
            parsedValueStr = readBuffer.substr(valueStartPos); 
        }
    }

    // Attempt 2: If simple parsing failed to find a value, try complex Gemini structure
    if (parsedValueStr.empty()) { // Only try complex if simple parse didn't extract anything
        size_t complexMovePos = readBuffer.find("\"text\": \"{\\\"move\\\":");
        if (complexMovePos != std::string::npos) {
            size_t complexStart = complexMovePos + std::string("\"text\": \"{\\\"move\\\":").length();
            size_t complexEnd = readBuffer.find("}\"}", complexStart); 
            if (complexEnd != std::string::npos) {
                parsedValueStr = readBuffer.substr(complexStart, complexEnd - complexStart);
            } else {
                 parsedValueStr = readBuffer.substr(complexStart);
            }
        }
    }
    
    // Trim and attempt to convert parsedValueStr if it's not empty
    if (!parsedValueStr.empty()) {
        parsedValueStr.erase(std::remove_if(parsedValueStr.begin(), parsedValueStr.end(), ::isspace), parsedValueStr.end());
        if (!parsedValueStr.empty()) {
            bool is_digits = true;
            for(char c : parsedValueStr) { if(!isdigit(c)) { is_digits = false; break; } }
            if(is_digits){
                try {
                    aiMove = std::stoi(parsedValueStr);
                    cout << "DEBUG: Successfully parsed move from API response: " << aiMove << endl;
                } catch (const std::invalid_argument& ia) {
                    cerr << "ERROR: Invalid argument for stoi (API parse): '" << parsedValueStr << "'" << endl;
                } catch (const std::out_of_range& oor) {
                    cerr << "ERROR: Out of range for stoi (API parse): '" << parsedValueStr << "'" << endl;
                }
            } else {
                 cerr << "ERROR: Extracted API move value '" << parsedValueStr << "' is not purely digits." << endl;
            }
        } else {
            cerr << "ERROR: Extracted API move value is empty after trimming." << endl;
        }
    }
    // --- End of Parsing Logic ---

    // If parsing failed or didn't find anything, aiMove will still be -1
    if (aiMove == -1) {
        cerr << "ERROR: Failed to parse a valid move from API response. Snippet: '";
        cerr << readBuffer.substr(0, 50) << (readBuffer.length() > 50 ? "..." : "") << "'" << endl;
        cout << "INFO: API parsing failed. Using fallback move." << endl;
        return getFallbackMove(board, emptyCells);
    }

    // 4. Validate AI's Move (already parsed, now check range and availability)
    if (aiMove < 1 || aiMove > 9) {
        cerr << "ERROR: API suggested move " << aiMove << " is out of range (1-9)." << endl;
        cout << "INFO: API move out of range. Using fallback move." << endl;
        return getFallbackMove(board, emptyCells);
    }

    int row = (aiMove - 1) / 3;
    int col = (aiMove - 1) % 3;

    if (board[row][col] == 'X' || board[row][col] == 'O') {
        cerr << "ERROR: API chose cell " << aiMove << " which is already taken by " << board[row][col] << "." << endl;
        cout << "INFO: API move for taken cell. Using fallback move." << endl;
        return getFallbackMove(board, emptyCells);
    }

    // 5. Return the valid, parsed, and validated move from API
    cout << "INFO: AI successfully provided move: " << aiMove << endl;
    return aiMove;
}

// Helper function to get a fallback move (first available empty cell)
int getFallbackMove(char board[][3], const std::vector<int>& emptyCells) {
    if (!emptyCells.empty()) {
        int fallbackMove = emptyCells[0]; // Choose the first available empty cell
        cout << "DEBUG: Using fallback move: " << fallbackMove << " (first available empty cell)." << endl;
        return fallbackMove;
    } else {
        cout << "DEBUG: Fallback failed: No empty cells available." << endl;
        return -1; // Should not happen if game isn't over
    }
}


void draw(char board[][3]){
    for(int i=0 ; i<3 ; i++){
        for(int j=0 ; j<3 ; j++){
            // if(board[i][j]=='X')
            // SetConsoleTextAttribute(h , 13); // Removed for Linux compatibility
            // else if(board[i][j]=='O')
            // SetConsoleTextAttribute(h,4); // Removed for Linux compatibility

            cout<<board[i][j]<<" ";
            // SetConsoleTextAttribute(h , 15); // Removed for Linux compatibility
            if(j!=2){
                cout<<"| ";
            }
        }
        cout<<endl;
        if(i!=2)
        cout<<"----------";
        cout<<endl;
    }
}


int isgameover(char board[][3]){
    bool draw=true;
    //Check all 3 rows
    for(int i=0 ; i<3 ; i++){
            if(board[i][0] == board[i][1] && board[i][1]==board[i][2] ){
                return 1;
            }
    }
    //check all 3 columns
    for(int i= 0 ; i<3 ; i++){
            if(board[0][i] == board[1][i] && board[1][i]==board[2][i] )
            {
                return 1;
        }
        }
    //check dioganals
    for(int i=0 ; i<1 ; i++){
        for(int j=0 ; j<1 ; j++){
            if(board[i][j] == board[i+1][j+1] && board[i+1][j+1]==board[i+2][j+2]){
                return 1;
            }
            if(board[i][j+2]==board[i+1][j+1] && board[i+1][j+1]==board[i+2][j]){
                return 1;
            }
        }
    }
   if(g_moveCount==9) // Updated to g_moveCount
    return -1;
    return 0;

}



void logic(char board[][3] ){
   char mark;
     int choice;
     int row,col;   
     mark = (player == 1) ? 'X' : 'O'; // AI is always 'O' (Player 2)
    isvalid = true; // Assume valid move at the start of a turn's logic

    cout << "DEBUG: Top of logic(). Player: " << player << ", GameMode: " << gameMode << endl;

    if (player == 1 || (player == 2 && gameMode == 1)) { // Human player's turn (Player 1 or Player 2 in PvP)
        cout << "Player " << player << " (" << mark << "), choose the box (1-9): ";
        cin >> choice;
        // system("cls"); // Removed for Linux compatibility

        if (cin.fail() || choice < 1 || choice > 9) {
            cout << "Invalid input. Please enter a number between 1 and 9." << endl;
            isvalid = false;
            cin.clear(); // Clear error flags
            cin.ignore(10000, '\n'); // Discard invalid input
            // return; // Skip the rest of the turn processing - LET THE END LOGIC HANDLE isvalid=false
        }

        row = (choice - 1) / 3;
        col = (choice - 1) % 3;

        if (board[row][col] != 'X' && board[row][col] != 'O') {
            board[row][col] = mark;
            isvalid = true;
            g_moveCount++;
        } else {
            cout << "Error: Box " << choice << " is already marked with " << board[row][col] << endl;
            isvalid = false;
        }
    } else { // AI's turn (Player 2 in PvAI mode)
        cout << "AI (Player 2 as 'O') is thinking..." << endl;
        choice = getAIMove(board); // getAIMove returns 1-9 or -1

        if (choice != -1) { // Valid move from AI
            cout << "AI chose box: " << choice << endl;
            row = (choice - 1) / 3;
            col = (choice - 1) % 3;
            
            // Double check, though getAIMove should handle this
            if (board[row][col] != 'X' && board[row][col] != 'O') { 
                board[row][col] = mark; // AI is 'O'
                isvalid = true;
                g_moveCount++;
            } else {
                 cout << "ERROR: AI chose an already marked cell (" << choice << ")! This shouldn't happen." << endl;
                 isvalid = false; // AI made an invalid move
            }
        } else {
            cout << "AI failed to make a move. Game might be stuck or AI logic error." << endl;
            isvalid = false; // AI failed, turn should not switch or game ends
        }
    }

    // Check game status and switch player
    cout << "DEBUG: Before game over check. Player: " << player << ", mark: " << mark << ", isvalid: " << isvalid << endl;
    int gameStatus = isgameover(board);
    if (gameStatus == 1) {
        // Player who just made the move (current 'player') wins
        cout << "Congrats Player " << player << " (" << mark << ") won!" << endl;
    } else if (gameStatus == -1) {
        cout << "Game Draw!" << endl;
    } else { // Game is not over
        if (isvalid) { // If the move was valid, switch player
            cout << "DEBUG: Valid move, switching player from " << player;
            player = (player == 1) ? 2 : 1;
            cout << " to " << player << endl;
        } else {
            // If move was invalid, current player (human or AI) needs to try again.
            cout << "DEBUG: Invalid move. Player " << player << " tries again." << endl;
        }
    }
    cout << "DEBUG: End of logic(). Player is now: " << player << endl;
}