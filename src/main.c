#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "VCHelpers.h"
#include "VCParser.h"
#include "LinkedListAPI.h"

void printAllParamsAndValues(Property* prop) {
    if (prop == NULL) {
        printf("Property is NULL\n");
        return;
    }

    // Print all parameters
    if (prop->parameters != NULL) {
        printf("Parameters:\n");
        Node* paramNode = prop->parameters->head;
        while (paramNode != NULL) {
            Parameter* param = (Parameter*)paramNode->data;
            printf("%s(%ld)=%s(%ld)\n", param->name,strlen(param->name),param->value,strlen(param->value));
            paramNode = paramNode->next;
        }
    } else {
        printf("No Parameters.\n");
    }

    // Print all values
    if (prop->values != NULL) {
        printf("Values:\n");
        Node* valueNode = prop->values->head;
        while (valueNode != NULL) {
            char* value = (char*)valueNode->data;
            printf("%s(%ld)\n", value,strlen(value));
            valueNode = valueNode->next;
        }
    } else {
        printf("No Values.\n");
    }
}
void printAllProperties(List* optionalProperties) {
    if (optionalProperties == NULL || optionalProperties->head == NULL) {
        printf("No optional properties available.\n");
        return;
    }

    // Loop through each Property in the list
    Node* propNode = optionalProperties->head;
    while (propNode != NULL) {
        Property* prop = (Property*)propNode->data;
        printf("\n--- Property ---\n");

        // Print name and group
        printf("Name:%s(%ld)\n", prop->name,strlen(prop->name));
        printf("Group:%s(%ld)\n", prop->group,strlen(prop->group));

        // Call the function to print parameters and values
        printAllParamsAndValues(prop);

        propNode = propNode->next;
    }
}

void printFnDetails(Card* obj) {
    if (obj == NULL || obj->fn == NULL) {
        printf("FN or obj is NULL.\n");
        return;
    }

    // Accessing the fn Property
    Property* fnProp = obj->fn;

    // Print the fn's name and group
    printf("\n--- FN Property ---\n");
    printf("Name:%s and len is %ld\n", fnProp->name,strlen(fnProp->name));
    printf("Group:%s and length is %ld\n", fnProp->group,strlen(fnProp->group));

    // Print all parameters for fn
    if (fnProp->parameters != NULL) {
        printf("Parameters:\n");
        Node* paramNode = fnProp->parameters->head;
        while (paramNode != NULL) {
            Parameter* param = (Parameter*)paramNode->data;
            printf("%s(%ld)=%s(%ld)\n", param->name,strlen(param->name),param->value,strlen(param->value));
            paramNode = paramNode->next;
        }
    } else {
        printf("No Parameters.\n");
    }

    // Print all values for fn
    if (fnProp->values != NULL) {
        printf("Values:\n");
        Node* valueNode = fnProp->values->head;
        while (valueNode != NULL) {
            char* value = (char*)valueNode->data;
            printf("val is %s(%ld)\n", value,strlen(value));
            valueNode = valueNode->next;
        }
    } else {
        printf("No Values.\n");
    }
}




int main() {
    // Sample vCard file name
    char* fileName = "testCardMins.vcf";

    // Extract the FN property
    // char* fn = extractFN(fileName);
    // if (fn != NULL) {
    //     printf("Full Name (FN): %s\n", fn);
    //     free(fn);
    // } else {
    //     printf("FN property not found or file could not be opened.\n");
    // }
    VCardErrorCode result;
    Card *test;
    printf("passing file name is %s\n",fileName);
    result=createCard(fileName,&test);


    if(result==OK){
        // char* tmp=test->fn->parameters->printData(getFromBack(test->fn->parameters));
        // printf("string in %s\n",tmp);
        // free(tmp);

        //char* tmp2=test->birthday->date;
        //printf("birthday val is %s\n",tmp2);
        //free(tmp2);

        // printf("name is fn shiuld be:%s\n",test->fn->name);

        // char* tmp3=dateToString(test->birthday);
        // printf("date is %s\n",tmp3);
        // free(tmp3);

        // char *tem=toString(test->fn->values);
        // printf("list of values:%s\n",tem);
        // free(tem);

        // char *tem1=toString(test->optionalProperties);
        // printf("list of values in optpro:%s\n",tem1);
        // free(tem1);
        printAllProperties(test->optionalProperties);
        printFnDetails(test);

        if(test->birthday!=NULL){
            printf("UTC:%d\n",test->birthday->UTC);
            printf("istext:%d\n",test->birthday->isText);
            printf("date:%s and len %ld\n",test->birthday->date,strlen(test->birthday->date));
            printf("time:%s and len %ld\n",test->birthday->time,strlen(test->birthday->time));
            printf("text:%s and len %ld\n",test->birthday->text,strlen(test->birthday->text));
        }

        if(test->anniversary!=NULL){
            printf("ANNI\n");
            printf("UTC:%d\n",test->anniversary->UTC);
            printf("istext:%d\n",test->anniversary->isText);
            printf("date:%s and len %ld\n",test->anniversary->date,strlen(test->anniversary->date));
            printf("time:%s and len %ld\n",test->anniversary->time,strlen(test->anniversary->time));
            printf("text:%s and len %ld\n",test->anniversary->text,strlen(test->anniversary->text));
        }
        
        VCardErrorCode bb=validateCard(test);
        printf("wrivalidint  to file %s\n",errorToString(bb));
        if(bb==OK){
            VCardErrorCode qq=writeCard("rohittt.vcf",test);
            printf("writing to file %s\n",errorToString(qq));
        }
        
        deleteCard(test);
        printf("result is %s\n",errorToString(result));
    }
    

    printf("result is %s\n",errorToString(result));
    //printf("Name: %s\n", prop->name);
    //printf("Group: %s\n", prop->group);
    //free(tmp);


    

    char** ret=dispValid();
    freeDispValid(ret);


    // Card *test2=NULL;
    // deleteCard(test2);
     

    return 0;
}
