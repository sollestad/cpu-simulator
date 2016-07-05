#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000
#define WORDSIZE 32
#define TWO 2

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

int word[WORDSIZE] = { 0 };
int word1[WORDSIZE] = { 0 };
int word2[WORDSIZE] = { 0 };

void resetWord();
void printState(stateType *);
void decimalToBinary( int );
void twosComplement( int );
int nand( int, int );
int binaryToDecimal( void );

int
main(int argc, char *argv[])
{
    printf("\n");
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
	printf("error: usage: %s <machine-code file>\n", argv[0]);
    exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
	printf("error: can't open file %s", argv[1]);
	perror("fopen");
	exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
	state.numMemory++) {
	if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
	    printf("error in reading address %d\n", state.numMemory);
	    exit(1);
	}
	printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    //initialize all registers to zero
    int i;
    for( i = 0; i < 8; i++ ){
        state.reg[i] = 0;
    }
    state.pc = 0;

    printState( &state );
	
	int count = 1;
    while( state.pc != -1 && state.pc < state.numMemory ){
        int num = state.mem[state.pc] >> 22;
        num = num & 7;

        if( num > 7 || num < 0 ){
            printf("Error: incorrect opcode\n");
            exit(1);
        }
        else{
           switch( num ){
                case 0: 	//add
                    {
                        int num1 = state.mem[state.pc] >> 19;
                        num1 = num1 & 7;
                        int num2 = state.mem[state.pc] >> 16;
                        num2 = num2 & 7;
                        int num3 = state.mem[state.pc] & 7;
                        state.reg[num3] = state.reg[num1] + state.reg[num2];
                        state.pc++;
                        break;
                    }
                case 1:		//nand
                    {
                        int num1 = state.mem[state.pc] >> 19;
                        num1 = num1 & 7;
                        int num2 = state.mem[state.pc] >> 16;
                        num2 = num2 & 7;
                        int num3 = state.mem[state.pc] & 7;
                        state.reg[num3] = nand( state.reg[num1], state.reg[num2] );
                        state.pc++;
                        break;
                    }
                case 2:		//lw
                    {
                        int num1 = state.mem[state.pc] >> 19;
                        num1 = num1 & 7;
                        int num2 = state.mem[state.pc] >> 16;
                        num2 = num2 & 7;
                        int num3 = state.mem[state.pc] & 65535;
                        state.reg[num2] = state.mem[state.reg[num1]+num3];
                        state.pc++;
                        break;
                    }
                case 3:		//sw
                    {
                        int num1 = state.mem[state.pc] >> 19;
                        num1 = num1 & 7;
                        int num2 = state.mem[state.pc] >> 16;
                        num2 = num2 & 7;
                        int num3 = state.mem[state.pc] & 65535;
                        state.mem[state.reg[num1]+num3] = state.reg[num2];
                        state.pc++;
                        break;
                    }
                case 4:		//beq
                    {   
                        int num1 = state.mem[state.pc] >> 19;
                        num1 = num1 & 7;
                        int num2 = state.mem[state.pc] >> 16;
                        num2 = num2 & 7;
                        int test = state.mem[state.pc] & 32768;
                        int num3 = state.mem[state.pc] & 65535;
                        if( test == 32768 ) num3 = num3 - 65535;
                        else num3 = num3 + 1; 
                        state.pc++;
                        if( state.reg[num1] == state.reg[num2] ){
                            state.pc += num3-1;
                        }
                        break;
                    }

                case 5:		//cmov
                    {
                        int num1 = state.mem[state.pc] >> 19;
                        num1 = num1 & 7;
                        int num2 = state.mem[state.pc] >> 16;
                        num2 = num2 & 7;
                        int num3 = state.mem[state.pc] & 7;
                        if( state.reg[num2] != 0 ){
                            state.reg[num3] = state.reg[num1];
                        }
                        state.pc++;
                        break;
                    }
                case 6: 	//halt
                        state.pc++;
                        break;
                case 7:		//noop
                        state.pc++;
                        break;
            }
            int i;
            for( i = 0; i < 8; i++ ){
                if( state.reg[i] > 2147483647 || state.reg[i] < -2147483647 ){
                    printf("Error:  register %d out of bounds\n", i);
					printf("%d\n", state.reg[i]); 
                    exit(1);
                }
            }
            if( num == 6 ){
                printf("machine halted\n");
				printf("total of %d instructions executed\n", count);
                printf("final state of machine:\n");
                printState( &state );
                
                state.pc = -1;
            }
            //end switch
        }
		count++;
        if( state.pc != -1 ) printState( &state );
    }

    

    return(0);
}

void
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("end state\n");
}

void decimalToBinary( int d ){
    int x = 0, y = 0, p = 31, flag = 0;
	resetWord();
	
	
	if( d > -1 ){ 
        while( d != 0 ){
            x = pow( TWO, p );
            y = d/x;
            if( y == 0 ){
                p = p - 1;
            }
            else{
                d = d%x;
                word[p] = 1;
                p = p - 1;
            }
        }
		
    }
    else{
		d = abs( d );
		decimalToBinary( d );
        twosComplement( d );
    }
}

int nand( int a, int b ){
    decimalToBinary( a );
    int i;
	for( i = WORDSIZE-1; i > -1; i-- ){
        word1[i] = word[i];
		printf("%d", word1[i]);
    }
    printf("\n");
	
    decimalToBinary( b );
	for( i = WORDSIZE-1; i > -1; i-- ){
        word2[i] = word[i];
		printf("%d", word2[i]);
    }
	printf("\n");
	
    for( i = WORDSIZE-1; i > -1; i-- ){
        word[i] = word1[i] + word2[i];
        if( word[i] == 2 ) word[i] = 0;
        else word[i] = 1;
    }
	
	for( i = WORDSIZE-1; i > -1; i-- ){
        printf("%d", word[i]);
    }

    return binaryToDecimal();
}

int binaryToDecimal( void )
{   
    //takes in array type
    //converts bit array to decimal
    int i;
    int dec = 0;

    for( i = 31; i > -1; i-- ){
        if( word[i] > 0 ){
            dec += pow( 2, i );
        }
    }
    return dec;
}

void twosComplement( int d ){
 
    int i;
    for( i = 31; i > -1; i-- ){
        word[i] = !word[i];
    }
	
	for( i = 0; i < 32; i++ ){
        word[i] = word[i]+1;
        if( word[i] == 1 ) break;
        else if ( word[i] == 2 ){
            word[i] = 0;
        }
    }
	
}

void resetWord(){
	int i;
	for( i = 0; i < 32; i++ ){
		word[i] = 0;
	}
	
}