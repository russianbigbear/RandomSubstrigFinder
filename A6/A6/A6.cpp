#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <locale>
#include <iomanip>

using namespace std;

/**Структура элемента дерева**/
struct Element {
	int _left_child;			//левый ребенок элемента
	int _right_child;			//првый ребенок элемента
	int _parent;				//родитель элемента
	map <char, int> _children;  //дети элемента

	/*констркутор элемента без параметров*/
	Element() { _left_child = 0; _right_child = 0; _parent = 0; }

	/*констркутор элемента с параметрами*/
	Element(int _left_child, int _right_child, int _parent) {
		this->_left_child = _left_child;
		this->_right_child = _right_child;
		this->_parent = _parent;
	}

	/*метод получения ребенка элемента с указанным символом*/
	int get_children(char c) {
		if (!_children.count(c)) this->_children[c] = -1; //значит по этой букве, из нулевой вершины нет пути
		return _children[c];
	}
};

/**Класс - суффиксное дерево**/
class Suffix_Tree {
private:

	/*инициализация суффиксного дерева, создание*/
	void Inicialization_Tree() {
		Element *tmp = new Element(-1, -1, -1); //задаём нереальные параметры
		this->_suffix_tree.push_back(tmp);		//заносим в дерево(в вектор елемент)
	}

public:
	vector<Element*> _suffix_tree; //суффиксное дерево(вектор элементов суффиксного дерева

	/*констркутор дерева без парметра*/
	Suffix_Tree() { Inicialization_Tree(); }

	/*Создание вершины и листа*/
	void Create_Vertex_And_Leaf(string str, int num, int i, int left_child, int right_child, int count, int tmpCount) {
		// НУЖНО ПРОВЕРЯТЬ ЕСТЬ ЛИ ВНУТРЕННЯЯ ВЕРШИНА, И ПО НЕЙ МОЖНО ОСУЩЕСТВИТЬ ПЕРЕХОД

		// создаем новую внутреннюю вершину
		Element *inner_vertex = new Element(_suffix_tree[num]->_left_child, i, _suffix_tree[num]->_parent);
		int index_inner_Vertex = _suffix_tree.size(); //индекс внутренней вершины
		inner_vertex->_children[str[i]] = num; // добавляем детей (один из детей это вершина на ребре которой находимся)
		inner_vertex->_children[str[left_child + count]] = index_inner_Vertex + 1; // второй ребенок это новая добавляемая вершина
		_suffix_tree.push_back(inner_vertex); //добавляем в вектор

		// создаем новый лист
		Element *out_vertex = new Element(left_child + count, right_child, _suffix_tree.size() - 1);
		_suffix_tree.push_back(out_vertex); //заносим его

		// Правим ребро в которое вставили внутреннюю вершину
		int tmpPrt = _suffix_tree[num]->_parent;
		char c = str[_suffix_tree[num]->_left_child];
		_suffix_tree[tmpPrt]->_children[c] = index_inner_Vertex;
		_suffix_tree[num]->_left_child += tmpCount; // МЕНЯЕТСЯ НАЧАЛО, на смещение ОТ НАЧАЛА ТЕКУЩЕГО РЕБРА
		_suffix_tree[num]->_parent = index_inner_Vertex;
	}
	
	/*добавление суффикса в дерево*/
	void Add_Suffix(string str, int _left_child, int _right_child) {

		int vertex = 0;
		int num = _suffix_tree[vertex]->get_children(str[_left_child]); // смотрим куда ведет первая буква суффикса из вершины

		// Если результат -1, значит по этой букве, из нулевой вершины нет пути
		if (num == -1) {
			// Значит нужно добавить вершину
			Element *tmp = new Element(_left_child, _right_child, 0);
			_suffix_tree.push_back(tmp);
			_suffix_tree[vertex]->_children[str[_left_child]] = _suffix_tree.size() - 1;
		}
		else { //вершина уже есть, значит идем по ребру
			bool Stop_flag = false; //флаг остановки действий
			int count = 0;			// сумма смещений в ребрах, начиная от нулевой вершины
			int tmpCount = 0;		// смещение в ТЕКУЩЕМ ребре от его начала
			int i = 0;

			//пока флаг остановки не true, идем по ребру
			while (!Stop_flag) {
				i = 0;
				tmpCount = 0;
				for (i = _suffix_tree[num]->_left_child; i < _suffix_tree[num]->_right_child && !Stop_flag; i++) {
					if (!(str[i] == str[_left_child + count])) {
						// Когда встретилось несовпадение новую вершину на ребре
						Create_Vertex_And_Leaf(str, num, i, _left_child, _right_child, count, tmpCount);// функция создания внутренней вершины и листа
						Stop_flag = true; //останавливаем проход по ребру
					}
					count++;
					tmpCount++;
				}

				if (Stop_flag) break;	//если уже была созданна новая вершина на ребре, то выходим из цикла обхода

				//если вершины нет на ребре, то добавляет в конец пути
				int jump = _suffix_tree[num]->_children[str[_left_child + count]];
				if (jump == 0 || jump == -1) {
					Element *outVertex = new Element(_left_child + count, _right_child, num);
					_suffix_tree[num]->_children[str[_left_child + count]] = _suffix_tree.size();
					_suffix_tree.push_back(outVertex); 	// добавляем в дерево
					Stop_flag = true;
				}
				num = jump;
			}
		}
	}

	/*метод построения дерева*/
	void Build_Tree(string str) {
		for (int i = 0; i < str.length(); i++) {
			char c = str[i];
			Add_Suffix(str, i, str.length());
		}
	}
};

/*функция поиска подстроки в строке*/
bool FindSubstring(Suffix_Tree* t, string find_subs, string text)
{
	int pos = 0; //текущая позиция
	int tmpR, tmpL; //временные дети

	//изщем детей вершины с похожим символом, при несовпадение выдаем ложь
	if ((t->_suffix_tree[pos]->_children.count(find_subs[0])) == 0)
		return false;
	//если совпадает то задаем новую позицию с найденным вхождениям символа
	else
		pos = t->_suffix_tree[pos]->_children[find_subs[0]];

	//проходимся по ребру и смотрим символы,если в какой-то момент строка не совпадает, то выходим
	for (int i = 0; i < find_subs.size(); i++) {
		tmpR = t->_suffix_tree[pos]->_right_child; //задаем временого правого ребенка
		tmpL = t->_suffix_tree[pos]->_left_child;  //задаем временого левого ребенка

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
				//если все сопало, о подстрака  найдена
				if (i == find_subs.size())
					return true;
			}
			pos = t->_suffix_tree[pos]->_children[find_subs[i]];
			i--;
		}
	}
	return true;
}

/*функция создаия .dot файла для вывода суффиксного дерева*/
void PrintToDot(Suffix_Tree *Dub, string text) {
	ofstream fout;
	fout.open("picture.gv");
	fout << "graph{\n"; // запись строки в файл
	string strComand = "";
	for (int i = 1; i < Dub->_suffix_tree.size(); i++) {
		fout << "\t" << Dub->_suffix_tree[i]->_parent << " -- " << i << "[label = \"";
		// выводим строку на ребро
		for (int j = Dub->_suffix_tree[i]->_left_child; j < Dub->_suffix_tree[i]->_right_child; j++) {
			fout << text[j];
		}
		fout << "\", weight = \"1\"];" << endl;
	}
	fout << "}";
	fout.close();
	system("dot picture.gv -Tpng -o picture.png");
	system("picture.png"); //выводим дерево
}

int main()
{
	setlocale(LC_ALL, "rus");
	Suffix_Tree *SufTree = new Suffix_Tree();
	string text, found;
	cout << "Введите текст: " << endl;

	getline(cin, text); //получаем текст
	text += "&"; //добавляем в конец не встречающийся символ
	SufTree->Build_Tree(text); //строим дерево
	PrintToDot(SufTree, text); //переводим в дот

	while (1) {
		cout << "Введите образец" << endl;
		getline(cin, found);
		if (found == "&")
			return 0;
		if (FindSubstring(SufTree, found, text))
			cout << "Строка <" << found << "> встречается в тексте\n";
		else
			cout << "Строка <" << found << "> не встречается в тексте\n";
	}
	return 0;
}
