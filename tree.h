#ifndef TREE_H_
#define TREE_H_

#include <iostream>
#include <stack>

using namespace std;

// Class representing a node in a syntax tree
class tree {
private:
    string val;     // Value stored in the node
    string type;    // Type of the node

public:
    tree *left_node;    // Pointer to the left child node
    tree *right_node;   // Pointer to the right child node
    
    // Set the type of the node
    void setType(string typ);
    
    // Set the value of the node
    void setValue(string value);
    
    // Get the type of the node
    string getType();
    
    // Get the value of the node
    string getValue();
    
    // Create a new node with the given value and type
    tree *buildNode(string value, string typ);
    
    // Create a new node with the same value and type as another node
    tree *buildNode(tree *x);
    
    // Display the syntax tree recursively with indentation
    void displaySyntaxTree(int indentationLevel);
};

// Implementation of the methods

// Set the type of the node
void tree::setType(string typ) {
    type = typ;
}

// Set the value of the node
void tree::setValue(string value) {
    val = value;
}

// Get the type of the node
string tree::getType() {
    return type;
}

// Get the value of the node
string tree::getValue() {
    return val;
}

// Create a new node with the given value and type
tree *buildNode(string value, string typ)
{
    tree *t = new tree();
    t->setValue(value);
    t->setType(typ);
    t->left_node = nullptr;
    t->right_node = nullptr;
    return t;
}

// Create a new node with the same value and type as another node
tree *buildNode(tree *x)
{
    tree *t = new tree();
    t->setValue(x->getValue());
    t->setType(x->getType());
    t->left_node = x->left_node;
    t->right_node = nullptr;
    return t;
}

// Display the syntax tree recursively with indentation
void tree::displaySyntaxTree(int indentationLevel) {
    int n = 0;
    while (n < indentationLevel) {
        cout << ".";
        n++;
    }

    // If the node type is ID, STR, or INT, then print <type:val>
    if (type == "ID" || type == "STR" || type == "INT") {
        cout << "<";
        cout << type;
        cout << ":";
    }

    // If the node type is BOOL, NIL, or DUMMY,  then print <val>
    if (type == "BOOL" || type == "NIL" || type == "DUMMY")
        cout << "<";

    cout << val;

    // If the node type is ID, STR, or INT, then print >
    if (type == "ID" || type == "STR" || type == "INT")
        cout << ">";

    // If the node type is BOOL, NIL, or DUMMY, then print >
    if (type == "BOOL" || type == "NIL" || type == "DUMMY")
        cout << ">";

    cout << endl;

    // Recursively display the left and right subtrees
    if (left_node != nullptr)
        left_node->displaySyntaxTree(indentationLevel + 1);

    if (right_node != nullptr)
        right_node->displaySyntaxTree(indentationLevel);
}

#endif 
