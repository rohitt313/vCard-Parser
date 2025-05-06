#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include "VCHelpers.h"
#include "VCParser.h"
#include "LinkedListAPI.h"


int validExten(char* fileName){         //when reading from file
    FILE *fPtr;
    char *extenExists;
    int retVal=1;

    if(fileName==NULL){   
        return 0;
    }
    else{
        if(fileName[0]=='\0') {   
            return 0;
        }
        extenExists=strrchr(fileName,'.');  

        if(extenExists==NULL){             //no exten
            return 0;
        }
        else{
            if(strcmp(extenExists,".vcf")!=0 && strcmp(extenExists,".vcard")!=0){
                return 0;                   //invalid exten
            }
        }

        fPtr=fopen(fileName,"r");

        if(fPtr==NULL) {
            return 0;  
        }
        else{
            fclose(fPtr);
        }
        return retVal;
    }
}

int validExten2(const char* fileName){        //when wrting to file
    FILE *fPtr;
    char *extenExists;
    int retVal=1;

    if(fileName==NULL){   
        printf("filename\n");
        return 0;
    }
    else{
        if(fileName[0]=='\0') {   
            printf("filename 0\n");
            return 0;
        }
        extenExists=strrchr(fileName,'.');  

        if(extenExists==NULL){             //no exten
            printf("extenexists\n");
            return 0;
        }
        else{
            if(strcmp(extenExists,".vcf")!=0 && strcmp(extenExists,".vcard")!=0){
                return 0;                   //invalid exten
            }
        }

        fPtr=fopen(fileName,"r");

        if(fPtr==NULL) {
            return 0;  
        }
        else{
            fclose(fPtr);
        }
        return retVal;
    }
}


int validCard(char* fileName){
    int equal;
    int size;
    int equal2;
    int equal3;
    int equal4;
    int equal5;
    int equal6;
    int equal7;
    int equal8;
    char *crlf;
    FILE * fPtr;
    char *line;
    char fn[4];
    int retVal;
    int validLns;
    int retProp;
    int retProp2;
    int retParam;
    int retExtraVal;

    fPtr=fopen(fileName,"r");

    if(fPtr==NULL){
        return -1;
    }
    else{
        fseek(fPtr, 0, SEEK_END);  
        size = ftell(fPtr);   
        rewind(fPtr);
        if(size==0){             //checking if file is empty
            retVal=0;
        }
        else{
            
            validLns=validLines(fileName);     //checking the crlf characters
            if(validLns==0){
                retVal=-2;
            }
            
            else{
                
                retProp=validProp(fileName);    //validate properties
                retProp2=validN(fileName);    //validate N
                //removethisprintf("retProp ret val is %d\n",retProp);   
                //removethisprintf("retProp2 ret val is %d\n",retProp2); 
                // if(retProp==0){
                //     retVal=-2;
                // }
                if(retProp==0 || retProp==-3 || retProp2==0){
                    if(retProp==0 || retProp2==0){
                        retVal=-2;
                    }
                    else if(retProp==-3){           //for bday/anni
                        retVal=-3;
                    }
                }
                else{
                    retParam=validParam(fileName);
                    retExtraVal=validExtraVal(fileName);
                    //removethisprintf("valid pram val is %d\n",retParam); 
                    //removethisprintf("valid extra val is %d\n",retExtraVal); 
                    if(retParam==1 && retExtraVal==1){
                        retVal=1;

                        line=malloc(sizeof(char)*500);     //checking BEGIN
                        if(fgets(line,500,fPtr)!=NULL){
                            line[(strlen(line)-1)]='\0';
                            crlf=strchr(line,'\r');
                            
                            if(crlf!=NULL){
                                *crlf ='\0';  // Replace '\r' with the null terminator
                            }
                            

                            equal=strcmp(line,"BEGIN:VCARD");

                            if(equal==0){
                                retVal=1;
                            }
                            else{
                                retVal=0;
                            }
                        }

                        if(retVal==1){
                            if(fgets(line,500,fPtr)!=NULL){             //checking version
                                line[(strlen(line)-1)]='\0';
                                crlf=strchr(line,'\r');

                                if(crlf!=NULL){
                                    *crlf ='\0';  // Replace '\r' with the null terminator
                                }

                                equal=strcmp(line,"VERSION:4.0");

                                if(equal==0){
                                    retVal=1;
                                }
                                else{
                                    retVal=0;
                                }         
                            }
                        }

                        if(retVal==1){
                            while(fgets(line,500,fPtr)!=NULL){
                                line[(strlen(line)-1)]='\0';
                                crlf=strchr(line,'\r');

                                if(crlf!=NULL){
                                    *crlf ='\0';  // Replace '\r' with the null terminator
                                }

                                fn[0]=line[0];
                                fn[1]=line[1];
                                fn[2]=line[2];
                                fn[3]='\0';
                                
                                equal=strcmp(fn,"FN:");  //checking FN
                                equal2=strcmp(fn,"fn:");
                                equal3=strcmp(fn,"Fn:");
                                equal4=strcmp(fn,"fN:");

                                equal5=strcmp(fn,"FN;");  //checking FN
                                equal6=strcmp(fn,"fn;");
                                equal7=strcmp(fn,"Fn;");
                                equal8=strcmp(fn,"fN;");



                                
                                
                                if((equal==0 || equal5==0) || (equal2==0||equal6==0) || 
                                (equal3==0||equal7==0) || (equal4==0||equal8==0)){
                                    retVal=1;
                                    break;                   //No need to check further
                                }
                                else{
                                    retVal=0;
                                }
                                
                            }
                        }

                        if(retVal==1){
                            retVal=0;
                            while(fgets(line,500,fPtr)!=NULL){     //checking end
                                line[(strlen(line)-1)]='\0';
                                crlf=strchr(line,'\r');

                                if(crlf!=NULL){
                                    *crlf ='\0';  // Replace '\r' with the null terminator
                                }
                                //removethisprintf("line  %s\n",line);    //print line
                                equal=strcmp(line,"END:VCARD");

                                if(equal==0){
                                    //removethisprintf("here\n");
                                    retVal=1;
                                }
                                else{
                                    retVal=0;
                                }         
                            }       
                        }
                        free(line);
                    }
                    else{
                        retVal=-2;
                    }
                }
                // line=malloc(sizeof(char)*500);     //checking BEGIN
                // if(fgets(line,500,fPtr)!=NULL){
                //     line[(strlen(line)-1)]='\0';
                //     crlf=strchr(line,'\r');
                    
                //     if(crlf!=NULL){
                //         *crlf ='\0';  // Replace '\r' with the null terminator
                //     }

                //     equal=strcmp(line,"BEGIN:VCARD");

                //     if(equal==0){
                //         retVal=1;
                //     }
                //     else{
                //         equal2=strcmp(line,"BEGIN");
                //         if(equal2==0){
                //             retVal=-2;
                //         }
                //         else{
                //             equal3=strcmp(line,"BEGIN:");
                //             if(equal3==0){
                //                 retVal=-2;
                //             }
                //             else{
                //                 retVal=0;
                //             }
                //         }
                //     } 
        
                //     free(line);
                // }

                // if(retVal==1){
                //     line=malloc(sizeof(char)*500);     //checking VERSION
                //     if(fgets(line,500,fPtr)!=NULL){
                //         line[(strlen(line)-1)]='\0';
                //         crlf=strchr(line,'\r');

                //         if(crlf!=NULL){
                //             *crlf ='\0';  // Replace '\r' with the null terminator
                //         }

                //         equal=strcmp(line,"VERSION:4.0");

                //         if(equal==0){
                //             retVal=1;
                //         }
                //         else{
                //             equal2=strcmp(line,"VERSION");
                //             if(equal2==0){
                //                 retVal=-2;
                //             }
                //             else{
                //                 equal3=strcmp(line,"VERSION:");
                //                 if(equal3==0){
                //                     retVal=-2;
                //                 }
                //                 else{
                //                     retVal=0;
                //                 }
                //             }
                //         }         
                //         free(line);
                //     }
                // }

                // if(retVal==1){
                //     line=malloc(sizeof(char)*500);     //checking FN
                //     while(fgets(line,500,fPtr)!=NULL){
                //         line[(strlen(line)-1)]='\0';
                //         crlf=strchr(line,'\r');

                //         if(crlf!=NULL){
                //             *crlf ='\0';  // Replace '\r' with the null terminator
                //         }
                //         fn[0]=line[0];
                //         fn[1]=line[1];
                //         fn[2]='\0';
                        
                //         equal=strcmp(fn,"FN");
                //         printf("fn is %s\n",fn);
                        

                //         if(equal==0){
                //             fnColon=strchr(line,':');

                //             if(fnColon!=NULL){
                //                 printf("in heressss\n");
                //                 fnColon=fnColon+1;
                            
                //                 while(*fnColon==' '){
                //                     fnColon=fnColon+1;
                //                 }

                //                 if(*fnColon=='\0'){
                //                     retVal=-2;
                //                 } 
                //                 else{
                //                     retVal=1;
                //                 }
                //             }
                //             else{
                //                 retVal=-2;
                //             }
                //             free(line);     //if it is found free line and dont check further
                //             break;
                //         }

                //         else{
                //             printf("here\n");
                //             retVal=0;
                //         }
        
                //         free(line);
                //     }
                // }
                // if(retVal==1){
                //     retVal=0;
                //     line=malloc(sizeof(char)*500);     //checking ENDCARD
                //     while(fgets(line,500,fPtr)!=NULL){
                //         line[(strlen(line)-1)]='\0';
                //         crlf=strchr(line,'\r');

                //         if(crlf!=NULL){
                //             *crlf ='\0';  // Replace '\r' with the null terminator
                //         }
                //         printf("line  %s\n",line);
                //         equal=strcmp(line,"END:VCARD");

                //         if(equal==0){
                //             printf("here\n");
                //             retVal=1;
                //         }
                //         else{
                //             equal2=strcmp(line,"END");
                //             if(equal2==0){
                //                 retVal=-2;
                //             }
                //             else{
                //                 equal3=strcmp(line,"END:");
                //                 if(equal3==0){
                //                     retVal=-2;
                //                 }
                //                 else{
                //                     retVal=0;
                //                 }
                //             }
                //         }         
                //         free(line);
                //     }

                    
                // }
            }
        }
        
        fclose(fPtr);    
        return retVal;
    }      
    
}

int validLines(char *fileName){
    FILE *fPtr=fopen(fileName,"r");
    char *line=malloc(sizeof(char)*500);
    char *crlf;
    int retVal=1;

    if(fPtr==NULL){
        return 0;
    }

    while(fgets(line,500,fPtr)!=NULL){
        line[(strlen(line)-1)]='\0';
        crlf=strchr(line,'\r');

        if(crlf==NULL){
            retVal=0;
        }
        
    }
    free(line);
    fclose(fPtr);
    return retVal;

}

int validProp(char* fileName){
    FILE *fPtr=fopen(fileName,"r");
    char *line=malloc(sizeof(char)*500);
    char *crlf;
    char *colon;
    char *semiCol;
    int retVal=1;

    if(fPtr==NULL){
        return 0;
    }

    while(fgets(line,500,fPtr)!=NULL){
        line[(strlen(line)-1)]='\0';
        crlf=strchr(line,'\r');

        if(crlf==NULL){
            return 0;
        }
        *crlf ='\0';
        unfold(&fPtr,&line);
        if(line[0]==' '||line[0]=='\t'){
            continue;
        }
        else{
            colon=strchr(line,':');
            if(colon==NULL || *(colon+1)=='\0' || line[0] == ':') {
                if(colon==NULL){
                    retVal=0;   //

                    semiCol=strchr(line,';');
                    if(semiCol!=NULL){
                        *semiCol='\0';
                        if(strcmp(line,"BDAY")==0){           //bday can be BDAY;VALUE
                            retVal=1;
                        }
                        
                        else if(*(semiCol+1)=='\0'){
                            retVal=0;                          //bday can be BDAY;(No VALUE)  
                            break; 
                        }
                        *semiCol=';';
                    }
            
                    
                    else{
                        //removethisprintf("no semicolon\n");
                        retVal=0; // Found an invalid property

                        if(line[0]=='B'&&line[1]=='D'&&line[2]=='A'&&line[3]=='Y'){           //special for bday
                            retVal=-3;
                        }
                        if(line[0]=='A'&&line[1]=='N'&&line[2]=='N'&&line[3]=='I'       //special for anni
                            &&line[4]=='V'&&line[5]=='E'&&line[6]=='R'&&line[7]=='S'
                            &&line[8]=='A'&&line[9]=='R'&&line[10]=='Y'){
                            retVal=-3;
                        }
                        break;
                    }
                }
                else{                                   
                    // printf("wait a min\n");
                    // retVal=0;
                    // break;

                    //removethisprintf("wait a min\n");
                    retVal=0;

                    if(line[0]=='B'&&line[1]=='D'&&line[2]=='A'&&line[3]=='Y'){           //bday can be BDAY;VALUE
                        retVal=-3;
                    }
                    if(line[0]=='A'&&line[1]=='N'&&line[2]=='N'&&line[3]=='I'       //special for anni
                        &&line[4]=='V'&&line[5]=='E'&&line[6]=='R'&&line[7]=='S'
                        &&line[8]=='A'&&line[9]=='R'&&line[10]=='Y'){
                        retVal=-3;
                    }

                    break;
                }  
            }
            // else{
            //     if(*(colon+1)==';'){          //prop:; invalid
            //         printf("no col semicol\n");
            //         retVal=0;
            //         break;
            //     }
            // }
        }  
    }
    free(line);
    fclose(fPtr);
    return retVal;
}

int validParam(char* fileName){
    const char *param[] = {"LANGUAGE", "VALUE", "PREF", "ALTID", "PID", "TYPE",
    "MEDIATYPE", "CALSCALE", "SORT-AS","GEO","TZ"};
    FILE *fPtr=fopen(fileName,"r");
    char *line=malloc(sizeof(char)*500);
    char *crlf;
    char* temp;
    int i;
    int retVal=1;

    if(fPtr==NULL){
        return 0;
    }

    while(fgets(line,500,fPtr)!=NULL){
        line[(strlen(line)-1)]='\0';
        crlf=strchr(line,'\r');

        if(crlf==NULL){
            return 0;
        }
        *crlf ='\0';
        //unfold(&fPtr,&line);
        if(line[0]==' '||line[0]=='\t'){
            continue;
        }

        else{
            temp=line;
            for(i=0;i<11;++i){
                temp=line;              //resetting the temp var
                if(temp[0]=='G' && temp[1]=='E' && temp[2]=='O'){    //GEO and TZ can be both prop and param
                    continue;
                }
                else if(temp[0]=='T' && temp[1]=='Z'){
                    continue;
                }
                else if(temp[0]=='C' && temp[1]=='L' && temp[2]=='I' && temp[3]=='E' && temp[4]=='N' && 
                    temp[5]=='T' && temp[6]=='P' && temp[7]=='I' && temp[8]=='D'){
                    continue;
                    }
                while((temp=strstr(temp, param[i]))!=NULL){      //CHANGE THIS LOGIC
                    temp=temp+strlen(param[i]);            //should point to equal sign
                    //removethisprintf("temp - 1 %c\n",*(temp-1));
                    if(*temp!='='){
                        retVal=0;
                    }
                    else if(*temp=='=' && (*(temp+1)==';'|| *(temp+1)==':')){    //param has no value
                        retVal=0;
                    }
                    
                    // else if(*temp=='=' && (*(temp-1)==';'|| *(temp-1)==':')){    //param has no name
                    //     retVal=0;
                    // }
                    //removethisprintf("Now temp points to: %s\n", temp);
                }
            }
        }
    }

    fclose(fPtr);
    free(line);
    return retVal;
}

int validN(char* fileName){
    FILE *fPtr=fopen(fileName,"r");
    char *line=malloc(sizeof(char)*500);
    char *crlf;
    int i;
    int semiColCount=0;
    int colCount=0;
    int retVal=1;

    if(fPtr==NULL){
        return 0;
    }
    else{
        while(fgets(line,500,fPtr)!=NULL){
            line[(strlen(line)-1)]='\0';
            crlf=strchr(line,'\r');
    
            if(crlf==NULL){
                free(line);
                fclose(fPtr);
                return 0;
            }
            *crlf='\0';
            unfold(&fPtr,&line);
            if(line[0]==' ' || line[0]=='\t'){        //skip that line as we already merged the lines
                continue;
            }

            if(line[0]=='N' && (line[1]==':'||line[2]==';')){
                for(i=0;i<strlen(line);++i){
                    if(line[i]==';'){
                        semiColCount=semiColCount+1;
                    }
                }

                for(i=0;i<strlen(line);++i){
                    if(line[i]==':'){
                        colCount=colCount+1;
                    }
                }
                //removethisprintf("semi col cont is %d\n",semiColCount);

                if(colCount!=1 || semiColCount!=4){
                    retVal=0;
                    break;
                }
            }
        }
    }
    fclose(fPtr);
    free(line);
    return retVal;
}

int validExtraVal(char* fileName){
    FILE *fPtr=fopen(fileName,"r");
    char *line=malloc(sizeof(char)*500);
    char *dupLine=malloc(sizeof(char)*500);
    char *crlf;
    char *colon;
    char *semiCol;
    int i;
    int semiColCount=0;
    int semiColCount2=0;
    int retVal=1;
    
    if(fPtr==NULL){
        return 0;
    }
    else{
        while(fgets(line,500,fPtr)!=NULL){
            semiColCount=0;
            semiColCount2=0;
            line[(strlen(line)-1)]='\0';
            crlf=strchr(line,'\r');
        
            if(crlf==NULL){
                free(line);
                fclose(fPtr);
                return 0;
            }
            *crlf='\0';
            unfold(&fPtr,&line);
            if(line[0]==' ' || line[0]=='\t'){        //skip that line as we already merged the lines
                continue;
            }
            strcpy(dupLine,line);

            for(i=0;i<strlen(line);++i){
                if((line[i]==':' && line[i+1]==':') || (line[i]==';' && line[i+1]==';')){
                    if(line[i]==';' && line[i+1]==';' && line[0]=='N'){   //N:erer;;;;
                        //do nothing
                    }
                    else{
                        retVal=0;
                        break;
                    }
                }
            }

            if(retVal==0){                // if error above break from while loop
                break;
            }

            else{
                colon=strchr(line,':');
                strcpy(dupLine,colon+1);

                for(i=0;i<strlen(dupLine);++i){
                    if(dupLine[i]==';'){
                        semiColCount=semiColCount+1;
                    }
                }

                if(semiColCount>0){                //checking for invalid list of values
                    semiCol=strtok(dupLine,";");

                    while(semiCol!=NULL){
                        semiColCount2=semiColCount2+1;
                        semiCol=strtok(NULL,";");
                    }
                    //removethisprintf("semicol count2 %d and semi col count1 %d\n",semiColCount2,semiColCount);
                    if(semiColCount2==semiColCount){       //GENDER:sdsds;  token and count is same
                        if(line[0]=='G' && line[1]=='E' && line[2]=='N' && line[3]=='D' &&
                            line[4]=='E' && line[5]=='R'){
                             // do nothing as GENDER:;sdsds is valid
                            if(line[strlen(line)-1]==':' || line[strlen(line)-1]==';'){
                                retVal=0;
                                break;
                            }
                        }
                        else if(line[0]=='A' && line[1]=='D' && line[2]=='R'){
                            //do nothing
                            
                        }
                        else{
                            retVal=0;
                            break;
                        }
                    }
                }
                if(retVal==0){
                    break;
                }
            }
        }
    }
    fclose(fPtr);
    free(line);
    free(dupLine);
    return retVal;
}

char* parseFn(char* fileName,List* parameters){
    FILE *fPtr=fopen(fileName,"r");
    char *line=malloc(sizeof(char)*500);
    char *dupLine=malloc(sizeof(char)*500);
    char *crlf;
    char *beforeColon;
    char *afterColon;
    char *semiCol;
    char *FN=NULL;
    char case1='1';
    char case2='2';
    char case3='3';
    char case4='4';

    if(fPtr==NULL){
        return 0;
    }

    if (line == NULL) {
        fclose(fPtr);
        return NULL;
    }

    while(fgets(line,500,fPtr)!=NULL){
        // if(line[0]==' ' || line[0]=='\t'){        //skip line unfolding
        //     continue;
        // }
        line[(strlen(line)-1)]='\0';
        crlf=strchr(line,'\r');

        if(crlf==NULL){
            free(line);
            fclose(fPtr);
            return 0;
        }
        *crlf ='\0';
        //free(dupLine);
        unfold(&fPtr,&line);
        if(line[0]==' ' || line[0]=='\t'){        //skip that line as we already merged the lines
            continue;
        }
        //dupLine=malloc(sizeof(char)*strlen(line)+1);
        
        if(dupLine!=NULL){
            strcpy(dupLine,line);
        }
        
        beforeColon=strchr(line,':');

        if(beforeColon!=NULL){
            *beforeColon='\0';
            semiCol=strchr(line,';');

            if(semiCol==NULL){                //if FN has no parameters
                if(strcmp(line,"FN")==0 || strcmp(line,"fn")==0 || strcmp(line,"Fn")==0 ||
                    strcmp(line,"fN")==0){ 
                    afterColon=strchr(dupLine,':');

                    if(afterColon!= NULL && *(afterColon + 1)!='\0'){
                        FN=malloc(sizeof(char)*strlen(afterColon+1)+1+1);    //to put 1,2,3,4
                        if(FN!=NULL){
                            if(strcmp(line,"FN")==0){
                                strcpy(FN,afterColon + 1);
                                strncat(FN, &case1, 1);
                            }
                            else if(strcmp(line,"fn")==0){
                                strcpy(FN,afterColon + 1);
                                strncat(FN, &case2, 1);
                            }
                            else if(strcmp(line,"Fn")==0){
                                strcpy(FN,afterColon + 1);
                                strncat(FN, &case3, 1);
                            }
                            else if(strcmp(line,"fN")==0){
                                strcpy(FN,afterColon + 1);
                                strncat(FN, &case4, 1);
                            }
                            //strcpy(FN,afterColon + 1);
                        }
                        break;
                    }
                }
            }
            else{                              //if FN has parameters
                *semiCol='\0';

                if(strcmp(line,"FN")==0 || strcmp(line,"fn")==0 || strcmp(line,"Fn")==0 ||
                    strcmp(line,"fN")==0){ 
                    afterColon=strchr(dupLine,':');

                    if(afterColon!= NULL && *(afterColon + 1)!='\0'){
                        FN=malloc(sizeof(char)*strlen(afterColon+1)+1+1);    //to put 1,2,3,4
                        if(FN!=NULL){
                            if(strcmp(line,"FN")==0){
                                strcpy(FN,afterColon + 1);
                                strncat(FN, &case1, 1);
                                parseParam(dupLine,parameters);            //if we have parameters
                            }
                            else if(strcmp(line,"fn")==0){
                                strcpy(FN,afterColon + 1);
                                strncat(FN, &case2, 1);
                                parseParam(dupLine,parameters);            //if we have parameters
                            }
                            else if(strcmp(line,"Fn")==0){
                                strcpy(FN,afterColon + 1);
                                strncat(FN, &case3, 1);
                                parseParam(dupLine,parameters);            //if we have parameters
                            }
                            else if(strcmp(line,"fN")==0){
                                strcpy(FN,afterColon + 1);
                                strncat(FN, &case4, 1);
                                parseParam(dupLine,parameters);            //if we have parameters
                            }
                            //strcpy(FN,afterColon + 1);
                        }
                        break;
                    }
                }
            }
        }
    }

    free(line);
    free(dupLine);
    fclose(fPtr);
    return FN;
}

char* getBday(char* fileName){
    FILE *fPtr=fopen(fileName,"r");
    char *line=malloc(sizeof(char)*500);
    char *dupLine=malloc(sizeof(char)*500);
    char *dupLine2=malloc(sizeof(char)*500);
    char *crlf;
    char *param;
    char *end;
    char *beforeColon;
    char *afterColon;
    char* beforeSemCol;
    char* afterSemCol;
    char *bDay=NULL;

    while(fgets(line,500,fPtr)!=NULL){
        // if(line[0]==' ' || line[0]=='\t'){        //skip line unfolding
        //     continue;
        // }
        line[(strlen(line)-1)]='\0';
        crlf=strchr(line,'\r');

        if(crlf==NULL){
            free(line);
            fclose(fPtr);
            return 0;
        }
        *crlf ='\0';
        unfold(&fPtr,&line);
        if(line[0]==' '||line[0]=='\t'){
            continue;
        }
        
        strcpy(dupLine,line);
        strcpy(dupLine2,line);
        beforeSemCol=strchr(line,';');
        beforeColon=strchr(line,':');

        if(beforeSemCol!=NULL && beforeColon!=NULL){             //both param and value
            *beforeSemCol='\0';

            if(caseComp(line,"BDAY")){
                param=strchr(dupLine2,'=');
    
                if(param!=NULL){
                    end=strchr(param,':');
                    if(end!=NULL){
                        *end='\0';
                    }
                }
                afterColon=strchr(dupLine,':');
                if(afterColon!= NULL && *(afterColon+1)!='\0'){     //extracting bday both parm and prop
                    bDay=malloc(sizeof(char)*(strlen(afterColon+1)+strlen(param))+1); //\0
                    if(bDay!=NULL){
                        strcpy(bDay,afterColon+1);
                        strcat(bDay,param);
                    }
                    
                }
                // if(afterColon!= NULL && *(afterColon+1)!='\0'){     //extracting bday both parm and prop
                //     bDay=malloc(sizeof(char)*strlen(afterColon+1)+1+1);   //comma+\0

                //     if(bDay!=NULL){
                //         strcpy(bDay,afterColon+1);
                //         strncat(bDay,&comma,1);
                //         //bDay=realloc(bDay,((sizeof(char)*strlen(afterColon+1)+1+1))+
                //         //sizeof(char)*strlen(param+1));
                //         //strcat(bDay,param);
                //     }
                // }
                break;
            }
        }

        else if(beforeSemCol!=NULL){
            *beforeSemCol='\0';

            if(caseComp(line,"BDAY")){
                afterSemCol=strchr(dupLine,';');
                if(afterSemCol!= NULL && *(afterSemCol+1)!='\0'){     //extracting bday
                    bDay=malloc(sizeof(char)*strlen(afterSemCol+1)+1);

                    if(bDay!=NULL){
                        strcpy(bDay,afterSemCol+1);
                    }
                }
                break;
            }
        }
        else if(beforeColon!=NULL){
            beforeColon=strchr(line,':');

            if(beforeColon!=NULL){
                *beforeColon='\0';
            
                if(caseComp(line,"BDAY")){
                    afterColon=strchr(dupLine,':');
                    if(afterColon!= NULL && *(afterColon+1)!='\0'){     //extracting bday
                        bDay=malloc(sizeof(char)*strlen(afterColon+1)+1);

                        if(bDay!=NULL){
                            strcpy(bDay,afterColon+1);
                        }
                    }
                    break;
                }
            }
        }
    }


    free(dupLine);
    free(dupLine2);
    free(line);
    fclose(fPtr);
    return bDay;           //NULL is returned if no bday
}

void parseDateTime(char* text,DateTime* bDay){
    char *tok;
    char *param=NULL;
    char *value=NULL;

    tok=strtok(text,"=");
    if(tok!=NULL){
        value=malloc(sizeof(char)*strlen(tok)+1);
        strcpy(value,tok);
    }
    tok=strtok(NULL,"=");
    if(tok!=NULL){
        param=malloc(sizeof(char)*strlen(tok)+1);
        strcpy(param,tok);
    }

    if(param==NULL){                          //just a value
        if(strchr(value,'T')==NULL && value[0]!='T'){                 //19960415
            bDay->date=malloc(sizeof(char)*strlen(value)+1);
            bDay->time=malloc(sizeof(char));
            bDay->text=malloc(sizeof(char));

            bDay->UTC=0;
            bDay->isText=0;
            strcpy(bDay->date,value);
            bDay->time[0]='\0';
            bDay->text[0]='\0';
        }
        else if(value[0]=='-' && value[1]=='-'){              //--0203
            bDay->date=malloc(sizeof(char)*strlen(value)+1);
            bDay->time=malloc(sizeof(char));
            bDay->text=malloc(sizeof(char));

            bDay->UTC=0;
            bDay->isText=0;
            strcpy(bDay->date,value);
            bDay->time[0]='\0';
            bDay->text[0]='\0';
        }
        else if(value[0]=='T'){              //T143000 T143000Z  
            bDay->date=malloc(sizeof(char));
            bDay->time=malloc(sizeof(char)*strlen(value)-1+1);   //not counting T
            bDay->text=malloc(sizeof(char));

            if(value[strlen(value)-1]=='Z'){         //UTC time
                bDay->UTC=1;
                value[strlen(value)-1]='\0';
            }
            else{
                bDay->UTC=0;
            }
            bDay->isText=0;
            bDay->date[0]='\0';
            memmove(value,value+1,strlen(value));      //not copying T by mving char to left 1 position
            strcpy(bDay->time,value);
            bDay->text[0]='\0';
        }                                                       //works with semi colon as well
        else if(strchr(value,'T')!=NULL && value[0]!='T'){      //20090808T143000   20090808T143000Z
            tok=strtok(value,"T");
            bDay->date=malloc(sizeof(char)*strlen(tok)+1);      //getting time
            strcpy(bDay->date,tok);

            tok=strtok(NULL,"T");
            if(tok!=NULL){                                      //getting date
                if(tok[strlen(tok)-1]=='Z'){
                    bDay->UTC=1;
                    tok[strlen(tok)-1]='\0';
                }
                else{
                    bDay->UTC=0;
                }
                bDay->time=malloc(sizeof(char)*strlen(tok)+1);
                strcpy(bDay->time,tok);
            }
            bDay->isText=0;
            bDay->text=malloc(sizeof(char));
            bDay->text[0]='\0';
        }
    }
    else{                                    //we have a param and value
        //removethisprintf("in the else statement\n");
        if(strcmp(param,"text")==0){          //BDAY;VALUE=text:circa 1800
            bDay->date=malloc(sizeof(char));
            bDay->time=malloc(sizeof(char));
            bDay->text=malloc(sizeof(char)*strlen(value)+1);

            bDay->UTC=0;
            bDay->isText=1;
            bDay->date[0]='\0';
            bDay->time[0]='\0';
            strcpy(bDay->text,value);
            //removethisprintf("val is text in function %s\n",bDay->text);
        }
        else if(strcmp(param,"DATE")==0 || strcmp(param,"date")==0){    //BDAY;VALUE=DATE:19991231
            bDay->date=malloc(sizeof(char)*strlen(value)+1);
            bDay->time=malloc(sizeof(char));
            bDay->text=malloc(sizeof(char));

            bDay->UTC=0;
            bDay->isText=0;
            strcpy(bDay->date,value);
            bDay->time[0]='\0';
            bDay->text[0]='\0';

        }

        else if(strcmp(param,"DATE")==0 || strcmp(param,"date")==0){    //BDAY;VALUE=DATE:19991231
            bDay->date=malloc(sizeof(char)*strlen(value)+1);
            bDay->time=malloc(sizeof(char));
            bDay->text=malloc(sizeof(char));

            bDay->UTC=0;
            bDay->isText=0;
            strcpy(bDay->date,value);
            bDay->time[0]='\0';
            bDay->text[0]='\0';

        }

        else if(strcmp(param,"DATE-TIME")==0 || strcmp(param,"date-time")==0){    //BDAY;VALUE=DATE:19991231
            tok=strtok(value,"T");
            bDay->date=malloc(sizeof(char)*strlen(tok)+1);      //getting time
            strcpy(bDay->date,tok);

            tok=strtok(NULL,"T");
            if(tok!=NULL){                                      //getting date
                if(tok[strlen(tok)-1]=='Z'){
                    bDay->UTC=1;
                    tok[strlen(tok)-1]='\0';
                }
                else{
                    bDay->UTC=0;
                }
                bDay->time=malloc(sizeof(char)*strlen(tok)+1);
                strcpy(bDay->time,tok);
            }
            bDay->isText=0;
            bDay->text=malloc(sizeof(char));
            bDay->text[0]='\0';

        }
    }

    //removethisprintf("param is %s\n",param);
    //removethisprintf("val is %s\n",value);
    free(param);
    free(value);
}

void parseProp(char* fileName,List* optionalProperties){
    FILE *fPtr;
    char *crlf;
    char *line=malloc(sizeof(char)*500);
    char *dupLine=malloc(sizeof(char)*500);
    char *dupLine2=malloc(sizeof(char)*500);
    //char *foldLine=malloc(sizeof(char)*500);
    char *dupLine3;
    char *colon;
    char *semiCol;
    char *tempSemi;
    char *tempDot;     //for group
    char *dotSplit;
    char *dotLine;
    char *extraVal;
    char *extraVal2;
    char *val;
    Property *optProp;
    int total=0;
    int count=0;
    int countFn=0;
    int countBday=0;
    int countAnni=0;
    int countFn2=0;
    int countBday2=0;
    int countAnni2=0;
    int extraFound=0;
    int retVal;
    
  
    fPtr=fopen(fileName,"r");
    if(fPtr==NULL){
        return;
    }
    while(fgets(line,500,fPtr)!=NULL){
        total=total+1;
    }
    rewind(fPtr);
    
    while(fgets(line,500,fPtr)!=NULL){
        count=count+1;

        if(count==1 || count==2 || count==total){    //ignore BEGIN VERSION and END
            continue;
        }
        else{                                       //getting first fn,bday and anniv
            // if(line[0]==' ' || line[0]=='\t'){        //skip line unfolding
            //     continue;
            // }
            line[(strlen(line)-1)]='\0';
            crlf=strchr(line,'\r');

            if(crlf==NULL){
                free(line);
                free(dupLine);
                fclose(fPtr);
            }
            *crlf ='\0';
            strcpy(dupLine,line);
            colon=strchr(line,':');
            if(colon!=NULL){
                *colon='\0';
                if (caseComp(line, "FN") || 
                caseComp(line, "BDAY") || 
                caseComp(line, "ANNIVERSARY")){

                    if(caseComp(line, "FN")){
                        countFn=countFn+1;
                        if(countFn==1){
                            countFn2=count;
                        }
                    }
                    else if(caseComp(line, "BDAY")){
                        countBday=countBday+1;
                        if(countBday==1){
                            countBday2=count;
                        }
                    }
                    else if(caseComp(line, "ANNIVERSARY")){
                        countAnni=countAnni+1;
                        if(countAnni==1){
                            countAnni2=count;
                        }
                    }
                }
            }
            semiCol=strchr(dupLine,';');
            if(semiCol!=NULL){
                *semiCol='\0';
                if(caseComp(dupLine, "BDAY")){
                    countBday=countBday+1;
                    if(countBday==1){
                        countBday2=count;
                    }
                }
            }   
        }
    }
    rewind(fPtr);

    count=0;

    while(fgets(line,500,fPtr)!=NULL){
        count=count+1;
        //removethisprintf("rrr and count is %s %d\n",line,count);
        if(count==1 || count==2 || count==total || count==countFn2 || count==countBday2
        || count==countAnni2 /*||line[0]==' ' || line[0]=='\t'*/){    //ignore BEGIN VERSION and END
            //removethisprintf("ttt and count is %s and %d\n",line,count);
            continue;
        }
    }

    rewind(fPtr);

    count=0;


    while(fgets(line,500,fPtr)!=NULL){
        count=count+1;
        //removethisprintf("qqq and count is %s %d\n",line,count);
        if(count==1 || count==2 || count==total || count==countFn2 || count==countBday2
        || count==countAnni2 /*||line[0]==' ' || line[0]=='\t'*/){    //ignore BEGIN VERSION and END
            //removethisprintf("haan and count is %s and %d\n",line,count);
            //removethisprintf("herere\n");
            continue;
        }
        else{
            dupLine3=malloc(sizeof(char)*500);
            //removethisprintf("lineeeee %s\n",line);
            

            

            line[(strlen(line)-1)]='\0';
            crlf=strchr(line,'\r');

            if(crlf==NULL){
                free(line);
                fclose(fPtr);
            }
            *crlf ='\0';
            //strcpy(foldLine,line);       //before unfolding
            //printf("foldline issss %s\n",foldLine);
            unfold(&fPtr,&line);
            if(line[0]==' '||line[0]=='\t'){        //skip that line as we already merged the lines
                free(dupLine3);
                continue;
            }
            
            // if(line[0]==' ' || line[0]=='\t'){        //skip that line as we already merged the lines
            //     continue;
            // }
            strcpy(dupLine,line);
            strcpy(dupLine3,line);
            //removethisprintf("is it here\n");
            

            optProp=malloc(sizeof(Property));
            optProp->name=NULL;
            optProp->group=NULL;
            optProp->values=initializeList(&valueToString,&deleteValue,&compareValues);
            optProp->parameters=initializeList(&parameterToString,&deleteParameter,&compareParameters);

            //removethisprintf("folde line correct %s\n",line);
            colon=strchr(line,':');
            //removethisprintf("after colon line correct %s\n",colon+1);
            

            if(colon!=NULL){
                *colon='\0';
                if(strcmp(line,"N")==0){              //special for N:
                    //removethisprintf("Yes N found\n");
                    colon=strchr(dupLine,':');
                    parserN(colon+1,optProp);
                    insertBack(optionalProperties,(void*)optProp);
                }
                else{
                    tempSemi=strchr(line,';');
                    if(tempSemi!=NULL){            //if param and value
                        *tempSemi='\0';
                        tempDot=strchr(line,'.');
                        if(tempDot!=NULL){
                            dotLine=malloc(sizeof(char)*500);
                            strcpy(dotLine,line);       //if we have a dot or group
                            dotSplit=strtok(dotLine,".");
                            dotSplit=strtok(NULL,".");
                            optProp->name=malloc(sizeof(char)*strlen(dotSplit)+1);
                            //removethisprintf("\ndotsplit heress %s\n",dotSplit);
                            strcpy(optProp->name,dotSplit);
                            free(dotLine);
                        }
                        else{
                            optProp->name=malloc(sizeof(char)*strlen(line)+1);
                            strcpy(optProp->name,line);
                        }
                    }
                    else{                          //if just value
                        tempDot=strchr(line,'.');
                        if(tempDot!=NULL){
                            dotLine=malloc(sizeof(char)*500);
                            strcpy(dotLine,line);       //if we have a dot or group
                            dotSplit=strtok(dotLine,".");
                            dotSplit=strtok(NULL,".");
                            optProp->name=malloc(sizeof(char)*strlen(dotSplit)+1);
                            strcpy(optProp->name,dotSplit);
                            free(dotLine);
                        }
                        else{
                            optProp->name=malloc(sizeof(char)*strlen(line)+1);
                            strcpy(optProp->name,line);
                        }
                    }
                    
                    //removethisprintf("optprop name is %s\n",optProp->name);
                    //removethisprintf("after line ddd %s\n",colon+1);
                    colon=strchr(dupLine,':');
                    
                    if(colon!=NULL){
                        strcpy(dupLine2,line);
                        extraVal=strchr(dupLine,':');       //check extra val
                        if(extraVal!=NULL){                
                            extraVal2=strchr(extraVal+1,';');
                            if(extraVal2!=NULL){           //if extra val
                                *extraVal2='\0';
                                //removethisprintf("\nduplica line %s\n",extraVal);
                                extraFound=1;
                            }
                        }
                        if(extraFound==1){              //if list of values
                            val=extraVal;
                        }
                        
                        val=malloc(sizeof(char)*strlen(colon+1)+1);   //if no extra val
                        strcpy(val,(colon+1));
                        //removethisprintf("\nval is sssss %s\n",val);
                        // optProp->group=malloc(sizeof(char));
                        // optProp->group[0]='\0';
                        retVal=parseGroup(line,&(optProp->group));        //parse group if exists
                        //removethisprintf("checking group is %s\n",optProp->group);
                        if(retVal==0){
                            optProp->group=malloc(sizeof(char));
                            optProp->group[0]='\0';
                            //removethisprintf("checking group is empty %s\n",optProp->group);
                        }
                        insertBack(optProp->values,(void*)val); 
                        //removethisprintf("\nwhy bro god %s\n",dupLine3);
                        parserExtrVal(dupLine3,optProp->values);     //Any extra values
                        parseParam(dupLine,optProp->parameters);
                        insertBack(optionalProperties,(void*)optProp);
                    }
                }
            }
            semiCol=strchr(line,';');
            if(semiCol!=NULL){
                *semiCol='\0';
                if(caseComp(line,"BDAY")){
                    optProp->name=malloc(sizeof(char)*strlen(line)+1);
                    strcpy(optProp->name,line);

                    semiCol=strchr(dupLine,';');

                    if(semiCol!=NULL){
                        val=malloc(sizeof(char)*strlen(semiCol+1)+1);
                        strcpy(val,(semiCol+1));
                        retVal=parseGroup(line,&(optProp->group));        //parse group if exists
                        if(retVal==0){
                            optProp->group=malloc(sizeof(char));
                            optProp->group[0]='\0';
                        }
                        //optProp->group=malloc(sizeof(char));
                        //optProp->group[0]='\0';
                        insertBack(optProp->values,(void*)val);
                        insertBack(optionalProperties,(void*)optProp);
                    }
                }
            }
            free(dupLine3);            
        }
        
        
    }
        
    free(line);
    free(dupLine);
    free(dupLine2);
    //free(foldLine);
    fclose(fPtr);
}

void parseParam(char *line,List* parameters){
    char *semiCol1;
    char *semiCol2;
    char *colon;
    char *paramType=NULL;
    char *paramVal=NULL;
    char *tempName=NULL;
    char *equal;
    char *dupLine=malloc(sizeof(char)*500);
    char *dupLine2;
    char *dupLine3=malloc(sizeof(char)*500);
    char *propName=NULL;
    Parameter *tempParam=NULL;

    strcpy(dupLine,line);
    semiCol1=strchr(dupLine,';');
    if(semiCol1!=NULL){                           //check if it has parameters
        //removethisprintf("which line is here %s\n",line);  

        // dupLine3=strchr(dupLine,';');            //getting prop name
        // *dupLine3='\0';
        // propName=malloc(sizeof(char)*strlen(dupLine3)+1);
        // strcpy(propName,dupLine3);
        // printf("sssss propName %s\n",propName);
        

        colon=strchr(dupLine,':');                //removing value
        if(colon!=NULL){
            *colon='\0';
            dupLine2=strchr(dupLine,';');
            //removethisprintf("param line ssssss %s\n",dupLine2+1);   //removing proeprty name
            semiCol1=strtok(dupLine2,";");                 //get each param with = sign
            while(semiCol1!=NULL){
                //removethisprintf("param is eachh %s\n",semiCol1);
                semiCol2=malloc(sizeof(char)*strlen(semiCol1)+1);
                strcpy(semiCol2,semiCol1);
                //removethisprintf("param is eachh semicol2 %s\n",semiCol2);

                equal=strchr(semiCol2,'=');
                tempName=malloc(sizeof(char)*strlen(semiCol2)+1);
                strcpy(tempName,semiCol2);
                paramVal=malloc(sizeof(char)*strlen(equal+1)+1);   //getting param val
                strcpy(paramVal,equal+1);

                equal=strchr(tempName,'=');
                *equal='\0';                                      //getting param name
                paramType=malloc(sizeof(char)*strlen(tempName)+1);
                strcpy(paramType,tempName);
                //removethisprintf("oaram type is ss %s\n",paramType);
                //removethisprintf("oaram val is ss %s\n",paramVal);

                tempParam=malloc(sizeof(Parameter));                     //Adding param to list
                tempParam->name=malloc(sizeof(char)*strlen(paramType)+1);
                strcpy(tempParam->name,paramType);
                tempParam->value=malloc(sizeof(char)*strlen(paramVal)+1);
                strcpy(tempParam->value,paramVal);
                insertBack(parameters,(void*)tempParam);

                //removethisprintf("param actual name is %s\n",tempParam->name);
                //removethisprintf("param actual value is %s\n",tempParam->value);



                free(paramVal);
                free(tempName);
                free(paramType);
                
                semiCol1=strtok(NULL,";");
                free(semiCol2);
                
            }
        }
        // if(propName!=NULL){                                   //checking for param after val
        //     if(case_insensitive_compare(propName,"TEL")){
        //         strcpy(dupLine3,line);
        //         colon=strchr(dupLine3,':');
        //         printf("this line issdsd %s\n",colon);

        //         semiCol3=strchr(colon+1,';');
        //         if(semiCol3!=NULL){
        //             semiCol4=malloc(sizeof(char)*strlen(semiCol3+1)+1);
        //             //printf("this line ssasa issdsd %s\n",semiCol3+1);
        //             strcpy(semiCol4,semiCol3+1);
        //             printf("this line ssasa issdsd %s\n",semiCol4);

        //             token=strtok(semiCol4,"=");
        //             tempParam2=malloc(sizeof(Parameter));
        //             tempParam2->name=malloc(sizeof(char)*strlen(token)+1);  //getting name of param
        //             strcpy(tempParam2->name,token);
        //             token=strtok(NULL,"=");                    
        //             tempParam2->value=malloc(sizeof(char)*strlen(token)+1);  //gettung value of param
        //             strcpy(tempParam2->value,token);
        //             insertBack(parameters,(void*)tempParam2);

        //             printf("temo val name is %s\n",tempParam2->name);
        //             printf("temo val val is %s\n",tempParam2->value);
                           
        //             free(semiCol4);
        //         }
        //     }
        // }
        if(propName!=NULL){
            free(propName);
        }
    }
    free(dupLine);
    free(dupLine3);
}

// void parserExtrVal(char *line,List* values){
//     char *line2=malloc(sizeof(char)*500);
//     char *col;
//     char *semiCol;
//     char *tempVal;
//     strcpy(line2,line);

//     col=strchr(line2,':');
//     if(col!=NULL){
//         semiCol=strchr(col+1,';');
//         printf("\ngood line is here %s \n",col);
//         if(semiCol!=NULL){
//             tempVal=malloc(sizeof(char)*strlen(semiCol+1)+1);
//             strcpy(tempVal,semiCol+1);
//             insertBack(values,(void*)tempVal);
//             printf("\ntempval2 is %s and length is %ld\n",tempVal,strlen(tempVal));
//         }
//     }
//     free(line2);
// }

void parserExtrVal(char *line,List* values){
    char *line2=malloc(sizeof(char)*500);
    char *col;
    int count=0;
    char *semiCol;
    char *tempVal;
    strcpy(line2,line);

    col=strchr(line2,':');
    if(col!=NULL){
        semiCol=strchr(col+1,';');
        //removethisprintf("\ngood line is here %s \n",col);
        if(semiCol!=NULL){                      //we have a list of values
            semiCol=strtok(col+1,";");
            while(semiCol!=NULL){
                count=count+1;
                //removethisprintf("semi col %s\n",semiCol);
                if(count==1){                // as 1st iteration contains the first value
                    if(line[0]=='G' && line[1]=='E' && line[2]=='N' && line[3]=='D' && line[4]=='E' &&
                        line[5]=='R' && line[7]==';'){    //special case for gender
                        // do nothing
                    }
                    else if(line[0]=='A' && line[1]=='D' && line[2]=='R'){
                        //do nothing as we need this :;Suite D2-630;2875 Laurier;Quebec;QC;G1V 2M2;Canada
                    }
                    else{
                        semiCol=strtok(NULL,";");
                        continue;
                    }
                }

    
                
                tempVal=malloc(sizeof(char)*strlen(semiCol)+1);
                strcpy(tempVal,semiCol);
                //removethisprintf("tempvalpo %s\n",tempVal);
                insertBack(values,(void*)tempVal);
                //removethisprintf("\ntempval2 is %s and length is %ld\n",tempVal,strlen(tempVal));
                semiCol=strtok(NULL,";");
            }
        }
    }
    free(line2);
}

char * getProp(char *line){               //get prop name if group exists
    char *semi;
    char *dot;
    char *dupLine=malloc(sizeof(char)*500);
    strcpy(dupLine,line);
    semi=strchr(dupLine,';');
    char *prop=NULL;
    
    if(semi!=NULL){
        *semi='\0';
        dot=strchr(dupLine,'.');
        if(dot!=NULL){
            prop=malloc(sizeof(char)*strlen(dot+1)+1);
            strcpy(prop,dot+1);
        }
    }
    free(dupLine);
    return prop;
}

int parseGroup(char *line, char **group){
    char *dot;
    char *dupLine=malloc(sizeof(char)*500);
    strcpy(dupLine,line);
    dot=strchr(dupLine,'.');
    int retVal=0;
    
    if(dot!=NULL){
        *dot='\0';
        *group=malloc(sizeof(char)*strlen(dupLine)+1);
        strcpy(*group,dupLine);
        retVal=1;
    }
    free(dupLine);
    return retVal;
}





// void parseProp(char* fileName,List* optionalProperties){
//     FILE *fPtr;
//     char *crlf;
//     char *line=malloc(sizeof(char)*500);
//     char *dupLine=malloc(sizeof(char)*500);
//     char *colon;
//     char *semiCol;
//     char *dot1;
//     char *dot2;
//     char *val;
//     Property *optProp;
//     int total=0;
//     int count=0;
    
  
//     fPtr=fopen(fileName,"r");
//     if(fPtr==NULL){
//         return;
//     }
//     while(fgets(line,500,fPtr)!=NULL){
//         total=total+1;
//     }
//     rewind(fPtr);
    
//     while(fgets(line,500,fPtr)!=NULL){
//         count=count+1;

//         if(count==1 || count==2 || count==total){    //ignore BEGIN VERSION and END
//             continue;
//         }
//         else{
//             optProp=malloc(sizeof(Property));
//             optProp->name=NULL;
//             optProp->group=NULL;
//             optProp->values=initializeList(&valueToString,&deleteValue,&compareValues);
//             optProp->parameters=initializeList(&parameterToString,&deleteParameter,&compareParameters);

//             line[(strlen(line)-1)]='\0';
//             crlf=strchr(line,'\r');

//             if(crlf==NULL){
//                 free(line);
//                 fclose(fPtr);
//             }
//             *crlf ='\0';
//             strcpy(dupLine,line);
//             dot1=strtok(line,".");                      //if group
            
//             if(dot1!=NULL){     
//                 dot2=strtok(NULL,".");                                   //second elem after .
//                 if(dot2!=NULL){
//                     optProp->group=malloc(sizeof(char)*strlen(dot1)+1);     //first elem before .
//                     strcpy(optProp->group,dot1);
//                     semiCol=strchr(dot2,';');                 //if group and param exists
//                     if(semiCol!=NULL){
//                         *semiCol='\0';                      //end it till ;
//                         optProp->name=malloc(sizeof(char)*strlen(dot2)+1);
//                         strcpy(optProp->name,dot2);         //get prop name

//                     }
//                     else{                                    //if group and no param
//                         colon=strchr(dot2,':');
//                         if(colon!=NULL){
//                             *colon='\0';
//                             optProp->name=malloc(sizeof(char)*strlen(dot2)+1);
//                             strcpy(optProp->name,dot2);         //get prop name
//                         }
//                     }
//                 }
//                 else{                      //no group
//                     optProp->group=malloc(sizeof(char));
//                     optProp->group[0]='\0';
//                     semiCol=strchr(dot1,';');     //if param present
//                     if(semiCol!=NULL){
//                         *semiCol='\0';
//                         optProp->name=malloc(sizeof(char)*strlen(dot1)+1);
//                         strcpy(optProp->name,dot1);
//                     }
//                     else{
//                         colon=strchr(dot1,':');
//                         if(colon!=NULL){
//                             *colon='\0';
//                             if(case_insensitive_compare(dot1,"FN") || 
//                                 case_insensitive_compare(dot1,"BDAY") || 
//                                 case_insensitive_compare(dot1,"ANNIVERSARY")){
//                                                                    //nothing happens
//                             }
//                             else{
//                                 optProp->name=malloc(sizeof(char)*strlen(dot1)+1);
//                                 strcpy(optProp->name,dot1);
//                             }
//                         }
//                     }
//                 }
//                 if(case_insensitive_compare(dot1,"FN") || 
//                     case_insensitive_compare(dot1,"BDAY") ||
//                     case_insensitive_compare(dot1,"ANNIVERSARY")){
//                                                                     //nothing
//                 }
//                 else{
//                     printf("put values here\n");
//                     colon=strchr(dupLine,':');
//                     if(colon!=NULL){    
//                         val=malloc(sizeof(char)*(strlen(colon+1))+1);
//                         strcpy(val,colon+1);
//                         insertBack(optProp->values,(void*)val);
//                         insertBack(optionalProperties,(void*)optProp);
//                         printf("name iswewewe %s\n",optProp->name);
//                         printf("val is %s\n",val);
                        
//                     }
//                 }
//             }
//         }
//     }

//     free(line);
//     free(dupLine);
//     fclose(fPtr);
// }

char* getAnni(char* fileName){
    FILE *fPtr=fopen(fileName,"r");
    char *line=malloc(sizeof(char)*500);
    char *dupLine=malloc(sizeof(char)*500);
    char *dupLine2=malloc(sizeof(char)*500);
    char *crlf;
    char *param;
    char *end;
    char *beforeColon;
    char *afterColon;
    char* beforeSemCol;
    char* afterSemCol;
    char *bDay=NULL;

    while(fgets(line,500,fPtr)!=NULL){
        line[(strlen(line)-1)]='\0';
        crlf=strchr(line,'\r');

        if(crlf==NULL){
            free(line);
            fclose(fPtr);
            return 0;
        }
        *crlf ='\0';
        unfold(&fPtr,&line);
        if(line[0]==' ' || line[0]=='\t'){        //skip that line as we already merged the lines
            continue;
        }
        
        strcpy(dupLine,line);
        strcpy(dupLine2,line);
        beforeSemCol=strchr(line,';');
        beforeColon=strchr(line,':');

        if(beforeSemCol!=NULL && beforeColon!=NULL){             //both param and value
            *beforeSemCol='\0';

            if(caseComp(line,"ANNIVERSARY")){
                param=strchr(dupLine2,'=');
    
                if(param!=NULL){
                    end=strchr(param,':');
                    if(end!=NULL){
                        *end='\0';
                    }
                }
                afterColon=strchr(dupLine,':');
                if(afterColon!= NULL && *(afterColon+1)!='\0'){     //extracting anniversary both parm and prop
                    bDay=malloc(sizeof(char)*(strlen(afterColon+1)+strlen(param))+1); //\0
                    if(bDay!=NULL){
                        strcpy(bDay,afterColon+1);
                        strcat(bDay,param);
                    }
                    
                }
                // if(afterColon!= NULL && *(afterColon+1)!='\0'){     //extracting anniversary both parm and prop
                //     bDay=malloc(sizeof(char)*strlen(afterColon+1)+1+1);   //comma+\0

                //     if(bDay!=NULL){
                //         strcpy(bDay,afterColon+1);
                //         strncat(bDay,&comma,1);
                //         //bDay=realloc(bDay,((sizeof(char)*strlen(afterColon+1)+1+1))+
                //         //sizeof(char)*strlen(param+1));
                //         //strcat(bDay,param);
                //     }
                // }
                break;
            }
        }

        else if(beforeSemCol!=NULL){
            *beforeSemCol='\0';

            if(caseComp(line,"ANNIVERSARY")){
                afterSemCol=strchr(dupLine,';');
                if(afterSemCol!= NULL && *(afterSemCol+1)!='\0'){     //extracting anniversary
                    bDay=malloc(sizeof(char)*strlen(afterSemCol+1)+1);

                    if(bDay!=NULL){
                        strcpy(bDay,afterSemCol+1);
                    }
                }
                break;
            }
        }
        else if(beforeColon!=NULL){
            beforeColon=strchr(line,':');

            if(beforeColon!=NULL){
                *beforeColon='\0';
            
                if(caseComp(line,"ANNIVERSARY")){
                    afterColon=strchr(dupLine,':');
                    if(afterColon!= NULL && *(afterColon+1)!='\0'){     //extracting anniversary
                        bDay=malloc(sizeof(char)*strlen(afterColon+1)+1);

                        if(bDay!=NULL){
                            strcpy(bDay,afterColon+1);
                        }
                    }
                    break;
                }
            }
        }
    }


    free(dupLine);
    free(dupLine2);
    free(line);
    fclose(fPtr);
    return bDay;           //NULL is returned if no anniversary
}

void parserN(char* line,Property* optProp){
    char *semiCol1=NULL;
    char *semiCol2=NULL;
    char *semiCol3=NULL;
    char *semiCol4=NULL;
    char *val1=NULL;     //surname
    char *val2=NULL;     //given name
    char *val3=NULL;     //additional names
    char *val4=NULL;     //honorific prefix
    char *val5=NULL;     //honorific suffix

    char *dupLine1=malloc(sizeof(char)*strlen(line)+1);
    char *dupLine2=NULL;
    char *dupLine3=NULL;
    char *dupLine4=NULL;
    char *dupLine5=NULL;
    strcpy(dupLine1,line);
    // strcpy(dupLine2,line);
    // strcpy(dupLine3,line);
    // strcpy(dupLine4,line);
    

    optProp->name=malloc(sizeof(char)*2);
    strcpy(optProp->name,"N");
    optProp->group=malloc(sizeof(char));
    optProp->group[0]='\0';

    semiCol1=strchr(dupLine1,';');

    if(semiCol1!=NULL){
        dupLine2=malloc(sizeof(char)*strlen(semiCol1+1)+1);       //N:Doe;John;;;  after Doe;
        strcpy(dupLine2,(semiCol1+1));

        semiCol2=strchr(dupLine2,';');                            //John;;;
        dupLine3=malloc(sizeof(char)*strlen(semiCol2+1)+1);
        strcpy(dupLine3,(semiCol2+1));

        semiCol3=strchr(dupLine3,';');                            //;;
        dupLine4=malloc(sizeof(char)*strlen(semiCol3+1)+1);
        strcpy(dupLine4,(semiCol3+1));

        semiCol4=strchr(dupLine4,';');                            //;
        dupLine5=malloc(sizeof(char)*strlen(semiCol4+1)+1);
        strcpy(dupLine5,(semiCol4+1));


        *semiCol1='\0';
        val1=malloc(sizeof(char)*strlen(dupLine1)+1);
        strcpy(val1,dupLine1);

        *semiCol2='\0';
        val2=malloc(sizeof(char)*strlen(dupLine2)+1);
        strcpy(val2,dupLine2);

        *semiCol3='\0';
        val3=malloc(sizeof(char)*strlen(dupLine3)+1);
        strcpy(val3,dupLine3);

        *semiCol4='\0';
        val4=malloc(sizeof(char)*strlen(dupLine4)+1);
        strcpy(val4,dupLine4);

        val5=malloc(sizeof(char)*strlen(dupLine5)+1);
        strcpy(val5,dupLine5);

        //removethisprintf("val1 : %s\n",val1);
        //removethisprintf("val2 : %s\n",val2);
        //removethisprintf("val3 : %s\n",val3);
        //removethisprintf("val4 : %s\n",val4);
        //removethisprintf("val5 : %s\n",val5);

        insertBack(optProp->values,(void*)val1);
        insertBack(optProp->values,(void*)val2);
        insertBack(optProp->values,(void*)val3);
        insertBack(optProp->values,(void*)val4);
        insertBack(optProp->values,(void*)val5);


    }
    else{                              //no list of val N:Stevenson
        val1=malloc(sizeof(char)*strlen(line)+1);
        strcpy(val1,line);
        insertBack(optProp->values,(void*)val1);

    }
    

    free(dupLine1);  
    free(dupLine2);   
    free(dupLine3);   
    free(dupLine4);  
    free(dupLine5);
}


int caseComp(const char *str1,const char *str2){   
    while(*str1 && *str2){
        if(toupper((unsigned char)*str1)!=toupper((unsigned char)*str2)) {
            return 0;           //no equal
        }
        str1=str1+1;
        str2=str2+1;
    }
    return(*str1=='\0' && *str2=='\0'); 
}

void unfold(FILE **fPtr, char **line) {
    long pos=ftell(*fPtr);  
    size_t size=500;     
    size_t len;
    size_t tempLen;
    char *temp=malloc(sizeof(char)*size);  
    char *dupLine=malloc(sizeof(char)*size);
    char *dupLine2=malloc(sizeof(char)*size);
    char *unfoldLine=malloc(sizeof(char)*size);
    char *space;

    if(fPtr==NULL||*fPtr==NULL||line==NULL||*line==NULL){
        free(temp);
        free(dupLine);
        free(dupLine2);
        free(unfoldLine);
        return;
    }
    
    if(temp==NULL||dupLine==NULL||dupLine2==NULL){
        free(temp);
        free(dupLine);
        free(dupLine2);
        free(unfoldLine);
        return;  
    }

     
    if(unfoldLine==NULL){
        free(temp);
        free(dupLine);
        free(dupLine2);
        return;  
    }

    strcpy(unfoldLine,*line);  
    len=strlen(unfoldLine);  

    while(fgets(temp,size,*fPtr)!=NULL){
        tempLen=strlen(temp);

        while((temp[tempLen-1]=='\r'||temp[tempLen-1]=='\n') && tempLen>0){
            temp[tempLen-1]='\0';       //removing CRLF chars
            tempLen=tempLen-1;
        }

        if(temp[0]==' '||temp[0]=='\t'){    //if that line should be folded
            strcpy(dupLine,temp+1);  
            strcpy(dupLine2,temp);     

            if(temp[0]==' ' && temp[1]==' ' && temp[2]==' ' && temp[3]==' '){  //if tab space
                space=dupLine2+4;
            }
            else if(temp[0]==' '){        //if just 1 space
                space=dupLine2+1;
            }


            // while(*space==' '||*space=='\t'){
            //     space=space+1;
            // }

            if(len+strlen(space)+1>size){
                size=len+strlen(space)+1;
                unfoldLine=realloc(unfoldLine,sizeof(char)*size);
                if(unfoldLine==NULL){
                    free(temp);
                    free(dupLine);
                    free(dupLine2);
                    return;  
                }
            }
            strcat(unfoldLine,space);
            len=strlen(unfoldLine);
        } 
        else{
            fseek(*fPtr,pos,SEEK_SET);
            break;
        }
    }

    free(*line);
    *line=unfoldLine;

    //removethisprintf("Folded line is: %s\n",*line);

    free(temp);
    free(dupLine);
    free(dupLine2);
}

char ** dispValid(void){
    int index=0;
    int count=0;
    Card *card=NULL;
    VCardErrorCode create;
    VCardErrorCode valid;
    DIR *cards;
    struct dirent *file;
    char *fileName=malloc(sizeof(char)*500);
    char **validList=NULL;


    cards=opendir(CDIR);
    if(cards==NULL){
        free(fileName);
        return NULL;
    }
    else{
        //count total valid vcf files
        while((file=readdir(cards))!=NULL) {
            if(file->d_name[strlen(file->d_name)-1]=='f' && file->d_name[strlen(file->d_name)-2]=='c' && file->d_name[strlen(file->d_name)-3]=='v'){
                snprintf(fileName,500,"%s/%s",CDIR,file->d_name);

                create=createCard(fileName,&card);
                if(create==OK){
                    valid=validateCard(card);
                    if(valid==OK){
                        count=count+1;
                    }
                    deleteCard(card);
                }

            }
        }

        rewinddir(cards);
        validList=malloc(sizeof(char*)*(count+1));   //extra for NULL
        if(validList==NULL){
            perror("Something went wrong");
            closedir(cards);
            return NULL;
        }

        while((file=readdir(cards))!=NULL){
            if(file->d_name[strlen(file->d_name)-1]=='f' && file->d_name[strlen(file->d_name)-2]=='c' && file->d_name[strlen(file->d_name)-3]=='v'){
                snprintf(fileName,500,"%s/%s",CDIR,file->d_name);

                create=createCard(fileName,&card);
                if(create==OK){
                    valid=validateCard(card);
                    if(valid==OK){
                        validList[index]=malloc(strlen(file->d_name)+1);
                        if(validList[index]==NULL){
                            perror("Something went wrong");
                            closedir(cards);
                            return NULL;
                        }
                        strcpy(validList[index],file->d_name);
                        index=index+1;
                        //printf("Valid: %s\n", file->d_name);
                    }
                    deleteCard(card);
                }

            }
        }
        validList[index]=NULL;
        closedir(cards);
    }
    free(fileName);
    
    return validList;
}

void freeDispValid(char** validList){
    int i=0;
    if(validList!=NULL){
        while(validList[i]!=NULL){
            free(validList[i]);  
            i=i+1;
        }
        free(validList);
    }
}

Card **validCards(void){
    int index=0;
    int count=0;
    Card *card=NULL;
    VCardErrorCode create;
    VCardErrorCode valid;
    DIR *cards;
    struct dirent *file;
    char *fileName=malloc(sizeof(char)*500);
    Card **validList=NULL;


    cards=opendir(CDIR);
    if(cards==NULL){
        free(fileName);
        return NULL;
    }
    else{
        //count total valid vcf files
        while((file=readdir(cards))!=NULL) {
            if(file->d_name[strlen(file->d_name)-1]=='f' && file->d_name[strlen(file->d_name)-2]=='c' && file->d_name[strlen(file->d_name)-3]=='v'){
                snprintf(fileName,500,"%s/%s",CDIR,file->d_name);

                create=createCard(fileName,&card);
                if(create==OK){
                    valid=validateCard(card);
                    if(valid==OK){
                        count=count+1;
                    }
                    deleteCard(card);
                }

            }
        }

        rewinddir(cards);
        validList=malloc(sizeof(Card*)*(count+1));   //extra for NULL
        if(validList==NULL){
            perror("Something went wrong");
            closedir(cards);
            return NULL;
        }

        while((file=readdir(cards))!=NULL){
            if(file->d_name[strlen(file->d_name)-1]=='f' && file->d_name[strlen(file->d_name)-2]=='c' && file->d_name[strlen(file->d_name)-3]=='v'){
                snprintf(fileName,500,"%s/%s",CDIR,file->d_name);

                create=createCard(fileName,&card);
                if(create==OK){
                    valid=validateCard(card);
                    if(valid==OK){
                        validList[index]=card;
                        index=index+1;
                        //printf("Valid: %s\n", file->d_name);
                    }
                    else{             //if that card is invalid
                        deleteCard(card);
                    }
                }

            }
        }
        validList[index]=NULL;
        closedir(cards);
    }
    free(fileName);
    
    return validList;
}

void freeValidCards(Card **validList){
    int i;
    if(validList==NULL){
        return;
    }
    for(i=0;validList[i]!=NULL;++i){
        deleteCard(validList[i]); 
    }
    free(validList); 
}

char ** dispBday(void){
    int index=0;
    int count=0;
    Card *card=NULL;
    VCardErrorCode create;
    VCardErrorCode valid;
    DIR *cards;
    struct dirent *file;
    char *fileName=malloc(sizeof(char)*500);
    char **validList=NULL;


    cards=opendir(CDIR);
    if(cards==NULL){
        free(fileName);
        return NULL;
    }
    else{
        //count total valid vcf files
        while((file=readdir(cards))!=NULL) {
            if(file->d_name[strlen(file->d_name)-1]=='f' && file->d_name[strlen(file->d_name)-2]=='c' && file->d_name[strlen(file->d_name)-3]=='v'){
                snprintf(fileName,500,"%s/%s",CDIR,file->d_name);

                create=createCard(fileName,&card);
                if(create==OK){
                    valid=validateCard(card);
                    if(valid==OK){
                        count=count+1;
                    }
                    deleteCard(card);
                }

            }
        }

        rewinddir(cards);
        validList=malloc(sizeof(char*)*(count+1));   //extra for NULL
        if(validList==NULL){
            perror("Something went wrong");
            closedir(cards);
            return NULL;
        }

        while((file=readdir(cards))!=NULL){
            if(file->d_name[strlen(file->d_name)-1]=='f' && file->d_name[strlen(file->d_name)-2]=='c' && file->d_name[strlen(file->d_name)-3]=='v'){
                snprintf(fileName,500,"%s/%s",CDIR,file->d_name);

                create=createCard(fileName,&card);
                if(create==OK){
                    valid=validateCard(card);
                    if(valid==OK){
                        validList[index]=malloc(sizeof(char)*500);
                        if(validList[index]==NULL){
                            perror("Something went wrong");
                            closedir(cards);
                            return NULL;
                        }

                        if(card->birthday!=NULL){
                            if(card->birthday->isText==1){          // only text
                                strcpy(validList[index],card->birthday->text);
                            }                                      //only time
                            else if(card->birthday->isText==0 && card->birthday->date[0]=='\0' && card->birthday->time[0]!='\0'){          
                                if(card->birthday->UTC==0){
                                    strcpy(validList[index],card->birthday->time);
                                }
                                else if(card->birthday->UTC==1){
                                    snprintf(validList[index],500,"%s (UTC)",card->birthday->time);
                                }
                            }                                            //only date
                            else if(card->birthday->isText==0 && card->birthday->date[0]!='\0' && card->birthday->time[0]=='\0'){
                                strcpy(validList[index],card->birthday->date);
                            } 
                            else if(card->birthday->isText==0 && card->birthday->date[0]!='\0' && card->birthday->time[0]!='\0'){
                                if(card->birthday->UTC==1){
                                    snprintf(validList[index],500,"Date: %s Time: %s (UTC)",card->birthday->date,card->birthday->time);
                                }
                                else if(card->birthday->UTC==0){
                                    snprintf(validList[index],500,"Date: %s Time: %s",card->birthday->date,card->birthday->time);
                                }
                            } 
                        }

                        else if(card->birthday==NULL){
                            strcpy(validList[index],"");
                        }
                        
                        index=index+1;
                    }
                    deleteCard(card);
                }
            }
        }
        validList[index]=NULL;
        closedir(cards);
    }
    free(fileName);
    
    return validList;
}

char ** dispAnni(void){
    int index=0;
    int count=0;
    Card *card=NULL;
    VCardErrorCode create;
    VCardErrorCode valid;
    DIR *cards;
    struct dirent *file;
    char *fileName=malloc(sizeof(char)*500);
    char **validList=NULL;


    cards=opendir(CDIR);
    if(cards==NULL){
        free(fileName);
        return NULL;
    }
    else{
        //count total valid vcf files
        while((file=readdir(cards))!=NULL) {
            if(file->d_name[strlen(file->d_name)-1]=='f' && file->d_name[strlen(file->d_name)-2]=='c' && file->d_name[strlen(file->d_name)-3]=='v'){
                snprintf(fileName,500,"%s/%s",CDIR,file->d_name);

                create=createCard(fileName,&card);
                if(create==OK){
                    valid=validateCard(card);
                    if(valid==OK){
                        count=count+1;
                    }
                    deleteCard(card);
                }

            }
        }

        rewinddir(cards);
        validList=malloc(sizeof(char*)*(count+1));   //extra for NULL
        if(validList==NULL){
            perror("Something went wrong");
            closedir(cards);
            return NULL;
        }

        while((file=readdir(cards))!=NULL){
            if(file->d_name[strlen(file->d_name)-1]=='f' && file->d_name[strlen(file->d_name)-2]=='c' && file->d_name[strlen(file->d_name)-3]=='v'){
                snprintf(fileName,500,"%s/%s",CDIR,file->d_name);

                create=createCard(fileName,&card);
                if(create==OK){
                    valid=validateCard(card);
                    if(valid==OK){
                        validList[index]=malloc(sizeof(char)*500);
                        if(validList[index]==NULL){
                            perror("Something went wrong");
                            closedir(cards);
                            return NULL;
                        }

                        if(card->anniversary!=NULL){
                            if(card->anniversary->isText==1){          // only text
                                strcpy(validList[index],card->anniversary->text);
                            }                                      //only time
                            else if(card->anniversary->isText==0 && card->anniversary->date[0]=='\0' && card->anniversary->time[0]!='\0'){          
                                if(card->anniversary->UTC==0){
                                    strcpy(validList[index],card->anniversary->time);
                                }
                                else if(card->anniversary->UTC==1){
                                    snprintf(validList[index],500,"%s (UTC)",card->anniversary->time);
                                }
                            }                                            //only date
                            else if(card->anniversary->isText==0 && card->anniversary->date[0]!='\0' && card->anniversary->time[0]=='\0'){
                                strcpy(validList[index],card->anniversary->date);
                            } 
                            else if(card->anniversary->isText==0 && card->anniversary->date[0]!='\0' && card->anniversary->time[0]!='\0'){
                                if(card->anniversary->UTC==1){
                                    snprintf(validList[index],500,"Date: %s Time: %s (UTC)",card->anniversary->date,card->anniversary->time);
                                }
                                else if(card->anniversary->UTC==0){
                                    snprintf(validList[index],500,"Date: %s Time: %s",card->anniversary->date,card->anniversary->time);
                                }
                            } 
                        }
                        else if(card->anniversary==NULL){
                            strcpy(validList[index],"");
                        }
                        
                        index=index+1;
                    }
                    deleteCard(card);
                }
            }
        }
        validList[index]=NULL;
        closedir(cards);
    }
    free(fileName);
    
    return validList;
}

char* Contact(Card* card){
    if(card!=NULL && card->fn!=NULL) {
        return ((char*)getFromBack(card->fn->values));
    }
    return NULL;
}

char otherProps(Card* card){
    if(card!=NULL && card->optionalProperties!=NULL){
        return (char)card->optionalProperties->length;
    }
    return 0;
}

int fileNameChange(char* oldFile,char* newFile){
    char *extenExists;
    char *oldChange=malloc(sizeof(char)*500);
    char *newChange=malloc(sizeof(char)*500);

    if(oldFile==NULL||newFile==NULL){   
        return 0;
    }
    else{
        if(oldFile[0]=='\0'||newFile[0]=='\0'){   
            return 0;
        }
        extenExists=strrchr(newFile,'.');  

        if(extenExists==NULL){             //no exten
            return 0;
        }
        else{
            if(strcmp(extenExists,".vcf")!=0 && strcmp(extenExists,".vcard")!=0){
                return 0;                   //invalid exten
            }
        }

        snprintf(oldChange,500,"cards/%s",oldFile);   //to get correct path
        snprintf(newChange,500,"cards/%s",newFile);

        if(rename(oldChange,newChange)==0){
            return 1;  // Success
        } 
        else {
            return 0; // Failure
        }
        
    }
    free(oldChange);
    free(newChange);
}

char ** sqlDispBday(void){
    int index=0;
    int count=0;
    Card *card=NULL;
    VCardErrorCode create;
    VCardErrorCode valid;
    DIR *cards;
    struct dirent *file;
    char *fileName=malloc(sizeof(char)*500);
    char **validList=NULL;
    char month[3];
    char year[5];
    char date[3];
    char hour[3];
    char min[3];
    char sec[3];

    cards=opendir(CDIR);
    if(cards==NULL){
        free(fileName);
        return NULL;
    }
    else{
        //count total valid vcf files
        while((file=readdir(cards))!=NULL) {
            if(file->d_name[strlen(file->d_name)-1]=='f' && file->d_name[strlen(file->d_name)-2]=='c' && file->d_name[strlen(file->d_name)-3]=='v'){
                snprintf(fileName,500,"%s/%s",CDIR,file->d_name);

                create=createCard(fileName,&card);
                if(create==OK){
                    valid=validateCard(card);
                    if(valid==OK){
                        count=count+1;
                    }
                    deleteCard(card);
                }

            }
        }

        rewinddir(cards);
        validList=malloc(sizeof(char*)*(count+1));   //extra for NULL
        if(validList==NULL){
            perror("Something went wrong");
            closedir(cards);
            return NULL;
        }

        while((file=readdir(cards))!=NULL){
            if(file->d_name[strlen(file->d_name)-1]=='f' && file->d_name[strlen(file->d_name)-2]=='c' && file->d_name[strlen(file->d_name)-3]=='v'){
                snprintf(fileName,500,"%s/%s",CDIR,file->d_name);

                create=createCard(fileName,&card);
                if(create==OK){
                    valid=validateCard(card);
                    if(valid==OK){
                        validList[index]=malloc(sizeof(char)*500);
                        if(validList[index]==NULL){
                            perror("Something went wrong");
                            closedir(cards);
                            return NULL;
                        }

                        if(card->birthday!=NULL){
                            if(card->birthday->isText==1){          // only text so it is empty
                                validList[index]='\0';
                            }                                      //only time so it is empty
                            else if(card->birthday->isText==0 && card->birthday->date[0]=='\0' && card->birthday->time[0]!='\0'){          
                                if(card->birthday->UTC==0){
                                    validList[index]='\0';
                                }
                                else if(card->birthday->UTC==1){
                                    validList[index]='\0';
                                }
                            }                                            //only date so NULL
                            else if(card->birthday->isText==0 && card->birthday->date[0]!='\0' && card->birthday->time[0]=='\0'){
                                validList[index]='\0';

                            } 
                            else if(card->birthday->isText==0 && card->birthday->date[0]!='\0' && card->birthday->time[0]!='\0'){
                                if(card->birthday->UTC==1){
                                    validList[index]='\0';
                                }
                                else if(card->birthday->UTC==0){
                                    year[0]=card->birthday->date[0];
                                    year[1]=card->birthday->date[1];
                                    year[2]=card->birthday->date[2];
                                    year[3]=card->birthday->date[3];
                                    year[4]='\0';

                                    month[0]=card->birthday->date[4];
                                    month[1]=card->birthday->date[5];
                                    month[2]='\0';

                                    date[0]=card->birthday->date[6];
                                    date[1]=card->birthday->date[7];
                                    date[2]='\0';

                                    hour[0]=card->birthday->time[0];
                                    hour[1]=card->birthday->time[1];
                                    hour[2]='\0';

                                    min[0]=card->birthday->time[2];
                                    min[1]=card->birthday->time[3];
                                    min[2]='\0';

                                    sec[0]=card->birthday->time[4];
                                    sec[1]=card->birthday->time[5];
                                    sec[2]='\0';

                                    snprintf(validList[index],500,"%s-%s-%s %s:%s:%s",year,month,date,hour,min,sec);
                                }
                            } 
                        }

                        else if(card->birthday==NULL){
                            strcpy(validList[index],"");
                        }
                        
                        index=index+1;
                    }
                    deleteCard(card);
                }
            }
        }
        validList[index]=NULL;
        closedir(cards);
    }
    free(fileName);
    
    return validList;
}


char ** sqlDispAnni(void){
    int index=0;
    int count=0;
    Card *card=NULL;
    VCardErrorCode create;
    VCardErrorCode valid;
    DIR *cards;
    struct dirent *file;
    char *fileName=malloc(sizeof(char)*500);
    char **validList=NULL;
    char month[3];
    char year[5];
    char date[3];
    char hour[3];
    char min[3];
    char sec[3];

    cards=opendir(CDIR);
    if(cards==NULL){
        free(fileName);
        return NULL;
    }
    else{
        //count total valid vcf files
        while((file=readdir(cards))!=NULL) {
            if(file->d_name[strlen(file->d_name)-1]=='f' && file->d_name[strlen(file->d_name)-2]=='c' && file->d_name[strlen(file->d_name)-3]=='v'){
                snprintf(fileName,500,"%s/%s",CDIR,file->d_name);

                create=createCard(fileName,&card);
                if(create==OK){
                    valid=validateCard(card);
                    if(valid==OK){
                        count=count+1;
                    }
                    deleteCard(card);
                }

            }
        }

        rewinddir(cards);
        validList=malloc(sizeof(char*)*(count+1));   //extra for NULL
        if(validList==NULL){
            perror("Something went wrong");
            closedir(cards);
            return NULL;
        }

        while((file=readdir(cards))!=NULL){
            if(file->d_name[strlen(file->d_name)-1]=='f' && file->d_name[strlen(file->d_name)-2]=='c' && file->d_name[strlen(file->d_name)-3]=='v'){
                snprintf(fileName,500,"%s/%s",CDIR,file->d_name);

                create=createCard(fileName,&card);
                if(create==OK){
                    valid=validateCard(card);
                    if(valid==OK){
                        validList[index]=malloc(sizeof(char)*500);
                        if(validList[index]==NULL){
                            perror("Something went wrong");
                            closedir(cards);
                            return NULL;
                        }

                        if(card->anniversary!=NULL){
                            if(card->anniversary->isText==1){          // only text so it is empty
                                validList[index]='\0';
                            }                                      //only time so it is empty
                            else if(card->anniversary->isText==0 && card->anniversary->date[0]=='\0' && card->anniversary->time[0]!='\0'){          
                                if(card->anniversary->UTC==0){
                                    validList[index]='\0';
                                }
                                else if(card->anniversary->UTC==1){
                                    validList[index]='\0';
                                }
                            }                                            //only date so put NULL
                            else if(card->anniversary->isText==0 && card->anniversary->date[0]!='\0' && card->anniversary->time[0]=='\0'){
                                validList[index]='\0';
                            } 
                            else if(card->anniversary->isText==0 && card->anniversary->date[0]!='\0' && card->anniversary->time[0]!='\0'){
                                if(card->anniversary->UTC==1){
                                    validList[index]='\0';
                                }
                                else if(card->anniversary->UTC==0){
                                    year[0]=card->anniversary->date[0];
                                    year[1]=card->anniversary->date[1];
                                    year[2]=card->anniversary->date[2];
                                    year[3]=card->anniversary->date[3];
                                    year[4]='\0';

                                    month[0]=card->anniversary->date[4];
                                    month[1]=card->anniversary->date[5];
                                    month[2]='\0';

                                    date[0]=card->anniversary->date[6];
                                    date[1]=card->anniversary->date[7];
                                    date[2]='\0';

                                    hour[0]=card->anniversary->time[0];
                                    hour[1]=card->anniversary->time[1];
                                    hour[2]='\0';

                                    min[0]=card->anniversary->time[2];
                                    min[1]=card->anniversary->time[3];
                                    min[2]='\0';

                                    sec[0]=card->anniversary->time[4];
                                    sec[1]=card->anniversary->time[5];
                                    sec[2]='\0';

                                    snprintf(validList[index],500,"%s-%s-%s %s:%s:%s",year,month,date,hour,min,sec);
                                }
                            } 
                        }

                        else if(card->anniversary==NULL){
                            strcpy(validList[index],"");
                        }
                        
                        index=index+1;
                    }
                    deleteCard(card);
                }
            }
        }
        validList[index]=NULL;
        closedir(cards);
    }
    free(fileName);
    
    return validList;
}

int contactChange(Card** obj,char * newContact,char* fileName){
    char *tempVal;
    List * list=(*obj)->fn->values;
    Node *del=list->tail;
    VCardErrorCode result;

    if(obj==NULL||newContact==NULL){   
        return 0;
    }
    else{
        if(newContact[0]=='\0'){        //contact can't be empty
            return 0;
        }
        else{
            if(del->previous!=NULL){
                del->previous->next=NULL;  
            } 
            else {
                list->head=NULL;  
            }
        
            list->tail=del->previous; 
        
            free(del->data);  
            free(del);        
            (list->length)=(list->length)-1;
        

            tempVal=malloc(sizeof(char)*strlen(newContact)+1);
            if(tempVal==NULL){
                return 0;
            }
            strcpy(tempVal,newContact);
            insertBack((*obj)->fn->values,(void*)tempVal);
            result=writeCardFn(fileName,(*obj));
            if(result==OK){
                return 1;
            }
            else{
                return 0;
            }
        }
    }
}

VCardErrorCode writeCardFn(const char* fileName, const Card* obj){
    FILE * fPtr;
    void *elem;
    char oldChange[500];
    Property* tempProp;
    Parameter* tempParam;
    char* tempVal;
    ListIterator optPropIter;
    ListIterator paramIter;
    ListIterator valIter;
    int count=0;

    if(fileName==NULL){            //if file can't be opened
        return WRITE_ERROR;
    }

    else if(obj==NULL){
        return WRITE_ERROR;
    }

    else{                         //file opens and name and everything is correct
        snprintf(oldChange,500,"cards/%s",fileName);
        fPtr=fopen(oldChange,"w");
        fprintf(fPtr,"BEGIN:VCARD\r\n");
        fprintf(fPtr,"VERSION:4.0\r\n");

        fprintf(fPtr,"%s:",obj->fn->name);        //printing fn
        fprintf(fPtr,"%s\r\n",(char*)getFromBack(obj->fn->values));

        if(obj->birthday!=NULL){                   //printing birthday
            if(obj->birthday->isText==0){
                fprintf(fPtr,"BDAY:");
            }
            else if(obj->birthday->isText==1){
                fprintf(fPtr,"BDAY;");
            }

            if(obj->birthday->isText==0){         //no text val just date/time
                if(obj->birthday->date[0]!='\0' && obj->birthday->time[0]=='\0'){    //only date
                    if(obj->birthday->date[0]=='-' && obj->birthday->date[1]=='-'){   //--0203
                        fprintf(fPtr,"%s\r\n",obj->birthday->date);
                    }
                    else if(strchr(obj->birthday->date,'T')==NULL && obj->birthday->date[0]!='T'){     //19960415
                        fprintf(fPtr,"%s\r\n",obj->birthday->date);
                    }
                }
                else if(obj->birthday->time[0]!='\0' && obj->birthday->date[0]=='\0'){   //only time T143000 T143000Z 
                    if(obj->birthday->UTC==1){     //UTC time
                        fprintf(fPtr,"%c%s%c\r\n",'T',obj->birthday->time,'Z');
                    }
                    else if(obj->birthday->UTC==0){     //No UTC time
                        fprintf(fPtr,"%c%s\r\n",'T',obj->birthday->time);
                    }
                }
                else if(obj->birthday->time[0]!='\0' && obj->birthday->date[0]!='\0'){   //both date and t
                    if(obj->birthday->UTC==1){     //UTC time
                        fprintf(fPtr,"%s%c%s%c\r\n",obj->birthday->date,'T',obj->birthday->time,'Z');
                    }
                    else if(obj->birthday->UTC==0){     //No UTC time
                        fprintf(fPtr,"%s%c%s\r\n",obj->birthday->date,'T',obj->birthday->time);
                    }
                
                }
            }
            else{                       //text and only 1 parameter
                fprintf(fPtr,"VALUE=text:%s\r\n",obj->birthday->text);
            }
        }

        if(obj->anniversary!=NULL){                //printing annviersary
            if(obj->anniversary->isText==0){
                fprintf(fPtr,"ANNIVERSARY:");
            }
            else if(obj->anniversary->isText==1){
                fprintf(fPtr,"ANNIVERSARY;");
            }

            if(obj->anniversary->isText==0){         //no text val just date/time
                if(obj->anniversary->date[0]!='\0' && obj->anniversary->time[0]=='\0'){    //only date
                    if(obj->anniversary->date[0]=='-' && obj->anniversary->date[1]=='-'){   //--0203
                        fprintf(fPtr,"%s\r\n",obj->anniversary->date);
                    }
                    else if(strlen(obj->anniversary->date)==8 && obj->anniversary->date[0]!='T'){     //19960415
                        fprintf(fPtr,"%s\r\n",obj->anniversary->date);
                    }
                }
                else if(obj->anniversary->time[0]!='\0' && obj->anniversary->date[0]=='\0'){   //only time T143000 T143000Z 
                    if(obj->anniversary->UTC==1){     //UTC time
                        fprintf(fPtr,"%c%s%c\r\n",'T',obj->anniversary->time,'Z');
                    }
                    else if(obj->anniversary->UTC==0){     //No UTC time
                        fprintf(fPtr,"%c%s\r\n",'T',obj->anniversary->time);
                    }
                }
                else if(obj->anniversary->time[0]!='\0' && obj->anniversary->date[0]!='\0'){   //both date and t
                    if(obj->anniversary->UTC==1){     //UTC time
                        fprintf(fPtr,"%s%c%s%c\r\n",obj->anniversary->date,'T',obj->anniversary->time,'Z');
                    }
                    else if(obj->anniversary->UTC==0){     //No UTC time
                        fprintf(fPtr,"%s%c%s\r\n",obj->anniversary->date,'T',obj->anniversary->time);
                    }
                
                }
            }
            else{                       //text and only 1 parameter
                fprintf(fPtr,"VALUE=text:%s\r\n",obj->anniversary->text);
            }

        }

        if(obj->optionalProperties->length!=0){  //opt properties present
            optPropIter=createIterator(obj->optionalProperties);

            while((elem=nextElement(&optPropIter))!=NULL){
                tempProp=(Property*)elem;

                if(tempProp->group[0]!='\0'){      //if group exists
                    fprintf(fPtr,"%s.",tempProp->group);
                }

                fprintf(fPtr,"%s",tempProp->name);   //print proeprty name

                if(tempProp->parameters->length!=0){    //if params exists
                    paramIter=createIterator(tempProp->parameters);

                    while((elem=nextElement(&paramIter))!=NULL){
                        tempParam=(Parameter*)elem;
                        fprintf(fPtr,";%s=%s",tempParam->name,tempParam->value);
                    }
                }
                
                valIter=createIterator(tempProp->values);   //printing values

                if(tempProp->values->length==1){                 //if just one value
                    while((elem=nextElement(&valIter))!=NULL){
                        tempVal=(char*)elem;
                        fprintf(fPtr,":%s\r\n",tempVal);
                    }
                }
                else if(tempProp->values->length>=1){          //if a list of values
                    count=0;
                    while((elem=nextElement(&valIter))!=NULL){
                        count=count+1;
                        tempVal=(char*)elem;
    
                        if(count==1){
                            fprintf(fPtr,":%s",tempVal);
                        }
                        else{
                            fprintf(fPtr,";%s",tempVal);
                        }
                    }
                    fprintf(fPtr,"\r\n");
                }
            }
        }   


        fprintf(fPtr,"END:VCARD\r\n");
        

        fclose(fPtr);
        return OK;
    }
}

int createCardSql(char* fileName,char* contact){
    int exten;
    int i=0;
    int dup=0;
    char **valid=NULL;
    char *tempName;
    Card* obj;
    VCardErrorCode result;

    exten=validExtenSql(fileName);

    if(exten==0){          //invalid extension
        return -1;
    }
    else if(contact[0]=='\0'){    //contatct name is empty
        return 0;
    }

    else{
        valid=dispValid();
        while(valid[i]!=NULL){
            if(strcmp(fileName,valid[i])==0){
                dup=1;
                break;
            }
            i=i+1;
        }
        freeDispValid(valid);
        if(dup==0){
            obj=malloc(sizeof(Card));
            obj->fn=malloc(sizeof(Property));
            obj->fn->name=malloc(sizeof(char)*2+1);
            obj->fn->group=malloc(sizeof(char));
            obj->fn->parameters=initializeList(&parameterToString,&deleteParameter,&compareParameters);
            obj->fn->values=initializeList(&valueToString,&deleteValue,&compareValues);
            obj->optionalProperties=initializeList(&propertyToString,&deleteProperty,&compareProperties);
            obj->birthday=NULL;
            obj->anniversary=NULL;

            strcpy(obj->fn->name,"FN");           //inserting that name
            obj->fn->group[0]='\0';
            tempName=malloc(sizeof(char)*strlen(contact)+1);
            strcpy(tempName,contact);
            insertBack(obj->fn->values,(void*)tempName);

            result=writeCardFn(fileName,obj);
            if(result==OK){
                return 1;
            }
            else{
                return 0;
            }
        }
        else{
            return -2;
        }
        
    }

}

int validExtenSql(const char* fileName){        
    char *extenExists;
    int retVal=1;

    if(fileName==NULL){   
        //printf("filename\n");
        return 0;
    }
    else{
        if(fileName[0]=='\0') {   
            //printf("filename 0\n");
            return 0;
        }
        extenExists=strrchr(fileName,'.');  

        if(extenExists==NULL){             //no exten
            //printf("extenexists\n");
            return 0;
        }
        else{
            if(strcmp(extenExists,".vcf")!=0 && strcmp(extenExists,".vcard")!=0){
                return 0;                   //invalid exten
            }
        }
        return retVal;
    }
}

    
    
