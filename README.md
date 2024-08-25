### Program Execution Instructions

The following sequence of commands is applicable within the root directory of the project for compiling the program and executing RPAL programs:

To compile the program and execute rpal programs:
```
> make
> ./myrpal file_name   
```

To generate the Token Sequence
```
> ./myrpal -lex file_name
```

To generate the Abstract Syntax Tree:
```
> ./myrpal -ast file_name
```

To generate the Standardized Tree:
```
> ./myrpal -st file_name
```

Here, the file name "input.txt" has been used.
