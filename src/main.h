#ifndef _main_h
#define _main_h

#include "main_shared.h" //required to prevent redefinition of "argStruct" and "varStruct"

//function prototypes

void handleIncorrectUsage(char* errorMessage);
int main(int argc, char* argv[]);
bool numberIsPower(int number, int power);
double lowerPrecision(double number, int precision);

#endif