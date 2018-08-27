#include <iostream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <fstream>

class Node
{
public:
    int weight;
    char symbol;
    Node *left_leaf, *right_leaf;

    Node() {}
    Node(Node* ll, Node* rl) : left_leaf(ll), right_leaf(rl) { weight = left_leaf->weight + right_leaf->weight; }
};


bool nodeSort(const Node* left_leaf, const Node* right_leaf) {
    return left_leaf->weight < right_leaf->weight;
}


void getLeaf(std::ifstream& f, std::map<char, int>& leaf) {
    while (!f.eof()) {
        char symbol = f.get();
        leaf[symbol]++;
    }
}

void getTree(std::list<Node*>& tree, std::map<char, int>& leaf) {
    for (auto& elem : leaf) {
        Node* leaf = new Node;
        leaf->symbol = elem.first;
        leaf->weight = elem.second;
        tree.push_back(leaf);
    }

    while (tree.size() != 1) {
        tree.sort(nodeSort);
        Node* left_leaf = tree.front(); tree.pop_front();
        Node* right_leaf = tree.front(); tree.pop_front();
        Node* sum_leaf = new Node(left_leaf, right_leaf); tree.push_front(sum_leaf);
    }
}

void getBits(Node* root, std::vector<bool>& bits, std::map<char, std::vector<bool> >& table) {
    if (root->left_leaf) {
        bits.push_back(0);
        getBits(root->left_leaf, bits, table);
    }

    if (root->right_leaf) {
        bits.push_back(1);
        getBits(root->right_leaf, bits, table);
    }

    if (root->symbol) table[root->symbol] = bits;

    if (bits.size()) bits.pop_back();
}

void comp(std::ifstream& compf_in, std::ofstream& compf_out, std::map<char, std::vector<bool> >& table) {
    int count = 0; char buf = '\0';

    while (!compf_in.eof()) {
        char key = compf_in.get();
        std::vector<bool> x = table[key];

        for (auto elem : x) {
            buf = buf | elem << (7 - count);
            count++;

            if (count == 8) {
                count = 0;
                compf_out << buf;
                buf = 0;
            }
        }
    }
}

void decomp(Node* root, std::ifstream& decompf_in, std::ofstream& decompf_out) {
    Node* l = root; int count = 0; char byte = decompf_in.get();

    while (!decompf_in.eof()) {
        bool b = byte & (1 << (7 - count));

        if (b) l = l->right_leaf;
        else l = l->left_leaf;

        if (l->symbol) {
            decompf_out << l->symbol;
            l = root;
        }
        count++;

        if (count == 8) {
            count = 0;
            decompf_in >> byte;
        }
    }

}

int main() {
    std::cout << "Compression:\n";
    std::cout << "Enter input file name: ";
    std::string comp_in; std::cin >> comp_in;
    std::ifstream compf_in(comp_in, std::ios::out | std::ios::binary);

    std::map<char, int> leaf; std::list<Node*> tree;
    getLeaf(compf_in, leaf); getTree(tree, leaf);

    std::vector<bool> bits; std::map<char, std::vector<bool> > table; Node* root = tree.front();
    getBits(root, bits, table);

    compf_in.clear(); compf_in.seekg(0);

    std::cout << "Enter output file name: ";
    std::string comp_out; std::cin >> comp_out;
    std::ofstream compf_out(comp_out, std::ios::out | std::ios::binary);

    comp(compf_in, compf_out, table);
    compf_out.close(); compf_in.close();
    std::cout << "Compression was succesful!\n";

    std::cout << "\nDecompression:\n";
    std::cout << "Enter input file name: ";
    std::string decomp_in; std::cin >> decomp_in;
    std::ifstream decompf_in(decomp_in, std::ios::in | std::ios::binary);

    std::cout << "Enter output directory: ";
    std::string decomp_out; std::cin >> decomp_out;
    std::ofstream decompf_out(decomp_out + "/res.txt", std::ios::out | std::ios::binary);

    decomp(root, decompf_in, decompf_out);
    decompf_in.close(); decompf_out.close();
    std::cout << "Decompression was succesful!\n";

    return 0;
}
