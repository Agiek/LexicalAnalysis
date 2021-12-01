#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

#define KEYWORD_NUM 37
string KEYWORD[KEYWORD_NUM] = { "auto","enum","restrict","unsigned","break","extern","return","void",
		"case","float","short","volatile","char","for","signed","while",
		"const","goto","sizeof","_Bool","continue","if","static","_Complex",
		"default","inline","struct","_Imaginary","do","int","switch","double","long",
		"typedef","else","register","union" };


char SEPARATER[8] = { ';',',','{','}','[',']','(',')' };    //分隔符
char OPERATOR[10] = { '+','-','*','/','>','<','=','!','.','&' };     //运算符
char FILTER[4] = { ' ','\t','\r','\n' };                    //过滤符
const int IDENTIFIER = 100;         //标识符值
const int CONSTANT = 101;           //常数值
const int FILTER_VALUE = 102;       //过滤字符值

/**文件各方面情况**/
struct MyStruct
{
	string name = "";
	int line_num = 1;//总行数
	int statement_num = 0;//语句行数
	int word_num = 0;//单词个数
	int char_num = 1;//字符个数
}fileAttr;


using namespace winrt;
using namespace Windows::UI::Xaml;


namespace winrt::LexicalAnalysis::implementation
{




	bool IsKeyword(string word) {
		for (int i = 0; i < KEYWORD_NUM; i++) {
			if (KEYWORD[i] == word) {
				return true;
			}
		}
		return false;
	}
	/**判断是否为分隔符**/
	bool IsSeparater(char ch) {
		for (int i = 0; i < 8; i++) {
			if (SEPARATER[i] == ch) {
				return true;
			}
		}
		return false;
	}

	/**判断是否为运算符**/
	bool IsOperator(char ch) {
		for (int i = 0; i < 10; i++) {
			if (OPERATOR[i] == ch) {
				return true;
			}
		}
		return false;
	}
	/**判断是否为过滤符**/
	bool IsFilter(char ch) {
		for (int i = 0; i < 4; i++) {
			if (FILTER[i] == ch) {
				return true;
			}
		}
		return false;
	}
	/**判断是否为大写字母**/
	bool IsUpLetter(char ch) {
		if (ch >= 'A' && ch <= 'Z') return true;
		return false;
	}
	/**判断是否为小写字母**/
	bool IsLowLetter(char ch) {
		if (ch >= 'a' && ch <= 'z') return true;
		return false;
	}
	/**判断是否为数字**/
	bool IsDigit(char ch) {
		if (ch >= '0' && ch <= '9') return true;
		return false;
	}
	/**返回每个字的值**/
	template <class T>
	int value(T* a, int n, T str) {
		for (int i = 0; i < n; i++) {
			if (a[i] == str) return i + 1;
		}
		return -1;
	}
	/**词法分析**/
	void analyse(FILE* fpin, vector <pair<string, string>>& tokens, vector <pair<string, string>>& Errorstruct) {
		char ch = ' ';
		string arr = "";
		bool isQuestionMark = false;				//记录？的出现
		while ((ch = fgetc(fpin)) != EOF) {//每次读取一个字符，直到读取结束

			arr = "";
			fileAttr.char_num++;
			if (IsFilter(ch)) {//判断是否为过滤符
				if (ch == '\n') {
					fileAttr.line_num++;
				}
			}
			else if (IsLowLetter(ch)) {       //判断是否为关键字或标识符
				while (IsLowLetter(ch)) {
					arr += ch;
					ch = fgetc(fpin);
					fileAttr.char_num++;
				}
				if (IsKeyword(arr)) {
					printf("%3d    ", value(KEYWORD, 15, arr));
					cout << arr << "  关键字" << endl;
					fseek(fpin, -1L, SEEK_CUR);
					fileAttr.char_num--;
					fileAttr.word_num++;
					tokens.push_back(make_pair(arr, "关键字"));
				}
				else
				{
					while (IsUpLetter(ch) || IsLowLetter(ch) || ch == '_' || IsDigit(ch)) {
						arr += ch;
						ch = fgetc(fpin);
						fileAttr.char_num++;
					}
					fseek(fpin, -1L, SEEK_CUR);
					fileAttr.char_num--;
					printf("%3d    ", CONSTANT);
					cout << arr << "  标识符" << endl;
					fileAttr.word_num++;
					tokens.push_back(make_pair(arr, "标识符"));
				}
			}
			else if (IsDigit(ch)) {           //判断是否为数字
				while (IsDigit(ch)) {
					arr += ch;
					ch = fgetc(fpin);
					fileAttr.char_num++;
				}
				if (ch == '.') {//浮点数
					arr += ch;
					ch = fgetc(fpin);

					fileAttr.char_num++;
					while (IsDigit(ch)) {
						arr += ch;
						ch = fgetc(fpin);
						fileAttr.char_num++;
					}
					fseek(fpin, -1L, SEEK_CUR);
					fileAttr.char_num--;
					printf("%3d    ", CONSTANT);
					cout << arr << "  浮点数" << endl;
					tokens.push_back(make_pair(arr, "浮点数"));

				}
				else {//整型数
					fseek(fpin, -1L, SEEK_CUR);
					fileAttr.char_num--;
					printf("%3d    ", CONSTANT);
					cout << arr << "  整形数" << endl;
					tokens.push_back(make_pair(arr, "整形数"));
				}
			}
			else if (IsUpLetter(ch) || IsLowLetter(ch) || ch == '_') {//判断是否为标识符
				while (IsUpLetter(ch) || IsLowLetter(ch) || ch == '_' || IsDigit(ch)) {
					arr += ch;
					ch = fgetc(fpin);
					fileAttr.char_num++;
				}
				fseek(fpin, -1L, SEEK_CUR);
				fileAttr.char_num--;
				printf("%3d    ", CONSTANT);
				cout << arr << "  未声明变量" << endl;
				fileAttr.word_num++;
				tokens.push_back(make_pair(arr, "标识符"));
			}
			else if (ch == '/') {				//判断是否为注释
				arr += ch;
				char ch = fgetc(fpin);
				fileAttr.char_num++;
				if (ch == '*') {
					int state = 0;
					while (true)
					{
						arr += ch;
						ch = fgetc(fpin);
						fileAttr.char_num++;
						if (state == 0) {//正常注释
							if (ch == '\"') {
								state = 2;
							}
							else if (ch == '*') {
								state = 1;
							}
						}
						else if (state == 1) {//遇到了一个*
							if (ch == '/') {
								state = 3;
								break;
							}
							else {
								state = 0;
							}
						}
						else if (state == 2) {//遇到了一个"
							if (ch == '\"') {
								state = 0;
							}
						}
					}
					arr += ch;
					printf("%3d    ", value(OPERATOR, 10, *arr.data()));
					cout << arr << "  注释" << endl;
				}
				else if (ch == '/') {
					while (ch != '\n') {
						arr += ch;
						ch = fgetc(fpin);
						fileAttr.char_num++;
					}
					fseek(fpin, -1L, SEEK_CUR);
					fileAttr.char_num--;
					printf("%3d    ", value(OPERATOR, 10, *arr.data()));
					cout << arr << "  注释" << endl;
				}
				else {
					fseek(fpin, -1L, SEEK_CUR);
					fileAttr.char_num--;
					printf("%3d    ", value(OPERATOR, 10, *arr.data()));
					cout << arr << "  运算符" << endl;
					tokens.push_back(make_pair(arr, "运算符"));
				}
			}
			//此下部分是崔晓凡写的
			else if (ch == '"') {		//读到"，识别到字符串了属于是.因为转义字符也是一个字，所以吧没那些奇奇怪怪的，就读到后引号或者换行就行
				bool isError = false;
				while ((ch = fgetc(fpin)) != EOF) {
					if (ch == '"') {
						fileAttr.word_num++;
						fileAttr.char_num++;
						break;
					}
					else if (ch == '\n') {
						fileAttr.word_num++;
						fileAttr.line_num++;//虽然没写完string就换行了，但是它依然是一行语句啊
						fileAttr.statement_num++;
						isError = true;
						break;
					}
					arr += ch;
				}
				cout << arr << "  字符串" << endl;
				tokens.push_back(make_pair(arr, "字符串"));
				if (isError) {
					Errorstruct.push_back(make_pair("字符串未完毕就换行", std::to_string(fileAttr.line_num - 1)));
				}
			}

			else if (ch == '?') {
				cout << "?" << "  运算符" << endl;
				if (!isQuestionMark)
					isQuestionMark = true;
				else {
					Errorstruct.push_back(make_pair("前一个三目运算符?还未结算完毕", std::to_string(fileAttr.line_num - 1)));
				}
			}
			else if (ch == ':') {
				cout << ":" << "  运算符" << endl;
				if (!isQuestionMark) {
					Errorstruct.push_back(make_pair("三目运算符:前没用检测到？", std::to_string(fileAttr.line_num - 1)));
				}
				else {
					isQuestionMark = false;
				}
			}
			//此上
			else switch (ch) {
			case '+':
			case '-':
			case '*':
				//case '/':  除法在注释处已经判断过了
			case '>':
			case '<':
			case '=':
			case '!':
			case '.':
			case '&':
			{
				arr += ch;
				printf("%3d    ", value(OPERATOR, 10, *arr.data()));
				cout << arr << "  运算符" << endl;
				tokens.push_back(make_pair(arr, "运算符"));
				break;
			}
			case ';':
			case ',':
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			{
				arr += ch;
				printf("%3d    ", value(SEPARATER, 8, *arr.data()));
				cout << arr << "  分隔符" << endl;
				if (ch == ';') {
					fileAttr.statement_num++;
				}
				break;
			}
			default:
				cout << "\"" << ch << "\":无法识别的字符！" << endl;
				tokens.push_back(make_pair(arr, "无法识别的字符"));
			}
		}

	}


	int main_prot()
	{
		char inFile[40]="src.txt";
		FILE* fpin;
		//"请输入源文件名（包括路径和后缀）:";
		while (true) {
			(fopen_s(&fpin, inFile, "r"));
			if (fpin != NULL)
				break;
			else {
				cout << "文件名错误！" << endl;
				cout << "请输入源文件名（包括路径和后缀）:";
			}

		}
		fileAttr.name = inFile;
		vector <pair<string, string>> tokens;
		vector <pair<string, string>> Errorstruct;
		cout << "------词法分析如下------" << endl;
		analyse(fpin, tokens, Errorstruct);
		cout << "\n------" + fileAttr.name + "文件情况如下------" << endl;

		cout << "    " << "    总行数:" << fileAttr.line_num << endl;
		cout << "    " << "  语句行数:" << fileAttr.statement_num << endl;
		cout << "    " << "  单词个数:" << fileAttr.word_num << endl;
		cout << "    " << "  字符个数:" << fileAttr.char_num << endl;

		return 0;
	}





	MainPage::MainPage()
	{
		InitializeComponent();
	}

	int32_t MainPage::MyProperty()
	{
		throw hresult_not_implemented();
	}

	void MainPage::MyProperty(int32_t /* value */)
	{
		throw hresult_not_implemented();
	}

	void MainPage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
	{

		winrt::hstring hs = L"啊是大家看法和会计师的";
		winrt::hstring hinput = myInput().Text();
		std::string a= "asf";
		main_prot();
		myButton().Content(box_value(L"Clicked"));
		//myOutput().Text(hs);
	}


}
