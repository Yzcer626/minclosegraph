#include <iostream>
#include <set>
#include <vector>
#include <list>
#include <unordered_map>
#include <cmath>
#include <compare>
#include <algorithm>
#define EPSILON 1e-8
#define M_PI 3.14159265358979323846

using namespace std;

class point {
    double x, y;

public:
    point():x(0), y(0) {}
    point(double a, double b):x(a), y(b) {}
    double getX() const { return x; }
    double getY() const { return y; }
    static bool onsegment(const point& p1, const point& p2, const point& p3) {
        double x1 = p2.x - p1.x;
        double x2 = p3.x - p2.x;
        double y1 = p2.y - p1.y;
        double y2 = p3.y - p2.y;
        double tan1 = atan2(y1, x1), tan2 = atan2(y2, x2);
        return fabs(tan1 - tan2) < EPSILON;
    }
    auto operator<=> (const point& p1) const = default;
    bool operator()(const point& p1, const point& p2) const{
        if (fabs(p1.x - p2.x) > EPSILON)
            return p1.x < p2.x;
        else
            return p1.y < p2.y;
    }
    friend class segment;
    friend class SegContainer;
};

class segment {
    point p1, p2;

public:
    mutable bool flag;
    set<point,point> intersect_p;
    segment(): p1(0,0), p2(0,0), flag(true) {}
    segment(point a, point b): p1(a), p2(b), flag(true) {}
    segment(point p): p1(p), p2(1000000000.0, p.getY()), flag(true) {}
    segment antisegment() const {
        return segment(p2, p1);
    }
    point getP1() const { return p1; }
    point getP2() const { return p2; }
    point maxpoint() const {
        if (p1 < p2)
            return p2;
        else
            return p1;
    }
    point minpoint() const {
        if (p1 < p2)
            return p1;
        else
            return p2;
    }
    friend istream& operator>>(istream& in, segment& l) {
        double x1, y1, x2, y2;
        char ch;
        in >> x1 >> ch >> y1 >> x2 >> ch >> y2;
        l = segment(point(x1, y1), point(x2, y2));
        return in;
    }
    friend bool operator==(const segment& l1, const segment& l2) {
        return (l1.p1 == l2.p1 && l1.p2 == l2.p2);
    }
    bool operator()(const segment& l1,const segment& l2) const{
        double x1 = l1.p2.x - l1.p1.x;
        double x2 = l2.p2.x - l2.p1.x;
        double y1 = l1.p2.y - l1.p1.y;
        double y2 = l2.p2.y - l2.p1.y;
        double tan1 = atan2(y1, x1), tan2 = atan2(y2, x2);
        if (tan1 < 0)
            tan1 += 2 * M_PI;
        if (tan2 < 0)
            tan2 += 2 * M_PI;
        if(l1.getP1() == l2.getP1()) {
            return tan1 < tan2;
        }
        else {
            return l1.getP1() < l2.getP1();
        }
    }
    static bool onsegment(const point& p, const segment& s) {
        if(p == s.getP1() || p == s.getP2()) {
            return true;
        }
        if(point::onsegment (s.getP1(), p, s.getP2())) {
            if(p >= s.minpoint() && p <= s.maxpoint())
                return true;
        }
        return false;
    }
    static bool intersected(segment& l1, segment& l2) {
        double k1, k2;
        point xa, xb, xc;
        xa.x = l1.p1.x - l1.p2.x;
        xa.y = l1.p1.y - l1.p2.y;
        xb.x = l1.p1.x - l2.p1.x;
        xb.y = l1.p1.y - l2.p1.y;
        xc.x = l1.p1.x - l2.p2.x;
        xc.y = l1.p1.y - l2.p2.y;
        k1 = xa.x * xb.y - xa.y * xb.x;
        k2 = xa.x * xc.y - xa.y * xc.x;
        if (k1 * k2 < 0 || (fabs(k1) < EPSILON && fabs(k2) > EPSILON) || (fabs(k2) < EPSILON && fabs(k1) > EPSILON))
            return true;
        else
            return false;
    }
    static bool insertIntersection(segment& l1, segment& l2) {
        if (intersected(l1, l2)) {
            double d;
            point p;
            d = (l1.p2.y - l1.p1.y) * (l2.p2.x - l2.p1.x) - (l2.p2.y - l2.p1.y) * (l1.p2.x - l1.p1.x);
            p.x = ((l1.p2.x - l1.p1.x) * (l2.p2.x - l2.p1.x) * (l2.p1.y - l1.p1.y) 
                 + (l1.p2.y - l1.p1.y) * (l2.p2.x - l2.p1.x) * l1.p1.x 
                 - (l2.p2.y - l2.p1.y) * (l1.p2.x - l1.p1.x) * l2.p1.x) / d;
            p.y = ((l1.p2.y - l1.p1.y) * (l2.p2.y - l2.p1.y) * (l2.p1.x - l1.p1.x) 
                 + (l1.p2.x - l1.p1.x) * (l2.p2.y - l2.p1.y) * l1.p1.y 
                 - (l2.p2.x - l2.p1.x) * (l1.p2.y - l1.p1.y) * l2.p1.y) / (-d);
            if(p >= l1.minpoint() && p <= l1.maxpoint() && p >= l2.minpoint() && p <= l2.maxpoint()){
                l1.intersect_p.insert(p);
                l2.intersect_p.insert(p);
                return true;
            }  
        }
        return false;
    }
};

class Graph {
    vector<point> pointlist;
public:
    int pointcount;
    int contained;
    static int graphnum;
    Graph():pointcount(0), contained(0) {
        vector<point> nearlist;
        list<segment> seglist;
    }
    void threepointmanage(){
        for(int i = pointlist.size() - 1; i >= 0; i--) {
            int size = pointlist.size();
            if(point::onsegment(pointlist[((i-1)%size + size)%size], pointlist[i], pointlist[((i+1)%size + size)%size])) {
                pointlist.erase(pointlist.begin() + i);
            }
        }
    }
    void contains(const point& p) {
        int count = 0;
        segment ray(p);
        int x = 1;
        for(auto& ps : pointlist) {
            segment ns(ps, pointlist[x++ % pointlist.size()]);
            if(segment::insertIntersection(ray, ns)) {
                count++;
            }
            if(segment::onsegment(p, ns)) {
                contained++;
                return;
            }
        }
        if(count % 2 == 1) {
            contained++;
        }
    }
    void print() const {
        cout << "第" << graphnum++ << "个图" << " : ";
        for(const auto& p : pointlist) {
            cout << "(" << p.getX() << "," << p.getY() << ") " << ' ';
        }
        cout << endl;
    }
    friend class GraphContainer;
};
int Graph::graphnum = 1;

class SegContainer {
    unordered_map<size_t, set<segment,segment>> segset;
public:
    static size_t hashSegment(const segment& l) {
        return (hash<double>()(l.getP1().getX())*31 ^ hash<double>()(l.getP1().getY())*17) % 100;
    }
    void insertSegment(const segment& l) {
        size_t key = hashSegment(l);
        segset[key].insert(l);
    }
    bool findSegment(const segment& l) {
        size_t key = hashSegment(l);
                     hash<double>()(l.getP1().getY());
        auto it = segset.find(key);
        if (it != segset.end()) {
            auto& s = it->second;
            auto sit = s.find(l);
            if (sit != s.end() && sit->flag) {
                return true;
            }
        }
        return false;
    }
    bool findAntisegment(const segment& l) {
        return findSegment(l.antisegment());
    }
    friend class GraphContainer;
};

class beginsegment {
    vector<segment> seglist;
public:
    void tointersection() {
        for(int i = 0; i < seglist.size() - 1; i++) {
            for(int j = i + 1; j < seglist.size(); j++) {
                segment l1 = seglist[i], l2 = seglist[j];
                segment::insertIntersection(seglist[i], seglist[j]);
            }
        }
    }
    void buildsegbyintersection(SegContainer& segcontainer) {
        for(const auto& s : seglist) {
            for(auto it = s.intersect_p.begin(); it != s.intersect_p.end(); it++) {
                if(next(it) != s.intersect_p.end()) {
                    segcontainer.insertSegment(segment(*it, *next(it)));
                    segcontainer.insertSegment(segment(*next(it), *it));
                }
            }
        }
    }
    friend class drawsegment;
};

class GraphContainer {
    vector<Graph> graphs;
public:
    set<point> allpoints;
    int graphcount;
    GraphContainer():graphcount(0) {
        vector<Graph> graphs;
        vector<point> allpoints;
    }
    void insertGraph(const Graph& g) {
        graphs.push_back(g);
        graphcount++;
        for(const auto& p : g.pointlist) {
            allpoints.insert(p);
        }
    }
    void buildclosegraph(const segment& s, SegContainer& segcontainer, size_t key) {
        Graph closegraph;
        segment current_s = s;
        point start_p = s.getP1();
        auto it = segcontainer.segset[key].find(s);
        if(it != segcontainer.segset[key].end() && it->flag) {
            closegraph.pointlist.push_back(s.getP1());
            closegraph.pointlist.push_back(s.getP2());
            closegraph.pointcount = 2;
            (it->flag) = false;
        }
        point current_p = s.getP2();
        set<segment,segment>::iterator nextit = it;
        for( ; current_p != start_p; ) {
            segment anti_seg = current_s.antisegment();
            nextit = segcontainer.segset[segcontainer.hashSegment(anti_seg)].find(anti_seg);
            if(nextit != segcontainer.segset[segcontainer.hashSegment(anti_seg)].end()) 
                nextit++;
            else
                nextit = segcontainer.segset[segcontainer.hashSegment(anti_seg)].begin();
            for( ; ; ){
                if(!(*nextit).flag || (*nextit).getP1() != anti_seg.getP1()) {
                    if(nextit != segcontainer.segset[segcontainer.hashSegment(anti_seg)].end()) {
                        nextit++;
                    }
                    else {
                        nextit = segcontainer.segset[segcontainer.hashSegment(anti_seg)].begin();
                    }
                }
                else
                    break;
            }
            current_s = *nextit;
            current_p = current_s.getP2();
            (nextit->flag) = false;
            if(current_p == start_p) 
                break;
            closegraph.pointlist.push_back(current_p);
            closegraph.pointcount++;
        }
        insertGraph(closegraph);
    }
    void build(SegContainer& segcontainer) {
        for(auto it = segcontainer.segset.begin(); it != segcontainer.segset.end(); it++) {
            for(const auto& s : it->second) {
                if(s.flag)
                    buildclosegraph(s, segcontainer, it->first);
                else 
                    continue;
            }
        }
    }
    void deletequalgraph() {
        vector<Graph> temp = graphs;
        for(auto& g : temp) {
            sort(g.pointlist.begin(), g.pointlist.end());
        }
        for(int i = 0; i < graphs.size(); i++) {
            for(int j = i + 1; j < graphs.size(); j++) {
                if(temp[i].pointlist == temp[j].pointlist) {
                    graphs.erase(graphs.begin() + j);
                    temp.erase(temp.begin() + j);
                    graphcount--;
                    j--;
                }
            }
        }
    }
    void allcontainjudge(){
        int pointcount = allpoints.size();
        for(int i = 0; i < graphs.size(); i++) {
            graphs[i].threepointmanage();
            for(const auto& p : allpoints) {
                graphs[i].contains(p);
            }
            if(graphs[i].contained == pointcount) {
                if(graphcount != 1) {
                    graphs.erase(graphs.begin() + i);
                    graphcount--;
                    i--;
                }
            }
        }
    }
    void print() const {
        for(const auto& g : graphs) {
            g.print();
        }
    }

};

class drawsegment {
public:
    static void firstdraw(beginsegment& seglist) {
        cout << "有几条线段？" << endl;
        int n;
        cin >> n;
        seglist.seglist.resize(n);
        for(int i = 0; i < n; i++) {
            cin >> seglist.seglist[i];
        }
    }
};

int main() {
    beginsegment seglist;
    drawsegment::firstdraw(seglist);
    seglist.tointersection();
    SegContainer segcontainer;
    seglist.buildsegbyintersection(segcontainer);
    GraphContainer graphcontainer;
    graphcontainer.build(segcontainer);
    graphcontainer.deletequalgraph();
    graphcontainer.allcontainjudge();
    graphcontainer.print();
}
