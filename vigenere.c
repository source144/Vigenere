// Gonen Matias
// CIS 3360, Spring 2020
// NID: go658748
// Version: 02/11/2020 02:08AM

// TODO: run against test cases
// TODO: Limit plaintext & ciphertext length (due to testcases)
// TODO: tidy up code!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define WARN(format, ...) fprintf(stderr, "VIGENERE: " format "\n", __VA_ARGS__)
#define DIE(format, ...) WARN(format, __VA_ARGS__), exit(EXIT_FAILURE)

// Constants
#define MAX_LEN 512
#define ROW_LEN 80
#define PAD		'x'

#pragma region "Struct defenitions"

typedef struct
{
	char *charAt;
	int length;
} string_t;

#pragma endregion

#pragma region "Function Prototypes"

// ~Implemented~
long getFileLength(FILE *fp);
string_t *readFile(char *filename);
string_t *destroyString(string_t *str);


// TODO: implement
string_t *simplify(string_t *str);
string_t *encrypt(string_t *plain, string_t *key);
string_t *pad(string_t *str);
void printc(string_t *str);


#pragma endregion

#pragma region "String Methods (string_t)"

string_t *destroyString(string_t *str)
{
	if (!str)
		return NULL;
		
	free(str->charAt);
	free(str);

	return NULL;
}

string_t *simplify(string_t *str)
{
	// TODO: not expecting NULL here.
	if (!str || !str->charAt)
		return NULL; // TODO: handle bad arg

	int newLen, i;
	char tmp[str->length];

	// Store simplified string in buffer
	for (i = newLen = 0; i < str->length; i++)
		if (isalpha(str->charAt[i]))
			tmp[newLen++] = tolower(str->charAt[i]);

	// Destroy the old string
	free(str->charAt);

	// Allocate memory for simplified string
	if (!(str->charAt = calloc(newLen + 1, sizeof(char))))
		DIE("%s", "bad memory allocation (encrypt)");

	// Move from buffer to alloc'd string
	for (i = 0; i < newLen; i++)
		str->charAt[i] = tmp[i];

	str->length = newLen;
	return str;
}

string_t *pad(string_t *str)
{
	// TODO: not expecting NULL here.
	if (!str && !(str = calloc(MAX_LEN + 1, sizeof(char))))
		DIE("%s", "bad memory allocation (pad)");

	char *padded;
	int i = 0;

	if (str->length >= MAX_LEN)
		return str;

	// Allocate memory for padded string
	if (!(padded = calloc(MAX_LEN + 1, sizeof(char))))
		DIE("%s", "bad memory allocation (pad)");

	// TODO: not expecting NULL here.
	if (str->charAt)
	{
		// Preserve existing string
		for (i = 0; i < str->length; i++)
			padded[i] = str->charAt[i];
	
		// Destroy the old string
		free(str->charAt);
	}

	// Pad existing string
	for (; i < MAX_LEN; i++)
		padded[i] = PAD;

	str->charAt = padded;
	str->length = MAX_LEN;
	return str;
}

#pragma endregion

#pragma region "File IO Methods (input)"

long getFileLength(FILE *fp)
{
	int res;

	// Reach EOF
	fseek(fp, 0, SEEK_END);

	// Save position indicator
	res = ftell(fp);
		
	// Reset position indicator
	fseek(fp, 0, SEEK_SET);

	return res;
}

string_t *readFile(char *filename)
{
	FILE *fp;
	string_t *str = calloc(1, sizeof(string_t));
	int fLen;
		
	if (!(fp = fopen(filename, "r")))
		DIE("%s", "Invalid file");

	// Get the length of the file
	fLen = getFileLength(fp);

	// Allocate memory to save file into memory
	str->length = fLen;
	str->charAt = calloc(str->length + 1, sizeof(char));

	// Read contents of file into str
	fread(str->charAt, sizeof(char), fLen, fp);

	return str;
}

#pragma endregion


string_t *encrypt(string_t *plain, string_t *key)
{
	int idx, i;
	string_t *cipher;

	if (!key || !key->charAt || !key->length)
		return plain; // FIXME: clone plain

	// Allocate memory for cipher string
	if (!(cipher = calloc(1, sizeof(string_t)))
		|| !(cipher->charAt = calloc(plain->length + 1, sizeof(char))))
		DIE("%s", "bad memory allocation (encrypt)");

	// Just trust me on this...
	for (i = 0; i < plain->length; i++)
		cipher->charAt[i] = (((plain->charAt[i] + key->charAt[i % key->length])) % 'a') % 26 + 'a';
	
	cipher->length = plain->length;
	return cipher;
}

void printc(string_t *str)
{
	int i;
	if (!str || !str->charAt)
		return;
	
	for (i = 0; i < str->length; i++)
		printf("%c%s", str->charAt[i], !((i + 1) % 80) ? "\n" : "");
	
	// TODO: do i want to end this with a newline?
}

int main(int argc, char *argv[])
{
	string_t *key, *plain, *cipher;
	if (argc < 3)
		DIE("%s\n%s\n%s\n\n%s", "Missing command line argument(s)",
			"arg1: Encryiption Key (filename)",
			"arg2: Plainetext to be encrypted (filename)",
			" *Ex.\t$ ./a.out key.txt plaintext.txt");

	key = simplify(readFile(argv[1]));
	plain = pad(simplify(readFile(argv[2])));
	cipher = encrypt(plain, key);

	printf("\n\nVigenere Key:\n\n");
	printc(key);

	printf("\n\n\nPlaintext:\n\n");
	printc(plain);
	
	printf("\n\n\nCiphertext:\n\n");
	printc(cipher);

	// As for testcases:
	//  -end output with a newline
	printf("\n");

	// Cleanup after ourselves :D
	destroyString(key);
	destroyString(plain);
	destroyString(cipher);
	
	return 0;
}
