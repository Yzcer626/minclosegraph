#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
using namespace std;
#define M_PI 3.14159265358979323846
#define EPSILON 0.00000001
#define max(x,y) ((x)>(y)?(x):(y))
#define min(x,y) ((x)<(y)?(x):(y))
#define equal(a,b) (fabs((a)-(b))<EPSILON?1:0)

typedef struct point {   
    double x, y;
} point;

typedef struct segment {
    point p1, p2;
    bool flag;
} segment;

segment antisegment(segment l) {
    segment line;
    line.p1 = l.p2;
    line.p2 = l.p1;
    return line;
}

int onsegment(point p1, point p2, point p3) {
    double x1 = p2.x - p1.x;
    double x2 = p3.x - p2.x;
    double y1 = p2.y - p1.y;
    double y2 = p3.y - p2.y;
    double tan1 = atan2(y1, x1), tan2 = atan2(y2, x2);
    if (equal(tan1, tan2))
        return 1;
    else
        return 0;
}

int intersect(struct segment l1, struct segment l2) {
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
    if (k1 * k2 < 0 || (fabs(k1) < EPSILON && k2 != 0) || (fabs(k2) < EPSILON && k1 != 0))
        return 1;
    else
        return 0;
}

int intersection(struct segment l1, struct segment l2, point* p) {
    if (!intersect(l1, l2))
        return 0;
    double d;
    d = (l1.p2.y - l1.p1.y) * (l2.p2.x - l2.p1.x) - (l2.p2.y - l2.p1.y) * (l1.p2.x - l1.p1.x);
    p->x = ((l1.p2.x - l1.p1.x) * (l2.p2.x - l2.p1.x) * (l2.p1.y - l1.p1.y) + (l1.p2.y - l1.p1.y) * (l2.p2.x - l2.p1.x) * l1.p1.x - (l2.p2.y - l2.p1.y) * (l1.p2.x - l1.p1.x) * l2.p1.x) / d;
    p->y = ((l1.p2.y - l1.p1.y) * (l2.p2.y - l2.p1.y) * (l2.p1.x - l1.p1.x) + (l1.p2.x - l1.p1.x) * (l2.p2.y - l2.p1.y) * l1.p1.y - (l2.p2.x - l2.p1.x) * (l1.p2.y - l1.p1.y) * l2.p1.y) / (-1 * d);
    if (fabs(p->x) < EPSILON)
        p->x = 0;
    if (fabs(p->y) < EPSILON)
        p->y = 0;
    if (p->x >= min(l1.p1.x, l1.p2.x) && p->x <= max(l1.p1.x, l1.p2.x) && p->x >= min(l2.p1.x, l2.p2.x) && p->x <= max(l2.p1.x, l2.p2.x))
        if (p->y >= min(l1.p1.y, l1.p2.y) && p->y <= max(l1.p1.y, l1.p2.y) && p->y >= min(l2.p1.y, l2.p2.y) && p->y <= max(l2.p1.y, l2.p2.y))
            return 1;
    return 0;
}

typedef struct hashtable {
    segment line;
    struct hashtable* next;
} hashtable;

typedef struct hashset {
    hashtable** bucket;
    int size;
    int count;
} hashset;

int hash1(int size, segment l) {
    long long x = (long long)(l.p1.x * 727);
    long long y = (long long)(l.p1.y * 177);
    return (int)((x * 31 + y * 7) % size);
}

hashset* createsegmentset(int size) {
    hashset* segmentset = (hashset*)malloc(sizeof(hashset));
    if (!segmentset)
        return NULL;
    segmentset->size = size;
    segmentset->count = 0;
    segmentset->bucket = (hashtable**)calloc(size, sizeof(hashtable*));
    if (!segmentset->bucket) {
        free(segmentset);
        return NULL;
    }
    return segmentset;
}

int pointequal(point p1, point p2) {
    return (fabs(p1.x - p2.x) < EPSILON) && (fabs(p1.y - p2.y) < EPSILON);
}

int segmentequal(segment l1, segment l2) {
    return (pointequal(l1.p1, l2.p1) && pointequal(l1.p2, l2.p2));
}

int compare(point p1, point p2) {
    if ((fabs(p1.x - p2.x) > EPSILON)) {
        if (p1.x > p2.x)
            return 1;
        else
            return -1;
    } else {
        if (p1.y > p2.y)
            return 1;
        else
            return -1;
    }
}

int segmentcompare(segment l1, segment l2) {
    double x1 = l1.p2.x - l1.p1.x;
    double x2 = l2.p2.x - l2.p1.x;
    double y1 = l1.p2.y - l1.p1.y;
    double y2 = l2.p2.y - l2.p1.y;
    double tan1 = atan2(y1, x1), tan2 = atan2(y2, x2);
    if (tan1 < 0)
        tan1 += 2 * M_PI;
    if (tan2 < 0)
        tan2 += 2 * M_PI;
    if (tan1 > tan2)
        return 1;
    else
        return -1;
}

int insertsegment(hashset* pset, segment l) {
    if (!pset)
        return -1;
    int index = hash1(pset->size, l);
    hashtable* current = pset->bucket[index];
    while (current) {
        if (segmentequal(current->line, l))
            return 0;
        current = current->next;
    }
    hashtable* newsegment = (hashtable*)malloc(sizeof(hashtable));
    if (!newsegment)
        return -1;
    newsegment->line = l;
    newsegment->line.flag = 1;
    newsegment->next = pset->bucket[index];
    pset->bucket[index] = newsegment;
    pset->count++;
    return 1;
}

int deletesegment(hashset* pset, segment l) {
    if (!pset || pset->count == 0)
        return -1;
    int index = hash1(pset->size, l);
    hashtable* current = pset->bucket[index];
    while (current) {
        if (segmentequal(current->line, l)) {
            current->line.flag = 0;
            pset->count--;
            return 1;
        }
        current = current->next;
    }
    return 0;
}

hashtable* findsegment(hashset* pset, segment l) {
    if (!pset)
        return NULL;
    int index = hash1(pset->size, l);
    hashtable* current = pset->bucket[index];
    while (current) {
        if (segmentequal(l, current->line))
            return current;
        current = current->next;
    }
    return NULL;
}

void destroyset(hashset* pset) {
    if (!pset)
        return;
    for (int i = 0; i < pset->size; i++) {
        hashtable* current = pset->bucket[i];
        while (current) {
            hashtable* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(pset->bucket);
    free(pset);
}

typedef struct nearpoint {
    point p;
    struct nearpoint* next;
} nearpoint;

typedef struct Graph {
    int pointcount;
    nearpoint** nearlist;
} Graph;

Graph* creategraph(int n) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph)
        return NULL;
    graph->pointcount = 0;
    graph->nearlist = (nearpoint**)calloc(n, sizeof(nearpoint*));
    if (!graph->nearlist) {
        free(graph);
        return NULL;
    }
    return graph;
}

int insertpoint(point p, Graph* graph, int n) {
    nearpoint* current = graph->nearlist[n];
    while (current) {
        if (pointequal(current->p, p))
            return 0;
        current = current->next;
    }
    nearpoint* newpoint = (nearpoint*)malloc(sizeof(nearpoint));
    if (!newpoint)
        return -1;
    newpoint->p = p;
    newpoint->next = graph->nearlist[n];
    graph->nearlist[n] = newpoint;
    graph->pointcount++;
    return 1;
}

nearpoint* merge(nearpoint* left, nearpoint* right) {
    nearpoint dummy;
    nearpoint* tail = &dummy;
    while (left && right) {
        if (compare(left->p, right->p) > 0) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }
    tail->next = left ? left : right;
    return dummy.next;
}

hashtable* segmentmerge(hashtable* left, hashtable* right) {
    hashtable dummy;
    hashtable* tail = &dummy;
    while (left && right) {
        if (segmentcompare(left->line, right->line) < 0) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }
    tail->next = left ? left : right;
    return dummy.next;
}

nearpoint* sort(nearpoint* head) {
    if (!head || !head->next)
        return head;
    nearpoint* slow = head;
    nearpoint* fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    nearpoint* mid = slow->next;
    slow->next = NULL;
    nearpoint* left = sort(head);
    nearpoint* right = sort(mid);
    return merge(left, right);
}

hashtable* segmentsort(hashtable* lines) {
    if (!lines || !lines->next)
        return lines;
    hashtable* slow = lines;
    hashtable* fast = lines->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    hashtable* mid = slow->next;
    slow->next = NULL;
    hashtable* left = segmentsort(lines);
    hashtable* right = segmentsort(mid);
    return segmentmerge(left, right);
}

void buildsegment(Graph* graph, int n, hashset* pset) {
    int i;
    for (i = 0; i < n; i++) {
        nearpoint* list = graph->nearlist[i];
        while (list && list->next) {
            segment l;
            l.p1 = list->p;
            l.p2 = list->next->p;
            insertsegment(pset, l);
            insertsegment(pset, antisegment(l));
            list = list->next;
        }
    }
}

void destroygraph(Graph* graph, int n) {
    if (!graph)
        return;
    for (int i = 0; i < n; i++) {
        nearpoint* current = graph->nearlist[i];
        while (current) {
            nearpoint* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(graph->nearlist);
    free(graph);
}

int headtaillink(hashset* pset, Graph* closegraph, int i) {
    //int qswl = 0;
    int count = 0;
    int s = 0;
    while (1) {
        while (1) {
            hashtable* origin = pset->bucket[s];
            while (origin && !origin->line.flag) {
                if (!origin)
                    break;
                origin = origin->next;
            }
            if (!origin)
                break;
            segment l = origin->line;
            point head;
            head = l.p1;
            insertpoint(l.p1, closegraph, i);
            deletesegment(pset, l);
            count++;
            while (1) {
                //int end = 0;
                int index = hash1(pset->size, antisegment(l));
                hashtable* current = pset->bucket[index];
                while (1) {
                    if (segmentequal(current->line, antisegment(l)))
                        break;
                    current = current->next;
                }
                while (1) {
                    if (current->next)
                        current = current->next;
                    else
                        current = pset->bucket[index];
                    if (current->line.flag && pointequal(current->line.p1, antisegment(l).p1))
                        break;
                }
                l = current->line;
                deletesegment(pset, l);
                count++;
                insertpoint(l.p1, closegraph, i);
                if (pointequal(head, l.p2))
                    return count;
            }
            if (pointequal(head, l.p2))
                return count;
        }
        s++;
    }
    return count;
}

int buildclosegraph(Graph* closegraph, hashset* pset) {
    int i = 0;
    int s = 0;
    int pointnum = closegraph->pointcount;
    int segmentnum = pset->count;
    while (1) {
        int n = headtaillink(pset, closegraph, i);
        s += n;
        closegraph->pointcount = pointnum;
        i++;
        if (s == segmentnum) {
            if (i == 2 && n == s / 2)
                i--;
            break;
        }
    }
    return i;
}

void graphtreat(nearpoint* nearlist) {
    nearpoint* current = nearlist;
    while (current) {
        nearpoint* endpoint = current->next;
        while (endpoint->next && onsegment(current->p, endpoint->p, endpoint->next->p)) {
            endpoint = endpoint->next;
        }
        current->next = endpoint;
        current = endpoint;
        if (!current->next) {
            if (onsegment(current->p, nearlist->p, nearlist->next->p))
                nearlist = nearlist->next;
            break;
        }
    }
}

int deleterrorgraph(Graph* graph, int n, int a[]) {
    int i;// j, max;
    int pointnum = 0;
    int* includepoint = (int*)calloc(n, sizeof(int));
    for (i = 0; i < n; i++) {
        nearpoint* current = graph->nearlist[i];
        while (current) {
            if (insertpoint(current->p, graph, n + 1) == 1)
                pointnum++;
            current = current->next;
        }
    }
    for (i = 0; i < n; i++) {
        nearpoint* include = graph->nearlist[n + 1];
        segment shoot;
        while (include) {
            point p = {10000000.0, include->p.y};
            shoot.p1 = include->p;
            shoot.p2 = p;
            nearpoint* ponseg = graph->nearlist[i];
            int onseg = 0;
            while (ponseg) {
                if (pointequal(ponseg->p, include->p)) {
                    onseg = 1;
                    break;
                }
                ponseg = ponseg->next;
            }
            ponseg = graph->nearlist[i];
            int t = 0;
            while (ponseg->next) {
                if (onseg) {
                    includepoint[i]++;
                    break;
                }
                nearpoint* endpoint = ponseg->next;
                while (endpoint->next && onsegment(ponseg->p, endpoint->p, endpoint->next->p)) {
                    endpoint = endpoint->next;
                }
                segment line = {ponseg->p, endpoint->p,1};
                if (intersection(shoot, line, &p) == 1)
                    t++;
                if (!endpoint->next) {
                    ponseg = endpoint;
                    segment last = {ponseg->p, graph->nearlist[i]->p,1};
                    if (intersection(shoot, last, &p) == 1)
                        t++;
                    break;
                }
                ponseg = endpoint;
            }
            if (t % 2 == 1) {
                includepoint[i]++;
            }
            include = include->next;
        }
    }
    int graphnum = n;
    for (i = 0; i < n; i++) {
        if (includepoint[i] == pointnum) {
            a[i] = 1;
            graphnum--;
        }
    }
    free(includepoint);
    return graphnum;
}

int equalgraph(nearpoint* lista, nearpoint* listb) {
    while (lista) {
        nearpoint* b = listb;
        while (b) {
            if (pointequal(lista->p, b->p))
                break;
            if (!b->next)
                return 0;
            b = b->next;
        }
        lista = lista->next;
    }
    return 1;
}

void deletequalgraph(Graph* graph, int n, int a[], int* graphnum) {
    int i, j;
    for (i = 0; i < n - 1; i++) {
        if (a[i])
            continue;
        for (j = i + 1; j < n; j++) {
            if (a[j])
                continue;
            if (equalgraph(graph->nearlist[i], graph->nearlist[j])) {
                a[i] = 1;
                (*graphnum)--;
                break;
            }
        }
    }
}

void printgraph(Graph* graph, int n) {
    int i;
    int* error = (int*)calloc(n, sizeof(int));
    int graphnum = deleterrorgraph(graph, n, error);
    deletequalgraph(graph, n, error, &graphnum);
    if (graphnum == 0) {
        printf("第1个图\n");
        nearpoint* current = graph->nearlist[0];
        while (current) {
            printf("(%.2lf,%.2lf) ", current->p.x, current->p.y);
            current = current->next;
        }
        printf("\n");
    } else {
        printf("有%d个图\n", graphnum);
        int gr = 1;
        for (i = 0; i < n; i++) {
            if (!error[i]) {
                printf("第%d个图\n", gr++);
                graphtreat(graph->nearlist[i]);
                nearpoint* current = graph->nearlist[i];
                while (current) {
                    printf("(%.2lf,%.2lf) ", current->p.x, current->p.y);
                    current = current->next;
                }
                printf("\n");
            }
        }
    }
    free(error);
}

int main() {
    cout<<"有几条线段？"<<endl;
    int n;
    scanf("%d", &n);
    segment* s = (segment*)malloc(n * sizeof(segment));
    int i, j;
    for (i = 0; i < n; i++) {
        scanf("%lf%*[, ]%lf%*[, ]%lf%*[, ]%lf", &s[i].p1.x, &s[i].p1.y, &s[i].p2.x, &s[i].p2.y);
    }
    Graph* graph = creategraph(n);
    for (i = 0; i < n - 1; i++)
        for (j = i + 1; j < n; j++) {
            point temppoint;
            if (intersection(s[i], s[j], &temppoint)) {
                insertpoint(temppoint, graph, i);
                insertpoint(temppoint, graph, j);
            }
        }
    for (i = 0; i < n; i++) {
        graph->nearlist[i] = sort(graph->nearlist[i]);
    }
    int pointnum = graph->pointcount / 2;
    int size = n * 1.5;
    hashset* segmentset = createsegmentset(size);
    buildsegment(graph, n, segmentset);
    for (i = 0; i < size; i++)
        segmentset->bucket[i] = segmentsort(segmentset->bucket[i]);
    i = pointnum;
    destroygraph(graph, n);
    i = (i - 1) * (i - 2) / 2;
    Graph* closegraph = creategraph(i);
    closegraph->pointcount = pointnum;
    //int num = i;
    n = buildclosegraph(closegraph, segmentset);
    destroyset(segmentset);
    printgraph(closegraph, n);
    destroygraph(closegraph, i);
    free(s);
    return 0;
}
