#include <unordered_map>
#include <vector>

class Node {
public:
	Node(int name, int ord_num = -1, int start_index = -1);
	~Node();

	void add_child(int character, Node& child, std::pair<int, int> pair);
	Node& get_child(int character);
	std::pair<int, int> get_edge(int character);
	bool contains_edge(int character);
	std::vector<int> get_characters();
	std::pair<int, int> get_edge_from_parent();
	Node* get_parent();
	void set_parent(Node*);

	bool is_root();
	bool is_inner_node();
	bool is_leaf();
	bool is_terminal();
	
	void set_suffix_link(Node*);
	Node* get_suffix_link();

	int get_name();
	int get_suffix_ord_num();
	int get_start_index();

private:
#pragma warning(push)
#pragma warning(disable:4251) // C4251: `member` needs to have dll-interface to be used by clients of class `class`
	std::unordered_map<int, Node*> children_;
	std::unordered_map<int, std::pair<int, int>> edges_;
#pragma warning(pop)

private:
	bool is_terminal_;
	int ord_num_;
	int start_index_;
	int name_;
	Node* parent_;
	Node* suffix_link_;
};
