#include <bits/stdc++.h>
using namespace std;

struct Slot { int id, day, period; };
struct Room { int roomId, capacity; string roomName; };

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

struct Variable { int id, lessonIdx, idx; };

struct Solver {
    vector<Slot>& allSlots;
    vector<Lesson>& lessons;
    vector<Room> rooms;

    vector<Variable> vars;

    vector<vector<char>> allowedSlot;
    vector<vector<char>> allowedRoom;
    vector<int> slotOf;
    vector<int> roomOf;

    vector<vector<int>> teacherBusy;
    vector<vector<int>> groupBusy;
    vector<vector<int>> roomBusy;

    int numSlots, numTeachers, numGroups, numRooms;

    vector<int> bestAssign, bestAssignRooms;
    int bestCost = INT_MAX;
    const int W_TEACH = 1;
    const int W_GROUP = 1;
    const int W_COLL  = 1;
    const int W_ROOM  = 1;
    const int W_DISALLOWED = 1000;


    mt19937 rng{random_device{}()};
    bool verify_and_report(ostream& os = cerr) const {

        for (int v = 0; v < (int)vars.size(); ++v) {
            if (slotOf[v] < 0 || roomOf[v] < 0) {
                os << "[ERR] v=" << v << " nieprzypisany\n";
                return false;
            }
        }
        vector<vector<int>> tBusy(numSlots, vector<int>(numTeachers, 0));
        vector<vector<int>> gBusy(numSlots, vector<int>(numGroups, 0));
        vector<vector<int>> rBusy(numSlots, vector<int>(numRooms, 0));

        for (int v = 0; v < (int)vars.size(); ++v) {
            int s = slotOf[v], r = roomOf[v];
            const Lesson& L = lessons[vars[v].lessonIdx];

            if (!allowedSlot[v][s]) { os << "[ERR] v="<<v<<" w niedozwolonym slocie "<<s<<"\n"; return false; }
            if (!allowedRoom[v][r]) { os << "[ERR] v="<<v<<" w niedozwolonej sali "<<r<<"\n"; return false; }

            if (++tBusy[s][L.teacher] > 1) { os << "[ERR] konflikt nauczyciela T"<<L.teacher<<" w slocie "<<s<<"\n"; return false; }
            if (++gBusy[s][L.group]   > 1) { os << "[ERR] konflikt grupy G"<<L.group<<" w slocie "<<s<<"\n"; return false; }
            if (++rBusy[s][r]         > 1) { os << "[ERR] konflikt sali R"<<r<<" w slocie "<<s<<"\n"; return false; }
        }
        for (int v = 0; v < (int)vars.size(); ++v) {
            int s = slotOf[v];
            const Lesson& L = lessons[vars[v].lessonIdx];
            for (int g : L.colidingGroups) {
                if (gBusy[s][g] > 0) {
                    os << "[ERR] kolizja kolidujących grup: G"<<L.group<<" vs G"<<g<<" w slocie "<<s<<"\n";
                    return false;
                }
            }
        }

        os << "[OK] Plan spełnia wszystkie twarde ograniczenia.\n";
        return true;
    }

    Solver(vector<Slot>& slots, vector<Lesson>& les, vector<Room> rms,
           int numGroups_, int numTeachers_)
      : allSlots(slots), lessons(les), rooms(move(rms)), numSlots(allSlots.size()), numGroups(numGroups_), numTeachers(numTeachers_) {

        numSlots = allSlots.size();
        numRooms = rooms.size();
        int cur = 0;
        for (int l = 0; l < lessons.size(); ++l) {
            for (int i = 0; i < lessons[l].hours; ++i) {
                vars.push_back({cur++, l, i});
            }
        }

        allowedSlot.assign(vars.size(), vector<char>(numSlots, 0));
        allowedRoom.assign(vars.size(), vector<char>(numRooms, 0));
        for (int v = 0; v < vars.size(); ++v) {
            const Lesson& L = lessons[vars[v].lessonIdx];
            for (int s : L.possibleSlots) {
                allowedSlot[v][s] = 1;
            }
            for (int r : L.possibleRooms) {
                allowedRoom[v][r] = 1;
            }
        }

        teacherBusy.assign(numSlots, vector<int>(numTeachers, 0));
        groupBusy.assign(numSlots, vector<int>(numGroups , 0));
        roomBusy.assign(numSlots, vector<int>(numRooms  , 0));

        slotOf.assign(vars.size(), -1);
        roomOf.assign(vars.size(), -1);
        bestAssign = slotOf;
        bestAssignRooms = roomOf;
    }

    int varCostNoSelf(int v, int s, int r) {
        const Lesson& L = lessons[vars[v].lessonIdx];

        int cost = 0;
        cost+=!allowedSlot[v][s] ? W_DISALLOWED : 0;
        cost+=!allowedRoom[v][r] ? W_DISALLOWED : 0;
        cost+=teacherBusy[s][L.teacher] > 0 ? W_TEACH : 0;
        cost+=groupBusy[s][L.group] > 0 ? W_GROUP : 0;
        cost+=roomBusy[s][r] > 0 ? W_ROOM : 0;
        for (int g : L.colidingGroups) {
            if (groupBusy[s][g] > 0) {
                cost += W_COLL;
                break;
            }
        }

        return cost;
    }

    int varCostRemovedSelf(int v, int s, int r) const {
        const Lesson& L = lessons[vars[v].lessonIdx];
        int cost=0;

        int curTeacherBusy=teacherBusy[s][L.teacher] - (slotOf[v]==s);
        int curGroupBusy=groupBusy[s][L.group] - (slotOf[v]==s);
        int curRoomBusy=roomBusy[s][r] - (slotOf[v]==s and roomOf[v]==r);

        cost+=!allowedSlot[v][s] ? W_DISALLOWED : 0;
        cost+=!allowedRoom[v][r] ? W_DISALLOWED : 0;
        cost+=curGroupBusy>0 ? W_GROUP : 0;
        cost+=curTeacherBusy>0 ? W_TEACH : 0;
        cost+=curRoomBusy>0 ? W_ROOM : 0;

        for (int g : L.colidingGroups) {
            if (groupBusy[s][g] > 0) {
                cost += W_COLL;
                break;
            }
        }
        return cost;

    }


    void buildInitial() {
        for (int s = 0; s < numSlots; s++) {
            fill(teacherBusy[s].begin(), teacherBusy[s].end(), 0);
            fill(groupBusy[s].begin(),   groupBusy[s].end(),   0);
            fill(roomBusy[s].begin(),    roomBusy[s].end(),    0);
        }
        fill(slotOf.begin(), slotOf.end(), -1);
        fill(roomOf.begin(), roomOf.end(), -1);
        vector<int> order(vars.size());
        iota(order.begin(), order.end(), 0);
        shuffle(order.begin(), order.end(), rng);


        for (int v : order) {
            const Lesson& L = lessons[vars[v].lessonIdx];
            int bestC=INT_MAX;
            int bestS = uniform_int_distribution<int>(0, numSlots-1)(rng);
            int bestR = uniform_int_distribution<int>(0, numRooms-1)(rng);

            vector<int> candS = L.possibleSlots;
            vector<int> candR = L.possibleRooms;
            shuffle(candS.begin(), candS.end(), rng);
            shuffle(candR.begin(), candR.end(), rng);
            for (int s : candS) {
                for (int r : candR) {
                    int cur=varCostNoSelf(v,s,r);
                    if (cur<bestC) {
                        bestC = cur;
                        bestR = r;
                        bestS = s;
                        if (bestC==0) {
                            break;
                        }
                    }
                }
                if (bestC==0) {
                    break;
                }
            }
            slotOf[v]=bestS;
            roomOf[v]=bestR;
            teacherBusy[bestS][L.teacher]++;
            groupBusy[bestS][L.group]++;
            roomBusy[bestS][bestR]++;

        }
        bestAssign = slotOf;
        bestAssignRooms = roomOf;
        bestCost = totalCost();
    }

    int totalCost() {
        int sum = 0;
        for (int v = 0; v <vars.size(); ++v) {
            sum += varCostRemovedSelf(v, slotOf[v], roomOf[v]);
        }
        return sum;
    }

    int deltaMove(int v, int ns, int nr)  {
        int s0 = slotOf[v], r0 = roomOf[v];

        int before = varCostRemovedSelf(v, s0, r0);
        int after  = varCostRemovedSelf(v, ns, nr);
        return after - before;
    }

    inline void applyMove(int v, int ns, int nr) {
        int s0 = slotOf[v], r0 = roomOf[v];
        const Lesson& L = lessons[vars[v].lessonIdx];
        teacherBusy[s0][L.teacher]--; groupBusy[s0][L.group]--; roomBusy[s0][r0]--;
        teacherBusy[ns][L.teacher]++; groupBusy[ns][L.group]++; roomBusy[ns][nr]++;
        slotOf[v] = ns; roomOf[v] = nr;
    }

    vector<int> orderRooms(int vid, int s) {
        vector<pair<int,int>> vals;
        for (int r = 0; r < numRooms; ++r) {
            if (!allowedRoom[vid][r]) continue;
            int sc = roomBusy[s][r];
            vals.push_back({sc, r});
        }
        sort(vals.begin(), vals.end());
        vector<int> out;
        out.reserve(vals.size());
        for (auto &p : vals) out.push_back(p.second);
        return out;
    }

    vector<int> orderValues(int vid) {
        vector<pair<int,int>> vals;
        const Lesson& L = lessons[vars[vid].lessonIdx];

        for (int s : L.possibleSlots) {
            int val=0;
            if (teacherBusy[s][L.teacher] > 0) val += 3;
            if (groupBusy[s][L.group] > 0) val += 3;
            for (int g : L.colidingGroups) {
                if (groupBusy[s][g] > 0) {
                    val += 3;
                    break;
                }
            }

            bool anyFree = 0;
            for (int r : L.possibleRooms) if (roomBusy[s][r]==0) {
                anyFree=1;
                break;
            }
            if (!anyFree) val += 1;
            vals.push_back({val, s});

        }
        sort(vals.begin(), vals.end());
        vector<int> out; out.reserve(vals.size());
        for (auto &p : vals) out.push_back(p.second);
        return out;
    }

    int pickVarBiased() {
        int pick = uniform_int_distribution<int>(0, vars.size()-1)(rng);
        int bestv = pick;
        int bestc = -1;
        for (int t = 0; t < 16; t++) {
            int v = uniform_int_distribution<int>(0, vars.size()-1)(rng);
            int c = varCostRemovedSelf(v, slotOf[v], roomOf[v]);
            if (c > bestc){
                bestc = c;
                bestv = v;
            }
        }
        return bestv;
    }
    void sa(int maxIters = 400000, double T0 = 5.0, double alpha = 0.9995) {
        int curCost = totalCost();
        bestCost = curCost;
        bestAssign = slotOf;
        bestAssignRooms = roomOf;

        uniform_real_distribution<double> U(0.0, 1.0);

        double T = T0;
        for (int it = 0; it < maxIters; it++) {
            int v = pickVarBiased();

            int s0 = slotOf[v];
            int r0 = roomOf[v];
            int ns = s0;
            int nr = r0;

            double z = U(rng);
            if (z < 0.5) {
                auto ordS = orderValues(v);
                ns = ordS[ uniform_int_distribution<int>(0, (int)ordS.size()-1)(rng) ];
                auto ordR = orderRooms(v, ns);
                nr = ordR[ uniform_int_distribution<int>(0, (int)ordR.size()-1)(rng) ];
            } else if (z < 0.8) {
                auto ordR = orderRooms(v, s0);
                nr = ordR[ uniform_int_distribution<int>(0, (int)ordR.size()-1)(rng) ];
            } else {
                auto ordS = orderValues(v);
                ns = ordS[ uniform_int_distribution<int>(0, (int)ordS.size()-1)(rng) ];
                auto ordR = orderRooms(v, ns);
                nr = ordR[ uniform_int_distribution<int>(0, (int)ordR.size()-1)(rng) ];
            }

            int d = deltaMove(v, ns, nr);
            if (d <= 0 || U(rng) < exp(-d / max(1e-9, T))) {
                applyMove(v, ns, nr);
                curCost += d;
                if (curCost < bestCost) {
                    bestCost = curCost;
                    bestAssign = slotOf;
                    bestAssignRooms = roomOf;
                    if (bestCost == 0) break;
                }
            }

            T *= alpha;
        }

        slotOf = bestAssign;
        roomOf = bestAssignRooms;
        for (int s = 0; s < numSlots; ++s) {
            fill(teacherBusy[s].begin(), teacherBusy[s].end(), 0);
            fill(groupBusy[s].begin(),   groupBusy[s].end(),   0);
            fill(roomBusy[s].begin(),    roomBusy[s].end(),    0);
        }
        for (int v = 0; v < (int)vars.size(); ++v) {
            const Lesson& L = lessons[vars[v].lessonIdx];
            teacherBusy[slotOf[v]][L.teacher]++;
            groupBusy  [slotOf[v]][L.group]++;
            roomBusy   [slotOf[v]][roomOf[v]]++;
        }
    }
};

/// ====== GENERATOR "NA STYK" ======
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const int DAYS = 5, PERIODS = 5; // 25 slotów/tydzień na salę
    vector<Slot> slots;
    slots.reserve(DAYS * PERIODS);
    for (int d = 0; d < DAYS; ++d)
        for (int p = 0; p < PERIODS; ++p)
            slots.push_back({ (int)slots.size(), d, p });

    // Sale - dobrane tak, by pojemność == popyt
    vector<Room> rooms;
    auto addRooms = [&](int cnt, int cap, string base) {
        for (int i = 0; i < cnt; ++i)
            rooms.push_back({ (int)rooms.size(), cap, base + to_string(i) });
    };
    addRooms(5,  30, "Math-"); // 5 * 25 = 125
    addRooms(10, 28, "Gen-");  // 10 * 25 = 250
    addRooms(5,  22, "Lab-");  // 5 * 25 = 125
    addRooms(3,  36, "Gym-");  // 3 * 25 = 75
    addRooms(6,  24, "Lang-"); // 6 * 25 = 150

    const int CLASSES = 25; // kluczowe dla "na styk"
    vector<string> groupName;
    groupName.reserve(CLASSES * 3);
    for (int c = 0; c < CLASSES; ++c) {
        string cname = "C" + to_string(c + 1);
        groupName.push_back(cname);           // FULL
        groupName.push_back(cname + "_G1");   // G1
        groupName.push_back(cname + "_G2");   // G2
    }
    int numGroups = (int)groupName.size();

    // Nauczyciele: dla prostoty każdy przedmiot ma własną pulę po 25 osób (po 1 na klasę)
    const int TEACHERS = 12 * CLASSES; // 12 "przedmiotów" wg poniższego przypisania
    vector<string> teacherName(TEACHERS);
    for (int t = 0; t < TEACHERS; ++t) teacherName[t] = "T" + to_string(t);

    // Wygodne indeksowanie: dla każdego przedmiotu zakres długości CLASSES
    auto Tmath = [&](int c){ return 0*CLASSES + c; };
    auto Tpol  = [&](int c){ return 1*CLASSES + c; };
    auto Thist = [&](int c){ return 2*CLASSES + c; };
    auto Tphys = [&](int c){ return 3*CLASSES + c; };
    auto Tbio  = [&](int c){ return 4*CLASSES + c; };
    auto Tpe   = [&](int c){ return 5*CLASSES + c; };
    auto Ten1  = [&](int c){ return 6*CLASSES + c; };
    auto Ten2  = [&](int c){ return 7*CLASSES + c; };
    auto Tict  = [&](int c){ return 8*CLASSES + c; };
    auto Tgeo  = [&](int c){ return 9*CLASSES + c; };
    auto Tmus  = [&](int c){ return 10*CLASSES + c; };
    auto Tart  = [&](int c){ return 11*CLASSES + c; };

    // Domeny slotów – pełne (żeby pojemność zgadzała się z popytem)
    vector<int> ALL_SLOTS(slots.size());
    iota(ALL_SLOTS.begin(), ALL_SLOTS.end(), 0);

    // Zbiory sal po typach (indeksy roomId)
    vector<int> MATH_ROOMS, LANG_ROOMS, LAB_ROOMS, GYM_ROOMS, GEN_ROOMS;
    for (auto &r : rooms) {
        if (r.roomName.rfind("Math-", 0) == 0) MATH_ROOMS.push_back(r.roomId);
        else if (r.roomName.rfind("Lang-", 0) == 0) LANG_ROOMS.push_back(r.roomId);
        else if (r.roomName.rfind("Lab-", 0) == 0)  LAB_ROOMS.push_back(r.roomId);
        else if (r.roomName.rfind("Gym-", 0) == 0)  GYM_ROOMS.push_back(r.roomId);
        else GEN_ROOMS.push_back(r.roomId);
    }

    // Lekcje: sumy godzin dobrane tak, by łączne zapotrzebowanie == łączna pojemność
    vector<Lesson> lessons; lessons.reserve(CLASSES * 30);
    auto addLesson = [&](int id, int group, vector<int> colidingGroups,
                         int teacher, string subject, int hours,
                         const vector<int>& slotsIdx, const vector<int>& roomsIdx)
    {
        lessons.push_back({id, group, colidingGroups, teacher, subject, hours,
                           slotsIdx, roomsIdx});
    };

    int nextId = 0;
    for (int c = 0; c < CLASSES; ++c) {
        int FULL = 3*c, G1 = FULL+1, G2 = FULL+2;
        vector<int> CF = {G1, G2}; // FULL koliduje z podgrupami
        vector<int> CG = {FULL};   // każda podgrupa koliduje z FULL

        // Math rooms
        addLesson(nextId++, FULL, CF, Tmath(c), "Matematyka", 5, ALL_SLOTS, MATH_ROOMS);

        // General rooms
        addLesson(nextId++, FULL, CF, Tpol(c),  "Polski",     4, ALL_SLOTS, GEN_ROOMS);
        addLesson(nextId++, FULL, CF, Thist(c), "Historia",   2, ALL_SLOTS, GEN_ROOMS);
        addLesson(nextId++, FULL, CF, Tgeo(c),  "Geografia",  2, ALL_SLOTS, GEN_ROOMS);
        addLesson(nextId++, FULL, CF, Tmus(c),  "Muzyka",     1, ALL_SLOTS, GEN_ROOMS);
        addLesson(nextId++, FULL, CF, Tart(c),  "Plastyka",   1, ALL_SLOTS, GEN_ROOMS);

        // Lab rooms
        addLesson(nextId++, FULL, CF, Tphys(c), "Fizyka",     2, ALL_SLOTS, LAB_ROOMS);
        addLesson(nextId++, FULL, CF, Tbio(c),  "Biologia",   2, ALL_SLOTS, LAB_ROOMS);
        addLesson(nextId++, FULL, CF, Tict(c),  "Informatyka",1, ALL_SLOTS, LAB_ROOMS);

        // Gym rooms
        addLesson(nextId++, FULL, CF, Tpe(c),   "WF",         3, ALL_SLOTS, GYM_ROOMS);

        // Language rooms (podział na G1/G2)
        addLesson(nextId++, G1,   CG,  Ten1(c), "Angielski G1", 3, ALL_SLOTS, LANG_ROOMS);
        addLesson(nextId++, G2,   CG,  Ten2(c), "Angielski G2", 3, ALL_SLOTS, LANG_ROOMS);
    }

    Solver solver(slots, lessons, rooms,
                  /*numGroups=*/(int)groupName.size(),
                  /*numTeachers=*/(int)teacherName.size());

    solver.buildInitial();
    solver.sa(1200000, 2.5, 0.99995);


    // Wyświetl siatkę
    vector<vector<string>> timetable(slots.size());
    for (int v = 0; v < (int)solver.vars.size(); ++v) {
        int s = solver.bestAssign[v];
        int r = solver.bestAssignRooms[v];
        const Lesson& L = solver.lessons[ solver.vars[v].lessonIdx ];
        string entry = "G" + to_string(L.group) + " " + L.subject +
                       " (T" + to_string(L.teacher) + "), " +
                       solver.rooms[r].roomName;
        timetable[s].push_back(entry);
    }
    for (auto& s : slots) {
        cout << "Dzien " << s.day << " | Lekcja " << s.period << " : ";
        if (timetable[s.id].empty()) { cout << "-\n"; continue; }
        for (int i = 0; i < (int)timetable[s.id].size(); ++i) {
            if (i) cout << " | ";
            cout << timetable[s.id][i];
        }
        cout << "\n";
    }
    cerr << "Koszt koncowy: " << solver.bestCost << "\n";
    return 0;
}
