#include"automat.h"
#include"regular.h"
#include<set>
#include<stack>
#include<deque>
#include <bitset>
#include <climits>
int main() {
	/*Automat A;
	A.readFromFile("test3.txt");
	auto ans = A.readString("101010101010101");
	A.toDeterminal();
	A.printInFile("DT.txt");
	ans = A.readString("101010101010101");
	A.minimizeDeterminal();
	A.printInFile("MDT.txt");
	ans = A.readString("101010101010101");
	RegularExpression C;
	auto D = C.makeAutomat("(0+11*00)*1*");
	auto f = C.isCorrect("011001010001");
	D.printInFile("Automat.txt");
	ans = D.readString("011001010001");
	RegularExpression E(D);
	std::cout << E << std::endl;
	auto f1 = E.isCorrect("011001010001");*/
	//Automat A, B, C, D;
	//A.readFromFile("test5.txt");
	//B = A;
	//B.toDeterminal();
	//B.printInFile("DT.txt");
	//C = B;
	//C.minimizeDeterminal();
	//C.printInFile("MDT.txt");
	//RegularExpression R1(A), R2=R1;
	//std::cout << R1;
	//D = R2.makeAutomat();
	//D.toDeterminal();
	//D.minimizeDeterminal();
	//D.printInFile("RegToAutomat.txt");
	//int count = 0;
 //	for (int i = 0; i < 1024; i++)
	//{
	//	auto str = std::bitset<10>(i).to_string();
	//	int f1 = A.readString(str),
	//		f2 = B.readString(str),
	//		f3 = C.readString(str),
	//		f4 = R1.isCorrect(str),
	//		f5 = D.readString(str);
	//	/*std::cout << str << " " << f1 << " " << f2 << " " << f3 << " " << f4 << " " << f5 << "\n";*/
	//	if (f1 + f2 + f3 + f4 + f5 != 5 && f1 + f2 + f3 + f4 + f5 != 0)std::cout << str << " " << f1 << " " << f2 << " " << f3 << " " << f4 << " " << f5 << "\n";
	//}
	RegularExpression R3;
	auto D = R3.makeAutomat("11*");
	D.printInFile("Automat.txt");
	RegularExpression R4(D);
	std::cout << R4;
	/*Automat D;
	D.readFromFile("test5.txt");
	D.minimizeDeterminal();
	D.printInFile("Min.txt");*/

	return 0;
}