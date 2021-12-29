#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b)) // Return the minimum of a and b

int minOpLen(
	int** Table,							// Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
	int idxS, int idxT, 					// (idxS = |SS|, idxT = |TS|) used to search solution from top-down in the Table
	int ins_cost, int del_cost, int sub_cost// insertion, deletion, substitution cost
);
//	Input:	Table, idxS, idxT, ins_cost, del_cost, sub_cost
//	Output:	Minimum size of OP, SR, and TR array (|OP| == |SR| == |TR|)

void getSOPT_insFirst(
	int** Table,							// Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
	int idxS, int idxT, 					// (idxS = |SS|, idxT = |TS|) used to search solution from top-down in the Table
	int opLen, 								// Minimum size of OP, SR, and TR array
	char* SS, char* TS, 					// Source and Edited string array
	char* SR, char* OP, char* TR, 			// Pointer to SR, OP, and TR array
	int ins_cost, int del_cost, int sub_cost// insertion, deletion, substitution cost
);
//	Input:	Table, idxS, idxT, opLen, SS, TS, ins_cost, del_cost, sub_cost
//	Output:	S, OP, T
//			(S: Source string with insertion marks, OP: Operation sequence, T: Edited string with deletions marks)

void getSOPT_delFirst(
	int** Table, 							// Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
	int idxS, int idxT, 					// (idxS = |SS|, idxT = |TS|) used to search solution from top-down in the Table
	int opLen, 								// Minimum size of OP, SR, and TR array
	char* SS, char* TS, 					// Source and Edited string array
	char* SR, char* OP, char* TR, 			// Pointer to SR, OP, and TR array
	int ins_cost, int del_cost, int sub_cost// insertion, deletion, substitution cost
);
//	Input:	Table, idxS, idxT, opLen, SS, TS, ins_cost, del_cost, sub_cost
//	Output:	S, OP, T
//			(S: Source string with insertion marks, OP: Operation sequence, T: Edited string with deletions marks)

void Edit_Distance(
		char* SS, char* TS,
		int ins_cost, int del_cost, int sub_cost,
		int** Table, char** SR, char** OP, char** TR,
		int* Mem_Allocated)
{
	int idxRow, idxCol, lenS, lenT, cost;
	int	opLen;

	// Get the last row and column indice of Table
	lenS = strlen(SS);
	lenT = strlen(TS);

	// Fill first row of Table
	for (idxRow = 0; idxRow <= lenS; idxRow++)
		Table[idxRow][0] = idxRow * del_cost;
	// Fill first column of Table
	for (idxCol = 0; idxCol <= lenT; idxCol++)
		Table[0][idxCol] = idxCol * ins_cost;

	// Fill rest of the table by iterating each row from lowest to largest index
	// using DP method of finding values of solutions bottom-up
	for (idxRow = 1; idxRow <= lenS; idxRow++) {
		for (idxCol = 1; idxCol <= lenT; idxCol++) {
			if (SS[idxRow-1] == TS[idxCol-1]) // No substitution cost when characters are the same
				cost = 0;
			else
				cost = sub_cost;

			// Assign minimum cost amongst deletion, insertion, and substitution at each step
			Table[idxRow][idxCol] = MIN(MIN(Table[idxRow - 1][idxCol] + del_cost, Table[idxRow][idxCol - 1] + ins_cost),
										Table[idxRow - 1][idxCol - 1] + cost);
		}
	}

	// Find minimum SR, OP, TR length
	opLen = minOpLen(Table, lenS, lenT, ins_cost, del_cost, sub_cost);

	*SR = new char[opLen];
	*OP = new char[opLen];
	*TR = new char[opLen];
	if (*SR == NULL || *OP == NULL || *TR == NULL) {
		printf("SR, OP, TR Memory Allocation Error\n");
		exit(-1);
	}
	else {
		*Mem_Allocated = 3 * opLen;
	}

	// Find solutions to SR, OP, TR array
	if(ins_cost <= del_cost)
		getSOPT_insFirst(Table, lenS, lenT, opLen, SS, TS, *SR, *OP, *TR, ins_cost, del_cost, sub_cost);
	else
		getSOPT_delFirst(Table, lenS, lenT, opLen, SS, TS, *SR, *OP, *TR, ins_cost, del_cost, sub_cost);
}

int minOpLen(int** Table, int idxS, int idxT, int ins_cost, int del_cost, int sub_cost) {
	// From |SR| == |OP| == |TR|, the function finds |TR| using
	// |TR| == |TS| + (# of deletions)

	int minLen = idxT;

	// Find the number of deletion operations
	while (idxS != 0 && idxT != 0) {
		if (Table[idxS - 1][idxT] == (Table[idxS][idxT] - del_cost)) {
			minLen++;
			idxS--;
		}
		else if (Table[idxS][idxT - 1] == (Table[idxS][idxT] - ins_cost)) {
			idxT--;
		}
		else {
			idxS--;
			idxT--;
		}
	}

	// Find the number of remaining deletion operations
	if (idxT == 0) {
		while (idxS > 0) {
			minLen++;
			idxS--;
		}
	}

	return minLen + 1; // Add 1 for null character
}

void getSOPT_insFirst(
	int** Table, int idxS, int idxT, int opLen,
	char* SS, char* TS,
	char* SR, char* OP, char* TR,
	int ins_cost, int del_cost, int sub_cost)
{
	// When there are two possible parents, insertion is prioritized

	opLen--; // Last index of OP, SR, TR
	SR[opLen] = OP[opLen] = TR[opLen] = '\0'; // Terminate SR, OP. TR with null

	// Find the order of operations from top -> bottom
	while (idxS != 0 && idxT != 0) {
		opLen--;

		if (Table[idxS][idxT - 1] == (Table[idxS][idxT] - ins_cost)) { // Insertion is used
			idxT--;
			SR[opLen] = '*';
			OP[opLen] = 'i';
			TR[opLen] = TS[idxT];
		}
		else if (Table[idxS - 1][idxT] == (Table[idxS][idxT] - del_cost)) { // Deletion is used
			idxS--;
			SR[opLen] = SS[idxS];
			OP[opLen] = 'd';
			TR[opLen] = '*';
		}
		else { // Neither insetion nor deletion is used

			if (Table[idxS][idxT] == Table[idxS - 1][idxT - 1]) { // No substitution
				SR[opLen] = SS[idxS - 1];
				OP[opLen] = '.';
				TR[opLen] = TS[idxT - 1];
			}
			else { // Substitution used
				SR[opLen] = SS[idxS - 1];
				OP[opLen] = 's';
				TR[opLen] = TS[idxT - 1];
			}
			idxS--;
			idxT--;
		}
	}

	if (idxS == 0) { // Find remaining insertions
		while (idxT > 0) {
			--opLen;
			--idxT;
			SR[opLen] = '*';
			OP[opLen] = 'i';
			TR[opLen] = TS[idxT];
		}
	}
	else { // Find remaining deletions
		while (idxS > 0) {
			--opLen;
			--idxS;
			SR[opLen] = SS[idxS];
			OP[opLen] = 'd';
			TR[opLen] = '*';
		}
	}
}

void getSOPT_delFirst(
	int** Table, int idxS, int idxT, int opLen,
	char* SS, char* TS,
	char* SR, char* OP, char* TR,
	int ins_cost, int del_cost, int sub_cost)
{
	// When there are two possible parents, deletion is prioritized

	opLen--; // Last index of OP, SR, TR
	SR[opLen] = OP[opLen] = TR[opLen] = '\0'; // Terminate SR, OP. TR with null

	// Find the order of operations from top -> bottom
	while (idxS != 0 && idxT != 0) {
		opLen--;

		if (Table[idxS - 1][idxT] == (Table[idxS][idxT] - del_cost)) { // Deletion is used
			idxS--;
			SR[opLen] = SS[idxS];
			OP[opLen] = 'd';
			TR[opLen] = '*';
		}
		else if (Table[idxS][idxT - 1] == (Table[idxS][idxT] - ins_cost)) { // Insertion is used
			idxT--;
			SR[opLen] = '*';
			OP[opLen] = 'i';
			TR[opLen] = TS[idxT];
		}
		else { // Neither insetion nor deletion is used

			if (Table[idxS][idxT] == Table[idxS - 1][idxT - 1]) { // No substitution
				SR[opLen] = SS[idxS - 1];
				OP[opLen] = '.';
				TR[opLen] = TS[idxT - 1];
			}
			else { // Substitution used
				SR[opLen] = SS[idxS - 1];
				OP[opLen] = 's';
				TR[opLen] = TS[idxT - 1];
			}
			idxS--;
			idxT--;
		}
	}

	if (idxS == 0) { // Find remaining insertions
		while (idxT > 0) {
			--opLen;
			--idxT;
			SR[opLen] = '*';
			OP[opLen] = 'i';
			TR[opLen] = TS[idxT];
		}
	}
	else { // Find remaining deletions
		while (idxS > 0) {
			--opLen;
			--idxS;
			SR[opLen] = SS[idxS];
			OP[opLen] = 'd';
			TR[opLen] = '*';
		}
	}
}