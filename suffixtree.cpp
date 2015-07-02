#include "suffixtree.h"

#include <iostream>
#include <string>

static const int ALPHABET_SIZE = 256;

/*
	Struktura koja predstavlja aktivnu poziciju, a sadrzi
	aktivni cvor (active_node), aktivni brid (active_edge) te
	aktivnu duljinu (active_length).
*/

struct ActivePoint {
	Node* active_node;
	int active_edge;
	int active_length;

	bool propagate(std::vector<int> const & labels);
};

/*
	Metoda obavlja propagaciju aktivne pozicije tako da
	modificira aktivni cvor na slijedeci cvor na putu,
	a aktivni brid i aktivnu duljinu osvjezava s obzirom
	na trenutnu poziciju.
*/
bool ActivePoint::propagate(std::vector<int> const & labels) {
	int const active_edge_label = labels[active_edge];
	std::pair<int, int> edge = active_node->get_edge(active_edge_label);
	if (edge.second == -1)
		return false;
	int edge_length = edge.second - edge.first;
	if (active_length < edge_length)
		return false;
	active_node = &active_node->get_child(active_edge_label);
	active_edge += edge_length;
	active_length -= edge_length;
	return true;
}

SuffixTree::~SuffixTree() {
}

/*
	Konstruktor gradi stablo za odredjeni znakovni niz (string).
	Konstruktor automatski dodaje znak kraja niza.
*/

SuffixTree::SuffixTree(std::string const & string)
	: 
	root_(0)
{
	init(std::vector<std::string>(1, string));
}  

/*
	Konstruktor gradi stablo za odredjeni niz znakovnih nizova
	(strings). Konstruktor automatski dodaje znakove kraja niza
	na kraj svakog znakovnog niza.
*/

SuffixTree::SuffixTree(std::vector<std::string> const & strings) 
	:
	root_(0)
{
	init(strings);
}

void SuffixTree::init(std::vector<std::string> const & strings)
{
	int terminal_mark = ALPHABET_SIZE;
	int index = 0;

	for (std::size_t i = 0; i < strings.size(); i++) {
		for (std::size_t j = 0; j < strings[i].size(); j++) {
			label_.push_back(encode_char(strings[i][j]));
		}
		index += strings[i].size();
		string_.append(strings[i] + "$");
		label_.push_back(terminal_mark++);
		suffix_end_.push_back(index++);
	}
	leafs_.reserve(label_.size());
	build_tree();
}

/*
	Glavna metoda za izgradnju stabla.
*/

void SuffixTree::build_tree() {
	// initialization
	ActivePoint active_point;
	active_point.active_node = &root_;
	active_point.active_edge = 0;
	active_point.active_length = 0;
	int remainder = 0;
	int suffix_ord_num = 0;
	int start_index = 0;
	int index = 1;

	for (int i = 0; i < static_cast<int>(string_.length()); i++) {
		remainder++;
		Node *to_set = NULL;
		while (remainder > 0) {
			if (active_point.active_length == 0)
				active_point.active_edge = i;

			if (!(active_point.active_node->contains_edge(label_[active_point.active_edge]))) {
				// explicit extension - not split
				Node* leaf = new Node(index++, suffix_ord_num, start_index++);
				leafs_.push_back(leaf);
				active_point.active_node->add_child(label_[i], *leaf, std::make_pair(i, -1));	// ili lijevi i = active_point.active_edge
				add_suffix_link(&to_set, active_point.active_node);
			}
			else {
				if (active_point.propagate(label_))
					continue;

				std::pair<int, int> edge = active_point.active_node->get_edge(label_[active_point.active_edge]);
				if (label_[edge.first + active_point.active_length] == label_[i]) {
					active_point.active_length++;
					
					// we have to add suffix link if there is a node marked with flag to_set
					add_suffix_link(&to_set, active_point.active_node);
					break;
				}

				// explicit extension - split
				std::pair<int, int> old_edge = active_point.active_node->get_edge(label_[active_point.active_edge]);
				std::pair<int, int> new_edge = std::make_pair(old_edge.first, old_edge.first + active_point.active_length);
				Node* n1 = new Node(index++);
				Node* leaf = new Node(index++, suffix_ord_num, start_index++);
				leafs_.push_back(leaf);

				n1->add_child(label_[i], *leaf, std::make_pair(i, -1));
				n1->add_child(label_[new_edge.second], active_point.active_node->get_child(label_[active_point.active_edge]), std::make_pair(new_edge.second, old_edge.second));
				active_point.active_node->add_child(label_[active_point.active_edge], *n1, new_edge);

				// add suffix link if needed
				add_suffix_link(&to_set, n1);
			}
			remainder--;

			if (active_point.active_node == &root_ && active_point.active_length > 0) {
				active_point.active_length--;
				active_point.active_edge = i - remainder + 1;
			}
			else {
				active_point.active_node = active_point.active_node->get_suffix_link();
				if (active_point.active_node == NULL)
					active_point.active_node = &root_;
			}
		}
		if (label_[i] >= ALPHABET_SIZE) {
			suffix_ord_num++;
			start_index = 0;
		}
	}
}


/*
	Metoda sluzi za dodavanje sufiksne veze.
	to_set - cvor kojem se dodaje sufiksna veza
	set - cvor na koji sufiksna veza pokazuje
*/

void SuffixTree::add_suffix_link(Node** to_set, Node* set) {
	if (*to_set != NULL) {
		(*to_set)->set_suffix_link(set);
	}
	(*to_set) = set;
}

/*
	Metoda vraca oznaku puta bilo kojeg cvora stabla.
	Oznaka puta je znakovni niz koji se dobiva konkatenacijom
	labela bridova od korijena do trenutnog cvora.
*/

std::string SuffixTree::get_path_label(Node* node) {
	if (node == NULL || node->is_root()) {
		return "";
	}
	std::pair<int, int> edge = node->get_edge_from_parent();
	return get_path_label(node->get_parent()) + decode_edge(edge.first, edge.second, node->get_suffix_ord_num());
}

/*
	Metoda na temelju oznaka pocetka (start) i kraja (end) te 
	rednog broja znakovnog niza (ord_num) vraca znakovnu
	reprezentaciju tog intervala.
*/

std::string SuffixTree::decode_edge(int start, int end, int ord_num) {
	if (end == -1) {
		end = suffix_end_[ord_num];
	}
	return string_.substr(start, end-start);
}

/*
	Metoda vraca sva sufiks-prefiks preklapanja u obliku parova
	(r_br, duljina), gdje r_br predstavlja redni broj znakovnog
	niza nad kojim je izgradjeno stablo, a duljina predstavlja
	duljinu prefiksa. Na taj nacin je preklapanje jedinstveno 
	odredjeno.
*/

std::vector<std::pair<int,int>> SuffixTree::get_suffix_prefix_matches() {
	std::vector<std::pair<int, int>> matches;
	for (std::size_t i = 0; i < leafs_.size(); i++) {
		Node* node = leafs_[i];
		if (node->get_start_index() == 0) {
			Node* parent = node->get_parent();
			std::string str = get_path_label(parent);
			int len = str.length();

			while (parent != NULL) {
				if (parent->is_terminal()) {
					if (len > 0) {
						matches.push_back(std::make_pair(node->get_suffix_ord_num(), len));
					}
				}

				if(parent->get_parent() == NULL)
					break;

				std::pair<int, int> edge = parent->get_edge_from_parent();
				len -= (edge.second - edge.first);
				parent = parent->get_parent();
			}
		}
	}
	return matches;
}

/*
	Metoda na temelju izgradjenog stabla vraca sve sufikse.
*/

std::vector<std::string> SuffixTree::get_all_suffixes() {
	std::vector<std::string> result;
	result.reserve(leafs_.size());
	for (std::vector<Node*>::iterator it = leafs_.begin(); it != leafs_.end(); it++) {
		result.push_back(get_path_label(*it));
	}
	return result;
}

/*
	Metoda na output_stream ispisuje stablo u obliku kojeg
	moze interpretirati GraphViz.
*/

void SuffixTree::print_tree(std::ostream& output_stream) {
	output_stream << "digraph {\n";
	output_stream << "rankdir = LR;\n";
	output_stream << "edge [arrowsize=0.4,fontsize=10]\n";
	output_stream << "node [label=\"\",style=filled,fillcolor=white,shape=circle,width=.1,height=.1]\n";
	print(root_, output_stream);
	output_stream << "}\n";
}

/*
	Pomocna metoda koja rekurzivno ispisuje stablo koristeci se
	poslanim cvorom (node) kao korjenom.
*/

void SuffixTree::print(Node& node, std::ostream& output_stream) {
	if (node.is_leaf()) {
		return;
	}

	// ispisuje se brid od roditelja do djeteta
	std::vector<int> characters = node.get_characters();
	for (std::size_t i = 0; i < characters.size(); i++) {
		std::pair<int, int> edge = node.get_edge(characters[i]);
		std::string label = " [label=\"" + decode_edge(edge.first, edge.second, node.get_child(characters[i]).get_suffix_ord_num()) + "\"]\n";
		output_stream << "node" << node.get_name() << " -> node" << node.get_child(characters[i]).get_name() << label;
		print(node.get_child(characters[i]), output_stream);
	}

	// ispisuje se sufiksna veza
	Node* point = &root_;
	if (node.get_suffix_link() != NULL)
		point = node.get_suffix_link();

	if (node.is_inner_node()) 
		output_stream << "node" << node.get_name() << " -> node" << node.get_suffix_link()->get_name() << " [label=\"\" style=dotted]\n";
}

/*
	Metoda provjerava sadrzi li izgradjeno stablo poslani znakovni
	niz (string).
*/

bool SuffixTree::contains_substring(std::string const & string) {
	int start_index = 0;
	Node* node = &root_;
	while(start_index < string.length()) {
		int character = encode_char(string[start_index]);
		if(node->contains_edge(character)) {
			std::pair<int, int> edge = node->get_edge(character);
			std::string edge_label = decode_edge(edge.first, edge.second, (node->get_child(character)).get_suffix_ord_num());

			for(int i = 0; i < edge_label.length(); i++) {
				if(i+start_index == string.length()) 
					return true;
				if(edge_label[i] != string[start_index + i])
					return false;
			}
			start_index += edge_label.length();
			if(start_index == string.length()) {
				return true;
			}
			node = &(node->get_child(character));
		}
		else 
			return false;
	}
	return true;
}

/*
	Metoda koja odredjuje brojevnu interpretaciju odredjenog
	znakovnog niza.
*/

int SuffixTree::encode_char(char c) {
	return (int)c;
}
