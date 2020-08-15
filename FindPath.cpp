#include <bits/stdc++.h>
using namespace std;

struct Element {
	int plus;
	int minus;
	int source;
	int ground;
	vector<bool> vis;
	stack<pair<int, int>> s;
	stack<int> path;
	map<pair<int, char>, int> m;
};

const int k = 8;
int n;
vector<vector<int>> graph;
vector<int> vis;
vector<Element> elements;
set<pair<int, char>> forbidden;

pair<int, char> determine(int node) {
	int s=k*k*3;
	if (node<=s) {
		if (node%3==0) {
			if (node%(k*3)==0) return {node/(k*3), 'r'};
			else return {node/(k*3)+1, 'r'};
		}
		else if ((node+1)%3!=0) return {-1, 'e'};
		return {(node-(determine(node+1).first-1)*k*3-2)/3+1, 'c'};
	}
	if (node-s<=k) return {node-s, 'r'};
	return {node-s-k, 'c'};
}

void connect(int element) {
	forbidden.insert(determine(element));
	int position = element-k*k*3;
	if (position<=k) {
		graph[element].push_back(position*3*k);
		graph[position*3*k].push_back(element);
	}
	else {
		graph[element].push_back(3*k*(k-1)+2+(position-k-1)*3);
		graph[3*k*(k-1)+2+(position-k-1)*3].push_back(element);
	}
}

void construct() {
	int s = k*k*3;
	graph.resize(s+2*k+1);
	vis.resize(s+2*k+1);
	int current = 1;
	
	for (int i=0;i<k;i++) {
		for (int j=0;j<k;j++) {
			graph[current].push_back(current+1);
			graph[current].push_back(current+2);
			graph[current+1].push_back(current);
			graph[current+2].push_back(current);
			graph[current+1].push_back(current+2);
			graph[current+2].push_back(current+1);
			if (j!=k-1) {
				graph[current+2].push_back(current+5);
				graph[current+5].push_back(current+2);
			}
			if (i!=k-1) {
				graph[current+1].push_back(current+k*3+1);
				graph[current+k*3+1].push_back(current+1);
			}
			current+=3;
		}
	}
	
	for (Element e: elements) {
		connect(e.source);
		connect(e.ground);
		graph[e.plus].push_back(e.minus);
		graph[e.minus].push_back(e.plus);
	}
	
	for (pair<int, char> p: forbidden) {
		for (Element &e: elements) {
			e.m[p]=-1;
		}
	}
}

void printPath(int index, stack<int> path) {
	printf("Path for element: %d\n", index);
	vector<int> p;
	while (!path.empty()) {
		p.push_back(path.top());
		path.pop();
	}
	for (int i=(int)p.size()-2;i>=0;i--) {
		pair<int, char> current = determine(p[i]);
		int r, c;
		if (current.second=='e') {
			r=determine(p[i]+2).first;
			c=determine(p[i]+1).first;
		}
		else if (current.second=='r') {
			r=current.first;
			c=determine(p[i]-1).first;
		}
		else {
			c=current.first;
			r=determine(p[i]+1).first;
		}
		if (p[i]>k*k*3) printf("(%d%c)", current.first, current.second);
		else printf("(%d, %d)", r, c);
		// printf("%d", p[i]);
		if (i>0) printf(" -> ");
	}
	cout<<endl;
}

bool dfs(int index) {
	Element e  = elements[index];
	e.s.push({e.source, e.source});
	e.path.push(e.source);
	pair<int, char> destination = determine(e.ground);
	
	while (!e.s.empty()) {
		pair<int, int> front = e.s.top();
		e.s.pop();
		// printf("Now visiting node: %d\n", front.first);
		
		pair<int, char> fnode = determine(front.first);
		if (fnode.first!=-1) {
			if (forbidden.find(fnode)==forbidden.end()) {
				forbidden.insert(fnode);
				if (e.m.find(fnode)==e.m.end()) e.m[fnode]=1;
				else e.m[fnode]++;
			}
			else {
				if (e.m.find(fnode)==e.m.end()) e.m[fnode]=1;
				else if (e.m[fnode]!=-1) e.m[fnode]++;
			}
		}
		
		// printf("Comparison: %d and %d\n", e.path.top(), front.second);
		while (e.path.top()!=front.second) {
			// printf("Popping: %d\n", e.path.top());
			e.vis[e.path.top()] = false;
			pair<int, char> t = determine(e.path.top());
			if (e.m[t]!=-1) e.m[t]--;
			if (e.m[t]==0) forbidden.erase(t); 

			e.path.pop();
		}
		if (front.first == e.ground) {
			e.path.push(e.ground);
			printPath(index, e.path);
			// printf("Forbidden in this path: ");
			// for (pair<int, char> f: forbidden) {
				// printf("%d%c ", f.first, f.second);
			// }
			// printf("\n");
			if (index+1==n || (index+1<n && dfs(index+1))) return true;
			continue;
		}
		e.vis[front.first] = true;
		e.path.push(front.first);
		if (front.first==e.plus) {
			e.s.push({e.minus, e.plus});
			continue;
		}
		
		deque<pair<int, int>> preserve;
		for (int node: graph[front.first]) {
			// printf("Checking node: %d\n", node);
			if (!e.vis[node]) {
				if (node==e.plus) {
					// printf("Adding %d to stack.\n", node);
					e.s.push({node, front.first});
					break;
				}
				if (node==e.minus) continue;
				
				if (node==e.ground) {
					if (!e.vis[e.minus]) continue;
					// printf("Adding %d to stack.\n", node);
					e.s.push({node, front.first});
					break;
				}
				
				pair<int, char> current = determine(node);
				if (current.second=='e') continue;
				// if (front.second==e.minus && 
						// (front.first+1==node || node+1==front.first)) {
							// // printf("Wont be adding %d because of %d and %d\n", node, front.first, front.second);
							// continue;
						// }
				if (fnode==current || (e.vis[e.minus] && current==destination)) {
					// e.s.push({node, front.first});
					preserve.push_front({node, front.first});
				}
				else if	(forbidden.find(current)==forbidden.end()) {
					preserve.push_back({node, front.first});
					// e.s.push({node, front.first});
				}
			}
		}
		while (!preserve.empty()) {
			// printf("Adding %d to stack.\n", preserve.back().first);
			e.s.push(preserve.back());
			preserve.pop_back();
		}
	}
	return false;
}

int main() {
	cin>>n;
	elements.resize(n);
	for (Element &e: elements) {
		for (int i=1;i<=3;i++) {
			int r, c, value;
			cin>>r>>c;
			value = (r-1)*k*3+(c-1)*3+1;
			if (i==1) e.plus = value;
			else if (i==2) e.minus = value;
			else {
				e.source = k*k*3+r;
				e.ground = k*k*3+c;
			}
		}
		e.vis.resize(k*k*3+2*k+1);
	}
	construct();
	// for (int x: graph[26]) cout<<x<<" ";
	// cout<<endl;
	// dfs(1);
	dfs(1);
	return 0;
}