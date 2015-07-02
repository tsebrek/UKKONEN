#include "node.h"

#include <iostream>
#include <string>
#include <vector>

class SuffixTree {
public:
	SuffixTree(std::string const &);
	SuffixTree(std::vector<std::string> const &);
	~SuffixTree();

	void print_tree(std::ostream&);
	std::vector< std::pair<int, int> > get_suffix_prefix_matches();
	std::vector<std::string> get_all_suffixes();
	bool contains_substring(std::string const &);

private:
	void init(std::vector<std::string> const &);
	std::string get_path_label(Node*);
	std::string decode_edge(int start, int end, int ord_num);
	int encode_char(char);
	void print(Node&, std::ostream&);
	void build_tree();
	void add_suffix_link(Node**, Node*);

private:
#pragma warning(push)
#pragma warning(disable:4251) // C4251: `member` needs to have dll-interface to be used by clients of class `class`
	Node root_;
	std::string string_;
	std::vector<int> label_;
	std::vector<int> suffix_end_;
	std::vector<Node*> leafs_;
#pragma warning(pop)
};
