#pragma warning(disable:4996)
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#define BUFFER 4096
using namespace std;

struct NODE {
	string W;	// word
	string C;	// class
	string M;	// meaning
	NODE* left;
	NODE* right;
	NODE* parent;
};

class Tree{
public:
	NODE* root;
	int EA;	// 단어 수(size)
	int DC; // 삭제한 단어 수 (Delete Count)
	Tree() {
		root = NULL;
		EA = 0;
		DC = 0;
	}// 생성자 초기화

	void insert(string w, string c, string m) {
		NODE* temp = new NODE;
		temp->W = w;
		temp->C = c;
		temp->M = m;
		temp->left = NULL;
		temp->right = NULL;
		temp->parent = NULL;

		if (root != NULL) {
			NODE* cursor = root;
			NODE* before = root;	// cursor 뒤 따라가는 노드. cursor의 부모노드
			bool L = false;
			while (cursor != NULL) {
				if (cursor->W == w) {
					temp->right = cursor->right;
					cursor->right = temp;
					temp->parent = cursor;
					EA++;
					return;
				}
				before = cursor;
				if (cursor->W > w) { cursor = cursor->left; L = true; }
				else { cursor = cursor->right; L = false; }
			}

			if (L) before->left = temp;
			else before->right = temp;

			temp->parent = before;
			EA++;
		} 
		else {
			root = temp;
			EA++;
		}

	}// insert 연산

	void add() {
		string w,c,  m = "";
		cout << "word: ";
		while (w == "") {getline(cin, w);}
		cout << "class: ";
		getline(cin, c);
		cout << "meaning: ";
		while (m == "") {getline(cin, m);}
		insert(w, c, m);
	}

	void find(string word) {
		NODE* temp = search(root, word);
		if(temp!= NULL) cout << "word: " << temp->W << "\nclass: " << temp->C << "\nmeaning:" << temp->M << endl;
		else cout << "Not found\n";
	}

	NODE* search(NODE* node, string word) {
		if (node == NULL) return NULL;
		if (node->W == word) return node;
		else if (node->W > word) return search(node->left, word);
		else return search(node->right, word);
	}

	void del(string word) {
		int temp = DC;
		Delete(word);
		if (temp == DC) cout << "Not found.\n";
		else cout << "Deleted successfully.\n";
	}

	void multi_delete(string file) {
		DC = 0;
		ifstream fin(file);
		string word;
		while (fin >> word) {
			Delete(word);
		}
		cout << DC << " words were deleted successfully.\n";
	}

	void Delete(string word) {
		NODE* cursor = search(root, word);
		if (cursor == NULL) return; 

		DC++;
		EA--;
		NODE *LC, *RC, *P;
		P = cursor->parent; 
		LC = cursor->left; RC = cursor->right;

		if (LC == NULL && RC == NULL) {
			if (cursor == root) { root = NULL; return; }

			if (cursor == P->right) P->right = NULL;
			else P->left = NULL;
		}// 자식이 없는 경우
		else if (LC != NULL && RC != NULL) {
			LC->parent = RC;
			RC->left = LC;

			if (cursor == root) {
				root = RC; root->parent = NULL;
				return;
			}

			if (P->right == cursor) P->right = RC;
			else P->left = RC;
			RC->parent = P;

		}// 자식이 2인 경우
		else {
			NODE* C;
			if (LC != NULL) C = LC;
			else C = RC;

			if (cursor == root) {
				root = C; root->parent = NULL;
				return;
			}
			
			if (P->right == cursor) P->right = C;
			else P->left = C;
			C->parent = P;
		}// 자식이 1인 경우
	}

	void print() {
		recursion(root);
	}

	void recursion(NODE* node) {
		if (node == NULL) return;
		recursion(node->left);
		cout << node->W << endl;
		recursion(node->right);
	}
};
Tree T;
void init();
void process();

int main() {
	init();
	process();
}

void init() {
	string w,c,m;
	FILE *fin = fopen("shuffled_dict.txt", "r");
	if (NULL == fin) cout << "File Open Fail\n";
	char line[BUFFER];
	while (NULL != fgets(line, sizeof(line), fin)) {
		if (strcmp("\n", line) == 0) continue;
		string str(line);	// char 배열 line을 string 형 str에 담음
		int pos1, pos2;
		pos1 = str.find("(");
		pos2 = str.find(")");

		w = str.substr(0, pos1 - 1);					// ( 앞까지는 단어
		c = str.substr(pos1, pos2 - pos1 + 1);			// () 사이는 품사
		m = str.substr(pos2 + 1, str.size() - pos2 - 2);// ) 뒤는 설명
		T.insert(w, c, m);
	}
	fclose(fin);
}

void process() {
	string line, cmd;	// 입력, 명령어
	while (1) {
		cout << "$ ";
		getline(cin, line);	// 공백 입력도 처리하기위함
		istringstream iss(line);
		iss >> cmd;	// 단어 사이의 공백 무시하고 입력받음
		
		if (cmd == "add") {  T.add(); }	// 읽기 동작
		else if (cmd == "size") { cout << T.EA << endl; }	// 사전 사이즈 출력
		else if (cmd == "find") { if (iss >> cmd) T.find(cmd); }	// 단어 찾기
		else if (cmd == "delete") { if (iss >> cmd) T.del(cmd); }
		else if (cmd == "deleteall") { if (iss >> cmd) T.multi_delete(cmd); }
		else if (cmd == "print") { T.print(); }
		else if (cmd == "exit") break;
	}
}
