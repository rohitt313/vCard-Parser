#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VCParser.h"
#include "VCHelpers.h"
#include "LinkedListAPI.h"


VCardErrorCode createCard(char* fileName, Card** obj){
    int resExten;
    int resCard;
    char *fnProp;
    char *fnPropTempName;
    char *objBday;
    char *objAnniv;
    FILE *fPtr;
    VCardErrorCode error;

    resExten=validExten(fileName);
    if(resExten==0){               //validiting extension
        *obj=NULL;
        return INV_FILE;
    }
    else{                         //correct extension checking format of card
        resCard=validCard(fileName);
        if(resCard==-2){          //if begin has no value
            *obj=NULL;
            error=INV_PROP;
        }
        else if(resCard==-3){
            *obj=NULL;
            error=INV_DT;         //if there was error opening it 
        }
        else if(resCard==-1){
            *obj=NULL;
            error=INV_FILE;         //if there was error opening it 
        }
        else if(resCard==0){       //if card invalid like start or end missing or verison
            *obj=NULL;
            error=INV_CARD;
        }     
        else if(resCard==1){       //card format is valid
            error=OK;
        }
        
    }
    if(error==OK){
        
        
        fPtr=fopen(fileName,"r");

        *obj=malloc(sizeof(Card));
        if(*obj==NULL){
            return OTHER_ERROR;
        }
        else{
            (*obj)->fn=malloc(sizeof(Property));
            

            if((*obj)->fn == NULL){
                free(*obj);
                *obj=NULL;
                return OTHER_ERROR;
            }
    
            else{
                (*obj)->fn->name=malloc(sizeof(char)*2+1);
                (*obj)->fn->group=malloc(sizeof(char));
                (*obj)->fn->parameters=initializeList(&parameterToString,&deleteParameter,&compareParameters);
                (*obj)->fn->values=initializeList(&valueToString,&deleteValue,&compareValues);
                (*obj)->optionalProperties=initializeList(&propertyToString,&deleteProperty,&compareProperties);
                (*obj)->birthday=NULL;
                (*obj)->anniversary=NULL;

                
                // (*obj)->anniversary=malloc(sizeof(DateTime));
                // if ((*obj)->anniversary != NULL) {
                //     (*obj)->anniversary->UTC = 0;         
                //     (*obj)->anniversary->isText = 0;      
                //     (*obj)->anniversary->date = NULL;     
                //     (*obj)->anniversary->time = NULL;     
                //     (*obj)->anniversary->text = NULL;     
                // }
                
                //parser FN
                strcpy((*obj)->fn->name,"FN");
                (*obj)->fn->group[0]='\0';
                fnProp=parseFn(fileName,(*obj)->fn->parameters);
                fnPropTempName=malloc(sizeof(char)*strlen(fnProp)+1);
                strcpy(fnPropTempName,fnProp);
                if(fnPropTempName[strlen(fnPropTempName)-1]=='1'){     //diff cases for fn
                    fnPropTempName[strlen(fnPropTempName)-1]='\0';
                    strcpy((*obj)->fn->name,"FN");
                }
                else if(fnPropTempName[strlen(fnPropTempName)-1]=='2'){
                    fnPropTempName[strlen(fnPropTempName)-1]='\0';
                    strcpy((*obj)->fn->name,"fn");
                }
                else if(fnPropTempName[strlen(fnPropTempName)-1]=='3'){
                    fnPropTempName[strlen(fnPropTempName)-1]='\0';
                    strcpy((*obj)->fn->name,"Fn");
                }
                else if(fnPropTempName[strlen(fnPropTempName)-1]=='4'){
                    fnPropTempName[strlen(fnPropTempName)-1]='\0';
                    strcpy((*obj)->fn->name,"fN");
                }

                insertBack((*obj)->fn->values,(void*)fnPropTempName);
                //removethisprintf("Ffull name is %s\n",fnPropTempName);
                //removethisprintf("Ffull name is case sens %s\n",(*obj)->fn->name);
                
                objBday=getBday(fileName);       //check if bday exists

                if(objBday!=NULL){
                    //removethisprintf("Ffull bday is %s\n",objBday);
                    (*obj)->birthday=malloc(sizeof(DateTime));
                    if ((*obj)->birthday!= NULL) {
                        (*obj)->birthday->UTC = 0;         
                        (*obj)->birthday->isText = 0;      
                        (*obj)->birthday->date = NULL;     
                        (*obj)->birthday->time = NULL;     
                        (*obj)->birthday->text = NULL;     
                    }
                    parseDateTime(objBday,(*obj)->birthday);
                    //removethisprintf("val is time %s\n",(*obj)->birthday->time);
                    char *aaa=dateToString((void*)(*obj)->birthday);
                    //removethisprintf("date is string %s\n",aaa);
                    free(aaa);
                }

                objAnniv=getAnni(fileName);                      //check if anni exists
                if(objAnniv!=NULL){
                    //removethisprintf("Ffull anniversary is %s\n",objAnniv);
                    (*obj)->anniversary=malloc(sizeof(DateTime));
                    if ((*obj)->anniversary!= NULL) {
                        (*obj)->anniversary->UTC = 0;         
                        (*obj)->anniversary->isText = 0;      
                        (*obj)->anniversary->date = NULL;     
                        (*obj)->anniversary->time = NULL;     
                        (*obj)->anniversary->text = NULL;     
                    }
                    parseDateTime(objAnniv,(*obj)->anniversary);
                    //removethisprintf("val is time %s\n",(*obj)->anniversary->time);
                    char *bbb=dateToString((void*)(*obj)->anniversary);
                    //removethisprintf("date is %s\n",bbb);
                    free(bbb);
                }

                parseProp(fileName,(*obj)->optionalProperties);  //parse proeprties
                // char*roh=propertyToString((void*) (*obj)->optionalProperties);
                // printf("property is %s\n",roh);
                // free(roh);
                

                free(fnProp);
                free(objBday);    //free all memory
                free(objAnniv);

                
                
                
                
                
                
                
                
                

                //testing print param,values and date
                // Parameter* tmpParam;
                // char *tempVal;
                // tempVal=malloc(sizeof(char)*20);
                // tmpParam=malloc(sizeof(Parameter));
                // tmpParam->name=malloc(sizeof(char)*20);
                // tmpParam->value=malloc(sizeof(char)*20);
                // strcpy(tmpParam->name,"Rohit");
                // strcpy(tmpParam->value,"Dalal");
                // strcpy(tempVal,"tempVal");
                // insertBack( (*obj)->fn->parameters, (void*)tmpParam);
                // insertBack( (*obj)->fn->values, (void*)tempVal);

                

                //testing print property

                
                //char *tempVal2;
                //tempVal2=malloc(sizeof(char)*20);
                //tmpParam2=malloc(sizeof(Parameter));
                //tmpParam2->name=malloc(sizeof(char)*20);
                //tmpParam2->value=malloc(sizeof(char)*20);
                //strcpy(tmpParam2->name,"Rohit");
                //strcpy(tmpParam2->value,"Dalal");
                //strcpy(tempVal2,"tempVal");
                //insertBack( (*obj)->fn->values, (void*)tempVal2);











                // (*obj)->fn->parameters=initializeList(&parameterToString,&deleteParameter,&compareParameters);
                // (*obj)->fn->values=initializeList(&valueToString,&deleteValue,&compareValues);
                // (*obj)->fn->name=malloc(sizeof(char)*2+1);
                // (*obj)->fn->group=malloc(sizeof(char)*13);
                // (*obj)->birthday=malloc(sizeof(DateTime));
                // (*obj)->birthday->date=malloc(sizeof(char)*20);
                // (*obj)->birthday->time=malloc(sizeof(char)*20);
                // (*obj)->birthday->text=malloc(sizeof(char)*20);

                
                // (*obj)->anniversary=malloc(sizeof(DateTime));
                // (*obj)->anniversary->date=malloc(sizeof(char)*20);
                // (*obj)->anniversary->time=malloc(sizeof(char)*20);
                // (*obj)->anniversary->text=malloc(sizeof(char)*20);
                // (*obj)->optionalProperties=initializeList(&propertyToString,&deleteProperty,&compareProperties);

                // //testing print param,values and date
                // // Parameter* tmpParam;
                // // char *tempVal;
                // // tempVal=malloc(sizeof(char)*20);
                // // tmpParam=malloc(sizeof(Parameter));
                // // tmpParam->name=malloc(sizeof(char)*20);
                // // tmpParam->value=malloc(sizeof(char)*20);
                // // strcpy(tmpParam->name,"Rohit");
                // // strcpy(tmpParam->value,"Dalal");
                // // strcpy(tempVal,"tempVal");
                // // insertBack( (*obj)->fn->parameters, (void*)tmpParam);
                // // insertBack( (*obj)->fn->values, (void*)tempVal);

                // (*obj)->birthday->UTC=true;
                // (*obj)->birthday->isText=true;
                // strcpy((*obj)->birthday->date,"date");
                // strcpy((*obj)->birthday->time,"time");
                // strcpy((*obj)->birthday->text,"text");

                // //testing print property

                // strcpy((*obj)->fn->name,"FN");
                // strcpy((*obj)->fn->group,"Group:Ro");
                // Parameter* tmpParam2;
                // //char *tempVal2;
                // //tempVal2=malloc(sizeof(char)*20);
                // char *tempVal3;
                // tempVal3=malloc(sizeof(char)*strlen(fnProp)+1);
                // tmpParam2=malloc(sizeof(Parameter));
                // tmpParam2->name=malloc(sizeof(char)*20);
                // tmpParam2->value=malloc(sizeof(char)*20);
                // strcpy(tmpParam2->name,"Rohit");
                // strcpy(tmpParam2->value,"Dalal");
                // //strcpy(tempVal2,"tempVal");
                // strcpy(tempVal3,fnProp);
                // insertBack( (*obj)->fn->parameters, (void*)tmpParam2);
                // //insertBack( (*obj)->fn->values, (void*)tempVal2);
                // insertBack( (*obj)->fn->values, (void*)tempVal3);






                


                //strcpy((*obj)->fn->name,"Rohit Dalal");
                //strcpy((*obj)->fn->group,"Student");

                //Parameter *tempParam;
                // Property *tempProp;
                //char *tempValue;


                //tempValue=malloc(sizeof(char)*13);
                //tempParam=malloc(sizeof(Parameter));
                // tempProp=malloc(sizeof(Property));
                // tempProp->parameters=initializeList(&parameterToString,&deleteParameter,&compareParameters);
                // tempProp->values=initializeList(&valueToString,&deleteValue,&compareValues);

                //strcpy(tempValue,"FN v Rohit");
                //tempParam->name=malloc(sizeof(char)*13);
                //tempParam->value=malloc(sizeof(char)*13);
                //strcpy(tempParam->name,"PN Rohit");
                //strcpy(tempParam->value,"PV Rohit");

                // tempProp->name=malloc(sizeof(char)*13);
                // tempProp->group=malloc(sizeof(char)*13);

                // Parameter *tempParam2;
                // Property *tempProp2;


                
                // strcpy(tempProp->name,"pr Rohit");
                // strcpy(tempProp->group,"prG Rohit");


                //insertBack((*obj)->fn->parameters, (void*)tempParam);
                //insertBack((*obj)->fn->values, (void*)tempValue);


        
                // line=malloc(sizeof(char)*500);     //checking ENDCARD
                // if(fgets(line,500,fPtr)!=NULL){
                //     line[(strlen(line)-1)]='\0';
                //     crlf=strchr(line,'\r');

                //     if(crlf!=NULL){
                //         *crlf ='\0';  // Replace '\r' with the null terminator
                //     }
                //     (*obj)->fn->name=malloc(sizeof(char)*3);
                //     strcpy((*obj)->fn->name,"ab");




                    // (*obj)->fn->name=malloc(sizeof(char)*3);
                    // strcpy((*obj)->fn->name,"FN");
                    // (*obj)->fn->group="";
                    // property=malloc(sizeof(char)*100);
                    // //(*obj)->fn->parameters="";
                    // (*obj)->fn->values=malloc(sizeof(List));

                    // (*obj)->fn->values=initializeList(printData,deleteData, compare);
                    

                    // propColon=strchr(line,':');

                    // if(propColon!=NULL){
                    //     propColon=propColon+1;
                    //     for(i=(propColon-line);i<strlen(line);++i){
                    //         property[i]=line[i];
                    //     }
                    // }
                    // insertBack((*obj)->fn->values,property);

                //}
                // freeList((*obj)->fn->values);
                //free((*obj)->fn);
                //free((*obj));
                // free(property);
                //free(line);
                //free((*obj)->fn->name);
                
            }
        }
        fclose(fPtr);
        
    }
    return error;
}

VCardErrorCode writeCard(const char* fileName, const Card* obj){
    FILE * fPtr;
    int resExten;
    void *elem;
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
    
    resExten=validExten2(fileName);
    if(resExten==0){               //validiting extension
        return WRITE_ERROR;
    }

    else if(obj==NULL){
        return WRITE_ERROR;
    }

    else{                         //file opens and name and everything is correct
        fPtr=fopen(fileName,"w");
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


VCardErrorCode validateCard(const Card* obj){
    int i;
    int count;
    int adrCount;
    int genderCount;
    int fnValid;
    int bdayValid;
    int anniValid;
    int optpropValid;
    int flag=1;
    void *elem;
    void *elem2;
    char *tempValue;
    Property* tempProp;
    Parameter* tempParameter;
    ListIterator optPropValue;
    ListIterator optPropIter;
    ListIterator optPropIter2;
    ListIterator optPropIter3;
    ListIterator optPropIter4;
    ListIterator optPropIter5;
    ListIterator optParamProp;

    const char *props[]={"BEGIN","VERSION","END","SOURCE","KIND","XML","FN","N","NICKNAME","PHOTO",
    "BDAY","ANNIVERSARY","GENDER","ADR","TEL","EMAIL","IMPP","LANG","TZ","GEO","TITLE","ROLE","LOGO",
    "ORG","MEMBER","RELATED","CATEGORIES","NOTE","PRODID","REV","SOUND","UID","CLIENTPIDMAP","URL",
    "KEY","FBURL","CALADRURI","CALURI"};

    int cardinality[38]={0,0,0,2,1,2,2,1,2,2,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,1,2,2,2,2,2,2};    //2 means more than 1
    int propCount[38]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int valCardinality[38]={0,0,0,1,1,1,1,5,2,1,1,1,3,7,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,1,1,1};
    //int valCardinality2[38]={0,0,0,1,1,1,1,5,50,1,1,1,2,7,50,1,1,1,1,1,1,1,1,50,1,1,50,1,1,1,1,1,2,1,1,1,1,1};

    //after 3 is ADR
    //CLIENTPIDMAP exact 2 values
    //GENDER 1 or 2 values

    //NICKNAME WITH COMMAS
    //CATEGORIES WITH COMMAS
    

    if(obj==NULL){
        return INV_CARD;
    }
    else{
        //fn name always has a value, never group, may have parameters, always only 1 value
        if(obj->fn==NULL){
            return INV_CARD;
        }
        if(obj->fn!=NULL){
            if(obj->fn->name==NULL || obj->fn->group==NULL || obj->fn->parameters==NULL
            || obj->fn->values==NULL){
                return INV_PROP;
            }
            else if(obj->fn->name[0]=='\0' || obj->fn->group[0]!='\0' || obj->fn->values->length>1){
                return INV_PROP;
            } 
            else if(obj->fn->name[0]!='\0' && obj->fn->group[0]=='\0' && obj->fn->parameters->length>=0 
            && obj->fn->values->length==1){
                if (((char*)getFromBack(obj->fn->values))[0]=='\0'){
                    return INV_PROP;
                }
                fnValid=1;
            }         
        }   

        if(obj->birthday==NULL || obj->birthday!=NULL){
            bdayValid=1;
        }

        if(obj->anniversary==NULL || obj->anniversary!=NULL){
            anniValid=1;
        }
 
        //validating option properties
        if(obj->optionalProperties==NULL){
            return INV_CARD;
        }

        if(obj->optionalProperties->length>=0){         //0 or more optprop/s
            optpropValid=1;
        }
        
        
        //Validating Card struct implementation constraints 
        if(fnValid==1 && bdayValid==1 && anniValid==1 && optpropValid==1){
            //removethisprintf("all is good in the cosmos\n");
            //removethisprintf("all is good in the cosmos %d\n",obj->optionalProperties->length);

            if(obj->birthday!=NULL){              //validating birthday
                //removethisprintf("yesss\n");
                if(obj->birthday->date==NULL || obj->birthday->time==NULL || obj->birthday->text==NULL){
                    return INV_DT;
                }

                else if(obj->birthday->isText==1){          //if date is text
                    if(obj->birthday->UTC==1 ||obj->birthday->date[0]!='\0' || obj->birthday->time[0]!='\0'
                    || obj->birthday->text[0]=='\0'){            //if utc is 1 for text datetime
                        return INV_DT;
                    }
                }
                else{                               //date and or time 
                    if(obj->birthday->date[0]=='\0' && obj->birthday->time[0]!='\0'){     //we just have time
                        if(strlen(obj->birthday->time)!=6 || obj->birthday->text[0]!='\0'){
                            return INV_DT;
                        }
                    }
                    else if(obj->birthday->time[0]=='\0' && obj->birthday->date[0]!='\0'){     //we just have date
                        if(strlen(obj->birthday->date)!=8 || obj->birthday->text[0]!='\0' || obj->birthday->UTC!=0){ 
                            if(strlen(obj->birthday->date)==6 && obj->birthday->date[0]=='-'   //special case for date
                            && obj->birthday->date[1]=='-'){
                                //do nothing
                            }
                            else{
                                return INV_DT;
                            }
                        }
                    }
                    else if(obj->birthday->date[0]!='\0' && obj->birthday->time[0]!='\0'){    //both date and time  
                        //removethisprintf("noice\n");
                        if(strlen(obj->birthday->date)!=8 || obj->birthday->text[0]!='\0' ||
                        strlen(obj->birthday->time)!=6){
                            return INV_DT;
                        }
                    }
                    // if(obj->birthday->date[0]=='\0' && obj->birthday->text[0]=='\0' && obj->birthday->time[0]!='\0'){     //we just have time
                    //     if(strlen(obj->birthday->time)!=6 || obj->birthday->text[0]!='\0'){
                    //         return INV_DT;
                    //     }
                    // }
                    // else if(obj->birthday->time[0]=='\0' && obj->birthday->text[0]=='\0' && obj->birthday->date[0]!='\0'){     //we just have date
                    //     if(strlen(obj->birthday->date)!=8 || obj->birthday->text[0]!='\0' || obj->birthday->UTC!=0){ 
                    //         if(strlen(obj->birthday->date)==6 && obj->birthday->date[0]=='-'   //special case for date
                    //         && obj->birthday->date[1]=='-'){
                    //             //do nothing
                    //         }
                    //         else{
                    //             return INV_DT;
                    //         }
                    //     }
                    // }
                    // else if(obj->birthday->date[0]!='\0' && obj->birthday->text[0]=='\0' &&    //both date and time
                    //     obj->birthday->time[0]!='\0'){
                        
                    //     printf("noice\n");
                    //     if(strlen(obj->birthday->date)!=8 || strlen(obj->birthday->time)!=6){
                    //         return INV_DT;
                    //     }
                    // }
                }
            }

            if(obj->anniversary!=NULL){              //validating anniversary
                //removethisprintf("yesss\n");
                if(obj->anniversary->date==NULL || obj->anniversary->time==NULL || obj->anniversary->text==NULL){
                    return INV_DT;
                }

                else if(obj->anniversary->isText==1){          //if date is text
                    if(obj->anniversary->UTC==1 ||obj->anniversary->date[0]!='\0' || obj->anniversary->time[0]!='\0'
                    || obj->anniversary->text[0]=='\0'){            //if utc is 1 for text datetime
                        return INV_DT;
                    }
                }
                else{                               //date and or time 
                    if(obj->anniversary->date[0]=='\0' && obj->anniversary->time[0]!='\0'){     //we just have time
                        if(strlen(obj->anniversary->time)!=6 || obj->anniversary->text[0]!='\0'){
                            return INV_DT;
                        }
                    }
                    else if(obj->anniversary->time[0]=='\0' && obj->anniversary->date[0]!='\0'){     //we just have date
                        if(strlen(obj->anniversary->date)!=8 || obj->anniversary->text[0]!='\0' || obj->anniversary->UTC!=0){ 
                            if(strlen(obj->anniversary->date)==6 && obj->anniversary->date[0]=='-'   //special case for date
                            && obj->anniversary->date[1]=='-'){
                                //do nothing
                            }
                            else{
                                return INV_DT;
                            }
                        }
                    }
                    else if(obj->anniversary->date[0]!='\0'  && obj->anniversary->time[0]!='\0'){    //both date and time
                        //removethisprintf("noice\n");
                        if(strlen(obj->anniversary->date)!=8 || obj->anniversary->text[0]!='\0'
                        ||strlen(obj->anniversary->time)!=6){
                            return INV_DT;
                        }
                    }
                    // if(obj->anniversary->date[0]=='\0' && obj->anniversary->text[0]=='\0' && obj->anniversary->time[0]!='\0'){     //we just have time
                    //     if(strlen(obj->anniversary->time)!=6 || obj->anniversary->text[0]!='\0'){
                    //         return INV_DT;
                    //     }
                    // }
                    // else if(obj->anniversary->time[0]=='\0' && obj->anniversary->text[0]=='\0' && obj->anniversary->date[0]!='\0'){     //we just have date
                    //     if(strlen(obj->anniversary->date)!=8 || obj->anniversary->text[0]!='\0' || obj->anniversary->UTC!=0){ 
                    //         if(strlen(obj->anniversary->date)==6 && obj->anniversary->date[0]=='-'   //special case for date
                    //         && obj->anniversary->date[1]=='-'){
                    //             //do nothing
                    //         }
                    //         else{
                    //             return INV_DT;
                    //         }
                    //     }
                    // }
                    // else if(obj->anniversary->date[0]!='\0' && obj->anniversary->text[0]=='\0' &&    //both date and time
                    //     obj->anniversary->time[0]!='\0'){
                        
                    //     printf("noice\n");
                    //     if(strlen(obj->anniversary->date)!=8 || strlen(obj->anniversary->time)!=6){
                    //         return INV_DT;
                    //     }
                    // }
                }
            }


            if(obj->optionalProperties->length>0){         //validating property names
                optPropIter=createIterator(obj->optionalProperties);

                while((elem=nextElement(&optPropIter))!=NULL){
                    tempProp=(Property*)elem;

                    if(tempProp->name==NULL || tempProp->group==NULL || 
                        tempProp->parameters==NULL || tempProp->values==NULL){
                        return INV_PROP;
                    }

                    for(i=0;i<38;++i){
                        if(strcmp(props[i],tempProp->name)==0){
                            flag=1;
                            if(strcmp("BEGIN",tempProp->name)==0){          //they can't be proeprty names
                                return INV_PROP;
                            }
                            else if(strcmp("VERSION",tempProp->name)==0){
                                return INV_CARD;
                            }
                            else if(strcmp("END",tempProp->name)==0){
                                return INV_PROP;
                            }
                            break;
                        }
                        else if(strcmp(props[i],tempProp->name)!=0){    //not a valid propery name
                            flag=0;
                        }

                        if(i==37 && flag==0){          //if that prop does not match any defined
                            return INV_PROP;
                        }
                        
                    }
                }

                optPropIter2=createIterator(obj->optionalProperties);     //validating cardinality
                while((elem=nextElement(&optPropIter2))!=NULL){
                    tempProp=(Property*)elem;
                    for(i=0;i<38;++i){
                        if(strcmp(tempProp->name,props[i])==0){
                            propCount[i]=propCount[i]+1;
                        }
                    }
                }

                for(i=0;i<38;++i){
                    if(i==0 || i==1 || i==2){    //dont check for begin version and end
                        continue;
                    }
                    else if(i==10 || i==11 ){    //special errror if bday/anni in optprop
                        if(propCount[i]!=0){
                            return INV_DT;
                        }
                    }
                    else{
                        if(cardinality[i]==1){
                            if(propCount[i]!=0 && propCount[i]!=1){    //propert that has cardinality 1 comes more than 1
                                //removethisprintf("at this index pos %d and count %d\n",i,propCount[i]);
                                return INV_PROP;
                            }
                        }
                    }
                    //removethisprintf("prop name %s and count %d\n",props[i],propCount[i]);
                }
                //removethisprintf("cardinialty verifies\n");

                //validating parameters
                optPropIter3=createIterator(obj->optionalProperties);     
                while((elem=nextElement(&optPropIter3))!=NULL){        //go through every optprop
                    tempProp=(Property*)elem;
                    if(tempProp->parameters->length>0){
                        optParamProp=createIterator(tempProp->parameters);
                        while((elem2=nextElement(&optParamProp))!=NULL){    // see that optptop param
                            tempParameter=(Parameter*)elem2;

                            if(tempParameter->name[0]=='\0' || tempParameter->value[0]=='\0'){
                                return INV_PROP;
                            }
                        }
                    }
                }
   
                //validating values
                optPropIter4=createIterator(obj->optionalProperties);     
                while((elem=nextElement(&optPropIter4))!=NULL){        //go through every optprop
                    count=0;
                    adrCount=0;
                    genderCount=0;
                    tempProp=(Property*)elem;
                    optPropValue=createIterator(tempProp->values);

                    if(tempProp->values->length==0){
                        return INV_PROP;
                    }
                    

                    while((elem2=nextElement(&optPropValue))!=NULL){    // see that optptop value
                        
                        tempValue=(char*)elem2;
                        count=count+1;
                        adrCount=adrCount+1;
                        genderCount=genderCount+1;
                        if(tempValue[0]=='\0'){
                            if(strcmp(tempProp->name,"N")==0){
                                if(count==1 && tempValue[0]=='\0'){       //1st val of N alwyas has val    
                                    return INV_PROP;
                                }
                            }
                            else if(strcmp(tempProp->name,"ADR")==0){
                                if(count==1 && tempValue[0]=='\0'){   //1st val of ADR is empty
                                }
                            }
                            else if(strcmp(tempProp->name,"GENDER")==0){    //for gender
                                if(tempProp->values->length==2){
                                    if(genderCount==1){
                                        if(tempValue[0]=='\0'){
                                            // do nothing as GENDER:;random is valid
                                        }
                                    }
                                    else if(genderCount==2){
                                        if(tempValue[0]=='\0'){
                                            return INV_PROP;
                                        }
                                    }
                                }
                                // if(tempProp->values->length==2){
                                    
                                //     gender1=malloc(sizeof(char)*strlen((char*)getFromBack(tempProp->values))+1);          
                                //     strcpy(gender1,(char*)getFromBack(tempProp->values));

                                //     gender2=malloc(sizeof(char)*strlen((char*)getFromFront(tempProp->values))+1);          
                                //     strcpy(gender2,(char*)getFromFront(tempProp->values));
                                // }
                                // else if(tempProp->values->length==1){
                                    
                                //     gender1=malloc(sizeof(char)*strlen((char*)getFromBack(tempProp->values))+1);          
                                //     strcpy(gender1,(char*)getFromBack(tempProp->values));
                                // }

                                // if(tempProp->values->length==1){
                                //     if(gender1[0]=='\0'){
                                //         free(gender1);
                                //         return INV_PROP;
                                //     }
                                // }
                                // else if(tempProp->values->length==2){
                                //     if(gender1[0]=='\0' && gender2[0]=='\0'){
                                //         return INV_PROP;
                                //     }
                                //     else if(gender2[0]=='\0' && gender1[0]!='\0'){
                                //         return INV_PROP;
                                //     }
                                // }
                            }
                            else{
                                return INV_PROP;
                            }
                        }
                    }
                }

                //validating list of values
                optPropIter5=createIterator(obj->optionalProperties);     
                while((elem=nextElement(&optPropIter5))!=NULL){        //go through every optprop
                    tempProp=(Property*)elem;

                    for(i=0;i<38;++i){
                        if(strcmp(props[i],tempProp->name)==0){
                            // if(valCardinality2[i]!=tempProp->values->length){      //since gender card is defined as 3
                            //     printf("wrong here %s\n",tempProp->name);
                            //     if(valCardinality2[i]==50){
                            //         continue;
                            //     }
                            //     else if(valCardinality2[i]==2 && tempProp->values->length<=2
                            //     && strcmp(tempProp->name,"GENDER")==0){
                            //         continue;
                            //     }
                            //     else{
                            //         return INV_PROP;
                            //     }     
                            // }  
                            if(valCardinality[i]!=tempProp->values->length || i==12){      //since gender card is defined as 3
                                //removethisprintf("wrong here %s\n",tempProp->name);
                                if(valCardinality[i]==3 && tempProp->values->length<=2){   //gender has 1/2 val
                                    continue;
                                }
                                else if(i!=32 && i!=7 && valCardinality[i]==2){       //2 means more than one value
                                    continue;
                                }
                                else{
                                    return INV_PROP;
                                }     
                            }          
                        }
                    }    
                }
            }
        }
        else{
            return INV_CARD;
        }   
        return OK;
    }
}

void deleteCard(Card* obj){
    if(obj==NULL){
        return;
    }
    else{
        deleteProperty(obj->fn);
        freeList(obj->optionalProperties);
        deleteDate(obj->birthday);
        deleteDate(obj->anniversary);
        free(obj);  
    }
}

void deleteProperty(void* toBeDeleted){
    Property *prop;
    if(toBeDeleted==NULL){
        return;
    }
    else{
        prop=(Property*)toBeDeleted;
        free(prop->name);
        free(prop->group);
        freeList(prop->parameters);
        freeList(prop->values);
        free(prop);

    }
}

void deleteParameter(void* toBeDeleted){
    Parameter *param;
    if(toBeDeleted==NULL){
        return;
    }
    else{
        param=(Parameter*)toBeDeleted;
        free(param->name);
        free(param->value);
        free(param);
    }

}

void deleteValue(void* toBeDeleted) {
    if (toBeDeleted == NULL) {
        return;
    }
    free((char*)toBeDeleted);  
}

void deleteDate(void* toBeDeleted){
    DateTime* dateTime;
    if(toBeDeleted==NULL){
        return;
    }
    else{
        dateTime=(DateTime*)toBeDeleted;
        
        free(dateTime->date);
        free(dateTime->time);
        free(dateTime->text);
        free(dateTime);
    }
}

char* propertyToString(void* prop){              //seg fault
    char* retVal=malloc(sizeof(char)*10);
    strcpy(retVal,"Hello");

    return retVal;  
}


char* parameterToString(void* param){
    char* retVal=malloc(sizeof(char)*10);
    strcpy(retVal,"Hello");

    return retVal;  
}


char* valueToString(void* val){
    char* retVal=malloc(sizeof(char)*10);
    strcpy(retVal,"Hello");

    return retVal;  
}

char* dateToString(void* date){
    char* retVal=malloc(sizeof(char)*10);
    strcpy(retVal,"Hello");

    return retVal;  
}

int compareProperties(const void* first, const void* second){
    return 1;
}

int compareParameters(const void* first, const void* second){
    return 1;
}

int compareValues(const void* first, const void* second){
    return 1;
}

int compareDates(const void* first, const void* second){
    return 1;
}



char* errorToString(VCardErrorCode err){
    char *error;

    if(err==OK){
        error="OK";
    }
    else if(err==INV_FILE){
        error="Invalid File";
    }
    else if(err==INV_CARD){
        error="Invalid Card";
    }
    else if(err==INV_PROP){
        error="Invalid Property";
    }
    else if(err==INV_DT){
        error="Invalid Date/Time";
    }
    else if(err==WRITE_ERROR){
        error="Write Error";
    }
    else if(err==OTHER_ERROR){
        error="Other Error";
    }
    else if(err==WRITE_ERROR){
        error="Write Error";
    }
    else{
        error="Invalid error code";
    }
    return error;
}

char* cardToString(const Card* obj){
    char * retVal=malloc(sizeof(char)*3);
    strcpy(retVal,"HI");
    return retVal;

}
