/**
** Program Name: HTML Validator
** File Name: htmlValidator.cpp
** Purpose: Validates a given HTML file for valid syntax (in the provided grammar)
** Author: Hugo Carlos Borges Pinto
** Date: June 15, 2019
*/
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
using namespace std;

// Documentation on implementation
void getFileName();
void validate();
void tokenizeHTML(string s, vector<string> &tokens);
bool validateHTML(vector<string> &tokens);
void displayErrorMessage(string tag, string errorMsg);

// Auxiliary functions
bool validateTag(string tag);
bool compareTags(string tagOpen, string tagClose);
bool isTagName(string s);
bool isTagOpen(string s);
bool isTagEmpty(string s);
bool isTagClose(string s);
bool isCharData(string s);
bool isBlank(const string &);
bool isDigit(char s);
bool isAlphabet(char s);
bool isWordSeparator(char s);

// Stores filename as a global variable throughout the lifecycle of the
// program to allow correct error messages displayed without having
// to pass the filename through all functions just in case the function
// finds and error. Without it filename would have to be passed to all
// functions that might find an error but would not be used unless an
// error is actually found
string filename;

/**
** The main() function which asks the user for a filename and
** validates that filename
*/
int main() {
  cout << boolalpha;  // Displays true/false instead of 1/0

  // Get the filename and validate
  getFileName();
  validate();

  return 0;
}

/**
 * Gets a filename from the user
 * @params none
 * @return none
 *
 * precondition: none
 * postcondition: the global variable filename will be set with the provided
 * filename
 */
void getFileName() {
  cout << "=============== HTML Validator ===============\n";

  cout << "Provided test files:"
       << endl;
  cout << "a.html (valid)\n";
  cout << "b.html (valid)\n";
  cout << "c.html (valid)\n";
  cout << "d.html (invalid - orphan closing tag)\n";
  cout << "e.html (invalid - wrong closing tags order)\n";
  cout << "f.html (invalid - tag not in grammar)\n";
  cout << "g.html (invalid - orphan closing tag)\n";
  cout << "h.html (invalid - orphan opening tag)\n";
  cout << "i.html (invalid - empty file)\n";
  cout << "z.html (invalid - non-existent file)\n";

  cout << "\nEnter the name of an html file in the current directory to validate: ";
  cin >> filename;
}

/**
 * Validates a file for proper HTML syntax (given the provided grammar)
 * @params none
 * @return none
 *
 * precondition: the filename must be set by the given function
 * postcondition: program will display success message and exit with success if file is
 * valid HTML, exit with failure filename is invalid or display error message and exit
 * with failure if file is invalid
 */
void validate() {
  cout << "Validating " << filename << "..." << endl;

  // Open the given file
  ifstream file{filename, ios::in};

  // and check if it is a valid file
  if (!file.is_open()) {
    // Display error message and exit with failure code if file can't be read
    cout << "ERROR: Invalid file name. \nCheck the if the filename is correct and the "
            "file is in the current directory"
         << endl;
    exit(-1); // Required exit by instructions
  }

  // Empty file cheking code from
  // https://stackoverflow.com/questions/26228424/how-to-detect-an-empty-file-in-c/26228684
  if (file.peek() == std::ifstream::traits_type::eof()) {
    cout << "ERROR: The file is empty and it is not valid" << endl;
    exit(-1);
  }

  // If file is valid, start reading its contents
  stringstream contents;  // stream of the file contents
  string line;            // string with the current line

  // Add each line to the contents stream
  while (getline(file, line, '\n')) contents << line;

  // Close file since contents are already in memory
  file.close();

  // Start vector for tokens extraction
  vector<string> tokens;

  // Tokenize the file contents
  tokenizeHTML(contents.str(), tokens);  //.str() converts sstream into string

  // Display success message if tokens are valid HTML
  if (validateHTML(tokens))
    cout << filename << " is a valid HTML file according to the given grammar." << endl;

  exit(0);
}

/**
 * Transforms a string into tokens based on HTML syntax for validation
 * @params string, the string to tokenize
 * @params vector<string>, a vector of strings where to store the extacted tokens
 * @return none
 *
 * precondition: the string and token vector must be initialized by the calling function
 * postcondition: the contents of the string will be split into tokens and stored in the
 * vector
 */
void tokenizeHTML(string s, vector<string> &tokens) {
  unsigned int i = 0;           // cursor to read the string
  unsigned int tokenCount = 0;  // tracks the position of the current partial token

  // Read the whole string
  while (i < s.length()) {
    // If > is found, that means the preceding content is a possible HTML tag
    // so extract it as it is using the the tokenCount partial as measure
    if (s[i] == '>') {
      // Extract the current token
      string token = s.substr(i - tokenCount, tokenCount + 1);

      // and add it to the tokens vector
      tokens.push_back(s.substr(i - tokenCount, tokenCount + 1));

      // Restart the count for partial tokens since it is going to start
      // looking for a new token after extraction
      tokenCount = 0;

      // If an < is found anywhere except the start of the string, the previous
      // content is another token, either normal content or another tag, so extract
      // it
    } else if (s[i] == '<' && i > 0) {
      // Extract the current token
      string token = s.substr(i - tokenCount + 1, tokenCount - 1);

      // Ignore tokens that consisting of just spaces or word separators
      if (!isBlank(token) && (token.size() > 1 || !isWordSeparator(token[0]))) {
        // Add it to the token vector if it is non-empty content
        tokens.push_back(token);
      }

      // Restart the count for partial tokens since it is going to start
      // looking for a new token after extraction of the current one
      tokenCount = 0;
    }

    i++;           // Increment string reader cursor position
    tokenCount++;  // Increment current token lenght beacause it is either looking for a
                   // new token or continues checking for the end of the current one
  }

  // When loop is done the whole string is tokenized on the tokens vector
}

/**
 * Validates a group of tokens for proper HTML syntax (given the provided grammar)
 * @params vector<string>, a vector of strings of tokens to validate
 * @return bool, true if tokens are valid HTML, false if not
 *
 * precondition: the tokens vector must be initialized by the calling function
 * postcondition: if tokens are invalid HTML, program will display error message and exit
 * with failure code
 */
bool validateHTML(vector<string> &tokens) {
  // Start stack to keep track of tags
  stack<string> tags;

  // Loop through the tokens
  for (unsigned int i = 0; i < tokens.size(); i++) {
    // Get current token to analyze
    string currentToken = tokens.at(i);

    // If the current token is a valid opening tag, add it to the stack
    if (isTagOpen(currentToken)) {
      tags.push(currentToken);

      // Otherwise, check if the current token is a closing tag
    } else if (isTagClose(currentToken)) {
      // If there are not opening tags to match on the stack, the closing tag
      // is orphan and the file is not a valid HTML file
      if (tags.empty()) displayErrorMessage(currentToken, "no open tags left to close");

      // Otherwise if tags is not empty, get the latest opened tag from the stack
      string tagOpen = tags.top();
      tags.pop();

      // and compare if the opening and closing tags match
      if (!compareTags(tagOpen, currentToken))
        // If they don't, display error message and exit since file it not valid HTML
        displayErrorMessage(currentToken,
                            "no matching opening tag. Should close " + tagOpen);

      // If they match, keep looking through the tokens
    }
  }

  // If all tokens are matched, file is valid HTML
  if (tags.size() == 0) return true;

  // Otherwise there are orphan opening tags, so file is not valid
  displayErrorMessage(tags.top(), "no matching closing tag.");
  return false;
}

/**
 * Displays error message with information about why the file is not invalid HTML syntax
 * @params string, the tag that invalidates the file according to the HTML grammar
 * @params string, the descriptive error message explaning the invalid token
 * @return none
 *
 * precondition: none
 * postcondition: the program will exit with a failure error code
 */
void displayErrorMessage(string problematicToken, string errorMsg) {
  // Open the given file
  ifstream file{filename, ios::in};

  // and check if it is a valid file
  // Being extra careful since validate() already checked it is a valid file, but
  // if the file is deleted during analysis it may crash the program
  if (!file.is_open()) {
    // Display error message and exit with failure code if file can't be read
    cout << "ERROR: Invalid file name. \nCheck the if the filename is correct and the "
            "file is in the current directory"
         << endl;
    exit(-1);
  }

  // Track line numbers
  string line;
  int lineCount = 0;

  cout << endl;  // Some space for the report

  // Go through the file to find the line with the error
  while (getline(file, line, '\n')) {
    lineCount++;  // Count the current line

    // print the line content with the line number as prefix
    cout << lineCount << ": " << line << endl;

    // Track the position of the problematic token to see where it is found
    // in the file
    unsigned int problematicTokenIndex = 0;

    // OPTIONAL: Display indicator under the line where the problem was found
    int errorPosition = 0;

    // Starts reading the current line to see if problematic token is present
    for (unsigned int i = 0; i < line.length(); i++) {
      // If first character match, checks if the rest matches
      if (line[i] == problematicToken[problematicTokenIndex]) {
        problematicTokenIndex++;  // Advances to the next character on problematic token
        if (errorPosition == 0) errorPosition = i + 1;  // marks token beginning
      }

      // If the problematic token was found in the line
      if (problematicTokenIndex >= problematicToken.length()) {
        file.close();  // Stop reading the file

        // OPTIONAL: Prepare spaces and indicator for printing
        string errorIndicator(errorPosition, ' ');  // Adds spaces before the indicator
        string problematicTokenIndicator(problematicToken.length(), '^');  // Marks token
        // OPTIONAL: Prints problem indicator under the line
        cout << "  " << errorIndicator << problematicTokenIndicator << endl;

        // Display message with the line number of the error, as required
        cout << "[ERROR] Line " << lineCount << ": " << problematicToken << " "
             << errorMsg << endl;

        // And exit the program
        exit(-1);
      }
    }
  }

  // Close the file if token was not found. Only here if file is modified
  // during analysis, as sanity check
  file.close();
}

/**
 * Checks if a token is a valid tag name according to the grammar
 * @params string, the token to be checked
 * @return bool, true if it is a valid tag name, false if not
 *
 * precondition: none
 * postcondition: none
 */
bool isTagName(string s) {
  // If there is only one character left in the beginning
  // it must be an alphabet character according to the grammar
  if (s.length() == 1) {
    if (isAlphabet(s[0]))
      return true;
    else
      return false;
  } else {
    // If there are more than one characters, it can be either an alphabet character
    // or a digit to be valid
    int last = s.length() - 1;
    char c = s[last];

    // If the current charcater is valid, check the rest of the string without it
    if (isAlphabet(c) || isDigit(c))
      return isTagName(s.substr(0, last));
    else
      return false;  // If not aphabet or digit, it is not valid name
  }
}

/**
 * Checks if a token is a valid tag opening HTML tag
 * @params string, the token to be checked
 * @return bool, true if it is a valid opening tag, false if not
 *
 * precondition: none
 * postcondition: none
 */
bool isTagOpen(string s) {
  if (s.empty()) return false;

  // A valid opening tag token must start with < and end with > and can't have
  // / as its second character or second to last, since those are closing and empty tags,
  // respectively
  if (s[0] != '<' || s[s.length() - 1] != '>' || s[s.length() - 2] == '/' || s[1] == '/')
    return false;

  // If the first conditions are met, remove the < and > to check if the tag name
  // is valid or not
  int start = 1, end = s.length() - 1;  // lenght - 1 for last char - 1 for '>'

  // Trim leading and trailing whitespaces since they are allowed by the grammar
  // but shouldn't be considered for checking a valid tag name
  while (isWordSeparator(s[start])) start++;
  while (isWordSeparator(s[end])) end--;

  // Prepare the trimmed tag name for checking
  string trimmedTagName = s.substr(start, end - start);

  // Checks if it is both a tag in the subset defined by the grammar and
  // if the tag name has valid characters
  return (validateTag(trimmedTagName) && isTagName(trimmedTagName));
}

/**
 * Checks if a token is a valid empty HTML tag
 * @params string, the token to be checked
 * @return bool, true if it is a valid empty tag, false if not
 *
 * precondition: none
 * postcondition: none
 */
bool isTagEmpty(string s) {
  if (s.empty()) return false;

  // A valid empty tag token must start with < and end with />
  if (s[0] != '<' || s[s.length() - 2] != '/' || s[s.length() - 1] != '>') return false;

  // If the first conditions are met, remove the < and /> to check if the tag name
  // is valid or not
  int start = 1, end = s.length() - 3;  // lenght - 1 for last char - 2 for '/>'

  // Trim leading and trailing spaces since they are allowed by the grammar
  // but shouldn't be considered for checking a valid tag name
  while (isWordSeparator(s[start])) start++;
  while (isWordSeparator(s[end])) end--;

  // Prepare the trimmed tag name for checking
  string trimmedTagName = s.substr(start, end - start + 1);

  // Checks if it is both a tag in the subset defined by the grammar and
  // if the tag name has valid characters
  return (validateTag(trimmedTagName) && isTagName(trimmedTagName));
}

/**
 * Checks if a token is a valid closing HTML tag
 * @params string, the token to be checked
 * @return bool, true if it is a valid closing tag, false if not
 *
 * precondition: none
 * postcondition: none
 */
bool isTagClose(string s) {
  // A valid closing tag token must start with < and end with /> and can't have
  // / as its second to last character, since those are for empty tags
  if (s[0] != '<' || s[1] != '/' || s[s.length() - 1] != '>' || s[s.length() - 2] == '/')
    return false;

  // If the first conditions are met, remove the < and /> to check if the tag name
  // is valid or not
  int start = 2;             // +2 for </
  int end = s.length() - 2;  // length - 1 for last and char - 1 for '>'

  // Trim leading and trailing spaces since they are allowed by the grammar
  // but shouldn't be considered for checking a valid tag name
  while (isWordSeparator(s[start])) start++;
  while (isWordSeparator(s[end])) end--;

  // Check if it is a valid tag name
  return isTagName(s.substr(start, end - start));
}

/**
 * Checks if a string is valid charData according to the gramar
 * @params string, the token to be checked
 * @return bool, true if it is a valid char data, false if not
 *
 * precondition: none
 * postcondition: none
 */
bool isCharData(string s) {
  // Empty string is valid charData in the grammar
  if (s.empty()) return true;

  // Anything except < is not valid char data
  for (unsigned int i = 0; i < s.length(); i++) {
    if (s[0] == '<') return false;
  }

  return true;
}

/**
 * Compares an opening and closing HTML tag to check if they match
 * @params string, the opening tag
 * @params string, the closing tag
 * @return bool, true if the tags match, false if not
 *
 * precondition: none
 * postcondition: none
 */
bool compareTags(string tagOpen, string tagClose) {
  // Extract the extra <, / and > from both tags
  string tagOpenName = tagOpen.substr(1, tagOpen.length() - 2);
  string tagCloseName = tagClose.substr(2, tagClose.length() - 3);

  // Compare the tag names
  return (tagOpenName.compare(tagCloseName) == 0 ? true : false);
}

/**
 * Checks if a tag name is included in the subset of the provided HTML grammar
 * @params string, the tag name to be checked
 * @return bool, true if the tag name is in the grammar, false if not
 *
 * precondition: none
 * postcondition: none
 */
bool validateTag(string tag) {
  // Initialize the set of allowed tag names by the given grammar
  const int TAGS_SIZE = 10;
  string tags[]{"html", "head", "body", "p", "br", "li", "h1", "h2", "ul", "ol"};

  // Check the tag against each element in the set
  for (int i = 0; i < TAGS_SIZE; i++)
    if (tag.compare(tags[i]) == 0) return true;

  // If not present, display proper error message and exit
  displayErrorMessage("<" + tag + ">", "is not a valid HTML tag (in the given grammar)");

  return false;
}

/**
 * Checks if a string contains only multiple blank spaces
 * @params string, the string to be checked
 * @return bool, true if the string has only blank spaces, false if not
 *
 * precondition: none
 * postcondition: none
 */
bool isBlank(const string &s) {
  // Checks if there is anything that is not a space in the string
  for (unsigned int i = 0; i < s.size(); i++) {
    if (s[i] != ' ') return false;
  }

  return true;
}

/**
 * Checks if a character is a digit
 * @params string, the character to be checked
 * @return bool, true if character is a digit, false if not
 *
 * precondition: none
 * postcondition: none
 */
bool isDigit(char s) { return isdigit(s); }

/**
 * Checks if a character is a letter of the alphabet
 * @params string, the character to be checked
 * @return bool, true if character is a letter, false if not
 *
 * precondition: none
 * postcondition: none
 */
bool isAlphabet(char s) { return isalpha(s); }

/**
 * Checks if a character is a word separator according to the provided grammar
 * @params string, the character to be checked
 * @return bool, true if character is a space, tab or newline, false if not
 *
 * precondition: none
 * postcondition: none
 */
bool isWordSeparator(char s) { return s == ' ' || s == '\t' || s == '\n'; }