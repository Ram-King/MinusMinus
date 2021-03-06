//
//  Interpret.cpp
//  MinusMinus
//
//  Created by Tony Biehl on 8/15/16.
//  Copyright © 2016 Tony Biehl. All rights reserved.
//  Updated 3/28/18

#include "Interpret.h"
#include <fstream>

using namespace std;

//** specialChar
// Check if special character delimiter of '(', ')', '=', '>', '<', or ':'
bool Interpret::specialChar(char ch){
    return ch == '(' || ch == ')' || ch == ',' ||              // call or def
           ch == '=' || ch == '>' || ch == '<' || ch == ':' || // logic or assignment
           ch == '-' || ch == '+' || ch == '*' || ch == '*';   // math
}

//** validID
// check for valid identifier
bool Interpret::validID(string id) {
    bool valid = true;
    char ch;
    for (int x = 0; x < id.length(); x++) {
        ch = id[x];
        if (!isalnum(ch))
            valid = false;
    }
    return valid;
}

//** compare
// take two values, use the SymbolTable to look up locals, and return true or false
bool Interpret::compare(string &line, SymbolTable& local){
    bool result = false;
    string operand1 = nextToken(line);
    string operation = nextToken(line);
    string operand2 = nextToken(line);
    if (operand1 == "" || operation == "" || operand2 == "")
        errorMsg("invalid operand for boolean");
    else {
        int value1 = valueToken(operand1, local);
        int value2 = valueToken(operand2, local);
        switch (operation[0])
        {
            case '>' :
                result = value1 > value2;
                break;
            case '=':
                result = value1 = value2;
                break;
            case '<':
                result = value1 < value2;
                break;
            default:
                errorMsg("Invalid comparision operation of " + operation);
        }
    }
    return result;
}

//** valueToken
// either get an integer literal or look up a variable in the SymbolTable then return the value
int Interpret::valueToken(string token, SymbolTable& local) {
    int result = 0;
    int x = 0;
    Symbol temp;
    bool valid;
    if (token != "") {
        if (isdigit(token[x]) || token[x] == '-') { // numeric value
            do
            {
                result *= 10;
                result += (token[x] - '0');
                if (!isdigit(token[x])) {
                    errorMsg("Invalid number of '" + token +"'");
                    x = token.length(); // exit loop
                }
                else
                    x++;
            }while (x < token.length());
        }
        else { // must be local variable
            temp.symbol = token; // look up variable
            if (local.get(temp)) { // valid local variable
                result = stk.peek(temp.offset, valid);
            }
            else
                errorMsg("Invalid variable '" + token + "'");
        }
    }
    return result;
}

//** execute
// execute starting at lineNum. Will use stk to allocate local variables and update lineNum in execution.
//   Will return true if ended with no errors and deallocate parms and locals from stk. 
//   Space for return value and the parms were allocated BEFORE execute was called
//   Note: use recursion when executing another function (save the lineNum in a local variable
//   and restore when other function is done)
bool Interpret::execute(int numParmsVars, SymbolTable& table) {
    
    Stack<int> whiles;
    Symbol args, parm;
    int parmOffset = 0, count = 0; // parms actually start at 1 for stack frame, return value is at 0

    //**********************************************************************
    //  Put in your code to interpret the MinusMinus language
    //  Don't forget to be recursive when calling other MinusMinus functions
    //**********************************************************************

	/* TIPS
	need to use recursion and multiple symbol tables
	every time you call another function, you're calling execute recursively.
	use the same stack for all the different function variables.
	numParmsVars is the number of parameters as well as the number of local variables in the function.
	
	when you call the function, you grab the parameters, push a return onto the stack, push the params onto a local symbol table,
	and store the line numbers (which don't need to be pushed onto the stack), then pop everything off. retVal, after the function
	executes, gets pushed to the top of the stack.
	
	pair variables in the symbol table with their offsets on the program stack.
	stack stores the actual values in an integer array.
	
	EXAMPLE
	x = 13;
	table [0][0] = "x";
	table [0][1] = 1;
	stk[1] = 13;
	
	Symbol1.name = "count";
	Symbol1.offset = stk.getSize();
	push(0);
	table.add(Symbol1);
	*/
	
    stk.pop(parmOffset); // remove the parameters
    return success;

}

//** nextComma
// Chop off the first part of the string and ensure it is a comma or end of line (error of not)
string Interpret::nextComma(string& line){
    string temp = nextToken(line);
    if (temp != ""){
        if (temp != ",")
            errorMsg("Must be comma separated");
    }
    return temp;
}

//** nextEquation
// Chop off the first part of the string up until a comma (or it IS a comma)
string Interpret::nextEquation(string& exp){
    string s = nextToken(exp);
    string temp = "";
    while (s != "" && s != ",") {
        temp += s;
        s = nextToken(exp);
    }
    return temp;
}

//** nextString
// Chop off the first part of the string from (and including) double quote to double quote
//   If does not start with double quote, it will return an empty string
string Interpret::nextString(string& line){
    string str = "";
    int x = 0;
    while (x < line.length() && (line[x] == ' ' || line[x] == '\t')) // go past leading whitespace
        x++;
    if (line.length() > 0 && line[x] == '\"'){
        str += line[x];
        do
        {
            x++;
            str += line[x];
        } while (x < line.length() && line[x] != '\"');
    }
    chop(line, x+1);
    return str;
}

//** nextToken
// Chop off the first part of the string that:
//    1) ends with white space or () special character
//    2) ends with "
//    3) or if comma flag on, skip whitespace and ends with a comma
string Interpret::nextToken(string& line){
    string token = "";
    char ch;
    int x = 0;
    bool notDelimit = true;
    while (x < line.length() && (line[x] == ' ' || line[x] == '\t')) // go past leading whitespace
        x++;
    if (x < line.length() && line[x] == '\"')
    {
        token = nextString(line);
    }
    else {
        while (x < line.length() && notDelimit) {
          ch = line[x];
          if (ch == ' ' || ch == '\t' || specialChar(ch)) { // check for delimiter
            if (specialChar(ch)) { // special token
                if (token == "") { // actual delimiter or beginning white space?
                    token += ch;
                    x++;
                    if (x < line.length() && token == ":" && line[x] == '=') { // assignment op of :=
                        token += '=';
                        x++;
                    }
                }
            }
            else { // move past space or tab
                x++;
            }
            notDelimit = false;
          }
          else { // add to token
            token += ch;
            x++; // move to next character
          }
        }
        chop(line,x);
    }
    //cout << "Token " << token << endl;
    return token;
}

//** Interpret
Interpret::Interpret(){
    size = errorCount = lineNum = 0;
}

//** start
// start the program by loading the xxx.mm (xxx for program, mm is minus, minus program),
//   find the functions, put them in the SymbolTable functions (function name and line number where it starts),
//   and, if the first pass is OK, start excuting the code by calling execute and passing it a firstlocal SymbolTable
void Interpret::start(string fileName){
    ifstream inFile(fileName);
    if (inFile.fail()) { // could not open
        cout << "Could not open program\n";
        return;
    }
    string line, function, temp;
    size = 0;
    SymbolTable firstLocal; // first local table to be passed
    // read in program
    while (getline(inFile, line)) {
        lines[size] = line;
        function = nextToken(line);
        temp = nextToken(line); // is it a function?
        if (temp == "(") // a function, place in general functions symbol table
        {
            while (temp != ")" && line != "")
                temp = nextToken(line);
            if (temp != ")")
                errorMsg("Bad function of " + function);
            else {
                Symbol newFunction;
                newFunction.symbol = function;
                newFunction.offset = size;
                functions.add(newFunction);
                //cout << "Added function " << function << " offset " << newFunction.offset << endl;
            }
        }
        size++; // bump size for next line
    }
    // set up the call to the first function (assume no parms)
    stk.push(0); // put space for return value
    if (errorCount == 0 && execute(0, firstLocal))
        cout << "\nProgram stop\n";
    else
        cout << endl << errorCount << " errors\n";

}

// Chop string and keep x to the end of the line
void Interpret::chop(string& line, int x){
    if (x < line.length())
        line = line.substr(x, line.length());
    else
        line = "";
}

//** errorMsg
// print lineNum and error and count it via errorCount. Return false for convenience
bool Interpret::errorMsg(string msg) {
    cout << "Error at line " << lineNum+1 << ": " << msg << endl;
    errorCount++;
    return false;
}

//** printString
// print the string constant (remove the quotes)
void Interpret::printString(string s) {
    int len = s.length();
    if (len < 2 || s[0] != '\"' || s[len-1] != '\"')
        errorMsg("Invalid string constant");
    else
        cout << s.substr(1, len-2);
}
//** equation
// convert infix expression to postfix but store on a stack (thus making it into prefix)
//   then call the calculate passing that expression stack and local SymbolTable 
//   and return the calculate's value
//   Note: this function will use precedence, nextToken, and a temporary operator stack
//         that follows a infix to postfix algorithm
int Interpret::equation(string exp, SymbolTable& local, bool& success){
    
        Stack<string> postFix;
        Stack<string> operatorStack;
        
        int length = exp.length();
        for(int i = 0; i < length; i++){
            string token = nextToken(exp);
            if(isalnum(token[0])){
                postFix.push(token);
            } else if(token == "("){
                operatorStack.push(token);
            } else if (isOperator(token)){
                while((operatorStack.getStackSize() != 0) && (operatorStack.peek() != "(") && (precedence(token) <= precedence(operatorStack.peek()))){
                    postFix.push(operatorStack.peek());
                    operatorStack.pop();
                }
                operatorStack.push(token);
            } else {
                
                while(operatorStack.peek() != "("){
                    // cout << "IN WHILE LOOP: " << operatorStack.peek() << endl;
                    postFix.push(operatorStack.peek());
                    operatorStack.pop();
                }
                operatorStack.pop();
            }
        }
        while(operatorStack.getStackSize() != 0){
            postFix.push(operatorStack.peek());
            operatorStack.pop();
        }
        
        // now do equation and return answer
        //for(int i = 0; i < postFix.getStackSize(); i++){
          //  cout << "POSTFIX: " <<  postFix.peek(i, success) << endl;
        //}
        
        //return calculate(postFix, local, success);
        //return 1;
    
    //************************************************************************
    //  Put in your code to convert infix to postfix
    //************************************************************************

    // now do equation and return answer
        
    return calculate(postFix, local, success);
}

//** precedence
// return precedence of two operators (* and / have precedence of 1, all others precedence of 0)
int Interpret::precedence(string s){
    int pred = 0;
    if (s == "*" || s == "/")
        pred = 1;
    return pred;
}

//** isOperator
// check if operator (true if operator, else false)
bool Interpret::isOperator(string s){
    if (s == "+" || s == "-" || s == "*" || s == "/")
        return true;
    return false;
}

//** convertOperand
// either get an integer literal or look up a variable in the SymbolTable then return the value
int Interpret::convertOperand(string s, SymbolTable& local, bool& success){
    int result = 0;
    string::size_type sz;   // alias of size_t
    if (isdigit(s[0]) || s[0] == '-') {
        result = stoi (s,&sz);
    }
    else {
        Symbol parm;
        parm.symbol = s;
        local.get(parm);
        result = stk.peek(parm.offset, success);
    }
    return result;
}

//** calculate
// calculate value from the postFix stack by treating it like prefix (easy to do with a stack)
//   use the local SymbolTable to look up variables
int Interpret::calculate(Stack<string>& postFix, SymbolTable& local, bool& success) {
    int result = 0, result2 = 0;
    string s, operand1, operand2;
    if (postFix.getStackSize() == 0) { // error
        errorMsg("Blank equation");
        return 0;
    }
    s = postFix.peek(); // get operator
    postFix.pop();
    if (!isOperator(s)) {
        if (postFix.getStackSize() > 1) // single operand?
           success = false; // no, bad equation
        else // yes, get single operand value
            result = convertOperand(s, local, success);
    }
    else{ // regular equation
        if (postFix.getStackSize() == 0) { // no operand?
            errorMsg("Missing operand");
            return 0;
        }
        operand2 = postFix.peek(); // get operand 2
        if (isOperator(operand2)) //
            result2 = calculate(postFix, local, success);
        else {
            postFix.pop(); // remove operand
            result2 = convertOperand(operand2, local, success);
        }
        if (postFix.getStackSize() == 0) { // no operand?
            errorMsg("Missing second operand");
            return 0;
        }
        operand1 = postFix.peek(); // get operand 1
        if (isOperator(operand1)) //
            result = calculate(postFix, local, success);
        else {
            postFix.pop(); // remove operand
            result = convertOperand(operand1, local, success);
        }
        //cout << "operator " << s << " operand1 " << operand1 << " operand2 " << operand2 << endl;
        //cout << "Values " << result << " " << result2 << endl;
        switch (s[0]) // do operation
        {
            case '+' : result += result2;
                break;
            case '-' : result -= result2;
                break;
            case '*' : result *= result2;
                break;
            case '/' : result /= result2;
                break;
        }
    }
    return result;
}

