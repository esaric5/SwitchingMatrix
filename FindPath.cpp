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
map<pair<int, int>, set<int>> switches;
set<int> voltages, grounds;
vector<pair<int, int>> coordinates;

void generateCircuitFile() {
	ofstream out;
	out.open("SwitchingMatrix.txt");
	out<<"$ 1 5.0E-6 10 50 5.0 50\n";
	for (int i=1, y=30, x;i<=k;i++, y+=140, x=30) {
		for (int j=1;j<=k;j++, x+=140) {
			pair<int, int> ij = {i, j};
			string s[3];
			s[0] = "s " + to_string(x) + " " + to_string(y+100) + " " + 
					to_string(x+100) + " " + to_string(y+100);
			s[1] = "s " + to_string(x) + " " + to_string(y) + " " + 
					to_string(x) + " " + to_string(y+100);
			s[2] = "s " + to_string(x) + " " + to_string(y) + " " + 
					to_string(x+100) + " " + to_string(y+100);
			bool b[3] = {false, false, false};
			if (switches.find(ij)!=switches.end()) {
				for (int ii=0;ii<3;ii++) {
					if (switches[ij].find(ii+1)!=switches[ij].end()) b[ii] = true;
				}
			}
			for (int ii=0;ii<3;ii++) {
				s[ii] += (b[ii] ? " 0 0 1 false\n" : " 0 1 0 false\n");
				out<<s[ii];
			}
			string w1 = "w " + to_string(x) + " " + to_string(y) + " " + 
						to_string(x+140) + " " + to_string(y) + " 0 0\n";
			string w2 = "w " + to_string(x+100) + " " + to_string(y+100) + 
						" " + to_string(x+100) + " " + 
						to_string(i!=8? y+240 : y+140) + " 0 0\n";
			
			if (j!=8 || voltages.find(i)!=voltages.end() || 
				grounds.find(i)!=grounds.end()) out<<w1;
			if (i!=8 || voltages.find(j+8)!=voltages.end() || 
				grounds.find(j+8)!=grounds.end()) out<<w2;
			
			if (i==8) {
				string v = "R " + to_string(x+100) + " " + to_string(y+140) + 
							" " + to_string(x+100) + " " + to_string(y+190) + 
							"  0 40.0 5.0 0.0\n";
				string g = "g " +  to_string(x+100) + " " + to_string(y+140) + 
						" " + to_string(x+100) + " " + to_string(y+190) + " 0\n";
				if (voltages.find(j+8)!=voltages.end()) out<<v;
				if (grounds.find(j+8)!=grounds.end()) out<<g;
			}
		}
		string v = "R " + to_string(x) + " " + to_string(y) + " " + 
					to_string(x+50) + " " + to_string(y) + "  0 40.0 5.0 0.0\n";
		string g = "g " +  to_string(x) + " " + to_string(y) + 
					" " + to_string(x+50) + " " + to_string(y) + " 0\n";
					
		if (voltages.find(i)!=voltages.end()) out<<v;
		if (grounds.find(i)!=grounds.end()) out<<g;	

	}
	
	for (int i=0;i<n*2;i+=2) {
		string r = "r " + to_string((coordinates[i].second-1)*140+30) + " " +
					to_string((coordinates[i].first-1)*140+130) + " " + 
					to_string((coordinates[i+1].second-1)*140+30) + " " +
					to_string((coordinates[i+1].first-1)*140+130) + " 200 100\n";
		out<<r;
	}
	// string r1 = "r 170 270 1010 690 200 100\n";
    // string r2 = "r 170 130 870 830 200 100\n";
	// out<<r1;
	// out<<r2;
	out.close();
}

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

void determineSwitches(stack<int> path) {
	vector<int> p;
	while (!path.empty()) {
		p.push_back(path.top());
		path.pop();
	}
	for (int i=(int)p.size()-2;i>=0;i--) {
		pair<int, char> f = determine(p[i]), s = determine(p[i+1]);
		if (abs(p[i]-p[i+1]) == 1) {
			if (f.second=='r' && s.second=='c') {
				switches[{f.first, s.first}].insert(3);
				// printf("{%d, %d, %d}, ", f.first, s.first, 3);
			}
			else if (f.second=='c' && s.second=='r') {
				switches[{s.first, f.first}].insert(3);
				// printf("{%d, %d, %d}, ", s.first, f.first, 3);
			}
			else {
				if (f.second=='c') {
					int row = determine(p[i]+1).first;
					switches[{row, f.first}].insert(1);
					// printf("{%d, %d, %d}, ", row, f.first, 1);
				}
				else if (s.second == 'c') {
					int row = determine(p[i+1]+1).first;
					switches[{row, s.first}].insert(1);
					// printf("{%d, %d, %d}, ", row, s.first, 1);
				}
			}
		}
		
		else if (abs(p[i]-p[i+1]) == 2) {
			if (f.second=='r') {
				int column = determine(p[i]-1).first;
				switches[{f.first, column}].insert(2);
				// printf("{%d, %d, %d}, ", f.first, column, 2);
			}
			else if (s.second=='r') {
				int column = determine(p[i+1]-1).first;
				switches[{s.first, column}].insert(2);
				// printf("{%d, %d, %d}, ", s.first, column, 2);
			}
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

int dist (pair<int, pair<int, int>> a, pair<int, int> goal) {
	return (a.second.first-goal.first)*(a.second.first-goal.first)+
			(a.second.second-goal.second)*(a.second.second-goal.second);
}
bool dfs(int index) {
	Element e  = elements[index];
	e.s.push({e.source, e.source});
	e.path.push(e.source);
	pair<int, char> destination = determine(e.ground);
	pair<int, int> goal = {determine(e.plus+2).first, determine(e.plus+1).first};
	
	// printf("Goal is at: (%dr, %dc)\n", goal.first, goal.second);
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
			// printPath(index, e.path);
			// printf("Forbidden in this path: ");
			// for (pair<int, char> f: forbidden) {
				// printf("%d%c ", f.first, f.second);
			// }
			// printf("\n");
			if (index+1==n || (index+1<n && dfs(index+1))) {
				// printPath(index, e.path);
				determineSwitches(e.path);
				return true;
			}
			continue;
		}
		e.vis[front.first] = true;
		e.path.push(front.first);
		if (front.first==e.plus) {
			e.s.push({e.minus, e.plus});
			if (destination.second=='r') {
				goal = {destination.first, k};
			}
			else goal = {k, destination.first};
			// printf("Goal is at: (%dr, %dc)\n", goal.first, goal.second);
			continue;
		}
		
		vector<pair<int, pair<int, int>>> preserve;
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
				if (front.second==e.minus && 
						(front.first+1==node || node+1==front.first)) {
							// printf("Wont be adding %d because of %d and %d\n", node, front.first, front.second);
							continue;
						}
				if (fnode==current || (e.vis[e.minus] && current==destination) ||
					forbidden.find(current)==forbidden.end()) {
					pair<int, int> rc;
					if (current.second=='r')
						rc = {current.first, determine(current.first-1).first};
					else rc = {determine(current.first+1).first, current.first}; 
					preserve.push_back({node, rc});
					// e.s.push({node, front.first});
				}
			}
		}
		sort(preserve.begin(), preserve.end(),
			[&](pair<int, pair<int, int>> a, pair<int, pair<int, int>> b) {
				return dist(a, goal) > dist(b, goal);;
				});
		while (!preserve.empty()) {
			// printf("Adding %d to stack.\n", preserve.back().first);
			e.s.push({preserve.back().first, front.first});
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
			if (i==1 || i==2) coordinates.push_back({r, c});
			if (i==1) e.plus = value;
			else if (i==2) e.minus = value;
			else {
				voltages.insert(r);
				grounds.insert(c);
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
	if (dfs(0)) generateCircuitFile();
	return 0;
}