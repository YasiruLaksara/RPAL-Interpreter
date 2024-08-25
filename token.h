#ifndef TOKEN_H_
#define TOKEN_H_

#include <iostream>

using namespace std;

// Class representing a token in a lexer
class token {

private:

    string type; // Type of the token
    string val;  // Value of the token


public:
    // Set the type of the token
    void setType(const string &tokenType);
    
    // Set the value of the token
    void setValue(const string &tokenValue);
    
    // Get the type of the token
    string getType();
    
    // Get the value of the token
    string getValue();
    
    // Overloaded inequality operator for comparing tokens
    bool operator!=(token t);
};


// Implementation of the methods


// Set the type of the token
void token::setType(const string &tokenType) {
    type = tokenType;
}


// Set the value of the token
void token::setValue(const string &tokenValue) {
    val = tokenValue;
}


// Get the type of the token
string token::getType() {
    return type;
}


// Get the value of the token
string token::getValue() {
    return val;
}


#endif 
