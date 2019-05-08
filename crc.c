#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*xor function of two binary number string format*/
char * xor(char * str1, char * str2) {
	char *result = ( char* )malloc( sizeof( char ) * 256 );
	int i;
	for(i = 0; i < strlen(str1); i++) {
		if(str1[i] == str2[i])
			result[i] = '0';
		else result[i] = '1';
	}
	result[++i] = '\0';
	return result;
}

void decaler(char* str) {
	int n = strlen(str),j;
	for(j = 0; j < n; j++) {
			str[j] = str[j+1];
	}
}
/*change this function to optimize the calculation*/
void CompleteTrame(char *subtrame, char *trame, int index ,int* position) {
	subtrame[index] = trame[*position];
	*position = *position + 1;
}
char * crc(char* trame, char* code) {
	
	//1-décalage de la trame
	int m = strlen(trame);
	int n = strlen(code);
	int lastPosition = n;
	char * newTrame = (char*)malloc(sizeof(char)*(m+n));
	strcpy(newTrame, trame);
	/*add n zero to right of newTrame*/
	int i;
	for(i = m; i < n+m-1; i++) {
		newTrame[i] = '0';
	}
	newTrame[++i] = '\0';
	
	//2-extraire les n premiers bits de la trame décalée
	char * subTrame; 
	subTrame = (char *)malloc((n+1)*sizeof(char));
		strncpy(subTrame, newTrame, n);
	while(lastPosition <= m+n-1 ) {
		
		if(subTrame[0] == '1') {
			
			subTrame = xor(subTrame, code);
		}	
		
		decaler(subTrame);

		CompleteTrame(subTrame,newTrame,n-1,&lastPosition);

	}

	return strcat(trame, subTrame);
}
int isValid(char* trame, char* code) {

	//faire le xor jusqu'à la fin de la trame
	//modifier la fonction crc
		//enlever la partie de bourrage de la trame
	
	int m = strlen(trame);
	int n = strlen(code);
	int lastPosition = n;
	
	//2-extraire les n premiers bits de la trame décalée
	char * subTrame; 
	subTrame = (char *)malloc((n+1)*sizeof(char));
		strncpy(subTrame, trame, n);
	while(lastPosition <= m+n-1 && strlen(subTrame) >= n) {
		
		if(subTrame[0] == '1') {
			
			subTrame = xor(subTrame, code);
		}	
		
		decaler(subTrame);
		
		CompleteTrame(subTrame,trame,n-1,&lastPosition);

	}
	printf("le reste de la division de la trame transmise et le code : %s\n", subTrame);
	//verificer si tous les bits du reste sont à 0
	int k = 0;
	int size = strlen(subTrame);
	for(k = 0; k < size; k++) {
		if(subTrame[k] == 1)
			return 0;
	}
	return 1;
}
int main(int argc, char* argv[]) {

	char trame[256];
	char code[256];
	char* trameTrasmise;
	strcpy(trame, "11100111");
	strcpy(code, "10110");	
	trameTrasmise = crc(trame, code);
	printf("%s trame a envoyé\n", trameTrasmise);
	

	if(isValid(trameTrasmise,code) == 1) {
		printf("la trame a été transmise sans erreur\n");
	} else {
		printf("erreur lors de la transmission de la trame\n");
	}


	return 0;
}
