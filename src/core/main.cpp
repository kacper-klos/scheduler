#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>
using namespace std;

const int MAXN = 64;

struct Slot {
    int id, day, period;
};

struct Room {
    int roomId, capacity;
    string roomName;
};

struct Lesson {
    int id;
    int group;
    vector<int> colidingGroups;
    int teacher;
    string subject;
    int hours;
    vector<int> possibleSlots;
    vector<int> possibleRooms;
};

struct Variable {
    int id, lessonIdx, idx;
};

struct Solver {
    vector<Slot>& allSlots;
    vector<Lesson>& lessons;
    vector<Variable> vars;
    vector<bitset<MAXN>> groupBusy;
    vector<bitset<MAXN>> teacherBusy;
    vector<bitset<MAXN>> roomBusy;
    vector<vector<int>> domain;
    vector<vector<int>> roomDomain;
    vector<Room> rooms;
    vector<int> bestAssign;
    vector<int> bestAssignRooms;
    vector<int> degree;
    vector<vector<int>> graph;

    Solver(vector<Slot>& slots, vector<Lesson>& lessons, vector<Room>& rms, int numGroups, int numTeachers)
        : allSlots(slots), lessons(lessons), rooms(rms) {
        int curid = 1;
        for (int l = 0; l < lessons.size(); l++) {
            for (int i = 0; i < lessons[l].hours; i++) {
                vars.push_back({curid++, l, i});
            }
        }
        int S = slots.size();
        groupBusy.assign(S, bitset<MAXN>());
        teacherBusy.assign(S, bitset<MAXN>());
        roomBusy.assign(S, bitset<MAXN>());
        domain.resize(vars.size());
        roomDomain.resize(vars.size());
        for (int i = 0; i < vars.size(); i++) {
            domain[i] = lessons[vars[i].lessonIdx].possibleSlots;
            roomDomain[i] = lessons[vars[i].lessonIdx].possibleRooms;
            sort(roomDomain[i].begin(), roomDomain[i].end());
            sort(domain[i].begin(), domain[i].end());
        }
        degree.assign(vars.size(), 0);
        graph.resize(vars.size());
        for (int i = 0; i < vars.size(); i++) {
            Lesson& L1 = lessons[vars[i].lessonIdx];
            for (int j = i + 1; j < vars.size(); j++) {
                Lesson& L2 = lessons[vars[j].lessonIdx];
                if (L1.teacher == L2.teacher or L1.group == L2.group or find(L1.colidingGroups.begin(), L1.colidingGroups.end(), L2.group) != L1.colidingGroups.end()) {
                    degree[i]++;
                    degree[j]++;
                    graph[i].push_back(j);
                    graph[j].push_back(i);
                }
            }
        }
    }

    vector<int> orderValues(int vid, const vector<int>& asg) {
        vector<pair<int, int>> vals;
        const Lesson& L = lessons[vars[vid].lessonIdx];
        for (int s : domain[vid]) {
            if (teacherBusy[s][L.teacher] || groupBusy[s][L.group]) {
                continue;
            }
            bool colide = false;
            for (int g : L.colidingGroups) {
                if (groupBusy[s][g]) {
                    colide = true;
                    break;
                }
            }
            if (colide) continue;
            int score = 0;
            for (int nb : graph[vid]) {
                if (asg[nb] != -1) continue;
                if (binary_search(domain[nb].begin(), domain[nb].end(), s)) {
                    score++;
                }
            }
            int freeRooms = 0;
            for (int r : roomDomain[vid]) {
                if (!roomBusy[s][r]) {
                    freeRooms++;
                    if (freeRooms > 1) break;
                }
            }
            if (freeRooms <= 1) score += 1;
            vals.push_back({score, s});
        }
        sort(vals.begin(), vals.end());
        vector<int> out;
        out.reserve(vals.size());
        for (auto& p : vals) out.push_back(p.second);
        return out;
    }

    vector<int> orderRooms(int vid, int s, const vector<int>& asg) {
        vector<pair<int, int>> vals;
        for (int r : roomDomain[vid]) {
            if (roomBusy[s][r]) continue;
            int score = 0;
            for (int nb : graph[vid]) {
                if (asg[nb] != -1) continue;
                if (!binary_search(domain[nb].begin(), domain[nb].end(), s)) continue;
                if (binary_search(roomDomain[nb].begin(), roomDomain[nb].end(), r)) {
                    score++;
                }
            }
            vals.push_back({score, r});
        }
        sort(vals.begin(), vals.end());
        vector<int> out;
        out.reserve(vals.size());
        for (auto& p : vals) out.push_back(p.second);
        return out;
    }

    bool canAssign(int vidx, int s, const vector<int>& asg) {
        const Lesson& L = lessons[vars[vidx].lessonIdx];
        if (!binary_search(domain[vidx].begin(), domain[vidx].end(), s) or teacherBusy[s][L.teacher] or groupBusy[s][L.group]) {
            return false;
        }
        for (int x : L.colidingGroups) {
            if (groupBusy[s][x]) {
                return false;
            }
        }
        return true;
    }

    int countSlots(int v) {
        const Lesson& L = lessons[vars[v].lessonIdx];
        int cnt = 0;
        for (int s : domain[v]) {
            if (teacherBusy[s][L.teacher] or groupBusy[s][L.group]) {
                continue;
            }
            bool colide = false;
            for (int g : L.colidingGroups) {
                if (groupBusy[s][g]) {
                    colide = true;
                    break;
                }
            }
            if (colide) {
                continue;
            }
            for (int r : roomDomain[v]) {
                if (!roomBusy[s][r]) {
                    cnt++;
                    break;
                }
            }
        }
        return cnt;
    }

    int getVariable(const vector<int>& asg) {
        int best = -1;
        int bestCnt = 1e9;
        int bestDegree = -1;
        for (int i = 0; i < vars.size(); i++) {
            if (asg[i] != -1) {
                continue;
            }
            int cnt = countSlots(i);
            if (cnt == 0) {
                return i;
            }
            if (cnt < bestCnt or (cnt == bestCnt and degree[i] > bestDegree)) {
                best = i;
                bestCnt = cnt;
                bestDegree = degree[i];
            }
        }
        return best;
    }

    bool dfs(vector<int>& curAssigned, vector<int>& curRooms) {
        int vidx = getVariable(curAssigned);
        if (vidx == -1) {
            bestAssign = curAssigned;
            bestAssignRooms = curRooms;
            return true;
        }
        const Lesson& L = lessons[vars[vidx].lessonIdx];
        for (int s : orderValues(vidx, curAssigned)) {
            if (!canAssign(vidx, s, curAssigned)) {
                continue;
            }
            for (int r : orderRooms(vidx, s, curAssigned)) {
                if (roomBusy[s][r]) {
                    continue;
                }
                curAssigned[vidx] = s;
                curRooms[vidx] = r;
                teacherBusy[s][L.teacher] = true;
                groupBusy[s][L.group] = true;
                roomBusy[s][r] = true;
                if (dfs(curAssigned, curRooms)) {
                    return true;
                }
                curAssigned[vidx] = -1;
                curRooms[vidx] = -1;
                teacherBusy[s][L.teacher] = false;
                groupBusy[s][L.group] = false;
                roomBusy[s][r] = false;
            }
        }
        return false;
    }
};

int main() {
    const int days = 5, periods = 8;
    vector<Slot> slots;
    slots.reserve(days * periods);
    for (int d = 0; d < days; ++d) {
        for (int p = 0; p < periods; ++p) {
            slots.push_back({ (int)slots.size(), d, p });
        }
    }

    // 12 sal: 0..2 matematyczne, 3..6 językowe, 7..8 lab, 9..10 sala gimn., 11 ogólna/historia
    vector<Room> rooms = {
        {0, 30, "R101 (math)"}, {1, 28, "R102 (math)"}, {2, 26, "R103 (math)"},
        {3, 24, "R201 (lang)"}, {4, 24, "R202 (lang)"}, {5, 24, "R203 (lang)"}, {6, 24, "R204 (lang)"},
        {7, 22, "R301 (lab)"},  {8, 22, "R302 (lab)"},
        {9,  40, "G401 (gym)"}, {10, 40, "G402 (gym)"},
        {11, 26, "R501 (hist/general)"}
    };

    vector<int> ALL_SLOTS;
    ALL_SLOTS.reserve(slots.size());
    for (int i = 0; i < (int)slots.size(); ++i) ALL_SLOTS.push_back(i);

    // grupy: dla każdej klasy c: base=3*c -> [base]=pełna klasa, [base+1]=G1, [base+2]=G2
    const int classes = 10; // C1..C10
    vector<string> groupName;
    groupName.reserve(classes * 3);
    for (int c = 0; c < classes; ++c) {
        string cname = "C" + to_string(c + 1);
        groupName.push_back(cname);
        groupName.push_back(cname + "_G1");
        groupName.push_back(cname + "_G2");
    }
    int numGroups = (int)groupName.size(); // 30

    // nauczyciele 0..15 (16 osób)
    vector<string> teacherName(16);
    for (int t = 0; t < 16; ++t) teacherName[t] = "T" + to_string(t);
    int numTeachers = (int)teacherName.size();

    // zestawy sal wg „typów”
    const vector<int> MATH_ROOMS   = {0,1,2};
    const vector<int> LANG_ROOMS   = {3,4,5,6};
    const vector<int> LAB_ROOMS    = {7,8};
    const vector<int> GYM_ROOMS    = {9,10};
    const vector<int> HIST_ROOMS   = {11,3,4}; // ogólna + 2 językowe jako fallback

    vector<Lesson> lessons;
    lessons.reserve(classes * 15);

    auto addLesson = [&](int id, int group, vector<int> colidingGroups,
                         int teacher, string subject, int hours,
                         vector<int> slotsIdx, vector<int> roomsIdx)
    {
        lessons.push_back({id, group, move(colidingGroups), teacher, subject,
                           hours, move(slotsIdx), move(roomsIdx)});
    };

    int nextLessonId = 0;

    // przydział nauczycieli per przedmiot (rotacja)
    auto T_MATH = [&](int c){ return 0 + (c % 4); };      // 0..3
    auto T_EN1  = [&](int c){ return 4 + (c % 4); };      // 4..7
    auto T_EN2  = [&](int c){ return 4 + ((c+1) % 4); };  // 4..7 (przesunięcie)
    auto T_PHYS = [&](int c){ return 8 + (c % 2); };      // 8..9
    auto T_HIST = [&](int c){ return 10 + (c % 2); };     // 10..11
    auto T_PE   = [&](int c){ return 12 + (c % 2); };     // 12..13
    auto T_POL  = [&](int c){ return 14 + (c % 2); };     // 14..15

    // generujemy ~15h/klasę: Mat(4), Eng G1(2), Eng G2(2), Phys(2), Hist(2), PE(2), Polish(3)
    for (int c = 0; c < classes; ++c) {
        int base = 3 * c;
        int FULL = base;
        int G1 = base + 1;
        int G2 = base + 2;

        // pełna klasa koliduje z obiema podgrupami
        vector<int> COL_FULL = {G1, G2};
        // każda podgrupa koliduje z pełną klasą (podgrupy mogą iść równolegle)
        vector<int> COL_G    = {FULL};

        // Matematyka (4h) – pełna klasa – tylko sale MATH_ROOMS
        addLesson(nextLessonId++, FULL, COL_FULL, T_MATH(c), "Matematyka", 4, ALL_SLOTS, MATH_ROOMS);

        // Angielski (2h) – G1 i G2 – sale językowe
        addLesson(nextLessonId++, G1,   COL_G,    T_EN1(c), "Angielski G1", 2, ALL_SLOTS, LANG_ROOMS);
        addLesson(nextLessonId++, G2,   COL_G,    T_EN2(c), "Angielski G2", 2, ALL_SLOTS, LANG_ROOMS);

        // Fizyka (2h) – pełna klasa – lab
        addLesson(nextLessonId++, FULL, COL_FULL, T_PHYS(c), "Fizyka", 2, ALL_SLOTS, LAB_ROOMS);

        // Historia (2h) – pełna klasa – historia/ogólne
        addLesson(nextLessonId++, FULL, COL_FULL, T_HIST(c), "Historia", 2, ALL_SLOTS, HIST_ROOMS);

        // WF (2h) – pełna klasa – sala gimn.
        addLesson(nextLessonId++, FULL, COL_FULL, T_PE(c), "WF", 2, ALL_SLOTS, GYM_ROOMS);

        // Polski (3h) – pełna klasa – ogólne/językowe
        addLesson(nextLessonId++, FULL, COL_FULL, T_POL(c), "Polski", 3, ALL_SLOTS, HIST_ROOMS);
    }

    Solver solver(slots, lessons, rooms, numGroups, numTeachers);

    vector<int> curAssigned(solver.vars.size(), -1);
    vector<int> curRooms(solver.vars.size(), -1);

    bool ok = solver.dfs(curAssigned, curRooms);
    if (!ok) {
        cout << "Brak rozwiazania.\n";
        return 0;
    }

    vector<vector<string>> timetable(slots.size());
    for (int vid = 0; vid < (int)solver.vars.size(); ++vid) {
        int s = solver.bestAssign[vid];
        int r = solver.bestAssignRooms[vid];
        const Lesson& L = solver.lessons[solver.vars[vid].lessonIdx];
        string entry = groupName[L.group] + " " + L.subject +
                       " (" + teacherName[L.teacher] + "), " +
                       rooms[r].roomName;
        timetable[s].push_back(entry);
    }

    for (auto& s : slots) {
        cout << "Dzien " << s.day << " | Lekcja " << s.period << " : ";
        if (timetable[s.id].empty()) cout << "-";
        else {
            for (int i = 0; i < (int)timetable[s.id].size(); ++i) {
                if (i) cout << " | ";
                cout << timetable[s.id][i];
            }
        }
        cout << "\n";
    }
    return 0;
}
