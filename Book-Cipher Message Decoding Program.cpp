// Book-Cipher Message Decoding Program
/// @brief This program decodes and prints a book-cipher message.

// preprocessor directives
// library includes
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

// set namespace
using namespace std;


/// -------------------------------------------------------------------------
/// Function Prototype(s)
/// -------------------------------------------------------------------------
string getFileName(const string& prompt);
void getMessage(const string& fileName, string msg[], int& size, int maxSize);
void copyArray(const string a[], string b[], int size);
int extractPageNumber(const string codeStr);
void sortArray(string a[], int size);
int getPageLines(const string& FN, int pageNum, string a[], int MAX_LINES);
void setCharLocation(const string codeStr,
                     int& pageNum, int& lineNum, int& charPos);
string decodeMessage(const string &bookKeyFN, const string message[],
                     int size);

// used for debugging
void printPageLines(const string a[], int size);
void printSideBySide(const string a[], const string b[], int size);

/// -------------------------------------------------------------------------
/// Main entry-point for this application
///
/// @returns Exit-code for the process - 0 for success, else an error code.
/// -------------------------------------------------------------------------
int main() {
    // Local constant definition(s)
    const int MAX_MESSAGE_SIZE = 100;      // max chars in msg
    const string MSG_PROMPT = "Enter the file name of the coded message: ";
    const string KEY_PROMPT = "Enter the file name of the cipher text key: ";
    // Variable declarations(s)
    int exitState;               // tracks the exit code for the process
    exitState = EXIT_SUCCESS;    // set state to successful run

    string messageFN;            // pa08a-CodedMessage1.txt
    string bookKeyFN;            // Wizard_of_Oz.txt

    string message[MAX_MESSAGE_SIZE]{};         // host array for message
    string messageSorted[MAX_MESSAGE_SIZE]{};   // host array for sorted msg

    int messageSize = 0;         // actual size of message for virtual array

    string decodedMessage = "";  // holds decoded message

    /* ******* Input Phase *********************************************** */
    // pa08a-CodedMessage0.txt
    messageFN = getFileName(MSG_PROMPT);

    // Wizard_of_Oz.txt
    bookKeyFN = getFileName(KEY_PROMPT);

    cout << endl;


    /* ******* Processing Phase ****************************************** */
    // get the cipher message and sort the values so that the message can be
    // decoded
    getMessage(messageFN, message, messageSize, MAX_MESSAGE_SIZE);
    copyArray(message, messageSorted, messageSize);
    sortArray(messageSorted, messageSize);

    // printSideBySide(message, messageSorted, messageSize);

    // decode the message
    decodedMessage = decodeMessage(bookKeyFN, messageSorted, messageSize);

    /* ******* Output Phase ********************************************** */
    cout << decodedMessage << endl;
    cout << endl;
    return exitState;
}

/// -------------------------------------------------------------------------
/// Function Implementation(s)
/// -------------------------------------------------------------------------

/// This function gets a file name from the user
/// @param prompt message to user describing expected information
/// @return the name of the file
string getFileName(const string& prompt) {
    string fileName;        // holds the name of the file

    // prompt the user for and obtain the filename
    cout << prompt;
    cin >> fileName;

    return fileName;
}

/// This function reads the secrete message file and populates the msg array
/// @param fileName user supplied file name
/// @param [out] msg an array that will host the message lines
/// @param [out] size used to track the end of the virtual array
/// @param maxSize Maximum number of lines the host array can handle
void getMessage(const string& fileName, string msg[], int& size, int maxSize) {
    ifstream messageFin;    // file stream handler for reading the message
    string line;            // holds a single line of input
    int idx;                // array index used for populating arrays

    // attempt to open the message file
    messageFin.open(fileName.c_str());

    // populate the array if the file could be opened
    if (messageFin.is_open()) {
        idx = 0;
        while (getline(messageFin, line) && idx < maxSize) {
            msg[idx++] = line;
        }
        size = idx;          // save the number of elements used by msg
    } else {
        cout << "Unable to open message file: " << fileName << endl;
    }
    messageFin.close();
}


/// This function makes a copy of an array
/// @param a immutable array that will be copied
/// @param b target array receiving the data
/// @param size size of virtual array
void copyArray(const string a[], string b[], int size) {
    for (int i = 0; i < size; i++) {
        b[i] = a[i];
    }
}

/// This function extracts the page number from the cipher code
/// to help the selection sort
/// @param codeStr cipher code string
/// @return the page number from the code string.
int extractPageNumber(const string codeStr) {
    int pageNum;    // stores the extracted page number

    const size_t pos1 = codeStr.find('.');          // page number pointer
    pageNum = stoi(codeStr.substr(0, pos1));

    return pageNum;
}

/// This function performs a selection sort on the virtual portion of the array
/// @param a is the virtual array to be sorted
/// @param size is the end of the virtual array
void sortArray(string a[], int size) {
    int i;          // outer loop index
    int j;          // inner loop index
    int minIdx;     // index of min val
    int p;          // extracted page number from a[j]
    int pMin;       // extracted page number from a[minIdx]
    string temp;    // temporary string holder for swapping element contents

    // adjust boundary between sorted and unsorted portions
    for (i = 0; i < size - 1; i++) {
        // Find the smallest element in unsorted portion of the array
        // so that it can be swapped
        minIdx = i;
        for (j = i + 1; j < size; j++) {
            // extract the page number and compare the integer values instead
            // of the strings.
            p = extractPageNumber(a[j]);
            pMin = extractPageNumber(a[minIdx]);
            if (p < pMin) {
                //            if (a[j] < a[minIdx]) {
                minIdx = j;
            }
        }

        // Swap the smallest element with the first unsorted element
        temp = a[minIdx];
        a[minIdx] = a[i];
        a[i] = temp;
    }
}

/// This function retrieves the lines of text on a particular page and returns
/// the number of lines read from the page to help with the decoding process.
/// @param [in] FN The name of the book (cypher text) file
/// @param pageNum the page number to retrieve
/// @param [out] a the array that will receive the lines from the page
/// @return the number of lines read from the page (used for debugging)
int getPageLines(const string& FN, int pageNum, string a[], int MAX_LINES) {
    int pageCount = -3;             // set to -3 to skip front matter
    int lineCount = 0;              // current line in page
    bool done = false;              // LCV
    string line;                    // holds line read from file

    ifstream fin;                   // input file handler

    // open the cipher text
    fin.open(FN.c_str());

    // read the lines on the page
    if (fin.is_open()) {

        // ignore all pages and information up to the target page
        while (pageCount != pageNum && getline(fin, line)) {
            if (line.find("page") != std::string::npos) {
                pageCount++;
            }
        }

        // read the lines after the page number of the target page
        // while tracking track the line count
        while (getline(fin, line) && !done) {
            // "page" is no where in the document other than identifying page
            // numbers. So we can use the presence of "page" to indicate we are
            // done reading the page.
            if (line.find("page") != std::string::npos) {
                done = true;
            } else {
                // place into the array and increment the line count
                // if we didn't see "page" because we're not done
                a[lineCount++] = line;

                // make sure we don't exceed the capability of the array
                // holding the lines from the page
                if (lineCount > MAX_LINES) {
                    done = true;
                }
            }
        }
    } else {
        cout << "Unable to open: " << FN << endl;
    }
    // done with the file...
    fin.close();
    // return the number of lines that were read
    return lineCount;
}

/// This function extract the page number, line number and character position
/// values from the code string to help with the decoding process
/// @param codeStr code string containing character location information
/// @param [out] pageNum the extracted page number
/// @param [out] lineNum the extracted line number
/// @param [out] charPos the extracted character position
void setCharLocation(const string codeStr,
                     int& pageNum, int& lineNum, int& charPos) {

    // set delimiters
    const size_t pos1 = codeStr.find('.');              // 1st code delimiter
    const size_t pos2 = codeStr.find('.', pos1 + 1);    // 2nd code delimiter

    // extract and convert to integers
    pageNum = stoi(codeStr.substr(0, pos1));
    lineNum = stoi(codeStr.substr(pos1 + 1, pos2 - pos1 - 1));
    charPos = stoi(codeStr.substr(pos2 + 1));

    // normalize lineNum and charPos so that it is looking at correct array
    // positions
    if (lineNum > 0) {
        lineNum--;
    }
    if (charPos > 0) {
        charPos--;
    }

}

/// This function decodes a cipher character sequence
/// @pre each character code must be on its own line
/// @param bookKeyFN the name of the cipher text key
/// @param message an array of message codes
/// @param size the size of the message array
string decodeMessage(const string &bookKeyFN, const string message[],
                     int size) {
    const int MAX_PAGE_LINES = 25;      // max line on a page

    string lines[MAX_PAGE_LINES];       // holds the lines of the book's page

    int pageNum = 0;                    // page number were char is located
    int lineNum = 0;                    // line number were char is located
    int lineCount = 0;                  // num lines on page
    int charPos = 0;                    // char position in line

    string decodedMessage = "";         // holds the decided message

    // loop through each character of the message, extract the character
    // and append it to the message
    for (int i = 0; i < size; i++) {
        setCharLocation(message[i], pageNum, lineNum, charPos);

        // get the book page
        lineCount = getPageLines(bookKeyFN, pageNum, lines, MAX_PAGE_LINES);

        // decode if the page has lines otherwise message user.
        if (lineCount > 1) {
            decodedMessage += lines[lineNum].at(static_cast<unsigned>(charPos));
        } else {
            cout << "Line " << lineNum
            << " on page, " << pageNum
            << ", in the '" << bookKeyFN
            << "' cipher key text has no characters."
            << endl;
        }
    }

    return decodedMessage;
}


/// -------------------------------------------------------------------------
/// Function used for debugging code
/// -------------------------------------------------------------------------

/// This function prints the lines from the page we captured
/// (included for debugging program)
/// @param a the array holding the page lines
/// @param size the virtual size of the page
void printPageLines(const string a[], int size) {
    for (int i = 0; i < size; i++) {
        cout << "line " << i << ": " << a[i] << endl;
    }
    cout << endl;
}

/// This function prints two arrays side-by-side using a column width of 12
/// @param a the unsorted array
/// @param b the sorted array
/// @param size the virtual size of the two arrays
void printSideBySide(const string a[], const string b[], int size) {
    const int COLW = 12;                // set the column width for output
    for (int i = 0; i < size; i++) {
        cout << left << "    "
        << setw(COLW) << a[i]
        << setw(COLW) << b[i]
        << endl;
    }
}