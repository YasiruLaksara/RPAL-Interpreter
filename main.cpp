#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string.h>
#include "parser.h"

using namespace std;

int main(int argc, const char **argv)
{

    if (argc > 1)
    {
        int argv_idx = 1; // Index of the file name in argv
        int ast_flag = 0;
        int lex_flag =0; // Flag to determine whether to print AST or ST


        if (argc == 3) // Check if AST or ST flag is provided
        {
            argv_idx = 2;
            if (strcmp(argv[1], "-ast") == 0){// Check if AST flag is provided
                ast_flag = 1;
            }
            else if (strcmp(argv[1], "-st") == 0) {// Check if ST flag is provided
                ast_flag = 2;
            }
            else if (strcmp(argv[1], "-lex") == 0) {// Check if ST flag is provided
                lex_flag = 1;
            }

        }
        else {

            // Check if AST or ST flag is not provided

            string filepath = argv[argv_idx];    // Get the file name from the command line
            const char *file = filepath.c_str(); // Convert string to a character array

            // Create an input stream object and read the file into a string
            ifstream input(filepath);
            if (!input)
            {
                std::cout << "File " << "\"" << filepath << "\"" << " not found!" << "\n";
                return 1;
            }
            // Open the file
            string file_str((istreambuf_iterator<char>(input)), (istreambuf_iterator<char>())); // Read the file into a string
            input.close();

            // Convert the string to a character array
            char file_array[file_str.size()];
            for (int row = 0; row < file_str.size(); row++)
                file_array[row] = file_str[row]; 

            // Create a parser object and start parsing
            parser rpal_cse_parser(file_array, 0, file_str.size(), ast_flag);
            rpal_cse_parser.cse_parse();
        }
 
    

        string filepath = argv[argv_idx];    // Get the file name from the command line
        const char *file = filepath.c_str(); // Convert string to a character array

        // Create an input stream object and read the file into a string
        ifstream input(filepath);
        if (!input)
        {
            std::cout << "File " << "\"" << filepath << "\"" << " not found!" << "\n";
            return 1;
        }
        // Open the file
        string file_str((istreambuf_iterator<char>(input)), (istreambuf_iterator<char>())); // Read the file into a string
        input.close();

        // Convert the string to a character array
        char file_array[file_str.size()];
        for (int row = 0; row < file_str.size(); row++)
            file_array[row] = file_str[row];

        // Create a parser object and start parsing
        parser rpal_parser(file_array, 0, file_str.size(), ast_flag);
        rpal_parser.parse();

        // If -lex flag is provided, print the output from the token sequence file
        if (lex_flag) {
            // Wait until the parsing is complete
            if (!rpal_parser.isParsingComplete()) {
                std::cerr << "Error: Parsing is not complete\n";
                return 1;
            }

            std::ifstream inFile("output_token_sequence.txt");
            if (!inFile) {
                std::cerr << "Unable to open file output_token_sequence.txt";
                exit(1);   // exit the program if file cannot be opened
            }

            std::string line;
            while (std::getline(inFile, line)) {
                std::cout << line << '\n';
            }

            inFile.close();
        }




    }
    else
        cout << " Error : Incorrect number of inputs " << endl;

    return 0;
}
