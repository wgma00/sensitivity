#include<iostream>
#include<algorithm>
#include<unordered_map>
#include<queue>
#include<sstream>
#include<cstdio>
#define MAXBITS 12
#define len(x) (int)(x).size()
using namespace std;

typedef unordered_map< bitset<MAXBITS>, vector< bitset<MAXBITS> > > State;


/**
 * Prints instructions for usage of programming to standard out.
 **/
void usage(){
    cout << "Usage:\n\tsim N K [LOG]\n";
    cout << "\tN - number to create a superset on in base 10 and greater than 0\n";
    cout << "\tK - maximum number of neighbours each element can have\n";
    cout << "\tLOG - optional, print the game to standard out, pass in 1 for yes, 0 for no\n";
}

/**
 * Determine if two bitsets a,b differ by at most one bit.
 **/
bool differ_by_one_bit(const bitset<MAXBITS>&a, const bitset<MAXBITS>&b){
    int cnt = 0;
    for(int i = 0;i < MAXBITS;i++){
        if(a[i] != b[i]){
            cnt++;
        }
    }
    return cnt == 1;
}


/**
 * Returns the first N bits from a bit in little-endian
 * */
string get_first_n_bits(const bitset<MAXBITS>&b, const int &N){
    string bin = b.to_string();
    return bin.substr(bin.size()-N, N); 
}

/**
 * Prints out the adjacency list representation of the State object.
 **/
void print_state(const State &st, const int &N){
    for(auto it: st){
        bitset<MAXBITS>node = it.first;
        vector< bitset<MAXBITS> >edges = it.second;
        cout << "node " << get_first_n_bits(node, N) << "(" << node.to_ulong() << ")" << " adj:";
        for(auto &adj_node: edges){
            cout << get_first_n_bits(adj_node, N) << "(" << adj_node.to_ulong() << ")" << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}


/**
 * Obtain the argument values N and K from arguments pass to the program,
 * then stores it in the respective variables. Returns a 0 on successful validation of
 * input, -1 otherwise.
 **/
int get_nk_log(int argc, char **argv, int *N, int *K, int *LOG){
    string arg_one(argv[1]);
    string arg_two(argv[2]);
    stringstream ss;

    ss << arg_one;
    ss >> *N;
    if(!ss){
        return -1;
    }
    ss.clear();

    ss << arg_two;
    ss >> *K;
    if(!ss){
        return -1;
    }
    ss.clear();

    if(argc == 4){// optinal command, LOG
        string arg_three(argv[3]);
        ss << arg_three;
        ss >> *LOG;
        if(!ss){
            return -1;
        } else if(*LOG != 0 && *LOG != 1){
            return -1;
        }
        ss.clear();
    }else{
        *LOG = 0;// default to 0

    }

    return 0;
}

/** Maps a Graph to 1-10000007, using the muliplication method.
 * */
unsigned long create_hash(const State &st){
    #define MOD (1000007)
    unsigned long ret = 1;
    for(auto it: st){
        ret *= it.first.to_ulong()+1;
        //ret = (ret%MOD * (it.first.to_ulong()+1)%MOD)%MOD;
    }
    #undef MOD
    return ret;
}


/**
 * Creates a new graph by removing all nodes and edges connecting to nodes 
 * which have the designed bit value in the given index.
 * */
State construct_new_graph(const State &st, int index, int designated_bit){
    State st_cpy = st;
    for(auto it: st){
        bitset<MAXBITS>node = it.first;
        if(node[index] == designated_bit){
            st_cpy.erase(node);
        }else{
            vector< bitset<MAXBITS> >edges = it.second;
            vector< bitset<MAXBITS> >new_adj;
            for(int i = 0;i < len(edges);i++){
                if(edges[i][index] != designated_bit){ 
                    new_adj.push_back(edges[i]);
                }
            }
            st_cpy.erase(node);
            st_cpy[node] = new_adj;
        }
    }
    return st_cpy;
}

bool has_at_least_one_edge(const State &st){
    for(auto it:st){
        vector< bitset<MAXBITS> >edges = it.second;
        if(len(edges) > 0){
            return true;
        }
    }
    return false;
}


unordered_multimap<unsigned long, bool>computed_graphs;

/**
 * Determines if there exists an optimal strategy on this subgraph.
 */
bool optimal_strategy(const State &st, bitset<MAXBITS>&used_bits, int n){
    //unsigned long hash = create_hash(st);
    if(!has_at_least_one_edge(st)){
        //computed_graphs.insert(pair<int,bool>(hash, false));
        return false;
    } 
    // else if(computed_graphs.find(hash) != computed_graphs.end()){ 
    //     cout << "computed!" << endl;
    //     for(auto it:st){
    //         cout << it.first.to_ulong() << " ";
    //     }
    //     cout << "\nhash: ";
    //     cout << create_hash(st) << endl;
    //     cout << endl;
    //     cout << computed_graphs.find(hash)->second << endl;
    //     return computed_graphs.find(hash)->second;
    // } 
    else if(n != 1){
        bool pos = true;
        for(int i = 0;pos && i < MAXBITS;i++){
            if(!used_bits[i]){
                used_bits[i] = 1;
                // try finding an optimal strategy by removing ith bit to 1
                State graph_one_removed = construct_new_graph(st, i, 1);
                State graph_zero_removed = construct_new_graph(st, i, 0);
                bool result = false;
                result |= optimal_strategy(graph_one_removed, used_bits, n-1);
                if(result){ // skip ahead since strategy exists
                    used_bits[i]=0;
                    continue;
                }
                result |= optimal_strategy(graph_zero_removed, used_bits, n-1);
                used_bits[i] = 0;
                pos &= result;
            }
        }
        //computed_graphs.insert(pair<unsigned long,bool>(hash, pos));
        return pos;
    }else{
        return has_at_least_one_edge(st);
    }
}

State load_config(){
    State start_adj;
    // 0000: 0100, 1000
    start_adj[bitset<MAXBITS>("0000")] = {bitset<MAXBITS>("0100"), bitset<MAXBITS>("1000")};

    // 0100: 0000, 0110
    start_adj[bitset<MAXBITS>("0100")] = {bitset<MAXBITS>("0000"), bitset<MAXBITS>("0110")}; 

    // 0110: 0100, 0111
    start_adj[bitset<MAXBITS>("0110")] = {bitset<MAXBITS>("0100"), bitset<MAXBITS>("0111")};

    // 0111: 0110, 0011
    start_adj[bitset<MAXBITS>("0111")] = {bitset<MAXBITS>("0110"), bitset<MAXBITS>("0011")};

    // 0011: 0111, 1011
    start_adj[bitset<MAXBITS>("0011")] = {bitset<MAXBITS>("0111"), bitset<MAXBITS>("1011")}; 

    // 1011: 0011, 1010
    start_adj[bitset<MAXBITS>("1011")] = {bitset<MAXBITS>("0011"), bitset<MAXBITS>("1010")}; 

    // 1010: 1011, 1000
    start_adj[bitset<MAXBITS>("1010")] = {bitset<MAXBITS>("1011"), bitset<MAXBITS>("1000")}; 

    // 1000, 1010, 0000
    start_adj[bitset<MAXBITS>("1000")] = {bitset<MAXBITS>("1010"), bitset<MAXBITS>("0000")}; 
    return start_adj;
}

void simulate(int N, int K, int LOG){
    bitset<MAXBITS>start, used_bits;
    //State start_adj;
    //start_adj[start] = vector< bitset<MAXBITS> >();
    State start_adj = load_config();
    queue< State >q;
    q.push(start_adj);
    while(!q.empty()){
        State st = q.front();
        q.pop();
        if(LOG){
            cout << "current graph: "; 
            for(auto it:st){
                cout << it.first.to_ulong() << " ";
            }
            cout << "\nHash: ";
            cout << create_hash(st) << endl;
            cout << endl;
            print_state(st, N);
        }
        if(optimal_strategy(st, used_bits, N)){
            cout << "Subgraph found\n" << endl;
            print_state(st, N);
            exit(0);
        }
        for(auto it: st){
            bitset<MAXBITS>node = it.first;
            vector< bitset<MAXBITS> >edges = it.second;
            // check if the flipped node is already in our 
            for(int i = 0;i < N;i++){
                node[i] = ~node[i];
                if(st.find(node) == st.end()){
                    // determine all nodes which differ by one bit with
                    // new node we're going to add. And determine if once
                    // this node is added whether or not the graph is still
                    // maximal K
                    bool valid = true;
                    for(auto jt:st){
                        bitset<MAXBITS>adj_node= jt.first;
                        // adding this node breaks the maximal k property
                        if(differ_by_one_bit(node, adj_node) && len(st[adj_node])+1 > K){
                            valid = false;
                            break;
                        }
                    }
                    if(valid){
                        // now add edges for nodes which differ by one bit
                        // with the new node we're adding to the graph
                        State st_cpy = st;
                        for(auto jt:st_cpy){
                            bitset<MAXBITS>adj_node = jt.first;
                            if(differ_by_one_bit(node, adj_node)){
                                st_cpy[node].push_back(adj_node);
                                st_cpy[adj_node].push_back(node);
                            }
                        }
                        q.push(st_cpy);
                    }
                }
                node[i] = ~node[i];
            }
        }
    } 
}


int main(int argc, char**argv) {
    if(argc <= 1 or argc > 4){
        usage();
        exit(1);
    }else{
        int N, K, LOG;
        int result = get_nk_log(argc, argv, &N, &K, &LOG);
        if(result < 0){
            usage();
            exit(1);
        }else{
            simulate(N, K, LOG);
            cout << "No subgraph found" << endl;
        }
    }
    return 0;
}
