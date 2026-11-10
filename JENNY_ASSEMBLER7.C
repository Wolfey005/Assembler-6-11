// Project title: JENNY_ASSEMBLER  PART 7
// date: 10/2026--
// i/o files: JENNY_W_P7.asm
// Description:
//   This program simulates a simple assembler and virtual machine.
//
//   1. Reads assembly language instructions from a text file.
//   2. Converts assembly instructions into machine code and
//      stores the machine code in memory.
//   3. Executes the machine code instructions.
//   4. Supports registers, constants, memory addresses,
//      BX indirect addressing ([BX]), and BX plus offset
//      addressing ([BX+offset]).
//   5. Supports loops, conditional jumps, functions,
//      function returns, and stack operations.
//   6. Displays the final state of memory and registers.
//
// Part 7 ASM Program:
//   - Accepts 10 numbers from the user using GET.
//   - Uses a loop to calculate a running total.
//   - Stores the final sum in a memory location.
//   - Outputs the result using PUT.
// _____________________________________________________________________________________________
// entering to git, Powershell -> (ctrl `), git add .  , git commit -m "phrase" , git push  ( pusting to web),,,   git status- 

#define _CRT_SECURE_NO_WARNINGS  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char ASM_FILE_NAME[] = "JENNY_W_P7.asm";

//GLOBAL CONSTANTS

#define MAX      200		   // Total simulated memory locations (no semi colonNumber of memory locations , 2bytes,
#define COL       9			  // number of columns for output
#define LINE_SIZE 150	     // For c-strings, max number of characters in command or any strings

//OPERAND TYPES, REGISTERS AND OTHER

#define AXREG 0             // ID for AX reg
#define BXREG 1            // ID for bx reg
#define CXREG 2           // ID for CX reg
#define DXREG 3          // ID for DX reg
#define CONSTANT 7      // flag= the operand is a literal num (binary 111)
#define ADDR 6         // address locations
#define BXADDR  4     // (iii rr 100)
#define BXPLUS  5    //(iii rr 101)

//boolean 
#define TRUE  1 
#define FALSE 0 

//COMMANDS/ OPCODS- 
//1 part commands
#define RET    3               //(binary ->(xxx xx x11) opCode for return 
#define FUN    4              //(binary ->(xxx xx 100) opCode for function
#define HALT   5             // op code to stop machine code instruction
#define GET    6	        // (binary ->(000 00 110) gets info form keyboard  puts in Ax 
#define PUT    7           // (binary ->(000 00 111)opCode for put 
//isDigitOrNeg

// 2 part commands & jump
#define JE   0          // jump equal			  
#define JNE  1         // jump not equal 
#define JA   2        // jump above
#define JAE  3       // jump above equal
#define JB   4      // jump below
#define JBE  5     // jump below equal
#define JMP  6    // jump always
#define JUMP 1   // any jump( 8 but bit shifting 3 makes it 1) 8?

//Boolean Logic
#define TRUE  1        //  positive flag a>b
#define FALSE 0       //  negative flag  a<b

//3 part commands
#define CMP     96           // (binary ->(011 xx xxx) compares two operands and sets the flag 
#define ADD     160         // (binary ->(101 xx xxx) opCode for ADD   3part command
#define MOVREG  192        // (binary ->(110 xx xxx) opCode for MOVreg 
#define MOVMEM  224       // (binary ->(111 xx xxx) opCode for movmem 


/**1. Registers and add the stack pointer********************
* Description: Simulates the CPU's internal storage(hardware)
*   It holds the four general purpose registers:
*   - AX,BX,CX,DX
*   - a flag(7) that is the status for logic ops and stack pointer
*
*-------------------------------------------------------------*/

struct Registers
{
    int AX;
    int BX;
    int CX;
    int DX;
    int flag;
}
regis;

//**2.GLOBAL VARIABLES***********************

typedef short int Memory;
Memory memory[MAX] = { 0 };
Memory address;
Memory stackPointer;

//**3.CORE ASSEMBLER 
// function prototypes for functions that will be changed, the function name may not be changed 

void splitCommand(char line[], char part1[], char part2[], char part3[]); // splits line of asm into it's three parts	
void convertToMachineCode(FILE* fin);                                    // Converts a single line of ASM to machine code	
void assembler();                                                       // Converts the entire ASM file, reads and  stores in memory
void runMachineCode();                                                 // Executes the machine code	
void putValue(int operand, int value);                                //stores value into specific register

Memory getValue(Memory operand);                                    //retrieves value form register or constant

//** 4.Helper functions,****************************
//**these functions may be changed *******************

int changeToNumber(char line[], int start);
//** 4a. Helper functions prototypes*******************
//******DO NOT CHANGE **********

int  whichOperand(char operand[LINE_SIZE]);               // Returns register IDs to operand strings 
void changeToLowerCase(char line[]);                    // Normalizes input to make sure commands are case-sensitive
void printMemoryDump();                                // Prints current state of memory(RAM) w/ commands rep. as int (for debugging)
int  isDigitOrNeg(char letter);                        // validates if beginning of a character is  positive or negative number
void registerStartValues();                          // Randomizes all registers and flag values at the start 

/***************************** MAIN *******************************/

int main()
{
    registerStartValues();
    address = 0;
    stackPointer = 0;

    assembler();
    printf("\n\n---Convert to Memory complete---\n");

    printMemoryDump();

    runMachineCode();

    printf("\n\n---Final Memory dump in integers---\n");
    printMemoryDump();

    printf("\n");

    return 0;
} //end MAIN

    /******************** F1. getValue ***********************
*  Description:
*        Retrieves and Returns the value associated with the specified operand.
*
*  Parameters:
*   -     Memory operand: Operand identifier  representing a register
*         (AXREG, BXREG, CXREG, DXREG), a constant (CONSTANT)
*         or a memory  reference (ADDR,BXADDER or BXPLUS).
 * Return value:
*      -Memory : The value associated with the specified operand.
*       Returns: -1 if the operand is invalid
    -----------------------------------------------------------*/

Memory getValue(Memory operand)
{
    Memory value1;
    if (operand == AXREG)
    {
        return regis.AX;
    }
    if (operand == BXREG)
    {
        return regis.BX;
    }
    if (operand == CXREG)
    {
        return regis.CX;
    }
    if (operand == DXREG)
    {
        return regis.DX;
    }
    if (operand == CONSTANT)
    {
        int value = memory[address];
        address++;
        return value;
    }
    if (operand == ADDR)
    {
        int Location = memory[address];
        address++;
        int numIwant = memory[Location];
        return numIwant;
    }
    if (operand == BXADDR)
    {
        return memory[regis.BX];
    }
    if (operand == BXPLUS)
    {
        int location = memory[address];
        address++;
        return memory[regis.BX + location];             // adding to the offset location
    }
    return -1;
}//  getValue

    /******************** F2. putValue **********************************
* Description:
*    Stores a value in specified destination operand.
*   The current destination may be :
*    a register(AX, BX,CX,or Dx) or memory reference (ADDR,BXADDER,or BXPLUS))
*
*
* Parameters:
*  - int operand : Destination operand identifier that specifies
*                   where the value will be stored.
*  - int value   : Value to be stored in destination operand
*
* Return value: None(void)
    -----------------------------------------------------------------*/

void putValue(int operand, int value)
{
    if (operand == AXREG)
    {
        regis.AX = value;
    }
    else if (operand == BXREG)
    {
        regis.BX = value;
    }
    else if (operand == CXREG)
    {
        regis.CX = value;
    }
    else if (operand == DXREG)
    {
        regis.DX = value;
    }
    else if (operand == ADDR)
    {
        int location = memory[address];
        address++;
        memory[location] = value;
    }
    else if (operand == BXADDR)
    {
        memory[regis.BX] = value;
    }
    else if (operand == BXPLUS)
    {
        int location = memory[address];
        address++;
        memory[regis.BX + location] = value;

    }
}// end of void putValue

/******************** F3. assembler ************************
* Description:
*   Reads asm file, converts to machine code and stores in memory
*   instruction into machine code, and stores the resulting
*   machine code in memory.
*
* parameters: none
* Return value: none (void)
-----------------------------------------------------------*/
void assembler()
{
    printf("Entered assembler()\n");
    address = 0;
    FILE* fin; 
    fopen_s(&fin, ASM_FILE_NAME, "r");

  
    if (fin == NULL)
    {
        printf("Error, file didn't open\n\nExiting program...\n\n");
        system("pause");
        exit(1);
    } // end open file

    for (int i = 0; i < MAX && !feof(fin); i++) 
    {
        convertToMachineCode(fin);
    }    //end forloop
} //end assembler 

/******************** F4. convertToMachineCode   *******************************************************************
* Description: Converts a single line of ASM to machine code from ASM file
*  1. Reads a line or asm code from source file, 
*    converts the instruction into machine code, and stores
*    the resulting machine code in memory.
* 
* parameters:
*  -FILE* fin- pointer to the input assembly file
*
* return value:
 *            none (void)
---------------------------------------------------------------------*/
void convertToMachineCode(FILE* fin)
{
    char line[LINE_SIZE];             // array of chars; reads line in asm and stores it             
    char part1[LINE_SIZE];           // p1 of asm line, "the command" (MOV,ADD,etc)            
    char part2[LINE_SIZE] = " ";    // p2 of asm line, "the destination"(AX,BX,[100],ect)           
    char part3[LINE_SIZE] = " ";   // p3 of asm line, "the source" (where it came from 10,AX,ect)

    int machineCode = 0;

    if (fgets(line, LINE_SIZE, fin) == NULL)
    {
        {
            return;
        }
    }
    changeToLowerCase(line);

    if (line[0] == ';')             // skips comments ";" from asm file 
    {
        return;
    }
    if (line[0] == '\n')             //(part 5) skips blank lines"\n" from asm files
    {
        address++;
        return;
    }
    if (isDigitOrNeg(line[0]))
    {
        changeToNumber(line, 0);
        memory[address] = changeToNumber(line, 0);
        address++;
        return;
    }
    splitCommand(line, part1, part2, part3);                  // calls split command 

    //for debugging, comment out when you have most commands converted.
    printf("\nCommand-CTMC-: |p1 = %s |p2= %s |p3= %s\n", part1, part2, part3);
    printf("\n");
    //====================================
    // 1 -PART COMMANDS
    // [Halt , PUT, GET,isDigitNeg], fun, Ret
    // 
    //=====================================
    if (part1[0] == 'h')                // Checks part 1 (the Instruction ) for HAlt instruction
    {
        memory[address] = HALT;         // Save HALT in memory, HALT= 5 
        printf("The COMMAND-H is : %d\n", memory[address]);
        address++;
        return;
    }
    if (part1[0] == 'p')                    // checks part1 for PUT instruction
    {
        memory[address] = PUT;
        printf("The COMMAND-P is : %d\n", memory[address]);
        address++;
        return;
    }
    if (part1[0] == 'g')                        // checks part1 for GET instruction
    {
        memory[address] = GET;
        printf("The COMMAND-G is : %d\n", memory[address]);
        address++;
        return;
    }
    if (part1[0] == 'r')                    // checks part1 for PUT instruction
    {
        memory[address] = RET;
        printf("The COMMAND-R is : %d\n", memory[address]);
        address++;
        return;
    }
    printf("\npart1 ='%s'", part1);
    if (isDigitOrNeg(part1[0]))
    {
        memory[address] = changeToNumber(part1, 0);
        printf("The COMMAND-ND is : %d\n", memory[address]);
        address++;
        return;
    }
    if (part1[0] == 'f')
    {
        memory[address] = FUN;
        printf("The COMMAND is-F : %d\n", memory[address]);
        address++;

        memory[address] = changeToNumber(line, 4);
        address++;
        int index = 6;
        while (line[index] != ']')
        {
            index++;
        }
        index += 2;

        int numberOfParameters = changeToNumber(line, index);  // number of parameters variable
        memory[address] = numberOfParameters;                   // number of parameters
        address++;
        for (int i = 0; i < numberOfParameters; i++)
        {
            index++;
            while (line[index] != ' ')
            {
                index++;             //8
            }
            memory[address] = changeToNumber(line, index);
            address++;
        }
        address++;      // Put an Empty line for the return value

        return;
    }
    //====================================
    // 2 -PART COMMANDS
    // jae, jne, jbe, jmp, jne, je, jb, ja
    // 
    //=====================================
    printf("\nBefore shift memory[% d] = % d\n", address, memory[address]); //DEBUGGING

    if (part1[0] == 'j')					     // optional jump function
    {
        if (part1[1] == 'a')					 //jump above
        {
            if (part1[2] == 'e') 				 //jump above equal
            {
                memory[address] = JAE;
            }
            else
            {
                memory[address] = JA;
            }
        }
        else if (part1[1] == 'b')                //jump below
        {
            if (part1[2] == 'e')                //jump below equal
            {
                memory[address] = JBE;
            }
            else
            {
                memory[address] = JB;
            }
        }
        else if (part1[1] == 'm')                //jump always
        {
            memory[address] = JMP;
        }
        else if (part1[1] == 'n')                //jump not equal
        {
            memory[address] = JNE;
        }
        else if (part1[1] == 'e')               //jump equal
        {
            memory[address] = JE;
        }
        memory[address] += JUMP << 3;      // put the jump command in the first 3 bits of machine code, JUMP = 1 but bit shifting 3 makes it 8, which is the opcode for all jumps

        printf("After shift memory[%d] = %d\n", address, memory[address]); //shows memory address after command is added
        printf("The COMMAND is-j: %d\n", memory[address]);

        address++;
        memory[address] = changeToNumber(part2, 0);
        printf("After++ target shift memory[%d] = %d\n", address, memory[address]);

        address++;
        return;
    }
    /***********3 Part Commands********************/
    //====================================
    // 3 PART COMMANDS
    // mov reg, mov [ mem, add , cmp
    // 
    //=====================================

    int operand2 = 0;
    if
        (part1[0] == 'm')                             // if part 1 ==0 , 0== m           
    {
        if (part2[0] == '[' || line[4] == '[')
        {
            machineCode = MOVMEM;
            char temp[LINE_SIZE];
            strcpy(temp, part2);				   // puts part 2 into temp, 
            strcpy(part2, part3);				  // part 3 goes into part 2
            strcpy(part3, temp);                 // swap done temp (p2) gpies
            printf("The COMMAND is-mm: %d\n", machineCode);
        }
        else
        {
            machineCode = MOVREG;
            printf("The COMMAND is-mr: %d\n", machineCode);
        }
    }
    else if (part1[0] == 'a')
    {
        machineCode = ADD;
        printf("The COMMAND is-a: %d\n", machineCode);
    }
    else if (part1[0] == 'c')
    {
        machineCode = CMP;
        printf("The COMMAND is-cmp: %d\n", machineCode);
    }
    operand2 = whichOperand(part3);
    machineCode = machineCode + (whichOperand(part2) << 3);
    machineCode = machineCode + operand2;
    printf("The COMMAND is-: %d\n", machineCode);                            // check if number is correct , binary number mov= 110, cx=10,a=0 b1,c=2, constant =111  get decimal number of 215
    memory[address] = machineCode;
    address++;

    if (operand2 == CONSTANT || operand2 == ADDR)
    {
        memory[address] = changeToNumber(part3, 0);
        address++;
    }
    if (operand2 == BXPLUS)
    {
        memory[address] = changeToNumber(part3, 4);
        address++;
    }
} // end convertToMachineCode

 /******************** F5. runMachineCode   *********************************
* Description: 
*         Simulates a virtual machine by executing the machine code
*        instructions  stored in memory. The function processes  arithmetic 
*        operations, data movement instructions, comparisons, jumps input/output
*        instructions, function calls and returns until a HALT instruction 
*        is reached. 
*             
*
* parameters: none
* returns: none (void)
*----------------------------------------------------------*/
void runMachineCode()
{
#define MASK1 224                 //111 00 000   
#define MASK2 24                 //000 11 000
#define MASK3 7	                //000 00 111  put

    Memory part1, part2, part3;
    int value1, value2;
    int doJump;
    int newAddress;
    int returnAddress;

    address = 0;                              //reset address to the first item in memory
    stackPointer = MAX - 1;
    Memory fullCommand = memory[address];     //not yet broken into the three parts
    address++;

    while (fullCommand != HALT)
    {
        part1 = fullCommand & MASK1;             // get the parts by ANDing with masks
        part2 = (fullCommand & MASK2) >> 3;      // copy and past for p1& p2 bitshift 3
        part3 = fullCommand & MASK3;

        //====================================
        //BLOCK 1: THREE- PART COMMANDS
        // Logic: [OPcode] {Target] [Source]
        // mov reg, mov mem,  add ,cmp
        //=====================================

        if (part1 == MOVREG) // part1 = MOVREG
        {
            value1 = getValue(part3);                                    //get value from part3,(seocnd operand) the being moved
            printf("\nValue in runMachineCode MOVREG: %d\n", value1);   // debugging, see where value is at
            putValue(part2, value1);
        }
        else if (part1 == MOVMEM)                                //part1 = movmem
        {
            value1 = getValue(part2);
            printf("\nValue in runMachineCode MOVMEM: %d\n", value1);
            printf("Address = %d\n", address);
            putValue(part3, value1);
        }
        else if (part1 == ADD)                                     // Part1 = ADD
        {
            value1 = getValue(part2);
            value2 = getValue(part3);
            printf("\nValue in runMachineCode ADD: %d\n", value1);
            printf("Value in runMachineCode ADD: %d\n", value2);
            //  putValue(value1 + value2, part2);

            putValue(part2, value1 + value2);
        }
        else if (part1 == CMP)                                    //part3 = CMP
        {
            value1 = getValue(part2);
            value2 = getValue(part3);                            //set flag  based on compare

            printf("\nValue in runMachineCode CMP: %d\n", value1);
            printf("Value in runMachineCode CMP: %d\n", value2);

            if (value1 < value2)                                // val1 less than val2 --> flag = -1
            {
                regis.flag = -1;
            }
            else if (value1 == value2)                         // equal values --> flag is 0
            {
                regis.flag = 0;
            }
            else                                             // val1 greater than val2 --> flag is 1
            {
                regis.flag = 1;
            }
        }// end 3 part commands
            //===========================================
            // BLOCK 2: two - PART COMMANDS
            // Logic: [Opcode/ CONDITION] [Jump Address]
            // jae, jne, jbe, jmp, jne, je, jb, ja
            //=============================================

        else if (part2 == JUMP)			                        // JMP opcode =8
        {
            int doJump = FALSE;

            printf("\n ----------JUMP DEBUG----------\n");
            printf("Flag = %d\n", regis.flag); // shows current flag val for debugging
            printf("part3 = %d\n", part3);    // shows current part3 val for debugging

            newAddress = memory[address];

            printf("Target = %d\n", newAddress); // shows target address for debugging

            address++;

            // -----------FLAG = 0 (equal)--------------------
            if (regis.flag == 0)
            {
                if (part3 == JE || part3 == JAE || part3 == JBE)
                {
                    doJump = TRUE;
                }
            }
            // -----------FLAG = 1  (Greater/above)--------------------
            else if (regis.flag == 1)
            {
                if (part3 == JA || part3 == JAE || part3 == JNE)
                {
                    doJump = TRUE;
                }
            }
            // -----------FLAG = -1 (less/)--------------------
            else if (regis.flag == -1)            //Condition : Below /LESS
            {
                if (part3 == JB || part3 == JBE || part3 == JNE)
                {
                    doJump = TRUE;
                }
            }
            else if (part3 == JMP)              // always check for the unconditional JMP 8
            {
                doJump = TRUE;
            }
            if (doJump == TRUE)
            {
                printf("JUMP TAKEN to:  %d\n", newAddress);

                address = newAddress;
            }
            else
            {
                printf("JUMP NOT TAKEN\n");
            }
            printf("(RMC)FullCommand: %d, |p1 = %d |p2= %d |p3= %d\n", fullCommand, part1, part2, part3); // debugging
        } //end jumps	
           //====================================
           // ----------PART3----------
           // 1 -PART COMMANDS
           // PUT, GET, isDigitNeg],FUN, ret
           // 
          //=====================================
        else if (part3 == PUT) //  part 3 = put 111
        {
            printf("\t\tAX (put) is: %d\n", regis.AX); //outputs put vaue of ax
        }
        else if (part3 == GET)
        {

            printf("\t\t Enter a Number: ");             // gets number from user and puts it in AX
            scanf("%d", &(regis.AX));                                   //  get input and put it in AX using scanf
            printf("\t\t Your new AX value IS NOW: %d\n", regis.AX); //outputs the value of ax from user input (get)

            printf("\nNEXT ADDRESS = %d\n", address);               // debugging shows the next address in memory after get is called
        }
        else if (part3 == FUN)
        {
            int funLocation = memory[address];
            address++;
            int addressOfNumParameters = address;               // declare address of parameter variable

            memory[funLocation - 1] = addressOfNumParameters; //   address of parameters
            returnAddress = address + memory[addressOfNumParameters] + 2; // calculate return address   22 +3+2 =27

            memory[stackPointer] = regis.flag;
            stackPointer--;

            memory[stackPointer] = regis.AX;
            stackPointer--;
            memory[stackPointer] = regis.BX;
            stackPointer--;
            memory[stackPointer] = regis.CX;

            stackPointer--;
            memory[stackPointer] = regis.DX;
            stackPointer--;
            memory[stackPointer] = returnAddress;
            stackPointer--;

            address = funLocation;
        }//end function
        else if (part3 == RET)
        {
            stackPointer++;
            returnAddress = memory[stackPointer];
            // whatever is in AX, placed in the location after the function call
            memory[returnAddress - 1] = regis.AX;
            stackPointer++;

            regis.DX = memory[stackPointer];
            stackPointer++;

            regis.CX = memory[stackPointer];
            stackPointer++;

            regis.BX = memory[stackPointer];
            stackPointer++;

            regis.AX = memory[stackPointer];
            stackPointer++;

            regis.flag = memory[stackPointer];
            address = returnAddress;
        }
        printf("\tpart1= %d part2= %d part3= %d", part1, part2, part3); // debugging

        if (address > 0)        //debugging tool change address to show what you are unsure about
        {
            printMemoryDump(); //debugging, comment out at end of semester 
        }

        fullCommand = memory[address]; // not yet broken into the stops the infinate loop 
        address++;
    }//end part 3, 1 part commands
}    // end runMachineCode 

/*********************************************************************************
/**********************HELPER FUNCTIONS******************************************
/******************Given with starting code my not be changed no rewritten *******
**********************Unless it is indicated in the function header comment*******
**********************************************************************************


/*********************************************************************************
/************** HELPER FUNCTIONS for Converting to machine code  *****************
/*********************************************************************************


/******************** F6. splitCommand  ***********************************
*
* Description: 
*           Separates a single assembly language instruction into its
*            command, destination operand, and source operand
*
* Parameters:
*  - char line [] : Assembly language instruction to be split. 
*    char part1[] : stores teh command  portion of the instruction
*    char part2[] : stores the destination operand.
*    char part3[] : Stores teh source operand
* 
* Returns:
*    None(void)
**********************************************************************/

void splitCommand(char line[], char part1[], char part2[], char part3[])
{
    int lineIndex = 0;         // char location in line
    int partIndex = 0;         //initialize to 0; char location in p1, p2, p3
    part2[0] = '\0';           //initialize p2 & p3 to "\0" = end string array and empty to start with
    part3[0] = '\0';           //in case there is only one or no operand 

    printf(" \n|(SC) Line is: %s", line);

    while (line[lineIndex] != ' ' && line[lineIndex] != '\0' && line[lineIndex] != '\n') 
    {
        part1[partIndex] = line[lineIndex];    // take what is in Line[  ] --> part 1[  ] (loops through)
        lineIndex++;
        partIndex++;
    }
    part1[partIndex] = '\0';

    if (line[lineIndex] != '\0')
    {
        lineIndex++;
        partIndex = 0;

        while (line[lineIndex] != ' ' && line[lineIndex] != '\0' && line[lineIndex] != '\n')
        {
            part2[partIndex] = line[lineIndex];   //copy the characters from line into the two parts, part2 and part3
            lineIndex++;                         // moves lineIndex one space
            partIndex++;                        // moves part Index one space
        }
        part2[partIndex] = '\0';              // string stopper character (outside of part2)
        lineIndex++;
        partIndex = 0;

        while (line[lineIndex] != ' ' && line[lineIndex] != '\0' && line[lineIndex] != '\n')
        {
            part3[partIndex] = line[lineIndex];
            lineIndex++;
            partIndex++;
        }
        part3[partIndex] = '\0';

        printf("(sC)p1 is: %s| p2 is: %s| p3 is: %s|\n", part1, part2, part3); //debugging
    } //end if
}//end splitCommand

/********************* F7. whichOperand   ***************************
*
* Description: Determines which operand is being represented by an operand
*           string and returns the corresponding operand identifier.
*		0-3 registers
*		4 bx address - added in part 7
*		5 bx address plus offset -added in part 7
*		6 address -added in part 5
*		7 constant
*parameters:
*  - Char operand[LINE_SIZE] - checks first Char of operand to identify op or reg
* Returns:
*  -AXREG<BXREG<CXREG<DXREG : Register operand
*  -BXADDR                 : BX indirect address operand
* - BXPLUS                 : BX plus offset operand.
* - CONSTANT               : Constant operand.
* - -1                     : Invalid operand

*********************************************************************/

int whichOperand(char operand[LINE_SIZE])
{
    char letter = operand[0];

    if (letter == 'a')
    {
        return AXREG;
    }
    else if (letter == 'b')
    {
        return BXREG;
    }
    else if (letter == 'c')
    {
        return CXREG;
    }
    else if (letter == 'd')
    {
        return DXREG;
    }
    else if (letter == '[')
    {
        if (operand[1] == 'b')
        {
            if (operand[3] == '+')
            {
                return BXPLUS;
            }
            else
            {
                return BXADDR;
            }
        } 
        return ADDR;
    }
    else if (isDigitOrNeg(letter))
    {
        return CONSTANT;
    }
      return -1;                    //something went wrong if -1 is returned
}  //end whichOperand

/******************* F8. ChangeToNumber ********************
* Description: 
*           Converts a numeric value stored in a character
*           string into an integer.
*
* Parameters:
*	- char line[] : Character string containing  the number
*  -  int start   : Starting position of the number within the character
*                   string. The starting location in string where number
*                   is located
* Return value:
*          - int : Integer value represented by the character string.
*----------------------------------------------------------------------*/

int changeToNumber(char line[], int start)
{
    int value = 0 ; 
    char number[16];
    int negative = 0;

    int i = 0;

    while (line[start] == '[' || line[start] == ' ')
    {
        start++;
    }
    if (line[start] == '-')
    {
        start++;
        negative = 1; 
    }
    while (i < 16 && isdigit(line[start]))
    {
        number[i] = line[start];
        i++;
        start++;
    }
    number[i] = '\0';
    value = atoi(number);                                //atoi converts string of dig int int value 
    if (negative == 1)
    {
        value = -value;
    }
    return value;
}//end changeToNumber

/********************* F9. changeToLowerCase   ********************
* Description: 
*           Converts all alphabetic characters in a character 
*           string  to lowercase.
*
* Parameters:
*  -char line[] - line of asm code that is converted to lowercase
*
*return value: none(void)
----------------------------------------------------------------*/
void changeToLowerCase(char line[])
{
    int index = 0;
    while (index < strlen(line))
    {
        line[index] = tolower(line[index]);
        index++;
    }
} //end changeToLowerCase

/********************* F10. isDigitOrNeg   ********************
* Description: 
*           Determines whether a character represents the beginning
*            of  a numeric value.
*           string  to lowercase.
* Parameters:
* - char letter : the character being checked
*
*return value:
*  - 1 if the char is a digit of a negative sign
*  - 0 if the char is not a digit or a negative sign
----------------------------------------------------------------*/
int isDigitOrNeg(char letter)
{
    int retValue = 0;
    if (letter == '-' || isdigit(letter))
    {
        retValue = 1;
    }
    return retValue;
} //end isDigitNeg

/********************* F11. registerStartValues  *****************************
*Description: Randomizes all registers and flag values at start of program (simulates hardware boot)
*  1. Uses srand to start the random number generations
*  2. Gives each register and flag a random value between 2 and 9999 (inclusive)
*
*parameters: none
*
*return value: none (void)
--------------------------------------------------------------------*/
void registerStartValues()
{
    //	stackPointer = 0;
    srand(0); //starts random number generator, will always have the same values, best for debugging
    //srand(time(NULL) ); 
    regis.AX = rand() % (9999 - 2 + 1) + 2; // rand() % (max - min + 1) + min
    regis.BX = rand() % 9998 + 2;
    regis.CX = rand() % 9998 + 2;
    regis.DX = rand() % 9998 + 2;
    regis.flag = rand() % 9998 + 2;
} //end of registerStartValues

/*********************************************************************************
/***********************   HELPER FUNCTIONS for debugging  ***********************
/*********************************************************************************

/**************************** F12. printMemoryDump   ********************************
* Description : prints memory out as integers
*	 MAX (constant) is the amount of elements in the memory memory array
*	 COL (constant) is the number of columns that are to be displayed
*
* parameters: none
*
* return value: none(void)
**************************************************************************************/
void printMemoryDump()
{
    int numRows = MAX / COL + 1; //number of rows that will print
    int carryOver = MAX % COL; //number of columns on the bottom row
    int location; 
    printf("\n");
    for (int row = 0; row < numRows; row++)
    {
        location = row;
        for (int column = 0; location < MAX && column < COL; column++)
        {
            if (!(numRows - 1 == row && carryOver - 1 < column))
            {
                printf("%5d.%5d", location, memory[location]);
                location += (numRows - (carryOver - 1 < column));
            }
        }
        printf("\n");
    }
    printf("\t%7s: %d\n", "AX", regis.AX);
    printf("\t%7s: %d\n", "BX", regis.BX);
    printf("\t%7s: %d\n", "CX", regis.CX);
    printf("\t%7s: %d\n", "DX", regis.DX);
    printf("\t%9s: %d\n", "flag", regis.flag);
    printf("Address: %d\n", address);
    printf("Stack Pointer: %d\n", stackPointer);
 /*   printf("\t\t The total entered numbers IS NOW: %d\n", regis.CX);
    printf("\t\t The total SUM of numbers IS NOW: %d\n", regis.DX);*/

} //end printMemoryDump 

 /***NEEDS WORK*** PART 7
*
* runMachineCode
*  ADD function not working
*   bx AX not
* only doing one command and then infinate loop
*   there was and error with parenthisis  placement
*     fullCommand = memory[address];
*      address++   was outside of the while loop , so it was not updating the fullCommand and  address was not incrementing
*
******************************
* 1.removed float line
* 2. removed extra startingValues
* 3. fixed getValue prototype
* 4. fixed put value prototype
* 5.added address ++
  ASM 7
  fixed infinate  loop
  fixed jumps 



******/
