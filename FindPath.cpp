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

const int k = 16;
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
	for (int i=1, y=30, x=30;i<=k;i++, y+=140, x=30) {
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
						to_string(i!=k? y+240 : y+140) + " 0 0\n";
			
			if (j!=k || voltages.find(i)!=voltages.end() || 
				grounds.find(i)!=grounds.end()) out<<w1;
			if (i!=k || voltages.find(j+k)!=voltages.end() || 
				grounds.find(j+k)!=grounds.end()) out<<w2;
			
			if (i==k) {
				string v = "R " + to_string(x+100) + " " + to_string(y+140) + 
							" " + to_string(x+100) + " " + to_string(y+190) + 
							"  0 40.0 5.0 0.0\n";
				string g = "g " +  to_string(x+100) + " " + to_string(y+140) + 
						" " + to_string(x+100) + " " + to_string(y+190) + " 0\n";
				if (voltages.find(j+k)!=voltages.end()) out<<v;
				if (grounds.find(j+k)!=grounds.end()) out<<g;
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
			}
			else if (f.second=='c' && s.second=='r') {
				switches[{s.first, f.first}].insert(3);
			}
			else {
				if (f.second=='c') {
					int row = determine(p[i]+1).first;
					switches[{row, f.first}].insert(1);
				}
				else if (s.second == 'c') {
					int row = determine(p[i+1]+1).first;
					switches[{row, s.first}].insert(1);
				}
			}
		}
		
		else if (abs(p[i]-p[i+1]) == 2) {
			if (f.second=='r') {
				int column = determine(p[i]-1).first;
				switches[{f.first, column}].insert(2);
			}
			else if (s.second=='r') {
				int column = determine(p[i+1]-1).first;
				switches[{s.first, column}].insert(2);
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
		if (i>0) printf(" -> ");
	}
	cout<<endl;
}

int dist (pair<pair<int, char>, pair<int, char>> a, pair<pair<int, char>, pair<int, char>> b) {
	return (a.first.first-b.first.first)*(a.first.first-b.first.first)+
			(a.second.first-b.second.first)*(a.second.first-b.second.first);
}

bool dfs(int index) {
	Element e  = elements[index];
	e.s.push({e.source, e.source});
	e.path.push(e.source);
	pair<pair<int, char>, pair<int, char>> sourceDestination = {determine(e.plus+2), determine(e.plus+1)}, destination = sourceDestination;
	pair<int, char> gnd = determine(e.ground);
	if (forbidden.find(destination.first)!=forbidden.end() && forbidden.find(destination.second)!=forbidden.end() &&
		determine(e.source)!=destination.first && determine(e.source)!=destination.second) return false;
	
	while (!e.s.empty()) {
		pair<int, int> front = e.s.top();
		e.s.pop();
		
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
		
		while (e.path.top()!=front.second) {
			e.vis[e.path.top()] = false;
			if (e.path.top()==e.plus) destination = sourceDestination;
			pair<int, char> t = determine(e.path.top());
			if (e.m[t]!=-1) e.m[t]--;
			if (e.m[t]==0) forbidden.erase(t); 

			e.path.pop();
		}
		if (front.first == e.ground) {
			// printPath(index, e.path);
			if (index+1==n || (index+1<n && dfs(index+1))) {
				determineSwitches(e.path);
				return true;
			}
			continue;
		}
		e.vis[front.first] = true;
		e.path.push(front.first);
		if (front.first==e.plus) {
			e.s.push({e.minus, e.plus});
			if (gnd.second=='r') destination = {gnd, {k+1, 'c'}};
			else destination={{k+1, 'r'}, gnd};
			continue;
		}
		
		deque<int> nodes;
		bool found = false, pushed = false;
		
		for (int node: graph[front.first]) {
			if (!e.vis[node]) {
				if (node==e.plus || (e.vis[e.minus] && node==e.ground)) {
					if (found) e.s.pop();
					else found = true;
					e.s.push({node, front.first});
					break;
				}
				
				if (found) continue;
				
				pair<int, char> current = determine(node);
				if (current.second=='e') continue;
				if (front.second==e.minus && (front.first+1==node || node+1==front.first)) continue;
				
				bool destRow = forbidden.find(destination.first)==forbidden.end() && destination.first.first!=k+1;
				bool destColumn = forbidden.find(destination.second)==forbidden.end() && destination.second.first!=k+1;
				
				auto isApproachingDestination = [&]() {
					if (fnode==current && current.second=='c') {
						int currentRow = determine(node+1).first, fromRow = determine(front.first+1).first;
						if (front.first==e.source) fromRow=k+1;
						if (abs(destination.first.first-currentRow)>abs(destination.first.first-fromRow)) return false;
					}
					
					if (fnode==current && current.second=='r') {
						int currentColumn = determine(node-1).first, fromColumn = determine(front.first-1).first;
						if (front.first==e.source) fromColumn=k+1;
						if (abs(destination.second.first-currentColumn)>abs(destination.second.first-fromColumn)) return false;
					}
					return true;
				};
				
				bool check = fnode==current || (e.vis[e.minus] && current==gnd);
				//In the same row/column as destination
				if (((check || destRow) && destination.first==current) || 
					((check || destColumn) && destination.second==current)) {
					if (!isApproachingDestination()) continue;
					found = true;
					e.s.push({node, front.first});
					continue;
				}
				
				//Try one switch approach first
				if (fnode==current) {
					if (isApproachingDestination() && 
						((current.second=='c' && (destRow || (e.vis[e.minus] && gnd.second=='r'))) || 
						(current.second=='r' && (destColumn || (e.vis[e.minus] && gnd.second=='c'))))) {
							nodes.push_front(node);
							pushed = true;
						}
					else nodes.push_back(node);
				}
				else if (forbidden.find(current)==forbidden.end()) {
					if (pushed) nodes.push_back(node);
					else nodes.push_front(node);
				}
			}
		}
		while (!nodes.empty()) {
			if (!found) e.s.push({nodes.back(), front.first});
			nodes.pop_back();
		}
	}
	
	while (!e.path.empty()) {
		e.vis[e.path.top()] = false;
		pair<int, char> t = determine(e.path.top());
		if (e.m[t]!=-1) e.m[t]--;
		if (e.m[t]==0) forbidden.erase(t); 
		e.path.pop();
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
	if (dfs(0)) {
		printf("Path found. Check SwitchingMatrix.txt.\n");
		generateCircuitFile();
	}
	else printf("Path not found.\n");
	return 0;
}