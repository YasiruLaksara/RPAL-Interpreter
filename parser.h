#include <iostream>
#include <stack>
#include <iterator>
#include <string.h>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <math.h>
#include <stdio.h>
#include "token.h"
#include "tree.h"
#include "environment.h"

using namespace std;

stack<tree *> st; // Stack for syntax tree

// Array of operators
char operators[] = {'+', '-', '*', '<', '>', '&', '.', '@', '/', ':', '=', '~', '|', '$', '!', '#', '%',
                    '^', '_', '[', ']', '{', '}', '"', '`', '?'};

// Array of binary operators
string binaryOperators[] = {"+", "-", "*", "/", "**", "gr", "ge", "<", "<=", ">", ">=", "ls", "le", "eq", "ne", "&", "or", "><"};

// Array of keywords
string keywords[] = {"let", "fn", "in", "where", "aug", "or", "not", "true", "false", "nil", "dummy", "within",
                 "and", "rec", "gr", "ge", "ls", "le", "eq", "ne"};

class parser
{
public:
    token nextToken;     // Next token to be read
    char readline[10000]; // Stores program for parsing
    int index;           // Index of character
    int sizeOfFile;      // Size of file
    int astFlag;         // Flag to check if AST or ST is to be printed
    bool parsingComplete; // Flag to check if parsing is complete
    int cse_flag;    // Flag to determine whether to print the output value through CSE


    // Constructor
    parser(char read_array[], int row, int size, int af)
    {
        copy(read_array, read_array + 10000, readline);
        index = row;
        sizeOfFile = size;
        astFlag = af;
    }




    // Checks if the given string is a keyword
    bool isReservedKey(string str)
    {
        for (int row = 0; row < 20; row++)
        {
            if (str == keywords[row])
                return true;
        }
        return false;
    }

    // Checks if the given character is an operator
    bool isOperator(char ch)
    {
        for (int row = 0; row < 25; row++)
        {
            if (ch == operators[row])
            {
                return true;
            }
        }
        return false;
    }

    // Checks if the given character is an alphabet letter
    bool isAlpha(char ch)
    {
        if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
        {
            return true;
        }
        return false;
    }

    // Checks if the given character is a digit
    bool isDigit(char ch)
    {
        if (ch >= 48 && ch <= 57)
        {
            return true;
        }
        return false;
    }

    // Checks if the given string is a binary operator
    bool isBinaryOperator(string op)
    {
        for (int row = 0; row < 18; row++)
        {
            if (op == binaryOperators[row])
            {
                return true;
            }
        }
        return false;
    }

    // Checks if the given string is a number
    bool isNumber(const std::string &s)
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isdigit(*it))
            ++it;
        return !s.empty() && it == s.end();
    }

    // Read the next token
    void read(string val, string type)
    {
        if (val != nextToken.getValue() || type != nextToken.getType()) // Check if the next token is the expected token
        {
            cout << "Parse error: Expected " << "\"" << val << "\"" << ", but " << "\"" << nextToken.getValue() << "\"" << " was found" << endl;
            exit(0);
        }

        if (type == "ID" || type == "INT" || type == "STR") // If the token is an identifier, integer, or string
            buildTree(val, type, 0);

        nextToken = get_token(readline); // Get the next token

        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = get_token(readline);
    }

    // Build a tree for the given string, type, and number of children
    void buildTree(string val, string type, int child)
    {
        if (child == 0) // Leaf node
        {
            tree *temp = buildNode(val, type);
            st.push(temp);
        }
        else if (child > 0) // Non-leaf node
        {
            stack<tree *> temp;
            int no_of_pops = child;
            while (!st.empty() && no_of_pops > 0)
            {
                temp.push(st.top());
                st.pop();
                no_of_pops--;
            }
            tree *tempLeft = temp.top();
            temp.pop();
            child--;
            if (!temp.empty() && child > 0)
            {
                tree *rightNode = temp.top();
                tempLeft->right_node = rightNode;
                temp.pop();
                child--;
                while (!temp.empty() && child > 0)
                {
                    tree *addRight = temp.top();
                    temp.pop();
                    rightNode->right_node = addRight;
                    rightNode = rightNode->right_node;
                    child--;
                }
            }
            tree *toPush = buildNode(val, type);
            toPush->left_node = tempLeft;
            st.push(toPush);
        }
    }

    // Get the next token
    token get_token(char read[])
    {
        token t;
        int row = index;         // Index of character
        string id = "";        // Identifier
        string num = "";       // Number
        string isop = "";      // Operator
        string isString = "";  // String
        string isPunctuation  = "";     // Punctuation
        string isComment = ""; // Comment
        string isSpace = "";   // Space

        // Check if end of file is reached
        if (read[row] == '\0' || row == sizeOfFile)
        {
            t.setType("EOF");
            t.setValue("EOF");
            return t;
        }

        // Read the next token
        while (row < sizeOfFile || row < 10000 || read[row] != '\0')
        {
            // Check if character is a digit
            if (isDigit(read[row]))
            {
                while (isDigit(read[row]))
                {
                    num = num + read[row];
                    row++;
                }
                index = row;
                t.setValue(num);
                t.setType("INT");
                ofstream outputFile("output_token_sequence.txt", ios::app); // Open output file in append mode
                outputFile << "<INTEGER> " << num << endl;  // Write token details to file
                outputFile.close();                           // Close the output file                
                return t;
            }
            // Check if character is an alphabet letter
            else if (isAlpha(read[row]))
            {
                while (isAlpha(read[row]) || isDigit(read[row]) || read[row] == '_')
                {
                    id = id + read[row];
                    row++;
                }

                if (isReservedKey(id))
                {
                    index = row;
                    t.setValue(id);
                    t.setType("KEYWORD");
                    ofstream outputFile("output_token_sequence.txt", ios::app); // Open output file in append mode
                    outputFile << "<IDENTIFIER> " << id << endl; // Write token details to file
                    outputFile.close();                            // Close the output file
                    return t;
                }
                else
                {
                    index = row;
                    t.setValue(id);
                    t.setType("ID");
                    ofstream outputFile("output_token_sequence.txt", ios::app); // Open output file in append mode
                    outputFile << "<IDENTIFIER> " << id << endl; // Write token details to file
                    outputFile.close();                            // Close the output file                    
                    return t;
                }
            }
            // Check if character is a comment
            else if (read[row] == '/' && read[row + 1] == '/')
            {
                while (read[row] == '\'' || read[row] == '\\' || read[row] == '(' || read[row] == ')' || read[row] == ';' || read[row] == ',' || read[row] == ' ' || read[row] == '\t' || isAlpha(read[row]) || isDigit(read[row]) || isOperator(read[row]))
                {

                    if (read[row] == '\n')
                    {
                        row++;
                        break;
                    }
                    else
                    {
                        isComment = isComment + read[row];
                        row++;
                    }
                }

                index = row;
                t.setValue(isComment);
                t.setType("DELETE");
                ofstream outputFile("output_token_sequence.txt", ios::app);   // Open output file in append mode
                outputFile << "<COMMENT> " << isComment << endl; // Write token details to file
                outputFile.close();                             // Close the output file                
                return t;
            }
            // Check if character is an operator
            else if (isOperator(read[row]))
            {
                while (isOperator(read[row]))
                {
                    isop = isop + read[row];
                    row++;
                }

                index = row;
                t.setValue(isop);
                t.setType("OPERATOR");
                ofstream outputFile("output_token_sequence.txt", ios::app);    // Open output file in append mode
                outputFile << "<OPERATOR> " << isop << endl;  // Write token details to file
                outputFile.close();                             // Close the output file                
                return t;
            }

            else if (read[row] == '\'')
            {
                isString = isString + read[row];
                row++;
                while (read[row] == '\'' || read[row] == '\\' || read[row] == '(' || read[row] == ')' || read[row] == ';' || read[row] == ',' || read[row] == ' ' || isAlpha(read[row]) || isDigit(read[row]) || read[row] == '_' || isOperator(read[row]))
                {
                    if (read[row] == '\'')
                    {
                        isString = isString + read[row];
                        row++;
                        break;
                    }
                    else if (read[row] == '\\')
                    {
                        isString = isString + read[row];
                        row++;
                        if (read[row] == 't' || read[row] == 'n' || read[row] == '\\' || read[row] == '"')
                        {
                            isString = isString + read[row];
                            row++;
                        }
                        else
                        {
                            row++;
                        }
                    }
                    else
                    {
                        isString = isString + read[row];
                        row++;
                    }
                }
                index = row;
                t.setValue(isString);
                t.setType("STR");
                ofstream outputFile("output_token_sequence.txt", ios::app);   // Open output file in append mode
                outputFile << "<STRING> " << isComment << endl; // Write token details to file
                outputFile.close();                             // Close the output file                   
                return t;
            }
            else if (read[row] == ')' || read[row] == '(' || read[row] == ';' || read[row] == ',')
            {
                isPunctuation  = isPunctuation  + read[row];
                row++;

                index = row;
                t.setValue(isPunctuation );
                t.setType("PUNCTION");
                ofstream outputFile("output_token_sequence.txt", ios::app);    // Open output file in append mode
                outputFile << "<PUNCTUATION> " << isPunctuation << endl;  // Write token details to file
                outputFile.close();                             // Close the output file
                return t;
            }
            else if (isspace(read[row]))
            {
                while (isspace(read[row]))
                {
                    isSpace = isSpace + read[row];
                    row++;
                }
                index = row;
                t.setValue(isSpace);
                t.setType("DELETE");
                // Skip writing space tokens to the output file
                return t;
            }
            else
            {
                string temp = "";
                temp = temp + read[row];
                t.setValue(temp);
                t.setType("UNKNOWN");
                row++;
                index = row;              
                return t;
            }
        }

        return t;
    }




    /* Below function initiates the parsing process for the CSE machine.
    It sets the CSE flag to indicate parsing in progress, retrieves tokens, 
    ignores DELETE tokens, parses the input, builds the abstract syntax tree,
    standardizes the tree, constructs control structures, and prepares for CSE machine execution.
    The final output of the CSE Machine  can not be obtained after calling this function.  */


    void parse()
    {
        nextToken = get_token(readline);          // Get the first token
        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = get_token(readline);

        procedure_E(); // Start parsing from E

        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = get_token(readline);

        if (index >= sizeOfFile - 1) // Check if end of file is reached
        {
            tree *t = st.top(); // Get the root of the tree

            // Print the abstact syntax tree if the flag is set
            if (astFlag == 1)
                t->displaySyntaxTree(0);

            // Make the ST
            MST(t);

            // Print the standardized tree if the flag is set
            if (astFlag == 2)
                t->displaySyntaxTree(0);

            // Create
            tree *controlStructureArray[200][200];
            buildControlStructures(t, controlStructureArray);

            int size = 0;
            int inner = 0;
            while (controlStructureArray[size][0] != NULL)
                size++;

            vector<vector<tree *> > controlNodeArray;
            for (int row = 0; row < size; row++)
            {
                vector<tree *> temp;
                for (int column = 0; column < 200; column++)
                {
                    if (controlStructureArray[row][column] != NULL)
                        temp.push_back(controlStructureArray[row][column]);
                }
                controlNodeArray.push_back(temp);
            }

            cse_machine(controlNodeArray);
        }
        parsingComplete = true;
    }



    /* Below function initiates the parsing process for the CSE machine.
    It sets the CSE flag to indicate parsing in progress, retrieves tokens, 
    ignores DELETE tokens, parses the input, builds the abstract syntax tree,
    standardizes the tree, constructs control structures, and prepares for CSE machine execution.
    The final output of the CSE Machine  can be obtained after calling this function.  */

    void cse_parse()
    {
        cse_flag =1;
        nextToken = get_token(readline);          // Get the first token
        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = get_token(readline);

        procedure_E(); // Start parsing from E

        while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
            nextToken = get_token(readline);

        if (index >= sizeOfFile - 1) // Check if end of file is reached
        {
            tree *t = st.top(); // Get the root of the tree

            // Print the abstact syntax tree if the flag is set
            if (astFlag == 1)
                t->displaySyntaxTree(0);

            // Make the ST
            MST(t);

            // Print the standardized tree if the flag is set
            if (astFlag == 2)
                t->displaySyntaxTree(0);

            // Create
            tree *controlStructureArray[200][200];
            buildControlStructures(t, controlStructureArray);

            int size = 0;
            int inner = 0;
            while (controlStructureArray[size][0] != NULL)
                size++;

            vector<vector<tree *> > controlNodeArray;
            for (int row = 0; row < size; row++)
            {
                vector<tree *> temp;
                for (int column = 0; column < 200; column++)
                {
                    if (controlStructureArray[row][column] != NULL)
                        temp.push_back(controlStructureArray[row][column]);
                }
                controlNodeArray.push_back(temp);
            }

            cse_machine(controlNodeArray);
        }
        parsingComplete = true;
    }




    bool isParsingComplete() {
    // Getter for parsingComplete flag
        return parsingComplete;
    }



   // Helper function for makeStandardTree
    void MST(tree *t)
    {
        makeStandardTree(t);
    }

    // Makes the tree standard
    tree *makeStandardTree(tree *t)
    {
        if (t == NULL)
            return NULL;
        makeStandardTree(t->left_node);
        makeStandardTree(t->right_node);

        if (t->getValue() == "let")
        {
            if (t->left_node->getValue() == "=")
            {
                t->setValue("gamma");
                t->setType("KEYWORD");
                tree *P = buildNode(t->left_node->right_node);
                tree *X = buildNode(t->left_node->left_node);
                tree *E = buildNode(t->left_node->left_node->right_node);
                t->left_node = buildNode("lambda", "KEYWORD");
                t->left_node->right_node = E; 
                tree *lambda = t->left_node;
                lambda->left_node = X;
                lambda->left_node->right_node = P;
            }
        }
        else if (t->getValue() == "and" && t->left_node->getValue() == "=")
        {
            tree *equal = t->left_node;
            t->setValue("=");
            t->setType("KEYWORD");
            t->left_node = buildNode(",", "PUNCTION");
            tree *comma = t->left_node;
            comma->left_node = buildNode(equal->left_node);
            t->left_node->right_node = buildNode("tau", "KEYWORD");
            tree *tau = t->left_node->right_node;

            tau->left_node = buildNode(equal->left_node->right_node);
            tau = tau->left_node;
            comma = comma->left_node;
            equal = equal->right_node;

            while (equal != NULL)
            {
                comma->right_node = buildNode(equal->left_node);
                comma = comma->right_node;
                tau->right_node = buildNode(equal->left_node->right_node);
                tau = tau->right_node;

                equal = equal->right_node;
            }
        }
        else if (t->getValue() == "where")
        {
            t->setValue("gamma");
            t->setType("KEYWORD");
            if (t->left_node->right_node->getValue() == "=")
            {
                tree *P = buildNode(t->left_node);
                tree *X = buildNode(t->left_node->right_node->left_node);
                tree *E = buildNode(t->left_node->right_node->left_node->right_node);
                t->left_node = buildNode("lambda", "KEYWORD");
                t->left_node->right_node = E;
                t->left_node->left_node = X;
                t->left_node->left_node->right_node = P;
            }
        }
        else if (t->getValue() == "within")
        {
            if (t->left_node->getValue() == "=" && t->left_node->right_node->getValue() == "=")
            {
                tree *X1 = buildNode(t->left_node->left_node);
                tree *E1 = buildNode(t->left_node->left_node->right_node);
                tree *X2 = buildNode(t->left_node->right_node->left_node);
                tree *E2 = buildNode(t->left_node->right_node->left_node->right_node);
                t->setValue("=");
                t->setType("KEYWORD");
                t->left_node = X2;
                t->left_node->right_node = buildNode("gamma", "KEYWORD");
                tree *temp = t->left_node->right_node;
                temp->left_node = buildNode("lambda", "KEYWORD");
                temp->left_node->right_node = E1;
                temp = temp->left_node;
                temp->left_node = X1;
                temp->left_node->right_node = E2;
            }
        }
        else if (t->getValue() == "rec" && t->left_node->getValue() == "=")
        {
            tree *X = buildNode(t->left_node->left_node);
            tree *E = buildNode(t->left_node->left_node->right_node);

            t->setValue("=");
            t->setType("KEYWORD");
            t->left_node = X;
            t->left_node->right_node = buildNode("gamma", "KEYWORD");
            t->left_node->right_node->left_node = buildNode("YSTAR", "KEYWORD");
            tree *ystar = t->left_node->right_node->left_node;

            ystar->right_node = buildNode("lambda", "KEYWORD");

            ystar->right_node->left_node = buildNode(X);
            ystar->right_node->left_node->right_node = buildNode(E);
        }
        else if (t->getValue() == "function_form")
        {
            tree *P = buildNode(t->left_node);
            tree *V = t->left_node->right_node;

            t->setValue("=");
            t->setType("KEYWORD");
            t->left_node = P;

            tree *temp = t;
            while (V->right_node->right_node != NULL)
            {
                temp->left_node->right_node = buildNode("lambda", "KEYWORD");
                temp = temp->left_node->right_node;
                temp->left_node = buildNode(V);
                V = V->right_node;
            }

            temp->left_node->right_node = buildNode("lambda", "KEYWORD");
            temp = temp->left_node->right_node;

            temp->left_node = buildNode(V);
            temp->left_node->right_node = V->right_node;
        }
        else if (t->getValue() == "lambda")
        {
            if (t->left_node != NULL)
            {
                tree *V = t->left_node;
                tree *temp = t;
                if (V->right_node != NULL && V->right_node->right_node != NULL)
                {
                    while (V->right_node->right_node != NULL)
                    {
                        temp->left_node->right_node = buildNode("lambda", "KEYWORD");
                        temp = temp->left_node->right_node;
                        temp->left_node = buildNode(V);
                        V = V->right_node;
                    }

                    temp->left_node->right_node = buildNode("lambda", "KEYWORD");
                    temp = temp->left_node->right_node;
                    temp->left_node = buildNode(V);
                    temp->left_node->right_node = V->right_node;
                }
            }
        }
        else if (t->getValue() == "@")
        {
            tree *E1 = buildNode(t->left_node);
            tree *N = buildNode(t->left_node->right_node);
            tree *E2 = buildNode(t->left_node->right_node->right_node);
            t->setValue("gamma");
            t->setType("KEYWORD");
            t->left_node = buildNode("gamma", "KEYWORD");
            t->left_node->right_node = E2;
            t->left_node->left_node = N;
            t->left_node->left_node->right_node = E1;
        }

        return NULL;
    }

    
/* ---------------------------------- CSE Machine --------------------------------- */

    // Creates the necessary control structures for the CSE machine based on the syntax tree
    void buildControlStructures(tree *x, tree *(*controlNodeArray)[200])
    {
        static int index = 1;   // Variable to keep track of the index
        static int column = 0;   // Variable to keep track of the column
        static int row = 0;    // Variable to keep track of the row
        static int betaCount = 1;  // Variable to count beta nodes

        // Base case: if the current node is NULL, return
        if (x == NULL)
            return;

        // Handle lambda nodes
        if (x->getValue() == "lambda") // If node is lambda
        {
            std::stringstream ss;

            int t_var_1 = row;
            int counter = 0;
            controlNodeArray[row][column] = buildNode("", "");
            row = 0;

            // Count the number of rows in the controlNodeArray
            while (controlNodeArray[row][0] != NULL)
            {
                row++;
                counter++;
            }
            row = t_var_1;
            ss << counter;
            index++;

            string str = ss.str();
            tree *temp = buildNode(str, "deltaNumber");

            // Populate the controlNodeArray with lambda information
            controlNodeArray[row][column++] = temp;

            controlNodeArray[row][column++] = x->left_node;

            controlNodeArray[row][column++] = x;

            int saved_index = row;
            int tempj = column + 3;

            // Recursively build control structures for lambda children
            while (controlNodeArray[row][0] != NULL)
                row++;
            column = 0;

            buildControlStructures(x->left_node->right_node, controlNodeArray);

            row = saved_index;
            column = tempj;
        }

        // Handle conditional nodes
        else if (x->getValue() == "->") // If node is conditional node
        {
            int saved_index = row;
            int tempj = column;
            int nextDelta = index;
            int counter = row;

            std::stringstream ss2;
            ss2 << nextDelta;
            string str2 = ss2.str();
            tree *temp1 = buildNode(str2, "deltaNumber");

            // Populate the controlNodeArray with conditional information
            controlNodeArray[row][column++] = temp1;

            int nextToNextDelta = index;
            std::stringstream ss3;
            ss3 << nextToNextDelta;
            string str3 = ss3.str();
            tree *temp2 = buildNode(str3, "deltaNumber");
            controlNodeArray[row][column++] = temp2;

            tree *beta = buildNode("beta", "beta");

            controlNodeArray[row][column++] = beta;

            // Count the number of rows in the controlNodeArray
            while (controlNodeArray[counter][0] != NULL)
            {
                counter++;
            }
            int firstIndex = counter;
            int lamdaCount = index;

            // Recursively build control structures for conditional children
            buildControlStructures(x->left_node, controlNodeArray);
            int diffLc = index - lamdaCount;

            // Recursively build control structures for conditional children
            while (controlNodeArray[row][0] != NULL)
                row++;
            column = 0;

            buildControlStructures(x->left_node->right_node, controlNodeArray);


            // Recursively build control structures for conditional children
            while (controlNodeArray[row][0] != NULL)
                row++;
            column = 0;

            buildControlStructures(x->left_node->right_node->right_node, controlNodeArray);

            stringstream ss23;
            if (diffLc == 0 || row < lamdaCount)
            {
                ss23 << firstIndex;
            }
            else
            {
                ss23 << row - 1;
            }

            string str5 = ss23.str();


            // Update controlNodeArray with conditional information
            controlNodeArray[saved_index][tempj]->setValue(str5);
            stringstream ss24;
            ss24 << row;
            string str6 = ss24.str();

            controlNodeArray[saved_index][tempj + 1]->setValue(str6);

            row = saved_index;
            column = 0;

            // Count the number of columns in the controlNodeArray
            while (controlNodeArray[row][column] != NULL)
            {
                column++;
            }
            betaCount += 2;
        }

        // Handle tau nodes
        else if (x->getValue() == "tau") // If node is tau node
        {
            tree *tauLeft = x->left_node;
            int numOfChildren = 0;
            while (tauLeft != NULL)
            {
                numOfChildren++;
                tauLeft = tauLeft->right_node;
            }
            std::stringstream ss;
            ss << numOfChildren;
            string str = ss.str();
            tree *countNode = buildNode(str, "CHILDCOUNT");

            // Populate the controlNodeArray with tau information
            controlNodeArray[row][column++] = countNode; // putting the number of kids of tua
            tree *tauNode = buildNode("tau", "tau");

            controlNodeArray[row][column++] = tauNode; // putting the tau node and not pushing x

            // Recursively build control structures for tau children
            buildControlStructures(x->left_node, controlNodeArray);
            x = x->left_node;
            while (x != NULL)
            {
                buildControlStructures(x->right_node, controlNodeArray);
                x = x->right_node;
            }
        }
        // Handle other nodes
        else
        {
            controlNodeArray[row][column++] = buildNode(x->getValue(), x->getType());
            buildControlStructures(x->left_node, controlNodeArray);
            if (x->left_node != NULL)
                buildControlStructures(x->left_node->right_node, controlNodeArray);
        }

        
    }

    // Control Stack Environment (CSE) Machine
    void cse_machine(vector<vector<tree *> > &controlStruct)
    {
        stack<tree *> control;                  // Stack to store control structures
        stack<tree *> machine_stack;                   // Stack to store operands
        stack<environment *> enviornmentStack; // Stack to store environments
        stack<environment *> enviornmentTracker;   // Stack to track environments

        int currEnvIndex = 0;                     // Index of the current environment
        environment *currEnv = new environment(); // Current environment (e0)
        currEnv->name = "env0";

        currEnvIndex++;
        machine_stack.push(buildNode(currEnv->name, "ENV"));
        control.push(buildNode(currEnv->name, "ENV"));
        enviornmentStack.push(currEnv);
        enviornmentTracker.push(currEnv);

        vector<tree *> cor_controlStruct;
        cor_controlStruct = controlStruct.at(0); // Get the first control structure
        for (int row = 0; row < cor_controlStruct.size(); row++)
        {
            control.push(cor_controlStruct.at(row)); // Push each element of the control structure to the control stack
        }

        while (!control.empty())
        {
            tree *nextToken;
            nextToken = control.top(); // Get the top of the control stack
            control.pop();             // Pop the top of the control stack


             // Handle nil tokens
            if (nextToken->getValue() == "nil")
            {
                nextToken->setType("tau");
            }

             // Check if the token is an operand or a special instruction
            if (nextToken->getType() == "INT" || nextToken->getType() == "STR" || nextToken->getValue() == "lambda" || nextToken->getValue() == "YSTAR" || nextToken->getValue() == "Print" || nextToken->getValue() == "Isinteger" || nextToken->getValue() == "Istruthvalue" || nextToken->getValue() == "Isstring" || nextToken->getValue() == "Istuple" || nextToken->getValue() == "Isfunction" || nextToken->getValue() == "Isdummy" || nextToken->getValue() == "Stem" || nextToken->getValue() == "Stern" || nextToken->getValue() == "Conc" || nextToken->getType() == "BOOL" || nextToken->getType() == "NIL" || nextToken->getType() == "DUMMY" || nextToken->getValue() == "Order" || nextToken->getValue() == "nil")
            {
                if (nextToken->getValue() == "lambda")
                {
                    tree *boundVariable = control.top(); // Get the variable bounded to lambda
                    control.pop();

                    tree *nextDeltaIndex = control.top(); // Get the index of the next control structure to access
                    control.pop();

                    tree *env = buildNode(currEnv->name, "ENV");

                    machine_stack.push(nextDeltaIndex); // Push the index of the next control structure
                    machine_stack.push(boundVariable);       // Push the bounded variable
                    machine_stack.push(env);             // Push the environment where lambda was created
                    machine_stack.push(nextToken);      // Push the lambda token
                }
                else
                {
                    machine_stack.push(nextToken); // Push the token to the machine stack
                }
            }
            // Handle gamma instruction
            else if (nextToken->getValue() == "gamma") 
            {
                tree *machineTop = machine_stack.top();
                if (machineTop->getValue() == "lambda") // Apply lambda (CSE Rule 4)
                {
                    machine_stack.pop(); // Pop the environment where it was created

                    tree *prevEnv = machine_stack.top();
                    machine_stack.pop(); // Pop the variable bounded to lambda

                    tree *boundVariable = machine_stack.top();
                    machine_stack.pop(); // Pop the variable bounded to lambda

                    tree *nextDeltaIndex = machine_stack.top();
                    machine_stack.pop(); // Pop the index of the next control structure to access

                    environment *newEnv = new environment(); // Create a  new environment

                    std::stringstream ss;
                    ss << currEnvIndex;
                    string str = ss.str(); // Generate a string for the new environment name

                    newEnv->name = "env" + str;

                    stack<environment *> tempEnv = enviornmentStack;


                    // Find the previous environment node
                    while (tempEnv.top()->name != prevEnv->getValue()) 
                        tempEnv.pop();

                    newEnv->prev = tempEnv.top(); // Set the previous environment node

                    // Bind variables to the environment
                    if (boundVariable->getValue() == "," && machine_stack.top()->getValue() == "tau") // If the operand is tau
                    {
                        vector<tree *> boundVariables;     // Vector to store bound variables
                        tree *leftOfComa = boundVariable->left_node; // Get the left node of the comma
                        while (leftOfComa != NULL)
                        {
                            boundVariables.push_back(buildNode(leftOfComa));
                            leftOfComa = leftOfComa->right_node;
                        }

                        vector<tree *> boundValues; // Vector to store bound values
                        tree *tau = machine_stack.top();  // Pop the tau token
                        machine_stack.pop();

                        tree *tauLeft = tau->left_node; // Get the left node of the tau
                        while (tauLeft != NULL)
                        {
                            boundValues.push_back(tauLeft);
                            tauLeft = tauLeft->right_node; // Get the right node of the tau
                        }

                        for (int row = 0; row < boundValues.size(); row++)
                        {
                            if (boundValues.at(row)->getValue() == "tau")
                            {
                                stack<tree *> res;
                                arrangeTuple(boundValues.at(row), res);
                            }

                            vector<tree *> nodeValueVector;
                            nodeValueVector.push_back(boundValues.at(row));

                            // Insert the bound variable and its value to the environment
                            newEnv->boundVariable.insert(std::pair<tree *, vector<tree *> >(boundVariables.at(row), nodeValueVector));
                        }
                    }
                    else if (machine_stack.top()->getValue() == "lambda")  // If the operand is lambda
                    {
                        vector<tree *> nodeValueVector;  // Vector to store the elements of the lambda expression
                        stack<tree *> temp;   // Temporary stack to hold elements while rearranging
                        int column = 0;  // Counter for columns in the lambda expression
                        while (column < 4)
                        {
                            temp.push(machine_stack.top());  // Push elements from the machine stack to temp stack
                            machine_stack.pop();
                            column++;
                        }

                        //Rearrange elements from temp stack to nodeValueVector
                        while (!temp.empty())
                        {
                            tree *popStack;
                            popStack = temp.top();
                            temp.pop();
                            nodeValueVector.push_back(popStack);  // Push elements from temp stack to nodeValueVector
                        }

                        // Insert the bound variable and its value to the environment
                        newEnv->boundVariable.insert(std::pair<tree *, vector<tree *> >(boundVariable, nodeValueVector));
                    }

                    // If the operand is Conc
                    else if (machine_stack.top()->getValue() == "Conc") 
                    {
                        vector<tree *> nodeValueVector; // Vector to store the elements of the Conc expression
                        stack<tree *> temp;   // Temporary stack to hold elements while rearranging
                        int column = 0;   // Counter for columns in the Conc expression
                        while (column < 2)
                        {
                            temp.push(machine_stack.top());  // Push elements from the machine stack to temp stack
                            machine_stack.pop();   // Pop elements from the machine stack
                            column++;   // Increment column counter
                        }


                        // Rearrange elements from temp stack to nodeValueVector
                        while (!temp.empty())
                        {
                            tree *popStack;
                            popStack = temp.top();
                            temp.pop();
                            nodeValueVector.push_back(popStack);  // Push elements from temp stack to nodeValueVector
                        }

                        // Insert the bound variable and its value to the environment
                        newEnv->boundVariable.insert(std::pair<tree *, vector<tree *> >(boundVariable, nodeValueVector));
                    }

                    // If the operand is eta
                    else if (machine_stack.top()->getValue() == "eta") 
                    {
                        vector<tree *> nodeValueVector;// Vector to store the elements of the eta expression
                        stack<tree *> temp;  // Temporary stack to hold elements while rearranging
                        int column = 0;   // Counter for columns in the eta expression
                        while (column < 4)
                        {
                            temp.push(machine_stack.top());  // Push elements from the machine stack to temp stack
                            machine_stack.pop();  // Pop elements from the machine stack
                            column++;
                        }

                        // Rearrange elements from temp stack to nodeValueVector
                        while (!temp.empty())
                        {
                            tree *popStack;
                            popStack = temp.top();
                            temp.pop();
                            nodeValueVector.push_back(popStack);  // Push elements from temp stack to nodeValueVector
                        }

                        // Insert the bound variable and its value to the environment
                        newEnv->boundVariable.insert(std::pair<tree *, vector<tree *> >(boundVariable, nodeValueVector));
                    }
                    else 
                    // If the operand is an Int
                    {
                        tree *boundVal = machine_stack.top(); // Get the integer value from the stack
                        machine_stack.pop();  // Pop the integer value from the stack

                        vector<tree *> nodeValueVector;  // Vector to store the integer value
                        nodeValueVector.push_back(boundVal);  // Push the integer value to the vector

                        // Insert the bound variable and its value to the environment
                        newEnv->boundVariable.insert(std::pair<tree *, vector<tree *> >(boundVariable, nodeValueVector));
                    }

                    currEnv = newEnv;  // Update the current environment
                    control.push(buildNode(currEnv->name, "ENV"));  // Push the current environment to the control stack
                    machine_stack.push(buildNode(currEnv->name, "ENV"));  // Push the current environment to the machine stack
                    enviornmentStack.push(currEnv); // Push the current environment to the environment stack
                    enviornmentTracker.push(currEnv);// Push the current environment to the environment tracker stack

                    istringstream is3(nextDeltaIndex->getValue());
                    int nextControlIndex;
                    is3 >> nextControlIndex;

                    vector<tree *> nextDelta = controlStruct.at(nextControlIndex); // Get the next control structure
                    for (int row = 0; row < nextDelta.size(); row++)
                    {
                        control.push(nextDelta.at(row)); // Push each element of the next control structure to the control stack
                    }
                    currEnvIndex++;
                }

                // If the top of machine stack is tau (Tuple Selection)
                else if (machineTop->getValue() == "tau") // CSE Rule 10 (Tuple Selection)
                {

                    tree *tau = machine_stack.top();  // Get tau node from top of stack
                    machine_stack.pop();  // Pop tau node from stack

                    tree *childIndex = machine_stack.top(); // Get the index of the child to be selected
                    machine_stack.pop();   // Pop the index from stack

                    istringstream is4(childIndex->getValue());
                    int tupleIndex;
                    is4 >> tupleIndex;

                    tree *tauLeft = tau->left_node;  // Get the left node of the tau
                    tree *selectedChild;
                    while (tupleIndex > 1) // Iterate to find the child to be selected
                    {
                        tupleIndex--;
                        tauLeft = tauLeft->right_node;// Move to the right node of tau
                    }

                    selectedChild = buildNode(tauLeft);
                    if (selectedChild->getValue() == "lamdaTuple")
                    {
                        tree *getNode = selectedChild->left_node;
                        while (getNode != NULL)
                        {
                            machine_stack.push(buildNode(getNode));
                            getNode = getNode->right_node;
                        }
                    }
                    else
                    {
                        machine_stack.push(selectedChild);
                    }
                }
                else if (machineTop->getValue() == "YSTAR") // Checks if the YSTAR token is on top of the control stack (CSE Rule 12)
                {
                    machine_stack.pop(); // Removes the YSTAR token from the machine stack
                    if (machine_stack.top()->getValue() == "lambda") // If the next item is a lambda token
                    {
                        tree *etaNode = buildNode(machine_stack.top()->getValue(), machine_stack.top()->getType()); // Creates an eta node
                        etaNode->setValue("eta");
                        machine_stack.pop(); // Removes the lambda token

                        tree *boundEnv1 = machine_stack.top(); // Retrieves the bounded environment
                        machine_stack.pop();

                        tree *boundVar1 = machine_stack.top(); // Retrieves the bounded variable
                        machine_stack.pop();

                        tree *deltaIndex1 = machine_stack.top(); // Retrieves the index of the next control structure
                        machine_stack.pop();

                        // Pushes the required nodes back to the machine stack
                        machine_stack.push(deltaIndex1);
                        machine_stack.push(boundVar1);
                        machine_stack.push(boundEnv1);
                        machine_stack.push(etaNode);
                    }
                    else
                        return; // Error: Expected lambda token
                }
                else if (machineTop->getValue() == "eta") // Checks if the eta token is on top of the control stack (CSE Rule 13)
                {
                    tree *eta = machine_stack.top(); // Retrieves the eta node
                    machine_stack.pop(); // Removes the eta node from the machine stack

                    tree *boundEnv1 = machine_stack.top(); // Retrieves the bounded environment
                    machine_stack.pop();

                    tree *boundVar1 = machine_stack.top(); // Retrieves the bounded variable
                    machine_stack.pop();

                    tree *deltaIndex1 = machine_stack.top(); // Retrieves the index of the next control structure
                    machine_stack.pop();

                    // Pushes the eta node back into the machine stack
                    machine_stack.push(deltaIndex1);
                    machine_stack.push(boundVar1);
                    machine_stack.push(boundEnv1);
                    machine_stack.push(eta);

                    // Pushes a lambda node with the same parameters as the eta node
                    machine_stack.push(deltaIndex1);
                    machine_stack.push(boundVar1);
                    machine_stack.push(boundEnv1);
                    machine_stack.push(buildNode("lambda", "KEYWORD"));

                    // Pushes two gamma nodes onto the control stack
                    control.push(buildNode("gamma", "KEYWORD"));
                    control.push(buildNode("gamma", "KEYWORD"));
                }
                else if (machineTop->getValue() == "Print") // Prints the next item on the stack
                {
                    machine_stack.pop(); // Removes the Print token
                    tree *itemToPrint = machine_stack.top(); // Retrieves the item to print from the stack

                    if (itemToPrint->getValue() == "tau") // If the next item is a tuple
                    {
                        tree *getTau = machine_stack.top();

                        stack<tree *> res;
                        arrangeTuple(getTau, res); // Arranges the tuple into a stack

                        stack<tree *> getRev; // Reverses the stack
                        while (!res.empty())
                        {
                            getRev.push(res.top());
                            res.pop();
                        }

                        cout << "("; // Prints the tuple
                        while (getRev.size() > 1)
                        {
                            if (getRev.top()->getType() == "STR")
                                cout << addSpaces(getRev.top()->getValue()) << ", ";
                            else
                                cout << getRev.top()->getValue() << ", ";
                            getRev.pop();
                        }
                        if (getRev.top()->getType() == "STR")
                            cout << addSpaces(getRev.top()->getValue()) << ")";
                        else
                            cout << getRev.top()->getValue() << ")";
                        getRev.pop();
                    }
                    else if (itemToPrint->getValue() == "lambda") // If the next item is a lambda token
                    {
                        machine_stack.pop(); // Removes the lambda token

                        tree *env = machine_stack.top(); // Retrieves the environment in which it was created
                        machine_stack.pop();

                        tree *boundVariable = machine_stack.top(); // Retrieves the variable bounded to lambda
                        machine_stack.pop();

                        tree *num = machine_stack.top(); // Retrieves the index of the next control structure to access
                        machine_stack.pop();

                        cout << "[lambda closure: " << boundVariable->getValue() << ": " << num->getValue() << "]";
                        return;
                    }
                    else // If the next item is a string or integer
                    {
                        if(cse_flag == 1){

                            if (machine_stack.top()->getType() == "STR")
                                cout << "Output of the above program is:\n"<< addSpaces(machine_stack.top()->getValue());
                            else
                                cout <<"Output of the above program is:\n"<< machine_stack.top()->getValue();
                        }
                        cse_flag = 0; // Reset the flag

                    }
                }


                else if (machineTop->getValue() == "Isinteger") // Checks if the next item in the stack is an integer
                {
                    machine_stack.pop(); // Removes the Isinteger token

                    tree *isNextInt = machine_stack.top(); // Retrieves the next item in the stack
                    machine_stack.pop();

                    if (isNextInt->getType() == "INT") // If the next item is an integer
                    {
                        tree *resultNode = buildNode("true", "boolean"); // Creates a node indicating true
                        machine_stack.push(resultNode);
                    }
                    else // If the next item is not an integer
                    {
                        tree *resultNode = buildNode("false", "boolean"); // Creates a node indicating false
                        machine_stack.push(resultNode);
                    }
                }
                else if (machineTop->getValue() == "Istruthvalue") // Checks if the next item in the stack is a boolean value
                {
                    machine_stack.pop(); // Removes the Istruthvalue token

                    tree *isNextBool = machine_stack.top(); // Retrieves the next item in the stack
                    machine_stack.pop();

                    if (isNextBool->getValue() == "true" || isNextBool->getValue() == "false") // If the next item is a boolean value
                    {
                        tree *resultNode = buildNode("true", "BOOL"); // Creates a node indicating true
                        machine_stack.push(resultNode);
                    }
                    else // If the next item is not a boolean value
                    {
                        tree *resultNode = buildNode("false", "BOOL"); // Creates a node indicating false
                        machine_stack.push(resultNode);
                    }
                }

                else if (machineTop->getValue() == "Isstring") // Check if the next item in the stack is a string
                {
                    machine_stack.pop(); // Remove the Isstring token

                    tree *isNextString = machine_stack.top(); // Get the next item in the stack
                    machine_stack.pop();

                    if (isNextString->getType() == "STR") // If the next item is a string
                    {
                        tree *resultNode = buildNode("true", "BOOL"); // Create a node indicating true
                        machine_stack.push(resultNode);
                    }
                    else // If the next item is not a string
                    {
                        tree *resultNode = buildNode("false", "BOOL"); // Create a node indicating false
                        machine_stack.push(resultNode);
                    }
                }
                else if (machineTop->getValue() == "Istuple") // Check if the next item in the stack is a tuple
                {
                    machine_stack.pop(); // Remove the Istuple token

                    tree *isNextTau = machine_stack.top(); // Get the next item in the stack
                    machine_stack.pop();

                    if (isNextTau->getType() == "tau") // If the next item is a tuple
                    {
                        tree *resultNode = buildNode("true", "BOOL"); // Create a node indicating true
                        machine_stack.push(resultNode);
                    }
                    else // If the next item is not a tuple
                    {
                        tree *resultNode = buildNode("false", "BOOL"); // Create a node indicating false
                        machine_stack.push(resultNode);
                    }
                }
                else if (machineTop->getValue() == "Isfunction") // Check if the next item in the stack is a function
                {
                    machine_stack.pop(); // Remove the Isfunction token

                    tree *isNextFn = machine_stack.top(); // Get the next item in the stack

                    if (isNextFn->getValue() == "lambda") // If the next item is a function (lambda)
                    {
                        tree *resultNode = buildNode("true", "BOOL"); // Create a node indicating true
                        machine_stack.push(resultNode);
                    }
                    else // If the next item is not a function
                    {
                        tree *resultNode = buildNode("false", "BOOL"); // Create a node indicating false
                        machine_stack.push(resultNode);
                    }
                }
                else if (machineTop->getValue() == "Isdummy") // Check if the next item in the stack is a dummy value
                {
                    machine_stack.pop(); // Remove the Isdummy token

                    tree *isNextDmy = machine_stack.top(); // Get the next item in the stack

                    if (isNextDmy->getValue() == "dummy") // If the next item is a dummy value
                    {
                        tree *resultNode = buildNode("true", "BOOL"); // Create a node indicating true
                        machine_stack.push(resultNode);
                    }
                    else // If the next item is not a dummy value
                    {
                        tree *resultNode = buildNode("false", "BOOL"); // Create a node indicating false
                        machine_stack.push(resultNode);
                    }
                }
                else if (machineTop->getValue() == "Stem") // Get the first character of a string
                {
                    machine_stack.pop();                      // Remove the Stem token
                    tree *isNextString = machine_stack.top(); // Get the next item in the stack

                    if (isNextString->getValue() == "") // If the next item is empty
                        return; // Return as there is nothing to process

                    if (isNextString->getType() == "STR") // If the next item is a string
                    {
                        string strRes = "'" + isNextString->getValue().substr(1, 1) + "'"; // Extract the first character and enclose in single quotes
                        machine_stack.pop(); // Remove the original string from the stack
                        machine_stack.push(buildNode(strRes, "STR")); // Push the extracted character back to the stack
                    }
                }

                else if (machineTop->getValue() == "Stern") // Extract remaining characters after the first character
                {
                    machine_stack.pop();                      // Remove Stern token from the stack
                    tree *isNextString = machine_stack.top(); // Check the next item in the stack

                    if (isNextString->getValue() == "")
                        return; // Return if the next item is empty

                    if (isNextString->getType() == "STR")
                    {
                        // Extract the remaining characters after the first character and enclose in single quotes
                        string strRes = "'" + isNextString->getValue().substr(2, isNextString->getValue().length() - 3) + "'";
                        machine_stack.pop(); // Remove the original string from the stack
                        machine_stack.push(buildNode(strRes, "STR")); // Push the modified string back to the stack
                    }
                }
                else if (machineTop->getValue() == "Order") // Determine the number of items in a tuple
                {
                    machine_stack.pop(); // Remove Order token from the stack

                    int numOfItems = 0;
                    tree *getTau = machine_stack.top(); // Get the next item in the stack

                    if (getTau->left_node != NULL)
                        getTau = getTau->left_node; // Move to the left child if present

                    while (getTau != NULL)
                    {
                        numOfItems++; // Count the number of items in the tuple
                        getTau = getTau->right_node; // Move to the next item
                    }

                    getTau = machine_stack.top();
                    machine_stack.pop();

                    if ((getTau->getValue() == "nil"))
                        numOfItems = 0; // Set number of items to 0 if the tuple is empty

                    stringstream numToString;
                    numToString << numOfItems; // Convert the count to string
                    string itemCountString = numToString.str();
                    tree *orderNode = buildNode(itemCountString, "INT"); // Create a node for the item count
                    machine_stack.push(orderNode); // Push the item count node back to the stack
                }
                else if (machineTop->getValue() == "Conc") // Concatenate two strings
                {
                    tree *concateNode = machine_stack.top(); // Remove Conc token from the stack
                    machine_stack.pop();

                    tree *firstString = machine_stack.top(); // Get the first string
                    machine_stack.pop();

                    tree *secondString = machine_stack.top(); // Get the second string

                    if (secondString->getType() == "STR" || (secondString->getType() == "STR" && secondString->left_node != NULL && secondString->left_node->getValue() == "true"))
                    {
                        machine_stack.pop();
                        // Concatenate the two strings and enclose in single quotes
                        string res = "'" + firstString->getValue().substr(1, firstString->getValue().length() - 2) + secondString->getValue().substr(1, secondString->getValue().length() - 2) + "'";
                        tree *resultNode = buildNode(res, "STR"); // Create a node for the concatenated string
                        machine_stack.push(resultNode); // Push the concatenated string node back to the stack
                        control.pop(); // Remove the control token from the stack
                    }
                    else
                    {
                        concateNode->left_node = firstString; // Link the left child of Conc node to the first string
                        machine_stack.push(concateNode); // Push the modified Conc node back to the stack
                        firstString->left_node = buildNode("true", "flag"); // Mark the first string as concatenated
                    }
                }
            }
            else if (nextToken->getValue().substr(0, 3) == "env") // If env token is at the top of the control stack (CSE Rule 5)
            {
                stack<tree *> stackToRestore;
                if (machine_stack.top()->getValue() == "lambda") // Remove lambda token and its parameters from the stack
                {
                    stackToRestore.push(machine_stack.top());
                    machine_stack.pop();
                    stackToRestore.push(machine_stack.top());
                    machine_stack.pop();
                    stackToRestore.push(machine_stack.top());
                    machine_stack.pop();
                    stackToRestore.push(machine_stack.top());
                    machine_stack.pop();
                }
                else
                {
                    stackToRestore.push(machine_stack.top()); // Remove value from the stack
                    machine_stack.pop();
                }

                tree *remEnv = machine_stack.top(); // Get the environment to be removed

                if (nextToken->getValue() == remEnv->getValue()) // Check if the environment to be removed matches the one at the top of the control stack
                {
                    machine_stack.pop(); // Remove the environment from the stack

                    stack<tree *> machineTrace = machine_stack;

                    enviornmentTracker.pop(); // Pop the environment from the environment tracker
                    if (!enviornmentTracker.empty())
                    {
                        currEnv = enviornmentTracker.top(); // Update the current environment
                    }
                    else
                        currEnv = NULL; // Set current environment to NULL if the environment tracker is empty
                }
                else
                    return; // Return if the environments do not match

                while (!stackToRestore.empty()) // Push the popped values back to the stack
                {
                    machine_stack.push(stackToRestore.top());
                    stackToRestore.pop();
                }
            }

            else if (nextToken->getType() == "ID" && nextToken->getValue() != "Print" && nextToken->getValue() != "Isinteger" && nextToken->getValue() != "Istruthvalue" && nextToken->getValue() != "Isstring" && nextToken->getValue() != "Istuple" && nextToken->getValue() != "Isfunction" && nextToken->getValue() != "Isdummy" && nextToken->getValue() != "Stem" && nextToken->getValue() != "Stern" && nextToken->getValue() != "Conc")
            {
                environment *temp = currEnv;
                int flag = 0;

                // Iterate through the environments to find the bound variable
                while (temp != NULL)
                {
                    // Iterate over the bound variables in the environment
                    map<tree *, vector<tree *> >::iterator itr = temp->boundVariable.begin();
                    while (itr != temp->boundVariable.end())
                    {
                        if (nextToken->getValue() == itr->first->getValue())
                        {
                            vector<tree *> temp = itr->second;

                            // Check if the bound variable has a single "Conc" operation
                            if (temp.size() == 1 && temp.at(0)->getValue() == "Conc" && temp.at(0)->left_node != NULL)
                            {
                                // Push "gamma" and operands of the "Conc" operation to the stacks
                                control.push(buildNode("gamma", "KEYWORD"));
                                machine_stack.push(temp.at(0)->left_node);
                                machine_stack.push(temp.at(0));
                            }
                            else
                            {
                                int row = 0;
                                while (row < temp.size())
                                {
                                    if (temp.at(row)->getValue() == "lamdaTuple")
                                    {
                                        // Push elements of the lambda tuple to the machine stack
                                        tree *currentLambda = temp.at(row)->left_node;
                                        while (currentLambda != NULL)
                                        {
                                            machine_stack.push(buildNode(currentLambda));
                                            currentLambda = currentLambda->right_node;
                                        }
                                    }
                                    else
                                    {
                                        if (temp.at(row)->getValue() == "tau")
                                        {
                                            // Arrange tuple elements and push to the machine stack
                                            stack<tree *> res;
                                            arrangeTuple(temp.at(row), res);
                                        }
                                        machine_stack.push(temp.at(row));
                                    }
                                    row++;
                                }
                            }
                            flag = 1;
                            break;
                        }
                        itr++;
                    }
                    if (flag == 1)
                        break;
                    temp = temp->prev;
                }
                if (flag == 0)
                    return;
            }

            // Check if a binary or unary operator is at the top of the control stack (CSE Rule 6 and CSE Rule 7)
            else if (isBinaryOperator(nextToken->getValue()) || nextToken->getValue() == "neg" || nextToken->getValue() == "not")
            {
                string op = nextToken->getValue();           // Retrieve the operator
                if (isBinaryOperator(nextToken->getValue())) // Check if the token represents a binary operator
                {
                    tree *node1 = machine_stack.top(); // Get the first operand from the stack
                    machine_stack.pop();

                    tree *node2 = machine_stack.top(); // Get the second operand from the stack
                    machine_stack.pop();

                    if (node1->getType() == "INT" && node2->getType() == "INT") // If both operands are integers
                    {
                        int num1;
                        int num2;

                        istringstream is1(node1->getValue());
                        is1 >> num1;
                        istringstream is2(node2->getValue());
                        is2 >> num2;

                        int res = 0;
                        double resPow;

                        // Perform the arithmetic operation and create a node with the result
                        if (op == "+") // Addition
                        {
                            res = num1 + num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = buildNode(str, "INT");
                            machine_stack.push(res);
                        }
                        else if (op == "-") // Subtraction
                        {
                            res = num1 - num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = buildNode(str, "INT");
                            machine_stack.push(res);
                        }
                        else if (op == "*") // Multiplication
                        {
                            res = num1 * num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = buildNode(str, "INT");
                            machine_stack.push(res);
                        }
                        else if (op == "/") // Division
                        {
                            if (num2 == 0) // Check for division by zero
                                cout << "Exception: STATUS_INTEGER_DIVIDE_BY_ZERO" << endl; 
                            res = num1 / num2;
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = buildNode(str, "INT");
                            machine_stack.push(res);
                        }
                        else if (op == "**") // Power
                        {
                            resPow = pow((double)num1, (double)num2);
                            stringstream ss;
                            ss << res;
                            string str = ss.str();
                            tree *res = buildNode(str, "INT");
                            machine_stack.push(res);
                        }
                        else if (op == "gr" || op == ">") // Greater than
                        {
                            string resStr = num1 > num2 ? "true" : "false";
                            tree *res = buildNode(resStr, "bool");
                            machine_stack.push(res);
                        }
                        else if (op == "ge" || op == ">=") // Greater than or equal to
                        {
                            string resStr = num1 >= num2 ? "true" : "false";
                            tree *res = buildNode(resStr, "bool");
                            machine_stack.push(res);
                        }
                        else if (op == "ls" || op == "<") // Less than
                        {
                            string resStr = num1 < num2 ? "true" : "false";
                            tree *res = buildNode(resStr, "bool");
                            machine_stack.push(res);
                        }
                        else if (op == "le" || op == "<=") // Less than or equal to
                        {
                            string resStr = num1 <= num2 ? "true" : "false";
                            tree *res = buildNode(resStr, "bool");
                            machine_stack.push(res);
                        }
                        else if (op == "eq" || op == "=") // Equal
                        {
                            string resStr = num1 == num2 ? "true" : "false";
                            tree *res = buildNode(resStr, "bool");
                            machine_stack.push(res);
                        }
                        else if (op == "ne" || op == "><") // Not equal
                        {
                            string resStr = num1 != num2 ? "true" : "false";
                            tree *res = buildNode(resStr, "bool");
                            machine_stack.push(res);
                        }
                    }
                    else if (node1->getType() == "STR" && node2->getType() == "STR") // If both operands are strings
                    {
                        if (op == "ne" || op == "<>") 
                        {
                            string resStr = node1->getValue() != node2->getValue() ? "true" : "false";
                            tree *res = buildNode(resStr, "BOOL");
                            machine_stack.push(res);
                        }
                        else if (op == "eq" || op == "==")
                        {
                            string resStr = node1->getValue() == node2->getValue() ? "true" : "false";
                            tree *res = buildNode(resStr, "BOOL");
                            machine_stack.push(res);
                        }
                    }
                    // If both operands are boolean values
                    else if ((node1->getValue() == "true" || node1->getValue() == "false") && (node2->getValue() == "false" || node2->getValue() == "true"))
                    {
                        if (op == "ne" || op == "<>")
                        {
                            string resStr = node1->getValue() != node2->getValue() ? "true" : "false";
                            tree *res = buildNode(resStr, "BOOL");
                            machine_stack.push(res);
                        }
                        else if (op == "eq" || op == "==")
                        {
                            string resStr = node1->getValue() == node2->getValue() ? "true" : "false";
                            tree *res = buildNode(resStr, "BOOL");
                            machine_stack.push(res);
                        }
                        else if (op == "or")
                        {
                            string resStr;
                            if (node1->getValue() == "true" || node2->getValue() == "true")
                            {
                                resStr = "true";
                                tree *res = buildNode(resStr, "BOOL");
                                machine_stack.push(res);
                            }
                            else
                            {
                                resStr = "false";
                                tree *res = buildNode(resStr, "BOOL");
                                machine_stack.push(res);
                            }
                        }
                        else if (op == "&")
                        {
                            string resStr;
                            if (node1->getValue() == "true" && node2->getValue() == "true")
                            {
                                resStr = "true";
                                tree *res = buildNode(resStr, "BOOL");
                                machine_stack.push(res);
                            }
                            else
                            {
                                resStr = "false";
                                tree *res = buildNode(resStr, "BOOL");
                                machine_stack.push(res);
                            }
                        }
                    }
                }
                // If unary operator
                else if (op == "neg" || op == "not")
                {
                    if (op == "neg") // If negation
                    {
                        tree *node1 = machine_stack.top();
                        machine_stack.pop();
                        istringstream is1(node1->getValue());
                        int num1;
                        is1 >> num1;
                        int res = -num1;
                        stringstream ss;
                        ss << res;
                        string str = ss.str();
                        tree *resStr = buildNode(str, "INT");
                        machine_stack.push(resStr);
                    }
                    else if (op == "not" && (machine_stack.top()->getValue() == "true" || machine_stack.top()->getValue() == "false")) // If not
                    {
                        if (machine_stack.top()->getValue() == "true")
                        {
                            machine_stack.pop();
                            machine_stack.push(buildNode("false", "BOOL"));
                        }
                        else
                        {
                            machine_stack.pop();
                            machine_stack.push(buildNode("true", "BOOL"));
                        }
                    }
                }
            }

            // If the next token on top of the control stack is "beta" (CSE Rule 8)
            else if (nextToken->getValue() == "beta")
            {
                // Get the boolean value from the top of the machine stack
                tree *boolVal = machine_stack.top();
                machine_stack.pop();

                // Get the index of the else statement from the control stack
                tree *elseIndex = control.top();
                control.pop();

                // Get the index of the then statement from the control stack
                tree *thenIndex = control.top();
                control.pop();

                int index;
                if (boolVal->getValue() == "true") // If the boolean value is true, go to the then statement
                {
                    istringstream is1(thenIndex->getValue());
                    is1 >> index;
                }
                else // If the boolean value is false, go to the else statement
                {
                    istringstream is1(elseIndex->getValue());
                    is1 >> index;
                }

                // Get the next control structure based on the index
                vector<tree *> nextDelta = controlStruct.at(index);

                // Push each element of the next control structure to the control stack
                for (int row = 0; row < nextDelta.size(); row++)
                    control.push(nextDelta.at(row));
            }


            // If the next token on top of the control stack is "tau" (CSE Rule 9)
            else if (nextToken->getValue() == "tau")
            {
                // Create a tuple node with "tau" value
                tree *tupleNode = buildNode("tau", "tau");

                // Get the number of items in the tuple from the control stack
                tree *noOfItems = control.top();
                control.pop();

                int numOfItems;

                // Convert the number of items to an integer
                istringstream is1(noOfItems->getValue());
                is1 >> numOfItems;

                if (machine_stack.top()->getValue() == "lambda") // If the first item is a lambda token
                {
                    // Pop lambda from the machine stack
                    tree *lamda = buildNode(machine_stack.top()->getValue(), machine_stack.top()->getType());
                    machine_stack.pop();

                    // Pop the environment in which it was created
                    tree *prevEnv = buildNode(machine_stack.top()->getValue(), machine_stack.top()->getType());
                    machine_stack.pop();

                    // Pop the variable bounded to lambda
                    tree *boundVariable = buildNode(machine_stack.top()->getValue(), machine_stack.top()->getType());
                    machine_stack.pop();

                    // Pop the index of next control structure
                    tree *nextDeltaIndex = buildNode(machine_stack.top()->getValue(), machine_stack.top()->getType());
                    machine_stack.pop();

                    // Create a lambda tuple and set parameters
                    tree *currentLambda = buildNode("lamdaTuple", "lamdaTuple");
                    currentLambda->left_node = nextDeltaIndex;
                    nextDeltaIndex->right_node = boundVariable;
                    boundVariable->right_node = prevEnv;
                    prevEnv->right_node = lamda;
                    tupleNode->left_node = currentLambda;
                }
                else
                {
                    // Set the left node of the tuple node to the first item from the machine stack
                    tupleNode->left_node = buildNode(machine_stack.top());
                    machine_stack.pop();
                }

                tree *sibling = tupleNode->left_node; // Get the first item in the tuple

                // For each item in the tuple
                for (int row = 1; row < numOfItems; row++)
                {
                    // Get the next item in the stack
                    tree *temp = machine_stack.top();
                    if (temp->getValue() == "lambda") // If the next item is a lambda token
                    {
                        // Pop lambda from the machine stack
                        tree *lamda = buildNode(machine_stack.top()->getValue(), machine_stack.top()->getType());
                        machine_stack.pop();

                        // Pop the environment in which it was created
                        tree *prevEnv = buildNode(machine_stack.top()->getValue(), machine_stack.top()->getType());
                        machine_stack.pop();

                        // Pop the variable bounded to lambda
                        tree *boundVariable = buildNode(machine_stack.top()->getValue(), machine_stack.top()->getType());
                        machine_stack.pop();

                        // Pop the index of next control structure
                        tree *nextDeltaIndex = buildNode(machine_stack.top()->getValue(), machine_stack.top()->getType());
                        machine_stack.pop();

                        // Create a lambda tuple and set parameters
                        tree *currentLambda = buildNode("lamdaTuple", "lamdaTuple");
                        currentLambda->left_node = nextDeltaIndex;
                        nextDeltaIndex->right_node = boundVariable;
                        boundVariable->right_node = prevEnv;
                        prevEnv->right_node = lamda;
                        sibling->right_node = currentLambda;
                        sibling = sibling->right_node;
                    }
                    else // If the next item is not a lambda token
                    {
                        machine_stack.pop();
                        sibling->right_node = temp;
                        sibling = sibling->right_node;
                    }
                }
                machine_stack.push(tupleNode); // Push the tuple to the stack
            }

            // If the next token on top of the control stack is "aug"
            else if (nextToken->getValue() == "aug")
            {
                // Get the first item from the machine stack
                tree *token1 = buildNode(machine_stack.top());
                machine_stack.pop();

                // Get the second item from the machine stack
                tree *token2 = buildNode(machine_stack.top());
                machine_stack.pop();

                // Check if both tokens are nil
                if (token1->getValue() == "nil" && token2->getValue() == "nil")
                {
                    // Create a tuple node with "tau" value
                    tree *tupleNode = buildNode("tau", "tau");
                    tupleNode->left_node = token1;
                    machine_stack.push(tupleNode);
                }
                // Check if the first token is nil
                else if (token1->getValue() == "nil")
                {
                    // Create a tuple node with "tau" value and set the left node to token2
                    tree *tupleNode = buildNode("tau", "tau");
                    tupleNode->left_node = token2;
                    machine_stack.push(tupleNode);
                }
                // Check if the second token is nil
                else if (token2->getValue() == "nil")
                {
                    // Create a tuple node with "tau" value and set the left node to token1
                    tree *tupleNode = buildNode("tau", "tau");
                    tupleNode->left_node = token1;
                    machine_stack.push(tupleNode);
                }
                // Check if the first token is not a tuple
                else if (token1->getType() != "tau")
                {
                    // Append token1 to the right of token2's existing tuple structure
                    tree *tupleNode = token2->left_node;
                    while (tupleNode->right_node != NULL)
                    {
                        tupleNode = tupleNode->right_node;
                    }
                    tupleNode->right_node = buildNode(token1);
                    machine_stack.push(token2);
                }
                // Check if the second token is not a tuple
                else if (token2->getType() != "tau")
                {
                    // Append token2 to the right of token1's existing tuple structure
                    tree *tupleNode = token1->left_node;
                    while (tupleNode->right_node != NULL)
                    {
                        tupleNode = tupleNode->right_node;
                    }
                    tupleNode->right_node = buildNode(token2);
                    machine_stack.push(token1);
                }
                // If both tokens are tuples, create a new tuple node and concatenate them
                else
                {
                    tree *tupleNode = buildNode("tau", "tau");
                    tupleNode->left_node = token1;
                    tupleNode->left_node->right_node = token2;
                    machine_stack.push(tupleNode);
                }
            }
        }

        cout << endl; // Print a newline at the end of processing
    }


    /* -------------------------------- Additional Functions-------------------------------- */

    // This function arranges nodes in the tauNode into a stack for further processing.
    void arrangeTuple(tree *tauNode, stack<tree *> &res)
    {
        if (tauNode == NULL)
            return;
        if (tauNode->getValue() == "lamdaTuple")
            return;

        // If the node is not a tau node or nil, push it into the result stack
        if (tauNode->getValue() != "tau" && tauNode->getValue() != "nil")
        {
            res.push(tauNode);
        }
        arrangeTuple(tauNode->left_node, res);
        arrangeTuple(tauNode->right_node, res);
    }

    // This function adds spaces to the string based on certain conditions.
    string addSpaces(string temp)
    {
        // Replace escape sequences '\n' and '\t' with proper escape sequences '\\n' and '\\t'
        for (int row = 1; row < temp.length(); row++)
        {
            if (temp[row - 1] == 92 && (temp[row] == 'n'))
            {
                temp.replace(row - 1, 2, "\\\n");
            }
            if (temp[row - 1] == 92 && (temp[row] == 't'))
            {
                temp.replace(row - 1, 2, "\\\t");
            }
        }

        // Remove escape characters '\' and single quotes "'" from the string
        for (int row = 0; row < temp.length(); row++)
        {
            if (temp[row] == 92 || temp[row] == 39)
            {
                temp.replace(row, 1, "");
            }
        }
        return temp;


    }






 
    /* -------------------------------- RPAL’s Phrase Structure G rammar: -------------------------------- */

    // E -> ’let’ D ’in’ E | ’fn’ Vb+ ’.’ E | Ew
    void procedure_E()
    {
        // Production rule: E -> ’let’ D ’in’ E
        if (nextToken.getValue() == "let")
        {
            read("let", "KEYWORD");
            procedure_D();

            read("in", "KEYWORD"); // read 'in' keyword
            procedure_E();

            buildTree("let", "KEYWORD", 2);
        }
        // Production rule: E -> ’fn’ Vb+ ’.’ E
        else if (nextToken.getValue() == "fn")
        {
            int n = 0;
            read("fn", "KEYWORD");
            do
            {
                procedure_Vb();
                n++;
            } while (nextToken.getType() == "ID" || nextToken.getValue() == "(");
            read(".", "OPERATOR");
            procedure_E();

            buildTree("lambda", "KEYWORD", n + 1);
        }
        // Production rule: E -> Ew
        else
        {
            procedure_Ew();
        }
    }

    // Ew ->  T ’where’ Dr | T
    void procedure_Ew()
    {
        procedure_T();

        if (nextToken.getValue() == "where") // If next token is 'where', then rule is: Ew ->  T ’where’ Dr | T
        {
            read("where", "KEYWORD");
            procedure_Dr();
            buildTree("where", "KEYWORD", 2);
        }
    }

    // T -> Ta ( ’,’ Ta )+ | Ta
    void procedure_T()
    {
        procedure_Ta();

        int n = 1;
        while (nextToken.getValue() == ",") // Iterate and read ","; Production rule: T -> Ta ( ’,’ Ta )+
        {
            n++;
            read(",", "PUNCTION");
            procedure_Ta();
        }
        if (n > 1)
        {
            buildTree("tau", "KEYWORD", n);
        }
    }

    // Ta -> Ta ’aug’ Tc | Tc
    void procedure_Ta()
    {
        procedure_Tc();

        while (nextToken.getValue() == "aug") // If next token is 'aug', then rule is: Ta -> Ta ’aug’ Tc
        {
            read("aug", "KEYWORD");
            procedure_Tc();
            buildTree("aug", "KEYWORD", 2);
        }
    }

    // Tc -> B ’->’ Tc ’|’ Tc | B
    void procedure_Tc()
    {
        procedure_B();

        if (nextToken.getValue() == "->") // If next token is '->', then rule is: Tc -> B ’->’ Tc ’|’ Tc
        {
            read("->", "OPERATOR");
            procedure_Tc();
            read("|", "OPERATOR");
            procedure_Tc();
            buildTree("->", "KEYWORD", 3);
        }
    }

    // B -> B ’or’ Bt | Bt
    void procedure_B()
    {
        procedure_Bt();
        while (nextToken.getValue() == "or") // If next token is 'or', then rule is: B -> B ’or’ Bt
        {
            read("or", "KEYWORD");
            procedure_Bt();
            buildTree("or", "KEYWORD", 2);
        }
    }

    // Bt -> Bt ’&’ Bs | Bs
    void procedure_Bt()
    {
        procedure_Bs();
        while (nextToken.getValue() == "&") // If next token is '&', then rule is: Bt -> Bt ’&’ Bs
        {
            read("&", "OPERATOR");
            procedure_Bs();
            buildTree("&", "KEYWORD", 2);
        }
    }

    // Bs -> ’not’ Bp | Bp
    void procedure_Bs()
    {
        if (nextToken.getValue() == "not") // If next token is 'not', then rule is: Bs -> ’not’ Bp
        {
            read("not", "KEYWORD");
            procedure_Bp();
            buildTree("not", "KEYWORD", 1);
        }
        else // Else the rule is: Bs -> Bp
        {
            procedure_Bp();
        }
    }

    /*
    Bp  -> A (’gr’ | ’>’ )
        -> A (’ge’ | ’>=’)
        -> A (’ls’ | ’<’ )
        -> A (’le’ | ’<=’)
        -> A ’eq’ A
        -> A ’ne’ A
        -> A ;
    */
    void procedure_Bp()
    {
        procedure_A();
        string temp = nextToken.getValue();
        string temp2 = nextToken.getType();

        if (nextToken.getValue() == "gr" || nextToken.getValue() == ">") // Bp -> A (’gr’ | ’>’ )
        {
            read(temp, temp2);
            procedure_A();
            buildTree("gr", "KEYWORD", 2);
        }
        else if (nextToken.getValue() == "ge" || nextToken.getValue() == ">=") // Bp -> A (’ge’ | ’>=’)
        {
            read(temp, temp2);
            procedure_A();
            buildTree("ge", "KEYWORD", 2);
        }
        else if (nextToken.getValue() == "ls" || nextToken.getValue() == "<") // Bp -> A (’ls’ | ’<’ )
        {
            read(temp, temp2);
            procedure_A();
            buildTree("ls", "KEYWORD", 2);
        }
        else if (nextToken.getValue() == "le" || nextToken.getValue() == "<=") // Bp -> A (’le’ | ’<=’)
        {
            read(temp, temp2);
            procedure_A();
            buildTree("le", "KEYWORD", 2);
        }
        else if (nextToken.getValue() == "eq") // Bp -> A ’eq’ A
        {
            read(temp, temp2);
            procedure_A();
            buildTree("eq", "KEYWORD", 2);
        }
        else if (nextToken.getValue() == "ne") // Bp -> A ’ne’ A
        {
            read(temp, temp2);
            procedure_A();
            buildTree("ne", "KEYWORD", 2);
        }
    }

    /*
    A   -> A ’+’ At
        -> A ’-’ At
        -> ’+’ At
        -> ’-’ At
        -> At ;
    */
    void procedure_A()
    {
        if (nextToken.getValue() == "+") // A -> ’+’ At
        {
            read("+", "OPERATOR");
            procedure_At();
        }
        else if (nextToken.getValue() == "-") // A -> ’-’ At
        {
            read("-", "OPERATOR");
            procedure_At();
            buildTree("neg", "KEYWORD", 1);
        }
        else
        {
            procedure_At();
        }

        // A   -> A ’+’ At | A ’-’ At
        while (nextToken.getValue() == "+" || nextToken.getValue() == "-")
        {
            string temp = nextToken.getValue();
            read(temp, "OPERATOR");
            procedure_At();
            buildTree(temp, "OPERATOR", 2);
        }
    }

    /*
    At  -> At ’*’ Af
        -> At ’/’  Af
        -> Af ;
    */
    void procedure_At()
    {
        procedure_Af(); // At -> Af

        while (nextToken.getValue() == "*" || nextToken.getValue() == "/") // At -> At ’*’ Af | At ’/’  Af
        {
            string temp = nextToken.getValue();
            read(temp, "OPERATOR");
            procedure_Af();
            buildTree(temp, "OPERATOR", 2);
        }
    }

    /*
    Af -> Ap ’**’ Af
       -> Ap ;
    */
    void procedure_Af()
    {
        procedure_Ap();

        if (nextToken.getValue() == "**") // Af -> Ap ’**’ Af
        {
            read("**", "OPERATOR");
            procedure_Af();
            buildTree("**", "KEYWORD", 2);
        }
    }

    /*
    Ap -> Ap ’@’ ’<IDENTIFIERENTIFIER>’ R
       -> R ;
    */
    void procedure_Ap()
    {
        procedure_R();
        while (nextToken.getValue() == "@")
        {
            read("@", "OPERATOR"); // read a type ID
            if (nextToken.getType() != "ID")
            {
                cout << "Exception: UNEXPECTED_TOKEN";
            }
            else
            {
                read(nextToken.getValue(), "ID");
                procedure_R();
                buildTree("@", "KEYWORD", 3);
            }
        }
    }

    /*
    R -> R Rn
      -> Rn ;
    */
    void procedure_R()
    {
        procedure_Rn();
        while (nextToken.getType() == "ID" || nextToken.getType() == "INT" || nextToken.getType() == "STR" || nextToken.getValue() == "true" || nextToken.getValue() == "false" || nextToken.getValue() == "nil" || nextToken.getValue() == "(" || nextToken.getValue() == "dummy")
        {
            procedure_Rn();
            buildTree("gamma", "KEYWORD", 2);
        }
    }

    /*
    Rn -> ’<ID>’
        -> ’<INT>’
        -> ’<STR>’
        -> ’true’
        -> ’false’
        -> ’nil’
        -> ’(’ E ’)’
        -> ’dummy’
    */
    void procedure_Rn()
    {
        if (nextToken.getType() == "ID" || nextToken.getType() == "INT" || nextToken.getType() == "STR") // Rn -> ’<ID>’ | ’<INT>’ | ’<STR>’
        {
            read(nextToken.getValue(), nextToken.getType());
        }
        else if (nextToken.getValue() == "true") // Rn -> ’true’
        {
            read("true", "KEYWORD");
            buildTree("true", "BOOL", 0);
        }
        else if (nextToken.getValue() == "false") // Rn -> ’false’
        {
            read("false", "KEYWORD");
            buildTree("false", "BOOL", 0);
        }
        else if (nextToken.getValue() == "nil") // Rn -> ’nil’
        {
            read("nil", "KEYWORD");
            buildTree("nil", "NIL", 0);
        }
        else if (nextToken.getValue() == "(") // Rn -> ’(’ E ’)’
        {
            read("(", "PUNCTION");
            procedure_E();
            read(")", "PUNCTION");
        }
        else if (nextToken.getValue() == "dummy") // Rn -> ’dummy’
        {
            read("dummy", "KEYWORD");
            buildTree("dummy", "DUMMY", 0);
        }
    }

    /*
    D -> Da ’within’ D => ’within’
      -> Da ;
    */
    void procedure_D()
    {
        procedure_Da();
        if (nextToken.getValue() == "within")
        {
            read("within", "KEYWORD");
            procedure_Da();
            buildTree("within", "KEYWORD", 2);
        }
    }

    /*
    Da -> Dr ( ’and’ Dr )+
       -> Dr ;
    */
    void procedure_Da()
    {
        procedure_Dr();

        int n = 1;
        while (nextToken.getValue() == "and")
        {
            n++;
            read("and", "KEYWORD");
            procedure_Dr();
        }
        if (n > 1)
        {
            buildTree("and", "KEYWORD", n);
        }
    }

    /*
    Dr -> ’rec’ Db
        -> Db ;
    */
    void procedure_Dr()
    {
        if (nextToken.getValue() == "rec")
        {
            read("rec", "KEYWORD");
            procedure_Db();
            buildTree("rec", "KEYWORD", 1);
        }
        else
        {
            procedure_Db();
        }
    }

    /*
   Db -> Vl ’=’ E => ’=’
      -> ’<ID>’ Vb+ ’=’ E => ’fcn_form’
      -> ’(’ D ’)’ ;
    */
    void procedure_Db()
    {
        if (nextToken.getValue() == "(")
        {
            read("(", "PUNCTION");
            procedure_D();
            read(")", "PUNCTION");
        }
        else if (nextToken.getType() == "ID")
        {
            read(nextToken.getValue(), "ID");
            int n = 1;
            if (nextToken.getValue() == "=" || nextToken.getValue() == ",")
            {
                while (nextToken.getValue() == ",")
                {
                    read(",", "PUNCTION");
                    read(nextToken.getValue(), "ID");
                    n++;
                }
                if (n > 1)
                {
                    buildTree(",", "KEYWORD", n);
                }
                read("=", "OPERATOR");
                procedure_E();
                buildTree("=", "KEYWORD", 2);
            }
            else
            {
                do
                {
                    procedure_Vb();
                    n++;
                } while (nextToken.getType() == "ID" || nextToken.getValue() == "(");
                read("=", "OPERATOR");
                procedure_E();
                buildTree("function_form", "KEYWORD", n + 1);
            }
        }
    }

    /*
    Vb -> ’<ID>’
        -> ’(’ Vl ’)’
        -> ’(’ ’)’
    */
    void procedure_Vb()
    {
        if (nextToken.getType() == "ID")
        {
            read(nextToken.getValue(), "ID");
        }
        else if (nextToken.getValue() == "(")
        {
            read("(", "PUNCTION");
            if (nextToken.getValue() == ")")
            {
                read(")", "PUNCTION");
                buildTree("()", "KEYWORD", 0);
            }
            else
            {
                procedure_Vl();
                read(")", "PUNCTION");
            }
        }
    }

    /*
    Vl -> ’<ID>’ list ’,’
    */
    void procedure_Vl()
    {
        int n = 1;
        read(nextToken.getValue(), "ID");

        while (nextToken.getValue() == ",")
        {
            read(",", "PUNCTION");
            read(nextToken.getValue(), "ID");
            n++;
        }
        if (n > 1)
        {
            buildTree(",", "KEYWORD", n);
        }
    }
};
