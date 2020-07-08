#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <locale>
#include <iomanip>

using namespace std;

/**��������� �������� ������**/
struct Element {
	int _left_child;			//����� ������� ��������
	int _right_child;			//����� ������� ��������
	int _parent;				//�������� ��������
	map <char, int> _children;  //���� ��������

	/*����������� �������� ��� ����������*/
	Element() { _left_child = 0; _right_child = 0; _parent = 0; }

	/*����������� �������� � �����������*/
	Element(int _left_child, int _right_child, int _parent) {
		this->_left_child = _left_child;
		this->_right_child = _right_child;
		this->_parent = _parent;
	}

	/*����� ��������� ������� �������� � ��������� ��������*/
	int get_children(char c) {
		if (!_children.count(c)) this->_children[c] = -1; //������ �� ���� �����, �� ������� ������� ��� ����
		return _children[c];
	}
};

/**����� - ���������� ������**/
class Suffix_Tree {
private:

	/*������������� ����������� ������, ��������*/
	void Inicialization_Tree() {
		Element *tmp = new Element(-1, -1, -1); //����� ���������� ���������
		this->_suffix_tree.push_back(tmp);		//������� � ������(� ������ �������)
	}

public:
	vector<Element*> _suffix_tree; //���������� ������(������ ��������� ����������� ������

	/*����������� ������ ��� ��������*/
	Suffix_Tree() { Inicialization_Tree(); }

	/*�������� ������� � �����*/
	void Create_Vertex_And_Leaf(string str, int num, int i, int left_child, int right_child, int count, int tmpCount) {
		// ����� ��������� ���� �� ���������� �������, � �� ��� ����� ����������� �������

		// ������� ����� ���������� �������
		Element *inner_vertex = new Element(_suffix_tree[num]->_left_child, i, _suffix_tree[num]->_parent);
		int index_inner_Vertex = _suffix_tree.size(); //������ ���������� �������
		inner_vertex->_children[str[i]] = num; // ��������� ����� (���� �� ����� ��� ������� �� ����� ������� ���������)
		inner_vertex->_children[str[left_child + count]] = index_inner_Vertex + 1; // ������ ������� ��� ����� ����������� �������
		_suffix_tree.push_back(inner_vertex); //��������� � ������

		// ������� ����� ����
		Element *out_vertex = new Element(left_child + count, right_child, _suffix_tree.size() - 1);
		_suffix_tree.push_back(out_vertex); //������� ���

		// ������ ����� � ������� �������� ���������� �������
		int tmpPrt = _suffix_tree[num]->_parent;
		char c = str[_suffix_tree[num]->_left_child];
		_suffix_tree[tmpPrt]->_children[c] = index_inner_Vertex;
		_suffix_tree[num]->_left_child += tmpCount; // �������� ������, �� �������� �� ������ �������� �����
		_suffix_tree[num]->_parent = index_inner_Vertex;
	}
	
	/*���������� �������� � ������*/
	void Add_Suffix(string str, int _left_child, int _right_child) {

		int vertex = 0;
		int num = _suffix_tree[vertex]->get_children(str[_left_child]); // ������� ���� ����� ������ ����� �������� �� �������

		// ���� ��������� -1, ������ �� ���� �����, �� ������� ������� ��� ����
		if (num == -1) {
			// ������ ����� �������� �������
			Element *tmp = new Element(_left_child, _right_child, 0);
			_suffix_tree.push_back(tmp);
			_suffix_tree[vertex]->_children[str[_left_child]] = _suffix_tree.size() - 1;
		}
		else { //������� ��� ����, ������ ���� �� �����
			bool Stop_flag = false; //���� ��������� ��������
			int count = 0;			// ����� �������� � ������, ������� �� ������� �������
			int tmpCount = 0;		// �������� � ������� ����� �� ��� ������
			int i = 0;

			//���� ���� ��������� �� true, ���� �� �����
			while (!Stop_flag) {
				i = 0;
				tmpCount = 0;
				for (i = _suffix_tree[num]->_left_child; i < _suffix_tree[num]->_right_child && !Stop_flag; i++) {
					if (!(str[i] == str[_left_child + count])) {
						// ����� ����������� ������������ ����� ������� �� �����
						Create_Vertex_And_Leaf(str, num, i, _left_child, _right_child, count, tmpCount);// ������� �������� ���������� ������� � �����
						Stop_flag = true; //������������� ������ �� �����
					}
					count++;
					tmpCount++;
				}

				if (Stop_flag) break;	//���� ��� ���� �������� ����� ������� �� �����, �� ������� �� ����� ������

				//���� ������� ��� �� �����, �� ��������� � ����� ����
				int jump = _suffix_tree[num]->_children[str[_left_child + count]];
				if (jump == 0 || jump == -1) {
					Element *outVertex = new Element(_left_child + count, _right_child, num);
					_suffix_tree[num]->_children[str[_left_child + count]] = _suffix_tree.size();
					_suffix_tree.push_back(outVertex); 	// ��������� � ������
					Stop_flag = true;
				}
				num = jump;
			}
		}
	}

	/*����� ���������� ������*/
	void Build_Tree(string str) {
		for (int i = 0; i < str.length(); i++) {
			char c = str[i];
			Add_Suffix(str, i, str.length());
		}
	}
};

/*������� ������ ��������� � ������*/
bool FindSubstring(Suffix_Tree* t, string find_subs, string text)
{
	int pos = 0; //������� �������
	int tmpR, tmpL; //��������� ����

	//����� ����� ������� � ������� ��������, ��� ������������ ������ ����
	if ((t->_suffix_tree[pos]->_children.count(find_subs[0])) == 0)
		return false;
	//���� ��������� �� ������ ����� ������� � ��������� ���������� �������
	else
		pos = t->_suffix_tree[pos]->_children[find_subs[0]];

	//���������� �� ����� � ������� �������,���� � �����-�� ������ ������ �� ���������, �� �������
	for (int i = 0; i < find_subs.size(); i++) {
		tmpR = t->_suffix_tree[pos]->_right_child; //������ ��������� ������� �������
		tmpL = t->_suffix_tree[pos]->_left_child;  //������ ��������� ������ �������

		if (tmpR - tmpL == 1){
			if (i + 1 < find_subs.size()){
				if ((t->_suffix_tree[pos]->_children.count(find_subs[i + 1])) == 0)
					return false;
				else
					pos = t->_suffix_tree[pos]->_children[find_subs[i + 1]];
			}
		}
		else{
			for (int j = tmpL; j < tmpR && i < find_subs.size(); j++) {
				if (text[j] != find_subs[i])
					return false;
				i++;
				//���� ��� ������, � ���������  �������
				if (i == find_subs.size())
					return true;
			}
			pos = t->_suffix_tree[pos]->_children[find_subs[i]];
			i--;
		}
	}
	return true;
}

/*������� ������� .dot ����� ��� ������ ����������� ������*/
void PrintToDot(Suffix_Tree *Dub, string text) {
	ofstream fout;
	fout.open("picture.gv");
	fout << "graph{\n"; // ������ ������ � ����
	string strComand = "";
	for (int i = 1; i < Dub->_suffix_tree.size(); i++) {
		fout << "\t" << Dub->_suffix_tree[i]->_parent << " -- " << i << "[label = \"";
		// ������� ������ �� �����
		for (int j = Dub->_suffix_tree[i]->_left_child; j < Dub->_suffix_tree[i]->_right_child; j++) {
			fout << text[j];
		}
		fout << "\", weight = \"1\"];" << endl;
	}
	fout << "}";
	fout.close();
	system("dot picture.gv -Tpng -o picture.png");
	system("picture.png"); //������� ������
}

int main()
{
	setlocale(LC_ALL, "rus");
	Suffix_Tree *SufTree = new Suffix_Tree();
	string text, found;
	cout << "������� �����: " << endl;

	getline(cin, text); //�������� �����
	text += "&"; //��������� � ����� �� ������������� ������
	SufTree->Build_Tree(text); //������ ������
	PrintToDot(SufTree, text); //��������� � ���

	while (1) {
		cout << "������� �������" << endl;
		getline(cin, found);
		if (found == "&")
			return 0;
		if (FindSubstring(SufTree, found, text))
			cout << "������ <" << found << "> ����������� � ������\n";
		else
			cout << "������ <" << found << "> �� ����������� � ������\n";
	}
	return 0;
}
