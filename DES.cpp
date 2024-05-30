#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <cstdint>
using namespace std;

const string key = "your password"; // 암호화에 사용될 키

// 1라운드를 들어가기전 IP테이블로 암호화
static const int IP[64] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7 };
// PC-1 테이블
static const int PC1[56] = {
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4 };
// PC-2 테이블
static const int PC2[48] = {
	14, 17, 11, 24, 1, 5,
	3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8,
	16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32 };
// F함수에 사용되는 E테이블
static const int E[] = {
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1 };
// F함수에 사용되는 S-BOX테이블
const int S_BOX[8][4][16] = {
	{// S1
		{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
		{0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
		{4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
		{15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
	{// S2
		{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
		{3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
		{0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
		{13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
	{// S3
		{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
		{13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
		{13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
		{1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
		// S4
		{
			{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
			{13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
			{10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
			{3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
			// S5
			{
				{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
				{14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
				{4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
				{11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
				// S6
				{
					{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
					{10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
					{9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
					{4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
					// S7
					{
						{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
						{13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
						{1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
						{6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
						// S8
						{
							{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
							{1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
							{7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
							{2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}} };


// F함수에 사용되는 P테이블
int P[] = {
	16, 7, 20, 21,
	29, 12, 28, 17,
	1, 15, 23, 26,
	5, 18, 31, 10,
	2, 8, 24, 14,
	32, 27, 3, 9,
	19, 13, 30, 6,
	22, 11, 4, 25 };

static vector<string> roundKey; 		// 라운드별로 사용될 키가 저장된 공간
static vector<string> binary_blocks;    // 8바이트씩 나누어진 후 이진화된 평문이 저장되는 공간
static vector<string> encrypted_blocks; // 16라운드 수행 후 나온 결과를 저장할 공간
static vector<string> decrypted_blocks; // 다시 복호화한 결과를 저장할 공간
static string plain_text = ""; 		 // 복원한 평문을 저장할 공간

// 라운드에 사용될 키를 생성하는 함수
void Key_Schedule(string key)
{
	string binaryKey = ""; // 입력된 키를 이진수로 변환 후 저장할 공간
	for (int i = 0; i < key.size(); i++)
	{ // 키의 사이즈만큼 반복하며 글자 1자당 8비트로 변환
		int msgAscii = int(key[i]);
		binaryKey += bitset<8>(msgAscii).to_string();
	}

	if (key.size() < 8)
	{ // 입력된 키가 64비트가 안된다면 나머지를 패딩으로 채운다
		binaryKey.append(64 - binaryKey.size(), '0');
	}

	// 64비트의 키를 PC-1테이블을 통해 56비트로 만든다.
	string pc1_result = "";
	for (int i = 0; i < 56; i++)
	{
		pc1_result += binaryKey[PC1[i] - 1];
	}

	// 키를 좌, 우로 나눈다.
	string keyLeft = pc1_result.substr(0, 28);
	string keyRight = pc1_result.substr(28, 28);

	// LeftShift를 진행하며 키를 저장한다.
	for (int i = 0; i < 16; i++)
	{ // 총 16라운드(16번) 반복
		char first = keyLeft[0];
		keyLeft.erase(0, 1);
		keyLeft += first;

		first = keyRight[0];
		keyRight.erase(0, 1);
		keyRight += first;

		string keyResult = "";
		keyResult += keyLeft;
		keyResult += keyRight;

		string temp = "";
		for (int i = 0; i < 48; i++)
		{
			temp += keyResult[PC2[i] - 1];
		}

		roundKey.push_back(temp);
	}
}

// 입력받은 평문을 8비트로 분할 후 이진화 하여 vector에 저장한다.
void plaintext_to_binary_blocks(string message)
{
	vector<string> message_blocks; // 8바이트씩 분할된 블럭을 저장할 공간
	// 평문을 8바이트씩 잘라 저장
	for (int i = 0; i < message.length(); i += 8)
	{
		message_blocks.push_back(message.substr(i, 8));
	}
	for (int i = 0; i < message_blocks.size(); i++)
	{
		cout << "[" << i << "] 분할된 블럭 : " << message_blocks[i] << endl;
	}
	cout << endl;

	// 분할된 평문을 이진화한다.
	for (int i = 0; i < message_blocks.size(); i++)
	{ 						 // 입력된 블럭의 개수만큼 반복
		string msgbinary = ""; // 임시로 이진화된 메세지를 저장(8바이트 단위로 초기화)
		for (int j = 0; j < message_blocks[i].length(); j++)
		{
			int msgAscii = int(message_blocks[i][j]);
			msgbinary += bitset<8>(msgAscii).to_string();
		}
		binary_blocks.push_back(msgbinary);

		if (binary_blocks[i].size() != 64)
		{
			int diff = 64 - binary_blocks[i].size(); // 64비트가 안되는 문자열은 패딩을 채워준다
			binary_blocks[i].append(diff, '0');
		}
	}
}


// IP테이블의 암호화(0), 복호화(1)을 수행할 함수
string IP_Table_Cipher(int task_num, string text)
{ 					   //(작업선택, 블럭선택)
	string ip_data = ""; // 라운드를 수행전 IP테이블을 통해 처음 입력을 섞어 저장할 변수

	switch (task_num)
	{
	case 0:
	{ // 암호화
		// IP테이블로 섞어준다
		string ip_input = text; // IP테이블을 통해서 섞을 블럭을 저장할 공간(8바이트)
		for (int j = 0; j < 64; j++)
		{
			ip_data += ip_input[IP[j] - 1];
		}
		return ip_data;
		break;
	}
	case 1:
	{ // 복호화
		for (int i = 0; i < 64; i++)
			ip_data += "0";
		for (int i = 0; i < 64; i++)
			ip_data[IP[i] - 1] = text[i];

		return ip_data;
		break;
	}
	default:
		cout << "IP_Table_Cipher 에러 발생!";
		break;
	}
	return ip_data;
}

// string형태의 비트열을 xor연산하고 string 형태로 반환하는 함수
string XOR(string a, string b)
{
	string result = "";
	int size = a.length(); // 들어온 문자열의 길이를 파악후 32, 48로 나눈다

	if (size == 32)
	{ // 길이가 32라면
		bitset<32> left(a);
		bitset<32> right(b);
		bitset<32> xor_result;
		xor_result = left ^ right;
		result = xor_result.to_string(); // 연산 결과를 result에 저장한다.
		return result;
	}
	else if (size == 48)
	{
		bitset<48> left(a);
		bitset<48> right(b);
		bitset<48> xor_result;
		xor_result = left ^ right;
		result = xor_result.to_string();
		return result;
	}
	else
	{
		cout << "XOR: 값이 잘못 되었습니다";
		exit(0);
	}
}

// E테이블의 수행함수
string E_Table_Cipher(string right)
{
	string e_result = "";
	for (int i = 0; i < 48; i++)
	{ // e테이블을 이용하여 48비트로 만든다
		e_result += right[E[i] - 1];
	}
	return e_result;
}

string P_Table_Cipher(string input)
{
	string output = "";
	for (int i = 0; i < 32; i++)
	{
		output += input[P[i] - 1];
	}
	return output;
}

string S_BOX_Cipher(string input)
{
	string output = "";
	vector<string> blocks;
	for (int i = 0; i < input.length(); i += 6)
	{
		blocks.push_back(input.substr(i, 6));
	}
	for (int i = 0; i < blocks.size(); i++)
	{
		string row_str = blocks[i].substr(0, 1) + blocks[i].substr(5, 1);
		string column_str = blocks[i].substr(1, 4);
		int row = bitset<2>(row_str).to_ulong();
		int column = bitset<4>(column_str).to_ulong();

		int temp = S_BOX[i][row][column];
		output += bitset<4>(temp).to_string();
	}
	return output;
}

// 라운드를 진행하며 호출될 F함수
string F_function(string right, int round, int mode)
{ 								 // mode : 0(암호화), 1(복호화)
	right = E_Table_Cipher(right); // 48비트로 늘린다.
	switch (mode)
	{
	case 0:
	{ 									   // 암호화
		right = XOR(right, roundKey[round]); // 암호화를 위해 48비트로 늘린 비트를 해당 라운드의 키와 암호화한다.
		right = S_BOX_Cipher(right); 		// 48비트로 늘어난 비트를 32비트로 압축
		right = P_Table_Cipher(right); 	  // 테이블을 다시 섞는다
		break;
	}
	case 1:
	{ 											// 복호화
		right = XOR(right, roundKey[15 - round]); // 복호화를 위해 48비트로 늘린 비트를 해당 라운드의 키와 암호화한다.
		right = S_BOX_Cipher(right); 			 // 48비트로 늘어난 비트를 32비트로 압축
		right = P_Table_Cipher(right); 		   // 테이블을 다시 섞는다
		break;
	}
	default:
		break;
	}
	return right;
}

// 암호화 라운드를 수행할 함수
void Encrypt_Round()
{

	// 입력된 평문의 블럭 수 만큼 반복한다 (한 블럭마다 16라운드를 반복 후 저장)
	for (int i = 0; i < binary_blocks.size(); i++)
	{
		string ip_data = IP_Table_Cipher(0, binary_blocks[i]); // 처음 입력을 ip테이블로 암호화
		// 섞어준 데이터를 32비트씩 분할한다.
		string left = ip_data.substr(0, 32);
		string right = ip_data.substr(32, 32);

		string f_result = ""; // F함수를 수행한 right를 저장할 공간
		// 16라운드 동안 반복한다
		for (int j = 0; j < 16; j++)
		{
			f_result = F_function(right, j, 0); // right를 f함수를 수행한다.
			left = XOR(left, f_result); 		// f함수를 수행한 right를 left와 xor연산

			// L과 R을 교환한다.
			string temp = right;
			right = left;
			left = temp;
		}

		// 마지막 라운드는 교환 x이기 때문에 교환한 블럭을 다시 원래대로 돌린다.
		string temp = right;
		right = left;
		left = temp;

		// temp를 초기화 후 left와 right를 이어 붙여 블럭에 저장한다.
		temp = "";
		temp += left;
		temp += right;
		encrypted_blocks.push_back(temp);
	}
}
// 복호화 라운드를 수행할 함수
void Decrypt_Round()
{

	// 입력된 암호문의 블럭 수 만큼 반복한다 (한 블럭마다 16라운드를 반복 후 저장)
	for (int i = 0; i < encrypted_blocks.size(); i++)
	{
		string left = encrypted_blocks[i].substr(0, 32); // 암호문의 64비트 블럭을 32비트씩 나누어 저장한다.
		string right = encrypted_blocks[i].substr(32, 32);
		string f_result = ""; // F함수를 수행한 left를 저장할 공간

		// 16라운드 동안 반복한다
		for (int j = 0; j < 16; j++)
		{
			f_result = F_function(right, j, 1); // 복호화를 위한 F함수 수행(키가 반대로 사용된다.)

			left = XOR(left, f_result);

			// L과 R을 교환한다.
			string temp = right;
			right = left;
			left = temp;
		}

		// 마지막 라운드는 교환 x이기 때문에 교환한 블럭을 다시 원래대로 돌린다.
		string temp = right;
		right = left;
		left = temp;

		// temp를 초기화 후 left와 right를 이어 붙여 블럭에 저장한다.
		temp = "";
		temp += left;
		temp += right;

		temp = IP_Table_Cipher(1, temp);
		decrypted_blocks.push_back(temp);
	}
}

void print_ASCII()
{
	for (int i = 0; i < decrypted_blocks.size(); i++)
	{
		vector<bitset<8>> binary_vec; 	// 8자씩 나눈 이진수를 저장할 공간
		string str = decrypted_blocks[i]; // 한블럭(8바이트)씩 처리한다.
		for (int i = 0; i < str.length(); i += 8)
		{
			binary_vec.push_back(bitset<8>(str.substr(i, 8)));
		}

		for (int j = 0; j < binary_vec.size(); j++)
		{
			int decimal_num = binary_vec[j].to_ulong(); // 10진수 변환
			char ascii_char = static_cast<char>(decimal_num);
			plain_text += ascii_char;
		}
	}
	cout << "복호화된 평문 : " << plain_text << endl;
}

int main(void)
{
	Key_Schedule(key); 			// 지정된 키를 입력하여 라운드에 사용될 서브키를 생성한다.

	cout << "암호화할 평문을 입력해 주세요 : ";
	string message; // 평문을 저장하기 위한 변수
	getline(cin, message);
	cout << "입력된 평문 : " << message << endl;
	cout << "현재 사용되는 key : " << key << endl << endl;
	plaintext_to_binary_blocks(message); // 입력된 평문을 8바이트 단위로 나누고 이진화 후 저장
	for (int i = 0; i < binary_blocks.size(); i++)
	{
		cout << "[" << i << "] 이진화된 블럭 : " << binary_blocks[i] << endl;
	}

	cout << endl;

	Encrypt_Round(); // 암호화 라운드를 수행시킨다.
	for (int i = 0; i < encrypted_blocks.size(); i++)
		cout << "[" << i << "] 암호화한 블럭 : " << encrypted_blocks[i] << endl;
	cout << endl;

	Decrypt_Round(); // 복호화 라운드를 수행;
	for (int i = 0; i < decrypted_blocks.size(); i++)
		cout << "[" << i << "] 복호화한 블럭 : " << decrypted_blocks[i] << endl;
	cout << endl;

	print_ASCII(); // 복호화한 블럭을 ASCII 코드로 다시 변환하여 출력한다.
	return 0;
}