#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>

#include <string.h>

#define MAX_NAME_SIZE     15
#define MIN_NUMBER        -37676
#define MAX_NUMBER        32767
#define EMPTY_STACK       0
#define NO_FRAME_POINTER  0
#define MAX_STACK         25
#define ARGV_PARMS        128
#define RET_ADDR          255
#define F_MAIN            0
#define F_MULTIPLY        1

#define F_ADD             2

#define F_LAST            F_ADD

#define MAX_ARRAY         F_LAST+1

/* Structure to hold information */
struct funRec_s
{
    char funName[MAX_NAME_SIZE];    /* Function name */
    int (*fAddr)();                 /* actual function address */
    int stackPtr;                   /* current top of stack */
    unsigned long *stkFrameAddr;    /* Stack frame pointer */
    unsigned int stack[MAX_STACK];  /* Actual stack */
};

typedef struct funRec_s funRec_t;   /* Create type from struct */

funRec_t funArray[MAX_ARRAY];       /* Array of stack structures */

bool dump=false;                    /* If true, do a stack dump */

/* Text names of functions. Used only in init() */

char *funNames[MAX_NAME_SIZE]={"main()","multiply()","add()"};

/* This function creates the address of the stack frame by
 * getting the address address of the variable of the variable
 * holding the frame address.
 */
void makeFrame(int stack)
{
  funRec_t *currStack;

  currStack=&funArray[stack]; /* Get address of current stack information */
  /* get address of stkFrameAddr variable and assign it as the frame address */
  currStack->stkFrameAddr=(unsigned long *)&funArray[stack].stkFrameAddr;

  printf("-- [%s] Stack frame created --\n",funArray[stack].funName);
}

/* This function resets the stack frame address variable to 0 */
void delFrame(int stack)
{
  int curPtr;
  funRec_t *currStack;

  currStack=&funArray[stack]; /* Get current stack information */
  currStack->stkFrameAddr=NO_FRAME_POINTER;
  printf("--[%s] Stack frame deleted --\n",funArray[stack].funName);
}

/* This function pushes data on to the proper function stack.
 * val = the value to push on the stack
 * stack = the stack to push it on to
 */
int push(int val,int stack)
{
  int curPtr;           /* Current stack pointer */
  funRec_t *currStack;  /* Current stack information */

  currStack=&funArray[stack]; /* Get current stack information */
  curPtr=currStack->stackPtr; /* Get current stack pointer */
  /* If stack has reached the maximum size, we have blown the stack */
  if (curPtr >= MAX_STACK)
  {
    printf("Blown stack!\n");
    return -1;
  }
  currStack->stack[curPtr+1]=val; /* Push the value on to the stack */
  if (val == RET_ADDR)
    printf("----> pushed return address on the stack for %s\n",currStack->funName);
  else if (val == ARGV_PARMS)
      printf("----> pushed argv parameters on the stack for %s\n", currStack->funName);
  else
    printf("----> pushed %d onto the stack for %s\n",val,currStack->funName);
  currStack->stackPtr++; /* Advance the stack pointer */
}

/* This function pops values off the stack
 * stack  = the stack from which to pop the data off of
 */
int pop(int stack)
{
  int value;            /* value popped off */
  int curPtr;           /* Current value of stack pointer */
  funRec_t *currStack;  /* Current stack information */

  currStack=&funArray[stack]; /* Get current stack information */
  curPtr=currStack->stackPtr; /* Get current stack pointer */
  /* if stack is empty, we can't do anything */
  if (curPtr <= EMPTY_STACK)
  {
    printf("Nothing on stack!\n");
    return -1;
  }
  value=currStack->stack[curPtr]; /* Pop value off the stack */
  if (value == RET_ADDR)
    printf("<---- popped return address off the stack from %s\n",currStack->funName);
  else if (value == ARGV_PARMS)
    printf("<---- popped argv parameters off the stack from %s\n",currStack->funName);
  else
    printf("<---- popped %d off the stack from %s\n",value,currStack->funName);
  currStack->stackPtr--; /* Move the stack pointer by 1 */
  return value;
}

/* This function is to dump the stack. The information dumped is:
 * Stack frame address
 * Return address
 * Parameters
 * Stack address for each parameter
 */
void stackDump(int stack)
{
  int i;                /* Counter for looping through stack */
  int curPtr;           /* Value of current stack pointer */
  funRec_t *currStack;  /* Hold current stack information */


  if (dump == true)     /* If -d provided on command line */
  {
    currStack=&funArray[stack]; /* Get current stack information */
    curPtr=currStack->stackPtr; /* Get current stack pointer */

    printf("\n--------[ Stack dump for %s ]--------\n",currStack->funName);
    printf("%p:\t[stack frame]\n",currStack->stkFrameAddr);
    /* Loop through stack printing out stack contents */
    for (i=1;i<=curPtr;i++)
    {
      printf("%p+%d:\t",currStack->stkFrameAddr,i);
      if (currStack->stack[i] == RET_ADDR)
        printf("[return address]\n");
      else if (currStack->stack[i] == ARGV_PARMS)
        printf("[argv parameters]\n");
      else
        printf("%d\n",currStack->stack[i]); /* Actual values */
    }
    printf("-------------------------------------------\n\n");

  }
}

/* This function does the multiply function
 * It multiplies two numbers together
 * The two numbers are retrieved from the stack
 * using the pop() command.
 */
int multiply(void)
{
  int rValue;       /* result of multiply, pushed on stack */
  int num1,num2;    /* 2 values to multiply, popped off stack */
  int return_addr;  /* return address */

  stackDump(F_MULTIPLY);        /* Dump stack */
  num2=pop(F_MULTIPLY);         /* Get the second number */
  num1=pop(F_MULTIPLY);         /* Get the first number */
  return_addr=pop(F_MULTIPLY);  /* get the return address */
  rValue=num1*num2;             /* Perform multiply operation */
  push(rValue,F_MULTIPLY);      /* Push result onto stack */
  printf("Return from %s\n",funArray[F_MULTIPLY].funName);
  return (return_addr);
}

/* This function does the add function
 * It adds the two numbers together
 * The two numbers are retrieved from the stack
 * using the pop() command.
 */
int add(void)
{
  int rValue;       /* Result of add operation, pushed on stack */
  int num1,num2;    /* The values to add, popped off the stack */
  int return_addr;  /* The return value */

  stackDump(F_ADD);         /* Dump the stack */
  num2=pop(F_ADD);          /* Pop second number off the stack */
  num1=pop(F_ADD);          /* Pop first number off the stack */
  return_addr=pop(F_ADD);   /* Pop return address off stack */
  rValue=num1+num2;               /* Perform add operation */
  push(rValue,F_ADD);       /* Push result back on stack */
  
  printf("Return from %s\n",funArray[F_ADD].funName);
  return (return_addr);
}

/* This function initializes the data structures to
 * support the stack system
 * Initializes: stack to EMPTY_STACK
 *              stackPtr to EMPTY_STACK
 *              function name to proper name
 *              Stack frame address to NO_FRAME_POINTER
 *              Assign function addresses to be called
 */
void initSystem(void)

{
  int i;

  /* Loop through data structures to initialize them */
  
    for (i=0;i<=F_LAST;i++)
  
    {
    
	funArray[i].stack[0]=EMPTY_STACK;
    	funArray[i].stackPtr=EMPTY_STACK;
    /* set the names of the functions */
    	strncpy(funArray[i].funName,funNames[i],MAX_NAME_SIZE-1);
    	funArray[i].stkFrameAddr=NO_FRAME_POINTER;
    /* Assign pointers to functions */

       if (i==F_MULTIPLY)

		funArray[i].fAddr=&multiply;
    
       if (i==F_ADD)

	 	funArray[i].fAddr=&add;
    }

}

/* This function prints out the help screen */

void help(char *progName)

{
  
	printf("%s Help -->\n",progName);
  	printf("  %s [-d] -a <num> -b num -o <operator>\n", progName);
  
	printf("     -d = dump stack");
  
	printf("     -a <num> = a whole integer (-32767 to 32767)\n");
  
	printf("     -b <num> = a whole integer (-32767 to 32767)\n");
  
	printf("     -o <operation> = + or x\n\n");

}


/* This function parses the command line. It is looking for the
 * following:
 *    a = first number
 *    b = second number
 *    oper = operation such as '+' or 'x'
 * Also, and optional "-d" can be added to dump the stack
 */

int parseCmdLine(int argc, char *argv[],int *a, int *b, int *oper)

{
  
	extern char *optarg;
  
	int retValue=0;
  
	int c;

  
	while ((c=getopt(argc,argv,"hda:b:o:")) != -1)
  
	{
    
		switch(c)
    
		{
      
			case 'd':/* dump the stack */
        
				dump = true;
        
				break;
      
			case 'a': /* The first number */
        
				*a=atoi(optarg);
        			if ((*a < MIN_NUMBER) || (*a > MAX_NUMBER))
          				retValue=-2;
        
				break;
      
			case 'b':/* The second number */
        
				*b=atoi(optarg);
        
				if ((*b < MIN_NUMBER) || (*b > MAX_NUMBER))
          				retValue=-2;
        
				break;
      
			case 'o': /* Operations on numbers */
 
				if (strncmp(optarg,"+",1)==0)
          
					*oper=F_ADD;
        
				else if (strncmp(optarg,"x",1)==0)
          
					*oper=F_MULTIPLY;
        
				else
        
					{
          
						printf("Invalid operation [%s]\n",optarg);
          
						retValue = -1;
        
					}	
	        
				break;
      			case 'h':
        			help(argv[0]);
        				retValue = -3
      
			default:  /* unknown parameter */
        
				printf("Error: invalid option -%c\n",(char)c);
        
				retValue=-1;
    
		}
  
	}
  
	return (retValue);

}


int main(int argc, char *argv[])
{
  int value;
  int retValue;
  int parsRValue;
  int parseLoc=1;
  int num1=0;
  int num2=0;
  int oper;

/* ---------------[ Setup ]----------------------*/
  if (argc < 3)
  {
    
    printf ("Error: Not enough parameters\n");
    
    help(argv[0]);
    
    return(-1);
  
  }
  
  if ((parsRValue=parseCmdLine(argc, argv,&num1, &num2, &oper)) < 0)
  
  {
    if (parsRValue == -2)
      printf("\n[Error] One of the numbers is out of bounds: [%d] or [%d]\n",num1,num2);
    
      help(argv[0]);
    
      return (-1);
  
  }
  
  initSystem();
  /*------------------------------------------------*/
  
  printf("[%s] Starting\n",funArray[F_MAIN].funName);
  makeFrame(F_MAIN);
  push(RET_ADDR,F_MAIN);
  push(ARGV_PARMS,F_MAIN);
  stackDump(F_MAIN);
  makeFrame(oper);
  push(RET_ADDR,oper);
  push(num1,oper);
  push(num2,oper);
  
  printf("--[%s] Calling\n",funArray[oper].funName);
  retValue=funArray[oper].fAddr();
  value=pop(oper);
  delFrame(oper);
  printf("--[%s] Result=%d\n",funArray[F_MAIN].funName,value);
  
  printf("--[%s] Leaving\n",funArray[F_MAIN].funName);
  
  delFrame(F_MAIN);
  return 0;
}

