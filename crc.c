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
int main(int argc, char* argv[]) {

	char trame[256];
	char code[256];
	strcpy(trame, "100100");
	strcpy(code, "1101");	
	printf("%s\n",crc(trame, code));

	//*verification du message transmit*//
	//if xor(trameTrasmise, polynomeGrenrateur) == 0 donc la trame est correcte
	/*strcpy(trame, "100100001");
		
	printf("%s\n",crc("100100001", "1101"));*/
	return 0;
}
