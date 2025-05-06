#ifndef _VCHELPERS_H
#define _VCHELPERS_H

#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include "VCParser.h"

int validExten(char* fileName);
int validExten2(const char* fileName);
int validCard(char* fileName);
int validLines(char* fileName);
int validProp(char* fileName);
int validParam(char* fileName);
int validExtraVal(char* fileName);
int validN(char* fileName);
int caseComp(const char *str1, const char *str2);
char* parseFn(char* fileName,List* parameters);
char* getBday(char* fileName);
void parseDateTime(char* text,DateTime* bDay);
char* getAnni(char* fileName);
void parseProp(char* fileName,List* optionalProperties);
void parseParam(char *line,List* parameters);
void parserN(char *line,Property* optionalProp);
int parseGroup(char *line, char **group);
void parserExtrVal(char *line,List* values);
void unfold(FILE **fPtr, char **line);
char** dispValid(void);
void freeDispValid(char** validList);
Card **validCards(void);
void freeValidCards(Card **validList);
char ** dispBday(void);
char ** dispAnni(void);
char* Contact(Card* card);
char otherProps(Card* card);
int fileNameChange(char* oldFile,char* newFile);
char ** sqlDispBday(void);
char ** sqlDispAnni(void);
int contactChange(Card** obj,char * newContact,char* fileName);
VCardErrorCode writeCardFn(const char* fileName, const Card* obj);
int createCardSql(char* fileName,char* contact);
int validExtenSql(const char* fileName);

#endif

