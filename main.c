#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MinQueueSize 5

// Define Queue structure
typedef struct QueueRecord *Queue;

struct QueueRecord
{

    int Capacity;
    int Front;
    int Rear;
    int Size;
    char **Array;
};

// Define Stack structure
typedef struct Stack *PtrToNode;

struct Stack
{

    char Word[50];
    int Index;
    int operationType; // 0 -> Insert, 1 -> Delete
    PtrToNode Next;
};

// Main Methods
char *operationName(int);
int removeWord(char *, char *);
void insertWord(char *, char *, int);
int wordIndex(char *, char *, int);

// Queue Methods
int isQueueEmpty(Queue);
int isQueueFull(Queue);
Queue CreateQueue(int);
void MakeQueueEmpty(Queue);
void DisposeQueue(Queue);
int Succ(int, Queue);
void Enqueue(char *, Queue);
char *Front(Queue);
void Dequeue(Queue);
char *FrontAndDequeue(Queue);

// Stack Methods
int isStackEmpty(struct Stack *);
struct Stack *CreateStack();
void MakeStackEmpty(struct Stack *);
void Pop(struct Stack *);
struct Stack *Top(struct Stack *);
void Push(char *, int, int, struct Stack *);
void DisposeStack(struct Stack *);

int main()
{
    // Creating the undo Stack and redo Stack by calling CreateStack() method
    struct Stack *undoStack = CreateStack();
    struct Stack *redoStack = CreateStack();

    // Creating the Queue by calling CreateQueue(Size) method
    Queue myQueue = CreateQueue(20);

    // Defining inputText and outputText and insertedText so we gonna use them later
    char inputText[100] = "";
    char outputText[100] = "";
    char insertedText[100];

    FILE *input, *output;

    int choice;

    // Printing the menu to user to choose which thing he want to do
    do
    {

        printf("Please Select one of the option you want:\n");
        printf("1. Load the input file which contains the initial text\n");
        printf("2. Print the loaded text\n");
        printf("3. Insert strings to the text\n");
        printf("4. Remove strings from the text\n");
        printf("5. Perform Undo operation\n");
        printf("6. Perform Redo operation\n");
        printf("7. Print the Undo Stack and Redo Stack\n");
        printf("8. Save the updated text to the output file\n");
        printf("9. Exit\n\n");
        printf("Enter the option: ");

        scanf("%d", &choice);

        switch (choice)
        {

        case 1:

            /*
            open the file and reading the text from it and save it in inputText and outputText strings
            and remove the \n character from the string
            */

            input = fopen("inputFile.txt", "r");

            // Check if there is a problem with reading
            if (input == NULL || fgets(inputText, sizeof(inputText), input) == NULL)
            {
                printf("\nUnable to read the text from the file!\n\n");
                exit(1);
            }
            else
            {
                strcpy(outputText, inputText);            // Copy the inputText to outputText
                outputText[strlen(outputText) - 1] = ' '; // Removing the \n character from outputText

                fclose(input);
                printf("\nThe Text loaded successfully\n\n");
            }
            break;

        case 2:

            // Print the loaded text by checking if its loaded or not then printing

            if (inputText[0] == '\0')
            {
                printf("\nYou must load the Text first!\n\n");
            }
            else
            {
                printf("\nThe input Text: %s\n", inputText);
            }
            break;

        case 3:

            // Insert text in the outputText

            if (outputText[0] == '\0')
            {
                printf("\nYou must load the Text first!\n\n");
            }
            else
            {
                int position;
                printf("Enter the position to insert (-1 means last): ");
                int isInteger = scanf("%d", &position);
                int outputTextLength = strlen(outputText);

                // Checking if the user enter a integer or not and if its valid or not
                if (isInteger == 0 || position < -1 || position > outputTextLength)
                {
                    printf("\nInvalid position!\n\n");
                    while (getchar() != '\n')
                        ;
                }
                else
                {
                    printf("Enter the text to insert: ");
                    getchar();
                    gets(insertedText);

                    // Copying the insertedText so we can save it before splitting
                    char copyInsertedText[100];
                    strcpy(copyInsertedText, insertedText);

                    char *token = strtok(insertedText, " ");
                    char *nextToken;

                    // Splitting the Text and enqueue it into myQueue
                    while (token != NULL)
                    {
                        char tokenCopy[100];
                        strcpy(tokenCopy, token);

                        nextToken = strtok(NULL, " ");
                        if (nextToken != NULL)
                        {
                            strcat(tokenCopy, " ");
                        }

                        Enqueue(tokenCopy, myQueue);
                        token = nextToken;
                    }

                    // Inserting each word from the queue to the outputText and push it into undo stack
                    while (!isQueueEmpty(myQueue))
                    {
                        int wordPos = wordIndex(copyInsertedText, Front(myQueue), position); // Determine the position of the word
                        Push(Front(myQueue), 0, wordPos, undoStack);                         // Push the word to undo stack
                        insertWord(outputText, FrontAndDequeue(myQueue), wordPos);           // Insert the word to outputText and dequeue the word from myQueue
                    }

                    printf("\nThe Text has been inserted!");
                    printf("\nNew Text: %s\n\n", outputText);
                }
            }
            break;

        case 4:

            // Remove text from the outputText and push it into the undo stack
            if (outputText[0] == '\0')
            {
                printf("\nYou must load the Text first!\n\n");
            }
            else
            {
                printf("Enter the text to delete: ");
                getchar();
                gets(insertedText);

                int wordPos = removeWord(outputText, insertedText); // Calling the method that delete the text

                if (wordPos == -2)
                {
                    printf("\nThe text you entered not found!");
                    printf("\nNew Text: %s\n\n", outputText);
                }
                else
                {
                    Push(insertedText, 1, wordPos, undoStack); // Push the text into the undo stack
                    printf("\nThe Text has been deleted!");
                    printf("\nNew Text: %s\n\n", outputText);
                }
            }
            break;

        case 5:

            /*
            Make undo operation so id the operationType equal to 0 it gonna delete
            the word from the outputText and push it into the redo stack and if the operationType equal to 1
            it gonna insert the word and push it to redo stack
            */

            if (isStackEmpty(undoStack))
            {
                printf("\nYou have to do something to undo!\n\n");
            }
            else
            {
                Push(Top(undoStack)->Word, Top(undoStack)->operationType, Top(undoStack)->Index, redoStack); // Push the word to redo stack

                if (Top(undoStack)->operationType == 0)
                {
                    removeWord(outputText, Top(undoStack)->Word); // Remove the word id operationType equal to 0
                }
                else
                {
                    insertWord(outputText, Top(undoStack)->Word, Top(undoStack)->Index); // Insert the word if operationType equal to 1
                }

                Pop(undoStack); // Pop the word from the undo stack

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

            if (isStackEmpty(redoStack))
            {
                printf("\nYou have to undo something to redo!\n\n");
            }
            else
            {

                if (Top(redoStack)->operationType == 0)
                {
                    insertWord(outputText, Top(redoStack)->Word, Top(redoStack)->Index); // Insert the word if operationType equal to 0
                }
                else
                {
                    removeWord(outputText, Top(redoStack)->Word); // Remove the word if operationType equal to 1
                }

                Pop(redoStack); // Pop the word from the redo stack

                printf("\nRedo Operation!");
                printf("\nNew Text: %s\n\n", outputText);
            }
            break;

        case 7:

            /*
            Prints the undo stack and redo stack through using temp stack which we gonna push
            every word from undo stack to temp stack and printing them we gonna return them
            and same with redo stack
            */

            printf("\nUndo Stack:\n");
            if (isStackEmpty(undoStack))
            {
                printf("\tEmpty Stack!\n");
            }
            else
            {

                struct Stack *tempStack = CreateStack(); // Creating the temp stack
                printf("\t%-15s%-15s%-15s\n", "Token", "Operation", "Index");

                // transfer the word from undo stack to temp stack
                while (!isStackEmpty(undoStack))
                {
                    Push(Top(undoStack)->Word, Top(undoStack)->operationType, Top(undoStack)->Index, tempStack);                             // Push the word from undo stack to temp stack
                    printf("\t%-15s%-15s%-5d\n", Top(undoStack)->Word, operationName(Top(undoStack)->operationType), Top(undoStack)->Index); // Printing the information of the word
                    Pop(undoStack);                                                                                                          // Pop the word from the undo stack
                }

                while (!isStackEmpty(tempStack))
                {
                    Push(Top(tempStack)->Word, Top(tempStack)->operationType, Top(tempStack)->Index, undoStack); // Push the word from temp stack to undo stack
                    Pop(tempStack);                                                                              // Pop the word from temp stack
                }

                DisposeStack(tempStack); // Dispose the temp stack
            }

            printf("\nRedo Stack:\n");
            if (isStackEmpty(redoStack))
            {
                printf("\tEmpty Stack!\n\n");
            }
            else
            {

                struct Stack *tempStack = CreateStack(); // Creating the temp stack
                printf("\t%-15s%-15s%-5s\n", "Token", "Operation", "Index");

                // transfer the word from the redo stack to temp stack
                while (!isStackEmpty(redoStack))
                {
                    Push(Top(redoStack)->Word, Top(redoStack)->operationType, Top(redoStack)->Index, tempStack);                             // Push the word from the redo stack to the temp stack
                    printf("\t%-15s%-15s%-5d\n", Top(redoStack)->Word, operationName(Top(redoStack)->operationType), Top(redoStack)->Index); // Printing the information of the word
                    Pop(redoStack);                                                                                                          // Pop the word from redo stack
                }

                // transfer the word from temp stack to redo stack
                while (!isStackEmpty(tempStack))
                {
                    Push(Top(tempStack)->Word, Top(tempStack)->operationType, Top(tempStack)->Index, redoStack); // Pushing the words from temp stack to redo stack
                    Pop(tempStack);                                                                              // Pop the word from temp stack
                }

                DisposeStack(tempStack); // Dispose the temp stack
                printf("\n");
            }
            break;

        case 8:

            /*
            Open the file and reading the text from it and save it in inputText and outputText strings
            and remove the \n character from the string
            */

            if (outputText[0] == '\0')
            {
                printf("\nYou must load the Text first!\n\n");
            }
            else
            {

                // Open the file to write
                output = fopen("outputFile.txt", "w");

                // Checking if there is a problem with opening the file
                if (output == NULL)
                {
                    printf("\nUnable to write the text to the file!\n\n");
                    exit(1);
                }

                fprintf(output, "%s", outputText); // Write to the file the outputText
                fclose(output);

                MakeStackEmpty(undoStack);
                MakeStackEmpty(redoStack);

                printf("\nThe Text has been saved!\n\n");
            }
            break;

        case 9:

            DisposeStack(redoStack);
            DisposeStack(undoStack);
            DisposeQueue(myQueue);
            printf("\nWe are gonna miss you <3\n");
            break;

        default:

            printf("\nAre you sure you entered the number between 1 - 9 :/\n\n");
        }
    } while (choice != 9);

    return 0;
}

/*===========================================================[Main Methods]===========================================================*/

// this method take the operationType and return the operation name
char *operationName(int operationType)
{
    if (operationType == 0)
    {
        return "Insert";
    }
    else
    {
        return "Remove";
    }
}

// This method used to insert a word into the text at a specified position
void insertWord(char *Text, char *Word, int position)
{
    // Insert at the end of the text
    if (position == -1)
    {
        strcat(Text, Word);
    }
    else
    {
        int textLength = strlen(Text);
        int wordLength = strlen(Word);

        // Shift all characters after the insert position to make a space for the word
        for (int i = textLength; i >= position; i--)
        {
            Text[i + wordLength] = Text[i];
        }

        // Put the word into the space we make
        for (int i = 0; i < wordLength; i++)
        {
            Text[position + i] = Word[i];
        }
    }
}

// This method remove a word from the text
int removeWord(char *Text, char *Word)
{
    int textLength = strlen(Text);
    int wordLength = strlen(Word);
    char *wordPointer = strstr(Text, Word);

    if (wordPointer != NULL)
    {
        // Determine the position of the word in the text
        int wordPosition = wordPointer - Text;

        // Shift characters after the word to cover the removed word
        for (int i = wordPosition; i < (textLength - wordLength); i++)
        {
            Text[i] = Text[i + wordLength];
        }

        // Add null terminator to mark the end of the modified text
        Text[textLength - wordLength] = '\0';

        return wordPosition;
    }
    else
    {
        // Return -2 if the strstr method didn't find the word in the test
        return -2;
    }
}

// This method used to find the index for inserting a word into the text
int wordIndex(char *Text, char *Word, int insertPosition)
{
    if (insertPosition == -1)
    {
        // Insert at the end of the text
        return -1;
    }
    else
    {
        char *wordPointer = strstr(Text, Word);
        int wordPosition = wordPointer - Text;           // Determine the position of the word in the text
        int newPosition = insertPosition + wordPosition; // Calculate new insert position

        return newPosition;
    }
}

/*===========================================================[Queue Methods]===========================================================*/

// This method used to check if the queue is empty or not
int isQueueEmpty(Queue Q)
{
    return Q->Size == 0;
}

// This method used to check if the queue is full or not
int isQueueFull(Queue Q)
{
    return Q->Size == Q->Capacity;
}

// This method create a queue with specified capacity
Queue CreateQueue(int Capacity)
{
    Queue Q;

    if (Capacity < MinQueueSize)
    {
        printf("Queue size is too small\n");
        exit(1);
    }
    else
    {
        Q = (Queue)malloc(sizeof(struct QueueRecord));

        if (Q == NULL)
        {
            printf("Out of memory!\n");
            exit(1);
        }

        Q->Array = (char **)malloc(sizeof(char *) * Capacity);

        if (Q->Array == NULL)
        {
            printf("Out of memory!\n");
            exit(1);
        }

        Q->Capacity = Capacity;
        MakeQueueEmpty(Q);

        return Q;
    }
}

// This method used to make a queue empty
void MakeQueueEmpty(Queue Q)
{
    Q->Size = 0;
    Q->Front = 1;
    Q->Rear = 0;
}

// This method used to dispose a queue
void DisposeQueue(Queue Q)
{
    if (Q != NULL)
    {
        free(Q->Array);
        free(Q);
    }
}

// This method used to get the next index in the circular queue
int Succ(int Value, Queue Q)
{
    if (++Value == Q->Capacity)
        Value = 0;

    return Value;
}

// This method used to enqueue a word into the queue
void Enqueue(char *X, Queue Q)
{
    if (isQueueFull(Q))
    {
        printf("Full Queue!\n");
        exit(0);
    }
    else
    {
        Q->Size++;
        Q->Rear = Succ(Q->Rear, Q);
        Q->Array[Q->Rear] = (char *)malloc(strlen(X) + 1);
        strcpy(Q->Array[Q->Rear], X);
    }
}

// This method used to get the front word of the queue
char *Front(Queue Q)
{
    if (isQueueEmpty(Q))
    {
        printf("Empty Queue!\n");
        exit(1);
    }
    else
    {
        return Q->Array[Q->Front];
    }
}

// This method used to dequeue a word from the queue
void Dequeue(Queue Q)
{
    if (isQueueEmpty(Q))
    {
        printf("Empty Queue!\n");
        exit(1);
    }
    else
    {
        Q->Size--;
        free(Q->Array[Q->Front]);
        Q->Front = Succ(Q->Front, Q);
    }
}

// This method used to get the front word of the queue and dequeue it after that
char *FrontAndDequeue(Queue Q)
{
    char *X = NULL;
    if (isQueueEmpty(Q))
    {
        printf("Empty Queue!\n");
        exit(1);
    }
    else
    {
        Q->Size--;
        X = Q->Array[Q->Front];
        Q->Front = Succ(Q->Front, Q);
    }

    return X;
}

/*===========================================================[Stack Methods]===========================================================*/

// This method used to check if stack is empty
int isStackEmpty(struct Stack *S)
{
    return S->Next == NULL;
}

// This method used to create a new stack
struct Stack *CreateStack()
{
    struct Stack *S = (struct Stack *)malloc(sizeof(struct Stack));

    if (S == NULL)
    {
        printf("Out of memory!\n");
        exit(1);
    }

    S->Next = NULL;
    MakeStackEmpty(S);

    return S;
}

// This method used to make a stack empty
void MakeStackEmpty(struct Stack *S)
{
    if (S == NULL)
    {
        printf("Out of memory!\n");
    }
    else
    {
        while (!isStackEmpty(S))
            Pop(S);
    }
}

// This method used to pop a word from the stack
void Pop(struct Stack *S)
{
    PtrToNode firstCell;

    if (isStackEmpty(S))
    {
        printf("Empty Stack!\n");
    }
    else
    {
        firstCell = S->Next;
        S->Next = S->Next->Next;
        free(firstCell);
    }
}

// This method used to get the top word of the stack
struct Stack *Top(struct Stack *S)
{
    if (isStackEmpty(S))
    {
        printf("Empty Stack!\n");
        exit(1);
    }
    else
    {
        return S->Next;
    }
}

// This method used to push a word onto the stack
void Push(char *Word, int operationType, int Index, struct Stack *S)
{
    PtrToNode temp = (struct Stack *)malloc(sizeof(struct Stack));

    if (temp == NULL)
    {
        printf("Out of memory!\n");
        exit(1);
    }

    strcpy(temp->Word, Word);
    temp->operationType = operationType;
    temp->Index = Index;
    temp->Next = S->Next;
    S->Next = temp;
}

// This method used to dispose a stack
void DisposeStack(struct Stack *S)
{
    MakeStackEmpty(S);
    free(S);
}
