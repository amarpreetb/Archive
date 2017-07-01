
#include <stdio.h>


// ---------------------------------------
// place function PROTOTYPES below here...
// ---------------------------------------
void welcome(void);
void printTitle(void);
void printFooter(double gTotal);
void flushKeyboard(void);
void pause(void);
int getInt(void);
int getIntLimited(int lowerLimit, int upperLimit);
double getDouble(void);
double getDoubleLimited(double lowerLimit, double upperLimit);
int yes(void);
int menu(void);
void GroceryInventorySystem(void);
// user interface tools function PROTOTYPES:



// application user interface function PROTOTYPES:	
	
void welcome(void) {
		printf(">---=== Grocery Inventory System ===---<\n\n");
	}

void printTitle(void) {

		printf(">Row |SKU| Name               | Price  |Taxed| Qty | Min | Total      |Atn<\n");
		printf(">----+---+--------------------+--------+-----+-----+-----+------------|---<\n");
	}

void printFooter(double gTotal) {
		printf(">--------------------------------------------------------+----------------<\n");

		if (gTotal > 0) {
			printf("                                      Grand Total: | %12.21<\n", gTotal);
		}
	}

void flushKeyboard(void) {
		char clrKeyboard = 0;

		while (clrKeyboard != '\n') {
			scanf("%c", &clrKeyboard);
		}
	}

void pause(void) {
		printf("Press <ENTER> to continue...");
		flushKeyboard();
	}

int getInt(void) {

		int value;
		char charInput = 'x';

		while (charInput != '\n') {

			scanf("%d%c", &value, &charInput);
			if (charInput != '\n') {

				flushKeyboard();
				printf("Invalid integer, please try again: ");
			}
		}
		return value;
	}

int getIntLimited(int lowerLimit, int upperLimit) {
		int value;
		do {
			value = getInt();
			if (value < lowerLimit || value > upperLimit) {
				printf("Invalid value, %d < value < %d: ", lowerLimit, upperLimit);
			}
		} while (value < lowerLimit || value > upperLimit);
		return value;
	}

double getDouble(void) {
		double Value;
		char NL = 'x';
		while (NL != '\n') {
			scanf("%lf%c", &Value, &NL);
			if (NL != '\n') {
				flushKeyboard();
				printf("Invalid number, please try again: ");
			}
		}
		return Value;

	}

double getDoubleLimited(double lowerLimit, double upperLimit) {
		double value;
		do {
			value = getDouble();
			if (value < lowerLimit || value > upperLimit) {
				printf("Invalid value, %lf < value < %lf: ", lowerLimit, upperLimit);
			}
		} while (value < lowerLimit || value > upperLimit);
		return value;
	}


int yes(void) {
	char ch;
	int r = 0;
	do {
		scanf("%c", &ch);
		flushKeyboard();
		if (!((ch == 'Y') || (ch == 'y') || (ch == 'N') || (ch == 'n'))) {
			printf("Only (Y)es or (N)o are acceptable: ");

		}
	} while (!((ch == 'Y') || (ch == 'y') || (ch == 'N') || (ch == 'n')));
	if ((ch == 'Y') || (ch == 'y')) {
		r = 1;
	}
	return r;
	}


int menu(void) {
		int option = 0;
		printf("1- List all items\n");
		printf("2- Search by SKU\n");
		printf("3- Checkout an item\n");
		printf("4- Stock an item\n");
		printf("5- Add new item or update item\n");
		printf("6- delete item\n");
		printf("7- Search by name\n");
		printf("0- Exit program\n");
		printf("> ");
		scanf("%d", &option);
		flushKeyboard();
		while (option < 0 || option > 7) {
			printf("Invalid value, 0 < value < 7: ");
			scanf("%d", &option);
			flushKeyboard();
		}
		return option;
	}

void GroceryInventorySystem(void)
	{
		welcome();
		int o;
		int done = 0;
		while (!done) {
			o = menu();
			if (o == 1) {
				printf("List Items under construction!\n");
				pause();
			}

			else if (o == 2) {
				printf("Search Items under construction!\n");
				pause();
			}

			else if (o == 3) {
				printf("Checkout Item under construction!\n");
				pause();
			}

			else if (o == 4) {
				printf("Stock Item under construction!\n");
				pause();
			}

			else if (o == 5) {
				printf("Add/Update Item under construction!\n");
				pause();
			}

			else if (o == 6) {
				printf("Delete Item under construction!\n");
				pause();
			}
			else if (o == 7) {
				printf("Search by name under construction!\n");
				pause();
			}

			else if (o == 0) {
				printf("Exit the program? (Y)es/(N)o: ");
				done = yes();
			}
		}
	}