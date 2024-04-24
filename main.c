#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#define MinQueueSize 5

//Define Queue structure
typedef struct QueueRecord* Queue;

struct QueueRecord {

    int Capacity;
    int Front;
    int Rear;
    int Size;
    char** Array;

};

//Define Stack structure
typedef struct Stack* PtrToNode;

struct Stack {

    char Word[50];
    int Index;
    int Operation; // 0 -> insert, 1 -> delete
    PtrToNode Next;

};

// Main Methods
char* operationName(int);
int removeWord(char*, char*);
void insertWord(char*, char*, int);
int wordIndex(char*, char*, int);

// Queue Methods
int isQueueEmpty(Queue);
int isQueueFull(Queue);
Queue CreateQueue(int);
void MakeQueueEmpty(Queue);
void DisposeQueue(Queue);
int Succ(int, Queue);
void Enqueue(char*, Queue);
char* Front(Queue);
void Dequeue(Queue);
char* FrontAndDequeue(Queue);

// Stacks Methods
int isStackEmpty(struct Stack*);
struct Stack* CreateStack();
void MakeStackEmpty(struct Stack*);
void Pop(struct Stack*);
struct Stack* Top(struct Stack*);
void Push(char*, int, int, struct Stack*);
void DisposeStack(struct Stack*);

int main(){
    //Creating the undo Stack and redo Stack by calling CreateStack() method
    struct Stack* undoStack = CreateStack();
    struct Stack* redoStack = CreateStack();

    //Creating the Queue by calling CreateQueue(Size) method
    Queue myQueue = CreateQueue(20);

    //defining inputText and outputText and insertedText so we gonna use them later
    char inputText[100] = "";
    char outputText[100]= "";
    char insertedText[100];

    FILE *input, *output;

    int choice;

    //printing the menu to user to chose which thing he want to do
    do{
        printf("Please Select one of the option you want:\n");
        printf("1. Load the input file which contains the initial text\n");
        printf("2. Print the loaded text\n");
        printf("3. Insert strings to the text\n");
        printf("4. Remove strings from the text\n");
        printf("5. Perform Undo operation\n");
        printf("6. Perform Redo operation\n");
        printf("7. Print the Undo Stack and the Redo stack\n");
        printf("8. Save the updated text to the output file\n");
        printf("9. Exit\n\n");
        printf("Enter the option: ");

        scanf("%d", &choice);

        switch(choice){

        case 1:

            /*
            open the file and reading the text from it and save it in inputText and outputText strings
            and remove the \n character from the string
            */

            input = fopen("inputFile.txt", "r"); //open the file to read

            // check if there is a problem with reading
            if (input == NULL || fgets(inputText, sizeof(inputText), input) == NULL) {
                printf("\nUnable to read the text from the file!\n\n");
                exit(1);

            } else {
                strcpy(outputText, inputText); // copy the inputText to outputText
                outputText[strlen(outputText) - 1] = '\0'; // removing the \n character in outputText

                fclose(input); //closing the file
                printf("\nThe Text loaded successfully\n\n");

             }
            break;

        case 2:

            //print the loaded text by checking if its loaded or not then printing

            if (inputText[0] == '\0') {
                printf("\nYou must load the Text first!\n\n");

            } else {
                printf("\nThe input Text: %s\n", inputText);

            }
            break;

        case 3:

            //insert texts into the outputText
            if (outputText[0] == '\0') {
                printf("\nYou must load the Text first!\n\n");

            } else {
                int position;
                printf("Enter the position to insert (-1 means last): ");
                int isInteger = scanf("%d", &position);
                int ouputTextLength = strlen(outputText);

                //checking if the user enter a integer or not and if its valid or not
                if(isInteger == 0 || position < -1 || position > ouputTextLength){
                    printf("\nInvalid position!\n\n");
                    while (getchar() != '\n');

                } else {
                    printf("Enter the text to insert: ");
                    getchar();
                    gets(insertedText);

                    //copying the insertText so we can save it before splitting
                    char copyInsertedText[100];
                    strcpy(copyInsertedText, insertedText);

                    char* token = strtok(insertedText, " ");
                    char* nextToken;
                    // splitting the Text and enqueue it into (MyQueue) queue
                    while (token != NULL) {
                        char tokenCopy[100];
                        strcpy(tokenCopy, token);

                        nextToken = strtok(NULL, " ");
                        if (nextToken != NULL) {
                            strcat(tokenCopy, " ");
                        }

                        Enqueue(tokenCopy, myQueue);
                        token = nextToken;
                    }

                    // inserting each word from the queue to the outputText and push it into undo stack
                    while(!isQueueEmpty(myQueue)){
                        int wordPos = wordIndex(copyInsertedText, Front(myQueue), position); // determine the position of the word
                        Push(Front(myQueue), 0, wordPos, undoStack); // push the word to undo stack
                        insertWord(outputText, FrontAndDequeue(myQueue), wordPos); // insert the word to outputText and dequeue the word from MyQueue

                     }

                    printf("\nThe Text has been inserted!");
                    printf("\nNew Text: %s\n\n", outputText);

                }

            }
            break;

        case 4:

            // Remove text from the outputText and push it into undo stack
            if (outputText[0] == '\0') {
                printf("\nYou must load the Text first!\n\n");

            } else {
                printf("Enter the text to delete: ");
                getchar();
                gets(insertedText);

                int pos = removeWord(outputText, insertedText); // calling the function that delete the text

                if(pos == -2){
                    printf("\nThe text you entered not found!");
                    printf("\nNew Text: %s\n\n", outputText);

                } else {
                    Push(insertedText, 1, pos, undoStack); // push the text into undo stack
                    printf("\nThe Text has been deleted!");
                    printf("\nNew Text: %s\n\n", outputText);


                }

            }
            break;

        case 5:

            /*
            Make undo operation so if the operationType equal to 0 it gonna delete
            the word from the outputText and push it into the redo stack and if the operationType equal to 1
            it gonna insert the word and push it to redostack
            */

            if(isStackEmpty(undoStack)){
                printf("\nYou have to do something to undo!\n\n");

            } else {

                Push(Top(undoStack)->Word, Top(undoStack)->Operation, Top(undoStack)->Index, redoStack); //push the word to redo stack

                if(Top(undoStack)->Operation == 0){
                    removeWord(outputText, Top(undoStack)->Word); //remove the word if operationType equal to 0

                } else {
                    insertWord(outputText, Top(undoStack)->Word, Top(undoStack)->Index); //insert the word if operationType equal to 1

                }

                Pop(undoStack); // pop the word from the undo stack

                printf("\nUndo Operation!");
                printf("\nNew Text: %s\n\n", outputText);

            }

            break;

        case 6:

            /*
            Make redo operation so if the operationType equal to 1 it gonna delete
            the word from the outputText and if the operationType equal to 0
            it gonna insert the word to outputText
            */

            if(isStackEmpty(redoStack)){
                printf("\nYou have to undo something to redo!\n\n");

            } else {

                if(Top(redoStack)->Operation == 0){
                    insertWord(outputText, Top(redoStack)->Word, Top(redoStack)->Index); //insert the word if operationType equal to 0

                } else {
                    removeWord(outputText, Top(redoStack)->Word); //remove the word if operationType equal to 1

                }

                Pop(redoStack); //pop the word from redo stack

                printf("\nRedo Operation!");
                printf("\nNew Text: %s\n\n", outputText);

            }

            break;

        case 7:

           /*
            prints the undo stack and redo stack through using temp stack which we gonna push
            every node from undo stack to temp stack and printing them then we gonna return them
            and same with redo stack
            */

            printf("\nUndo Stack:\n");
            if(isStackEmpty(undoStack)){
                    printf("\tEmpty Stack!\n");

            } else {

                struct Stack* tempStack = CreateStack(); //creating temp stack
                printf("\t%-15s%-15s%-5s\n", "Token", "Operation", "Index");

                //transfer the nodes from undo stack to temp stack
                while(!isStackEmpty(undoStack)){

                    Push(Top(undoStack)->Word, Top(undoStack)->Operation, Top(undoStack)->Index, tempStack); // pushing the nodes from undo stack to temp stack
                    printf("\t%-15s%-15s%-5d\n", Top(undoStack)->Word, operationName(Top(undoStack)->Operation), Top(undoStack)->Index); //printing the information of the node
                    Pop(undoStack); //pop the node from the undo stack
                }

                //transfer the nodes from temp stack to undo stack
                while(!isStackEmpty(tempStack)){

                    Push(Top(tempStack)->Word, Top(tempStack)->Operation, Top(tempStack)->Index, undoStack); // pushing the node from temp stack to undo stack
                    Pop(tempStack); //pop the node from temp stack
                }

                DisposeStack(tempStack); // dispose the temp stack
            }

            printf("\nRedo Stack:\n");
            if(isStackEmpty(redoStack)){
                    printf("\tEmpty Stack!\n\n");

            } else {

                struct Stack* tempStack = CreateStack(); //creating temp stack
                printf("\t%-15s%-15s%-5s\n", "Token", "Operation", "Index");

                //transfer the nodes from redo stack to temp stack
                while(!isStackEmpty(redoStack)){

                    Push(Top(redoStack)->Word, Top(redoStack)->Operation, Top(redoStack)->Index, tempStack); // pushing the nodes from redo stack to temp stack
                    printf("\t%-15s%-15s%-5d\n", Top(redoStack)->Word, operationName(Top(redoStack)->Operation), Top(redoStack)->Index); //printing the information of the node
                    Pop(redoStack); //pop the node from redo stack
                }

                //transfer the nodes from temp stack to redo stack
                while(!isStackEmpty(tempStack)){

                    Push(Top(tempStack)->Word, Top(tempStack)->Operation, Top(tempStack)->Index, redoStack); // pushing the nodes from temp stack to redo stack
                    Pop(tempStack); //pop the node from temp stack
                }

                DisposeStack(tempStack); // dispose the temp stack
                printf("\n");
            }
           break;

        case 8:

           /*
            open the file and reading the text from it and save it in inputText and outputText strings
            and remove the \n character from the string
            */
            if(outputText[0] == '\0'){
                printf("\nYou must load the Text first!\n\n");
            } else {

                //open the file to write
                output = fopen("outputFile.txt", "w");

                //checking if there is a problem with opening the file
                if (output == NULL) {
                    printf("\nUnable to write the text to the file!\n\n");
                    exit(1);
                }

                fprintf(output, "%s", outputText); // write to the file the outputText
                fclose(output); // closing the file

                printf("\nThe Text has been saved!\n\n");
            }
            break;

        case 9:

            DisposeStack(undoStack);
            DisposeStack(redoStack);
            DisposeQueue(myQueue);
            printf("\nWe are gonna miss you <3\n");
            break;


        default:

            printf("\nAre you sure that you entered the number between 1 - 9 :/\n\n");

        }

        } while(choice != 9);

    return 0;
}

/*===========================================================[Main Methods]===========================================================*/

// this method take the operationType and return the operation name
char* operationName(int operationType){
    if(operationType == 0){
        return "Insert";
    } else{
        return "remove";
    }
}

// this methods used to insert a word into the text at a specified position
void insertWord(char* Text, char* Word, int position) {
    // Insert at the end of the text
    if(position == -1){
        strcat(Text, Word);

    } else {
        int textLength = strlen(Text);
        int wordLength = strlen(Word);

        // Shift all characters after the insert position to make a space for the word
        for (int i = textLength; i >= position; i--) {
            Text[i + wordLength] = Text[i];
        }

        // put the word into the space we make
        for (int i = 0; i < wordLength; i++) {
            Text[position + i] = Word[i];
        }
    }
}

// this method remove a word from the text
int removeWord(char* Text, char* Word) {

    int TextLength = strlen(Text);
    int wordLength = strlen(Word);
    char* wordPointer = strstr(Text, Word);

    if (wordPointer != NULL) {
        // determine the position of the word in the text
        int wordPosition = wordPointer - Text;

        // Shift characters after the word to cover the removed word
        for (int i = wordPosition; i < (TextLength - wordLength); i++) {
            Text[i] = Text[i + wordLength];
        }

        // Add null terminator to mark the end of the modified text
        Text[TextLength - wordLength] = '\0';

        return wordPosition;

    } else {
        //return -2 if the strstr method did't find the word in the test
        return -2;

    }

}

// this method the find the index for inserting a word into the text
int wordIndex(char* Text, char* Word, int insertPosition) {
    if(insertPosition == -1){
        // insert at the end of the text
        return -1;

    } else {
        char* wordPointer = strstr(Text, Word);
        int wordPosition = wordPointer - Text; // determine the position of the word in the text
        int newPosition = insertPosition + wordPosition; // Calculate the insertion position considering the word's position

        return newPosition;
    }

}

/*===========================================================[Queue Methods]===========================================================*/

// this method used to check if the queue is empty or not
int isQueueEmpty(Queue Q) {
    return Q->Size == 0;
}

// this method used to check if the queue is full or not
int isQueueFull(Queue Q) {
    return Q->Size == Q->Capacity;
}

// this method create a queue with specified capacity
Queue CreateQueue(int Capacity) {
    Queue Q;

    if (Capacity < MinQueueSize) {
        printf("Queue size is too small\n");
        exit(1);
    } else {
        Q = (Queue)malloc(sizeof(struct QueueRecord));

        if (Q == NULL) {
            printf("Out of memory!\n");
            exit(1);
        }

        Q->Array = (char**)malloc(sizeof(char*) * Capacity);

        if (Q->Array == NULL) {
            printf("Out of memory!\n");
            exit(1);
        }

        Q->Capacity = Capacity;
        MakeQueueEmpty(Q);

        return Q;
    }
}

// this method used to make a queue empty
void MakeQueueEmpty(Queue Q) {
    Q->Size = 0;
    Q->Front = 1;
    Q->Rear = 0;
}

// this method used to dispose a queue
void DisposeQueue(Queue Q) {
    if (Q != NULL) {

        free(Q->Array);
        free(Q);
    }
}

// this method used to get the next index in the circular queue
int Succ(int Value, Queue Q) {
    if (++Value == Q->Capacity)
        Value = 0;

    return Value;
}

// this method used to enqueue an element into the queue
void Enqueue(char* X, Queue Q) {
    if (isQueueFull(Q)) {
        printf("Full Queue!\n");

    } else {
        Q->Size++;
        Q->Rear = Succ(Q->Rear, Q);
        Q->Array[Q->Rear] = (char*)malloc(strlen(X) + 1);
        strcpy(Q->Array[Q->Rear], X);

    }
}

//this method used to get the front element of the queue
char* Front(Queue Q) {
    if (isQueueEmpty(Q)) {
        printf("Empty Queue!\n");
        return NULL;

    } else {
        return Q->Array[Q->Front];

    }
}

// this method used to dequeue an element from the queue
void Dequeue(Queue Q) {
    if (isQueueEmpty(Q)) {
        printf("Empty Queue!\n");

    } else {
        Q->Size--;
        free(Q->Array[Q->Front]);
        Q->Front = Succ(Q->Front, Q);

    }
}

// this method used to get the front element of the queue and dequeue it after that
char* FrontAndDequeue(Queue Q) {
    char* X = NULL;
    if (isQueueEmpty(Q)) {
        printf("Empty Queue!\n");
    } else {
        Q->Size--;
        X = Q->Array[Q->Front];
        Q->Front = Succ(Q->Front, Q);
    }
    return X;
}


/*===========================================================[Stack Methods]===========================================================*/

// this method used to check if a stack is empty
int isStackEmpty(struct Stack* S){
    return S->Next == NULL;

}

// this method used to create a new stack
struct Stack* CreateStack(){
    struct Stack* S = (struct Stack*)malloc(sizeof(struct Stack));

    if(S == NULL){
        printf("Out of memory!\n");
        exit(1);
    }

    S->Next = NULL;
    MakeStackEmpty(S);

    return S;
}

// this method used to make a stack empty
void MakeStackEmpty(struct Stack* S){
    if(S == NULL){
        printf("Out of memory!\n");

    } else {
        while(!isStackEmpty(S))
            Pop(S);
    }
}

// this method used to pop an element from the stack
void Pop(struct Stack* S){
    PtrToNode firstCell;

    if(isStackEmpty(S)){
        printf("Empty stack!\n");

    } else {
        firstCell = S->Next;
        S->Next = S->Next->Next;
        free(firstCell);

    }
}

// this method used to get the top element of the stack
struct Stack* Top(struct Stack* S){

    if(isStackEmpty(S)){
        printf("Empty stack!\n");
        exit(1);

    } else {
        return S->Next;
    }

}

// this method used to push an element onto the stack
void Push(char* Word, int operationType, int Index, struct Stack* S){
    PtrToNode temp = (struct Stack*)malloc(sizeof(struct Stack));

    if(temp == NULL){
        printf("Out of memory!\n");
        exit(1);
    }

    strcpy(temp->Word, Word);
    temp->Operation = operationType;
    temp->Index = Index;
    temp->Next = S->Next;
    S->Next = temp;

}

// this method used to dispose a stack
void DisposeStack(struct Stack* S){
    MakeStackEmpty(S);
    free(S);

}
