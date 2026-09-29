// AHC072: complete construction, causal cargo repair and local beam until 1.9s.
// Complete incumbents persist; accepted local edits reconnect physically.
#include <bits/stdc++.h>
using namespace std;

namespace ahc {
    constexpr int INF = 10000000;
    constexpr int DR[4] = {-1, 1, 0, 0}, DC[4] = {0, 0, -1, 1};
    constexpr char DIR[4] = {'U', 'D', 'L', 'R'};
    using Clock = chrono::steady_clock;
    int sz(size_t n) { return static_cast<int>(n); }
    double elapsed(Clock::time_point start) {
        return chrono::duration<double, milli>(Clock::now() - start).count();
    }

    struct TimeExpired {};
    struct SliceExpired {};
    struct Timer {
        Clock::time_point start=Clock::now();
        double deadline=1000,slice=1e100;
        bool enabled=false;
        int best=-1;
        double ms() const { return elapsed(start); }
        void check() {
            double t=ms();
            if (enabled && t>=deadline) throw TimeExpired{};
            if (enabled && t>=slice) throw SliceExpired{};
        }
    };

    struct Grid {
        int n = 0, K = 0, V = 0, M = 0;
        vector<int> color, origin, initial, home, nest, floor;
        vector<array<array<int,9>,4>> ray;
        vector<vector<int>> dist, nearHome;
        void read() {
            if (!(cin >> n >> K)) throw runtime_error("missing_input");
            V = n*n; initial.assign(V,-1); nest.assign(V,-1); home.assign(K,-1);
            for (int r=0;r<n;++r) {
                string row; cin >> row;
                if (sz(row.size()) != n) throw runtime_error("bad_row");
                for (int c=0;c<n;++c) {
                    int p=r*n+c; char ch=row[c];
                    if (ch=='#') continue;
                    floor.push_back(p);
                    if (ch>='a' && ch<='z') {
                        initial[p]=sz(color.size()); color.push_back(ch-'a'); origin.push_back(p);
                    }
                    if (ch>='A' && ch<='Z') { home[ch-'A']=p; nest[p]=ch-'A'; }
                }
            }
            M=sz(color.size());
            for (int p:home) if (p<0) throw runtime_error("missing_nest");
        }
        void prepare(Timer& timer) {
            vector<bool> open(V,false); for (int p:floor) open[p]=true;
            ray.resize(V); for (auto& a:ray) for (auto& b:a) b.fill(-1);
            for (int p:floor) for (int d=0;d<4;++d) for (int l=1;l<=8;++l) {
                int r=p/n+DR[d]*l, c=p%n+DC[d]*l;
                if (r<0 || r>=n || c<0 || c>=n || !open[r*n+c]) break;
                ray[p][d][l]=r*n+c;
            }
            dist.assign(V,vector<int>(V,INF)); vector<int> queue(V);
            for (int root:floor) {
                timer.check(); int head=0,tail=0;
                dist[root][root]=0; queue[tail++]=root;
                while (head<tail) {
                    int p=queue[head++];
                    for (int d=0;d<4;++d) {
                        int v=ray[p][d][1];
                        if (v>=0 && dist[root][v]==INF) {
                            dist[root][v]=dist[root][p]+1; queue[tail++]=v;
                        }
                    }
                }
            }
            nearHome.resize(K);
            for (int c=0;c<K;++c) {
                nearHome[c]=floor;
                stable_sort(nearHome[c].begin(),nearHome[c].end(),[&](int a,int b) {
                    return dist[a][home[c]]<dist[b][home[c]];
                });
            }
        }
        int hd(int p,int c) const { return dist[home[c]][p]; }
        int direction(int p,int v) const {
            for (int d=0;d<4;++d) if (ray[p][d][1]==v) return d;
            throw runtime_error("non_adjacent_tree_edge");
        }
    };
    // q is retained for composing modules; k is the actual output for this replay.
    struct Op { int p, q, d, len, k; };
    struct Tower { array<int,8> ids{}; int h = 0; };
    struct Board {
        vector<Tower> cells;
        vector<int> loc;
        int remaining;
        explicit Board(const Grid& g):cells(g.V),loc(g.origin),remaining(g.M) {
            for (int id=0;id<g.M;++id) { auto& s=cells[g.origin[id]]; s.ids[s.h++]=id; }
        }
        int consume(const Grid& g,int p) {
            int count=0; auto& s=cells[p];
            while (s.h && g.nest[p]==g.color[s.ids[s.h-1]]) {
                loc[s.ids[--s.h]]=-1; --remaining; ++count;
            }
            return count;
        }
        pair<int,int> apply(const Grid& g,Op& op) {
            auto& src=cells[op.p]; int k=src.h-op.q;
            if (op.q<1 || k<0 || op.len<1 || op.len>8 || op.len>k+1)
            throw runtime_error("launch_height");
            int v=g.ray[op.p][op.d][op.len];
            if (v<0 || cells[v].h+op.q>8) throw runtime_error("landing_capacity");
            op.k=k; auto& dst=cells[v];
            for (int j=src.h-1;j>=k;--j) {
                int id=src.ids[j]; dst.ids[dst.h++]=id; loc[id]=v;
            }
            src.h=k; int a=consume(g,op.p),b=consume(g,v); return {a,b};
        }
    };
    struct Builder {
        const Grid& g; Timer& timer;
        Board board; vector<Op> ops;
        explicit Builder(const Grid& grid,Timer& t):g(grid),timer(t),board(grid) {}
        Builder(const Grid& grid,Timer& t,const Board& state):g(grid),timer(t),board(state) {}
        void emit(int p,int q,int d,int len) {
            Op op{p,q,d,len,-1}; board.apply(g,op); ops.push_back(op);
            if ((ops.size()&127)==0) timer.check();
        }
        void append(const vector<Op>& block) {
            for (const auto& op:block) emit(op.p,op.q,op.d,op.len);
        }
        void direct(int p,int q,int col) {
            while (p!=g.home[col]) {
                int d=-1;
                for (int a=0;a<4;++a) {
                    int v=g.ray[p][a][1];
                    if (v>=0 && g.hd(v,col)+1==g.hd(p,col)) { d=a; break; }
                }
                if (d<0) throw runtime_error("disconnected_floor");
                emit(p,q,d,1); p=g.ray[p][d][1];
            }
        }
    };

    // Classical subset Steiner DP. The home terminal in the finish table is never
    // physical cargo: its reconstructed tree is rooted at the home before replay.
    struct TreeBack { int arg = -1; unsigned char kind = 0; }; // seed, split, edge
    struct Steiner {
        const Grid& g;
        vector<int> terminal, cost;
        vector<TreeBack> back;
        int masks, blocked;
        Steiner(const Grid& grid,vector<int> terms,int forbidden,Timer& timer)
        :g(grid),terminal(move(terms)),masks(1<<sz(terminal.size())),blocked(forbidden) {
            long long work=0;
            cost.assign(masks*g.V,INF); back.resize(masks*g.V);
            for (int mask=1;mask<masks;++mask) {
                timer.check();
                if (popcount(static_cast<unsigned>(mask))==1) {
                    int p=terminal[countr_zero(static_cast<unsigned>(mask))];
                    if (p!=blocked) cost[mask*g.V+p]=0;
                }
                for (int sub=(mask-1)&mask;sub;sub=(sub-1)&mask) {
                    int other=mask^sub; if (sub>=other) continue;
                    for (int p:g.floor) if (p!=blocked) {
                        int value=cost[sub*g.V+p]+cost[other*g.V+p],ix=mask*g.V+p;
                        if (value<cost[ix]) { cost[ix]=value; back[ix]={sub,1}; }
                    }
                }
                using Node=pair<int,int>; priority_queue<Node,vector<Node>,greater<Node>> queue;
                for (int p:g.floor) if (cost[mask*g.V+p]<INF) queue.emplace(cost[mask*g.V+p],p);
                while (!queue.empty()) {
                    auto [value,p]=queue.top(); queue.pop(); if (value!=cost[mask*g.V+p]) continue;
                    if ((++work&511)==0) timer.check();
                    for (int d=0;d<4;++d) {
                        int v=g.ray[p][d][1]; if (v<0 || v==blocked) continue;
                        int ix=mask*g.V+v;
                        if (value+1<cost[ix]) {
                            cost[ix]=value+1; back[ix]={p,2}; queue.emplace(value+1,v);
                        }
                    }
                }
            }
        }
        int value(int mask,int p) const { return mask ? cost[mask*g.V+p] : 0; }
        vector<vector<int>> tree(int mask,int p) const {
            if (value(mask,p)>=INF) throw runtime_error("missing_tree");
            vector<vector<int>> edges(g.V);
            vector<pair<int,int>> todo{{mask,p}}; vector<unsigned char> seen(cost.size(),0);
            while (!todo.empty()) {
                auto [s,v]=todo.back(); todo.pop_back();
                if (!s || seen[s*g.V+v]) continue;
                seen[s*g.V+v]=1; auto b=back[s*g.V+v];
                if (b.kind==1) { todo.emplace_back(b.arg,v); todo.emplace_back(s^b.arg,v); }
                if (b.kind==2) {
                    edges[v].push_back(b.arg); edges[b.arg].push_back(v); todo.emplace_back(s,b.arg);
                }
            }
            return edges;
        }
    };
    struct Cohort {
        int index, col, all, homeBit, base = INF;
        vector<int> ids;
        unique_ptr<Steiner> gather, finish;
        Cohort(int i,int c,vector<int> list):index(i),col(c),all((1<<sz(list.size()))-1),
        homeBit(1<<sz(list.size())),ids(move(list)) {}
        int m() const { return sz(ids.size()); }
        int bit(int id) const {
            for (int i=0;i<m();++i) if (ids[i]==id) return 1<<i;
            return 0;
        }
        int G(int s,int p) const { return gather->value(s,p); }
        int H(int s,int p) const { return finish->value(s|homeBit,p); }
    };
    vector<Cohort> partition(const Grid& g) {
        vector<Cohort> groups;
        for (int col=0;col<g.K;++col) {
            vector<int> left; for (int id=0;id<g.M;++id) if (g.color[id]==col) left.push_back(id);
            while (!left.empty()) {
                auto first=max_element(left.begin(),left.end(),[&](int a,int b) {
                    return g.hd(g.origin[a],col)<g.hd(g.origin[b],col);
                });
                vector<int> ids{*first}; left.erase(first);
                while (ids.size()<4 && !left.empty()) {
                    auto score=[&](int id) {
                        int distance=INF;
                        for (int old:ids) distance=min(distance,g.dist[g.origin[id]][g.origin[old]]);
                        return pair<int,int>{distance,-g.hd(g.origin[id],col)};
                    };
                    auto it=min_element(left.begin(),left.end(),[&](int a,int b) { return score(a)<score(b); });
                    ids.push_back(*it); left.erase(it);
                }
                sort(ids.begin(),ids.end()); groups.emplace_back(sz(groups.size()),col,move(ids));
            }
        }
        return groups;
    }
    void replayTree(Builder& out,const Steiner& field,int mask,int query,int root,
    const vector<int>& ids) {
        auto edges=field.tree(mask,query);
        vector<int> parent(out.g.V,-2),order{root}; parent[root]=-1;
        for (int j=0;j<sz(order.size());++j) for (int v:edges[order[j]]) if (parent[v]==-2) {
            parent[v]=order[j]; order.push_back(v);
        }
        vector<unsigned char> owned(out.g.M,0); vector<int> count(out.g.V,0);
        for (int id:ids) if (out.board.loc[id]>=0) {
            int p=out.board.loc[id]; if (parent[p]==-2) throw runtime_error("tree_terminal_missing");
            owned[id]=1; ++count[p];
        }
        for (auto it=order.rbegin();it!=order.rend();++it) {
            int p=*it,q=count[p]; if (!q || parent[p]<0) continue;
            const auto& src=out.board.cells[p];
            if (q>src.h) throw runtime_error("tree_packet_missing");
            for (int j=src.h-q;j<src.h;++j) if (!owned[src.ids[j]]) throw runtime_error("tree_packet_buried");
            int v=parent[p]; out.emit(p,q,out.g.direction(p,v),1); count[v]+=q;
        }
    }
    vector<int> subsetIds(const Cohort& c,int mask) {
        vector<int> ids; for (int j=0;j<c.m();++j) if (mask&(1<<j)) ids.push_back(c.ids[j]); return ids;
    }
    void gather(Builder& out,const Cohort& c,int mask,int root) {
        replayTree(out,*c.gather,mask,root,root,subsetIds(c,mask));
    }
    void finish(Builder& out,const Cohort& c,int remaining,int held) {
        replayTree(out,*c.finish,remaining|c.homeBit,held,out.g.home[c.col],c.ids);
        for (int id:c.ids) if (out.board.loc[id]>=0) throw runtime_error("pure_finish_incomplete");
    }

    enum Kind : unsigned char { NONE, MOVE, GROUND, AFTER, BEFORE, END_HEAD, END_PURE, END_HOST, END_GUEST, CUT_GUEST, CUT_HOST };
    struct Decision {
        short mask = 0;
        signed char dir = -1, len = 0;
        Kind kind = NONE;
    };
    struct Event { int p, mask, side; Decision decision; };
    using Script = vector<Event>;

    // side 0 = guest below host; side 1 = guest above host. Empty guest has side 0.
    // Main moves decrease host-home distance. Added masks only grow: no search heap.
    struct Trunk {
        const Grid& g; const Cohort& host; const Cohort& guest;
        int goal; // 0/1: head view, 2: tail, 3: build both head channels together
        bool shared=false;
        vector<int> cost, bg, count;
        vector<int> otherCost;
        vector<unsigned char> outside;
        vector<Decision> back;
        vector<Decision> otherBack;
        int ix(int s,int side,int p) const { return (2*s+side)*g.V+p; }
        int value(int s,int side,int p) const {
            return shared && goal==1 ? otherCost[ix(s,side,p)] : cost[ix(s,side,p)];
        }
        int background(int p,int s) const {
            return outside[p] && !(bg[p]&s);
        }
        void offer(int index,int value,Decision decision) {

            if (value<cost[index]) { cost[index]=value; back[index]=decision; }
        }
        void offerOther(int index,int value,Decision decision) {

            if (value<otherCost[index]) { otherCost[index]=value; otherBack[index]=decision; }
        }
        void transition(int index,int next,int extra,Decision decision) {
            offer(index,extra+cost[next],decision);
            if (shared) offerOther(index,extra+otherCost[next],decision);
        }
        Trunk(const Grid& grid,const Cohort& h,const Cohort& t,int mode,Timer& timer)
        :g(grid),host(h),guest(t),goal(mode) {
            shared=mode==3;
            cost.assign(2*(guest.all+1)*g.V,INF); back.resize(cost.size());
            if (shared) { otherCost.assign(cost.size(),INF); otherBack.resize(cost.size()); }
            // Cache background and its guest bit; avoids ID scans in subset transitions.
            bg.resize(g.V); outside.resize(g.V,0); count.resize(guest.all+1);
            for (int p:g.floor) {
                bg[p]=guest.bit(g.initial[p]); outside[p]=g.initial[p]>=0 && !host.bit(g.initial[p]);
            }
            for (int s=0;s<=guest.all;++s) count[s]=popcount(static_cast<unsigned>(s));
            for (int s=guest.all;s>=0;--s) {
                timer.check(); int r=count[s],q=host.m(),left=guest.all^s;
                for (int p:g.nearHome[host.col]) for (int side=0;side<(r?2:1);++side) {
                    int index=ix(s,side,p),base=background(p,s);
                    if (base+q+r>8) continue;
                    bool hostReceived=(p==g.home[host.col] && (r==0 || side==0));
                    bool guestReceived=(p==g.home[guest.col] && r && side==1);
                    if (goal!=2) {
                        if (guestReceived) continue;
                        if (p==g.home[host.col]) {
                            if (s==guest.all) {
                                if (shared) {
                                    if (!side) offer(index,0,{0,-1,0,END_HEAD});
                                    else offerOther(index,0,{0,-1,0,END_HEAD});
                                } else if (side==goal) offer(index,0,{0,-1,0,END_HEAD});
                            }
                            continue;
                        }
                    } else {
                        if (hostReceived) {
                            int anchor=r?p:g.home[guest.col];
                            offer(index,guest.H(left,anchor),{0,-1,0,END_HOST}); continue;
                        }
                        if (guestReceived) {
                            offer(index,g.hd(p,host.col)+guest.H(left,g.home[guest.col]),{0,-1,0,END_GUEST}); continue;
                        }
                        if (!r) offer(index,g.hd(p,host.col)+guest.base,{0,-1,0,END_PURE});
                        if (r) terminal(index,p,s,side,q,r,left);
                    }
                    int at=bg[p]&left;
                    if (at && (!r || side==0)) {

                        transition(index,ix(s|at,0,p),0,{static_cast<short>(at),-1,0,GROUND});
                    }
                    for (int d=0;d<4;++d) {
                        int v=g.ray[p][d][1];
                        if (v<0 || g.hd(v,host.col)+1!=g.hd(p,host.col)) continue;
                        int nextSide=r?1-side:0;
                        if (q+r+background(v,s)<=8) transition(index,ix(s,nextSide,v),1,{0,static_cast<signed char>(d),1,MOVE});
                        // Feeder first, carrier second: feeder joins the bottom block.
                        if (r && side!=1) continue;
                        if (v==g.home[guest.col]) continue;
                        for (int add=left;add;add=(add-1)&left) {
                            if (add&at) continue; // ID below the waiting carrier cannot be extracted.
                            int a=count[add];
                            if (base+q+r+a>8 || background(v,s|add)+q+r+a>8) continue;
                            int feed=guest.G(add,v); if (feed>=INF) continue;

                            transition(index,ix(s|add,0,v),feed+1,
                            {static_cast<short>(add),static_cast<signed char>(d),1,BEFORE});
                        }
                    }
                    // Carrier first, feeder second: feeder joins the top block.
                    if (p!=g.home[guest.col] && (!r || side==1))
                    for (int add=left;add;add=(add-1)&left) {
                        if (add&at) continue;
                        int a=count[add];
                        if (base+q+r+a>8) continue;
                        int feed=guest.G(add,p); if (feed>=INF) continue;

                        transition(index,ix(s|add,1,p),feed,{static_cast<short>(add),-1,0,AFTER});
                    }
                }
            }

        }
        void terminal(int index,int p,int s,int side,int q,int r,int left) {
            if (side==1) {
                for (int d=0;d<4;++d) for (int len=1;len<=q+1;++len) {
                    int y=g.ray[p][d][len]; if (y<0) break;
                    bool hostSource=p==g.home[host.col],guestDone=y==g.home[guest.col];
                    if (!hostSource && !guestDone && q+r+background(y,s)>8) continue;
                    int rest=guest.H(left,y); if (rest>=INF) continue;

                    offer(index,1+g.hd(p,host.col)+rest,
                    {0,static_cast<signed char>(d),static_cast<signed char>(len),CUT_GUEST});
                }
            } else {
                for (int d=0;d<4;++d) for (int len=1;len<=r+1;++len) {
                    int y=g.ray[p][d][len]; if (y<0) break;
                    if (g.hd(y,host.col)+len!=g.hd(p,host.col)) continue;
                    int rest=guest.H(left,p); if (rest>=INF) continue;

                    offer(index,1+g.hd(y,host.col)+rest,
                    {0,static_cast<signed char>(d),static_cast<signed char>(len),CUT_HOST});
                }
            }
        }
        Script script(int p) const {
            Script result; int s=0,side=0;
            for (int step=0;step<g.V+guest.m()+8;++step) {
                auto choice=shared && goal==1 ? otherBack[ix(s,side,p)] : back[ix(s,side,p)];
                if (choice.kind==NONE) throw runtime_error("missing_trunk_plan");
                result.push_back({p,s,side,choice});
                if (choice.kind>=END_HEAD) return result;
                int r=popcount(static_cast<unsigned>(s));
                if (choice.kind==MOVE) { p=g.ray[p][choice.dir][1]; side=r?1-side:0; }
                if (choice.kind==GROUND) { s|=choice.mask; side=0; }
                if (choice.kind==AFTER) { s|=choice.mask; side=1; }
                if (choice.kind==BEFORE) { p=g.ray[p][choice.dir][1]; s|=choice.mask; side=0; }
            }
            throw runtime_error("trunk_plan_cycle");
        }
    };

    struct Foot {
        int cohort = -1, mask = 0, cost = INF, delta = INF;
    };
    using Feet = vector<array<Foot,4>>;
    Feet supportFields(const Grid& g,const vector<Cohort>& groups,Timer& timer) {
        Feet result(5*g.V);
        for (const auto& c:groups) {
            timer.check();
            for (int p:g.floor) {
                if (p==g.home[c.col]) continue;
                int id=g.initial[p],at=c.bit(id);
                // A foreign original under a new support would be buried. This also
                // makes the support table independent of the later passenger role.
                if (id>=0 && !at) continue;
                for (int s=1;s<=c.all;++s) {
                    if (at && !(s&at)) continue;
                    int h=popcount(static_cast<unsigned>(s));
                    int value=c.G(s,p)+c.H(c.all^s,p); if (value>=INF) continue;
                    Foot item{c.index,s,value,value-c.base}; auto& list=result[h*g.V+p];
                    int slot=-1;
                    for (int j=0;j<4;++j) if (list[j].cohort>=0 && groups[list[j].cohort].col==c.col) {
                        slot=j; break;
                    }
                    if (slot<0) slot=3;
                    if (item.delta>=list[slot].delta) continue;
                    list[slot]=item;
                    stable_sort(list.begin(),list.end(),[](const Foot& a,const Foot& b) { return a.delta<b.delta; });
                }
            }
        }
        return result;
    }
    vector<vector<int>> neighbours(const Grid& g,const vector<Cohort>& groups) {
        vector<vector<int>> result(groups.size());
        for (const auto& h:groups) for (int col=0;col<g.K;++col) if (col!=h.col) {
            int best=-1; pair<int,int> rank{INF,INF};
            for (const auto& t:groups) if (t.col==col) {
                int gap=INF;
                for (int a:h.ids) for (int b:t.ids) gap=min(gap,g.dist[g.origin[a]][g.origin[b]]);
                pair<int,int> value{gap,t.base};
                if (value<rank) { rank=value; best=t.index; }
            }
            if (best>=0) result[h.index].push_back(best);
        }
        return result;
    }
    struct Start {
        int cost = INF, margin = INF, root = -1, foot = -1, footMask = 0, dir = -1, len = 0, goal = 0;
        shared_ptr<Script> script;
    };
    vector<Start> starts(const Grid& g,const vector<Cohort>& groups,const Feet& feet,
    const Cohort& p,const Cohort& q,const Trunk& head,Timer& timer) {
        vector<Start> best;
        auto colour=[&](const Start& a) { return a.foot<0 ? -1 : groups[a.foot].col; };
        auto put=[&](Start item) {
            item.margin=item.cost-p.base-(item.foot<0?0:groups[item.foot].base);
            if (item.cost>=INF) return;
            bool replaced=false;
            for (auto& old:best) if (colour(old)==colour(item)) {
                if (item.margin<old.margin) old=move(item);
                replaced=true; break;
            }
            if (!replaced) best.push_back(move(item));
            stable_sort(best.begin(),best.end(),[](const Start& a,const Start& b) { return a.margin<b.margin; });
            if (best.size()>2) best.resize(2); // Later T excludes at most one colour.
        };
        for (int u:g.floor) {
            timer.check(); int collect=p.G(p.all,u); if (collect>=INF) continue;
            Start bare{collect+head.value(0,0,u),INF,u,-1,0,-1,0,head.goal,{}};
            put(move(bare));
            if (g.hd(u,q.col)<=g.hd(g.home[p.col],q.col)) continue;
            for (int h=1;h<=4;++h) {
                const auto& options=feet[h*g.V+u]; if (options[0].cohort<0) continue;
                int value=INF,dir=-1,len=0;
                for (int d=0;d<4;++d) for (int l=1;l<=h+1;++l) {
                    int y=g.ray[u][d][l]; if (y<0) break;
                    if (g.hd(y,p.col)+l!=g.hd(u,p.col)) continue;
                    int cost=1+head.value(0,0,y);
                    if (cost<value) { value=cost; dir=d; len=l; }
                }
                if (value>=INF) continue;
                for (const auto& foot:options) if (foot.cohort>=0) {
                    int col=groups[foot.cohort].col; if (col==p.col || col==q.col) continue;
                    put({collect+value+foot.cost,INF,u,foot.cohort,foot.mask,dir,len,head.goal,{}});
                }
            }
        }
        for (auto& s:best) {
            int root=s.len?g.ray[s.root][s.dir][s.len]:s.root;
            s.script=make_shared<Script>(head.script(root));
        }
        return best;
    }
    struct End {
        int cost = INF, root = -1, dir = -1, len = 0;
        shared_ptr<Script> script;
    };
    End ending(const Grid& g,const Cohort& p,const Cohort& q,const Trunk* tail,int goal) {
        End result;
        auto value=[&](int v) { return tail ? tail->value(0,0,v) : g.hd(v,q.col); };
        int root=g.home[p.col];
        if (goal==0) result={value(root),root,-1,0,{}};
        else for (int d=0;d<4;++d) for (int l=1;l<=p.m()+1;++l) {
            int y=g.ray[root][d][l]; if (y<0) break;
            if (g.hd(y,q.col)+l!=g.hd(root,q.col)) continue;
            int cost=1+value(y);
            if (cost<result.cost) result={cost,y,d,l,{}};
        }
        if (tail && result.cost<INF) result.script=make_shared<Script>(tail->script(result.root));
        return result;
    }
    struct Candidate {
        int p, q, t, predicted, base, saved;
        Start start; End end;
    };

    void checkPacket(const Builder& out,const Cohort& h,const Cohort& t,int mask,int p,int q) {
        const auto& tower=out.board.cells[p];
        if (tower.h<q) throw runtime_error("trunk_packet_missing");
        for (int j=tower.h-q;j<tower.h;++j) {
            int id=tower.ids[j];
            if (!h.bit(id) && !(t.bit(id)&mask)) throw runtime_error("trunk_packet_membership");
        }
    }
    void checkPurePacket(const Builder& out,const Cohort& c,int mask,int p,int q) {
        const auto& tower=out.board.cells[p];
        if (tower.h<q) throw runtime_error("pure_packet_missing");
        for (int j=tower.h-q;j<tower.h;++j)
        if (!(c.bit(tower.ids[j])&mask)) throw runtime_error("pure_packet_membership");
    }
    void directHost(Builder& out,const Cohort& host,int p) {
        int alive=0;
        for (int id:host.ids) if (out.board.loc[id]>=0) {
            if (out.board.loc[id]!=p) throw runtime_error("host_not_at_finish");
            ++alive;
        }
        if (alive && alive!=host.m()) throw runtime_error("partially_received_host");
        if (alive) {
            checkPurePacket(out,host,host.all,p,alive);
            out.direct(p,alive,host.col);
        }
    }
    void replayScript(Builder& out,const Script& script,const Cohort& host,const Cohort& guest) {
        for (const auto& e:script) {
            out.timer.check(); auto choice=e.decision;
            int p=e.p,s=e.mask,r=popcount(static_cast<unsigned>(s)),left=guest.all^s;
            int destination=p;
            switch (choice.kind) {
                case MOVE:
                checkPacket(out,host,guest,s,p,host.m()+r);
                out.emit(p,host.m()+r,choice.dir,1);
                destination=out.g.ray[p][choice.dir][1]; break;
                case GROUND:
                checkPacket(out,host,guest,s|choice.mask,p,host.m()+r+popcount(static_cast<unsigned>(choice.mask))); break;
                case AFTER:
                gather(out,guest,choice.mask,p);
                checkPacket(out,host,guest,s|choice.mask,p,host.m()+r+popcount(static_cast<unsigned>(choice.mask))); break;
                case BEFORE:
                destination=out.g.ray[p][choice.dir][1];
                gather(out,guest,choice.mask,destination);
                checkPacket(out,host,guest,s,p,host.m()+r);
                out.emit(p,host.m()+r,choice.dir,1); break;
                case END_HEAD: break;
                case END_PURE:
                directHost(out,host,p);
                finish(out,guest,guest.all,out.g.home[guest.col]); break;
                case END_HOST:
                finish(out,guest,left,r?p:out.g.home[guest.col]); break;
                case END_GUEST:
                directHost(out,host,p);
                finish(out,guest,left,out.g.home[guest.col]); break;
                case CUT_GUEST:
                checkPurePacket(out,guest,s,p,r);
                destination=out.g.ray[p][choice.dir][choice.len];
                out.emit(p,r,choice.dir,choice.len); directHost(out,host,p);
                finish(out,guest,left,destination); break;
                case CUT_HOST:
                checkPurePacket(out,host,host.all,p,host.m());
                destination=out.g.ray[p][choice.dir][choice.len];
                out.emit(p,host.m(),choice.dir,choice.len); directHost(out,host,destination);
                finish(out,guest,left,p); break;
                default: throw runtime_error("unknown_trunk_decision");
            }

            if (choice.kind>=END_HEAD) return;
        }
        throw runtime_error("unterminated_script");
    }

    int compileFrame(Builder& out,const vector<Cohort>& groups,const Candidate& c) {
        const auto& p=groups[c.p]; const auto& q=groups[c.q]; const auto& start=c.start;
        vector<int> owners{c.p,c.q}; if (c.t>=0) owners.push_back(c.t); if (start.foot>=0) owners.push_back(start.foot);
        for (int i:owners) for (int id:groups[i].ids)
        if (out.board.loc[id]!=out.g.origin[id]) throw runtime_error("frame_owner_not_initial");
        int begin=sz(out.ops.size());
        if (start.foot>=0) gather(out,groups[start.foot],start.footMask,start.root);
        gather(out,p,p.all,start.root);
        if (start.len) out.emit(start.root,p.m(),start.dir,start.len);
        replayScript(out,*start.script,p,q);
        for (int id:q.ids) if (out.board.loc[id]!=out.g.home[p.col]) throw runtime_error("head_guest_not_at_boundary");
        for (int id:p.ids) {
            if (start.goal==0 && out.board.loc[id]>=0) throw runtime_error("head_host_not_received");
            if (start.goal==1 && out.board.loc[id]!=out.g.home[p.col]) throw runtime_error("head_source_prefix_missing");
        }
        if (start.goal==1) {
            checkPurePacket(out,q,q.all,out.g.home[p.col],q.m());
            out.emit(out.g.home[p.col],q.m(),c.end.dir,c.end.len);
        }
        if (c.t>=0) replayScript(out,*c.end.script,q,groups[c.t]);
        else directHost(out,q,c.end.root);
        if (start.foot>=0) {
            const auto& foot=groups[start.foot];
            finish(out,foot,foot.all^start.footMask,start.root);
        }
        for (int i:owners) for (int id:groups[i].ids) if (out.board.loc[id]>=0) throw runtime_error("frame_incomplete");
        int actual=sz(out.ops.size())-begin;
        if (actual>c.predicted) throw runtime_error("frame_cost_underestimated");

        return actual;
    }
    struct Construction {
        vector<Op> ops;
    };

    Construction portfolio(const Grid& g,Timer& timer,const vector<Cohort>& groups,
    const vector<vector<Op>>& pure,const vector<Candidate>& candidates) {

        vector<int> order(candidates.size()); iota(order.begin(),order.end(),0);
        stable_sort(order.begin(),order.end(),[&](int a,int b) { return candidates[a].saved>candidates[b].saved; });
        vector<bool> used(groups.size(),false); Builder out(g,timer);
        for (int index:order) {
            const auto& c=candidates[index]; vector<int> owners{c.p,c.q};
            if (c.t>=0) owners.push_back(c.t);
            if (c.start.foot>=0) owners.push_back(c.start.foot);
            bool clash=false; for (int i:owners) clash=clash || used[i];
            if (clash) { continue; }
            Board saved=out.board; size_t size=out.ops.size();
            try {
                int actual=compileFrame(out,groups,c);
                if (actual>=c.base) throw runtime_error("no_actual_saving");
                for (int i:owners) used[i]=true;
            } catch (const exception&) {
                out.board=move(saved); out.ops.resize(size);
            }
        }
        for (const auto& c:groups) if (!used[c.index]) out.append(pure[c.index]);
        if (out.board.remaining) throw runtime_error("portfolio_incomplete");
        return {move(out.ops)};
    }
    struct Solver {
        const Grid& g; Timer& timer;
        vector<Cohort> groups;
        vector<vector<Op>> pure;
        vector<vector<int>> neighbour;
        Feet feet;
        unordered_map<int,unique_ptr<Trunk>> tails;
        vector<Candidate> candidates;
        Construction best;
        string outputText;
        Solver(const Grid& grid,Timer& t):g(grid),timer(t) {}
        void keep(Construction result) {
            if (timer.enabled) timer.check();
            if (timer.best>=0 && sz(result.ops.size())>=timer.best) return;
            if (result.ops.size()>100000) return;
            string text; text.reserve(result.ops.size()*24);
            for (const auto& op:result.ops) {
                text+=to_string(op.p/g.n); text+=' '; text+=to_string(op.p%g.n); text+=' ';
                text+=to_string(op.k); text+=' '; text+=DIR[op.d]; text+=' '; text+=to_string(op.len); text+='\n';
            }
            if (timer.enabled) timer.check();
            best=move(result); outputText=move(text); timer.best=sz(best.ops.size());

        }
        Trunk& tail(int q,int t) {
            int key=q*sz(groups.size())+t; auto it=tails.find(key);
            if (it!=tails.end()) return *it->second;
            timer.check();
            auto value=make_unique<Trunk>(g,groups[q],groups[t],2,timer);
            auto* ptr=value.get(); tails.emplace(key,move(value)); return *ptr;
        }
        void run() {
            Builder insurance(g,timer);
            for (int id=0;id<g.M;++id) insurance.direct(g.origin[id],1,g.color[id]);
            if (insurance.board.remaining) throw runtime_error("insurance_incomplete");
            keep({move(insurance.ops)});
            groups=partition(g); pure.resize(groups.size());
            Builder base(g,timer);
            for (auto& c:groups) {
                vector<int> terminals; for (int id:c.ids) terminals.push_back(g.origin[id]);
                c.gather=make_unique<Steiner>(g,terminals,g.home[c.col],timer);
                terminals.push_back(g.home[c.col]);
                c.finish=make_unique<Steiner>(g,move(terminals),-1,timer);
                c.base=c.H(c.all,g.home[c.col]);

                int offset=sz(base.ops.size()); finish(base,c,c.all,g.home[c.col]);
                pure[c.index]=vector<Op>(base.ops.begin()+offset,base.ops.end());
                if (sz(pure[c.index].size())>c.base) throw runtime_error("pure_cost_underestimated");
                c.base=sz(pure[c.index].size());

            }
            if (base.board.remaining) throw runtime_error("pure_baseline_incomplete");
            keep({move(base.ops)});
            feet=supportFields(g,groups,timer);
            neighbour=neighbours(g,groups);
            for (const auto& p:groups) {
                timer.check(); size_t old=candidates.size();
                for (int qi:neighbour[p.index]) {
                    const auto& q=groups[qi];
                    auto head=make_unique<Trunk>(g,p,q,3,timer);
                    array<vector<Start>,2> front;
                    for (int goal=0;goal<2;++goal) {
                        head->goal=goal;
                        front[goal]=starts(g,groups,feet,p,q,*head,timer);
                    }
                    vector<int> passengers{-1};
                    for (int ti:neighbour[qi]) if (groups[ti].col!=p.col) passengers.push_back(ti);
                    for (int ti:passengers) {
                        const Trunk* table=ti<0 ? nullptr : &tail(qi,ti);
                        for (int goal=0;goal<2;++goal) {
                            End end=ending(g,p,q,table,goal); if (end.cost>=INF) continue;
                            for (const auto& start:front[goal]) {
                                if (ti>=0 && start.foot>=0 && groups[ti].col==groups[start.foot].col) continue;
                                int predicted=start.cost+end.cost;
                                int baseline=p.base+q.base+(ti<0?0:groups[ti].base)+(start.foot<0?0:groups[start.foot].base);
                                if (predicted>=baseline) continue;
                                candidates.push_back({p.index,qi,ti,predicted,baseline,baseline-predicted,start,end});
                            }
                        }
                    }
                }
                if (candidates.size()!=old) keep(portfolio(g,timer,groups,pure,candidates));

            }
            if (timer.best<0) throw runtime_error("no_complete_solution");
        }
    };

    // New primary. No cohort ownership, event tables, prescribed ID routes, or
    // local "deliver everything in this window" commitment.
    struct Word {
        array<int,8> c{};
        int h=0;
        bool pure() const {
            for (int i=1;i<h;++i) if (c[i]!=c[0]) return false;
            return true;
        }
        int consume(int col) {
            int before=h; while (h && c[h-1]==col) --h; return before-h;
        }
    };
    Word packetWord(const Grid& g,const Board& board,int p) {
        Word w; const auto& t=board.cells[p]; w.h=t.h;
        for (int i=0;i<t.h;++i) w.c[i]=g.color[t.ids[i]];
        return w;
    }
    // Only the order of colour runs matters to this ESTIMATE. Counts, background
    // capacity, reversal parity and launch costs are handled by real route BFS.
    struct TourCost {
        struct Profile { int bottom,top,afterBottom,afterTop; };
        const Grid& g;
        unordered_map<uint64_t,Profile> cache;
        explicit TourCost(const Grid& grid):g(grid) {}
        Profile profile(const Word& w) {
            uint64_t key=0; array<int,8> runs{}; int n=0;
            for (int i=0;i<w.h;++i) if (!i || w.c[i]!=w.c[i-1]) {
                runs[n++]=w.c[i]; key=(key<<5)|static_cast<uint64_t>(w.c[i]+1);
            }
            auto found=cache.find(key); if (found!=cache.end()) return found->second;
            array<array<array<int,26>,8>,8> memo{};
            for (auto& a:memo) for (auto& b:a) b.fill(-1);
            auto solve=[&](auto&& self,int l,int r,int at)->int {
                if (l>r) return 0;
                int& value=memo[l][r][at]; if (value>=0) return value;
                return value=min(g.hd(g.home[at],runs[l])+self(self,l+1,r,runs[l]),
                g.hd(g.home[at],runs[r])+self(self,l,r-1,runs[r]));
            };
            Profile result{runs[0],runs[n-1],solve(solve,1,n-1,runs[0]),solve(solve,0,n-2,runs[n-1])};
            cache.emplace(key,result); return result;
        }
        int value(int p,const Word& w) {
            if (!w.h) return 0;
            auto a=profile(w);
            return min(g.hd(p,a.bottom)+a.afterBottom,g.hd(p,a.top)+a.afterTop);
        }
        int total(const Board& board) {
            int sum=0; for (int p:g.floor) sum+=value(p,packetWord(g,board,p)); return sum;
        }
    };

    struct PacketBuilder {
        struct Query {
            int p,q,sourceReceived,orientations;
            Word prefix,cargo;
        };
        struct Move {
            vector<Op> ops;
            int gain=-INF,received=0,q=0,destination=-1,biasCost=0;
            int cost() const { return sz(ops.size())+biasCost; }
        };
        struct FullCandidate { Move move; int forecastGain,forecastCost; };
        const Grid& g; Timer& timer; Solver& solver;
        TourCost tours;
        int macros=0;
        vector<int> distance,previous,queue,walkSeen;
        int walkEpoch=0;
        vector<unsigned char> direction,length;
        explicit PacketBuilder(Solver& s):g(s.g),timer(s.timer),solver(s),tours(g),
        distance(2*g.V),previous(2*g.V),queue(2*g.V),walkSeen(2*g.V,0),direction(2*g.V),length(2*g.V) {}
        Query query(const Board& board,int p,int q) const {
            auto w=packetWord(g,board,p); Query a{p,q,0,1,{},{} }; a.prefix.h=w.h-q; a.cargo.h=q;
            for (int i=0;i<a.prefix.h;++i) a.prefix.c[i]=w.c[i];
            for (int i=0;i<q;++i) a.cargo.c[i]=w.c[w.h-q+i];
            a.sourceReceived=a.prefix.consume(g.nest[p]);
            for (int i=0;i<q;++i) if (a.cargo.c[i]!=a.cargo.c[q-1-i]) a.orientations=2;
            return a;
        }
        Word landing(const Board& board,const Query& a,int p,int side,int& received) const {
            Word w=p==a.p ? a.prefix : packetWord(g,board,p);
            // side is the CURRENT orientation, relative to the original suffix.
            // Orientation 0 is original order, orientation 1 is its reverse.
            for (int i=0;i<a.q;++i) w.c[w.h++]=a.cargo.c[side ? a.q-1-i : i];
            received=a.sourceReceived+w.consume(g.nest[p]); return w;
        }
        template<class Visit> void walk(const Board& board,const Query& a,Visit&& visit) {

            if (++walkEpoch==INT_MAX) { fill(walkSeen.begin(),walkSeen.end(),0); walkEpoch=1; }
            int first=a.p*a.orientations,head=0,tail=0; queue[tail++]=first; distance[first]=0; walkSeen[first]=walkEpoch;
            while (head<tail) {
                int index=queue[head++],p=index/a.orientations,side=index%a.orientations;
                if ((head&63)==0) timer.check();
                if (index!=first) {
                    visit(index,p,side,distance[index]);
                    int top=a.cargo.c[side ? 0 : a.q-1];
                    if (top==g.nest[p]) continue; // A changed word starts a new macro.
                }
                int bg=p==a.p ? a.prefix.h : board.cells[p].h;
                int range=index==first ? board.cells[a.p].h-a.q+1 : bg+1;
                int nextSide=a.orientations==2 ? 1-side : 0;
                for (int d=0;d<4;++d) for (int l=1;l<=range;++l) {
                    int y=g.ray[p][d][l]; if (y<0) break;
                    int height=y==a.p ? a.prefix.h : board.cells[y].h;
                    if (height+a.q>8) continue;
                    int next=y*a.orientations+nextSide; if (walkSeen[next]==walkEpoch) continue;
                    walkSeen[next]=walkEpoch; distance[next]=distance[index]+1; previous[next]=index;
                    direction[next]=static_cast<unsigned char>(d); length[next]=static_cast<unsigned char>(l);
                    queue[tail++]=next;
                }
            }

        }
        vector<Op> path(const Query& a,int end) const {
            vector<Op> ops; int first=a.p*a.orientations;
            while (end!=first) {
                int from=previous[end];
                ops.push_back({from/a.orientations,a.q,direction[end],length[end],-1}); end=from;
            }
            reverse(ops.begin(),ops.end()); return ops;
        }
        static bool better(const Move& a,const Move& b) {
            if (b.ops.empty()) return true;
            long long left=static_cast<long long>(a.gain)*b.cost(),right=static_cast<long long>(b.gain)*a.cost();
            if (left!=right) return left>right;
            left=static_cast<long long>(a.received)*b.cost(); right=static_cast<long long>(b.received)*a.cost();
            if (left!=right) return left>right;
            if (a.q!=b.q) return a.q>b.q;
            return a.cost()<b.cost();
        }
        Move choose(const Board& board,bool mixed,int only=-1,int biasGain=0,int biasCost=0,int biasReceived=0) {
            vector<int> values(g.V,0); for (int p:g.floor) values[p]=tours.value(p,packetWord(g,board,p));
            array<bool,9> heightExists{};
            for (int p:g.floor) heightExists[board.cells[p].h]=true;
            Move best; vector<FullCandidate> full;
            auto forecastBetter=[](const FullCandidate& a,const FullCandidate& b) {
                long long x=static_cast<long long>(a.forecastGain)*b.forecastCost;
                long long y=static_cast<long long>(b.forecastGain)*a.forecastCost;
                return x!=y ? x>y : a.move.q>b.move.q;
            };
            for (int p:g.floor) {
                if ((only>=0 && p!=only) || !board.cells[p].h) continue;
                timer.check();
                for (int q=board.cells[p].h;q>=1;--q) {
                    auto a=query(board,p,q); int prefixCost=tours.value(p,a.prefix);
                    if (!mixed && !a.cargo.pure()) continue;
                    // Removing only part of an end run leaves the source estimate
                    // unchanged; appending that pure run cannot lower the target's
                    // deque tour. With no possible full-eight merge, it cannot beat
                    // a positive-gain move. This removes work without narrowing it.
                    if (only<0 && !best.ops.empty() && best.gain>0 && a.cargo.pure() &&
                    prefixCost==values[p] && !a.sourceReceived && !heightExists[8-q]) {
                        continue;
                    }
                    walk(board,a,[&](int index,int y,int side,int steps) {
                        int received=0; Word w=landing(board,a,y,side,received);
                        if (!mixed && !w.pure()) return;
                        int gain=values[p]+(y==p ? 0 : values[y])-tours.value(y,w)-(y==p ? 0 : prefixCost);
                        if (w.h==8) {
                            if (only>=0) {
                                // An eight-slime carrier may reach its nest with the
                                // right colour BURIED. Launch the covering suffix
                                // from that nest: source consumption is a delivery.
                                // This is one final action, not another event search.
                                for (int k=1;k<8;++k) if (w.c[k-1]==g.nest[y]) {
                                    Word sourceAfter=w; sourceAfter.h=k;
                                    int removed=sourceAfter.consume(g.nest[y]);
                                    int suffix=8-k;
                                    for (int d=0;d<4;++d) for (int l=1;l<=k+1;++l) {
                                        int z=g.ray[y][d][l]; if (z<0) break;
                                        Word target=z==p ? a.prefix : packetWord(g,board,z);
                                        if (target.h+suffix>8) continue;
                                        for (int i=7;i>=k;--i) target.c[target.h++]=w.c[i];
                                        int totalReceived=biasReceived+a.sourceReceived+removed+target.consume(g.nest[z]);
                                        if (target.h>7 || (!mixed && (!sourceAfter.pure() || !target.pure()))) continue;
                                        int before=values[p]+(y==p ? 0 : values[y])+(z==p ? 0 : values[z]);
                                        int after=tours.value(y,sourceAfter)+tours.value(z,target)+
                                        (p==y || p==z ? 0 : prefixCost);
                                        int totalGain=biasGain+before-after;
                                        if (totalGain<0 || (!totalGain && !totalReceived)) continue;
                                        Move item; item.gain=totalGain; item.received=totalReceived;
                                        item.q=q; item.destination=z; item.biasCost=biasCost;
                                        item.ops=path(a,index);
                                        item.ops.push_back({y,suffix,d,l,-1});
                                        if (better(item,best)) best=move(item);
                                    }
                                }
                                return;
                            }
                            auto plan=tours.profile(w);
                            int forecast=gain+tours.value(y,w)-min(plan.afterBottom,plan.afterTop);
                            int cost=steps+max(1,min(g.hd(y,plan.bottom),g.hd(y,plan.top)));
                            if (forecast<=0) return;
                            FullCandidate item{{},forecast,cost}; item.move.q=q;
                            if (full.size()==4 && !forecastBetter(item,full.back())) return;
                            item.move={path(a,index),gain,received,q,y,0};
                            full.push_back(move(item)); stable_sort(full.begin(),full.end(),forecastBetter);
                            if (full.size()>4) full.pop_back();
                            return;
                        }
                        if (only>=0 && !received) return;
                        int totalGain=biasGain+gain,totalReceived=biasReceived+received;
                        if (totalGain<0 || (!totalGain && !totalReceived)) return;
                        Move item; item.gain=totalGain; item.received=totalReceived; item.q=q;
                        item.destination=y; item.biasCost=biasCost;
                        // Compare without reconstructing a losing route.
                        if (!best.ops.empty()) {
                            long long x=static_cast<long long>(item.gain)*best.cost();
                            long long z=static_cast<long long>(best.gain)*(steps+biasCost);
                            if (x<z) return;
                            if (x==z) {
                                x=static_cast<long long>(item.received)*best.cost();
                                z=static_cast<long long>(best.received)*(steps+biasCost);
                                if (x<z || (x==z && q<=best.q && steps+biasCost>=best.cost())) return;
                            }
                        }
                        item.ops=path(a,index);
                        if (better(item,best)) best=move(item);
                    });
                }
            }
            for (const auto& item:full) {
                Builder preview(g,timer,board); preview.append(item.move.ops);
                auto drain=choose(preview.board,mixed,item.move.destination,item.move.gain,sz(item.move.ops.size()),item.move.received);
                if (drain.ops.empty()) continue;
                vector<Op> all=item.move.ops; all.insert(all.end(),drain.ops.begin(),drain.ops.end());
                drain.ops=move(all); drain.biasCost=0; drain.q=item.move.q;
                if (better(drain,best)) best=move(drain);
            }
            return best;
        }
        vector<Op> finishCopy(const Builder& prefix) {
            Builder out(g,timer,prefix.board);
            // A stable tower is <=7: a one-slime carrier can cross ANY floor cell.
            // Stop at its first delivery, then recompute the real word. No evacuation.
            while (out.board.remaining) {
                timer.check();
                int p=-1; for (int x:g.floor) if (out.board.cells[x].h) { p=x; break; }
                if (p<0) throw runtime_error("packet_missing_live_tower");
                auto a=query(out.board,p,1); vector<Op> best; int shortest=INF;
                walk(out.board,a,[&](int index,int y,int side,int steps) {
                    if (steps>=shortest) return;
                    int received=0; auto w=landing(out.board,a,y,side,received);
                    if (!received || w.h>7) return;
                    shortest=steps; best=path(a,index);
                });
                if (best.empty()) throw runtime_error("packet_finish_unreachable");
                int before=out.board.remaining; out.append(best);
                if (out.board.remaining>=before) throw runtime_error("packet_finish_no_delivery");
            }
            vector<Op> result=prefix.ops; result.insert(result.end(),out.ops.begin(),out.ops.end());
            return result;
        }
        void run(bool mixed) {
            macros=0; Builder out(g,timer);
            int phi=tours.total(out.board);
            while (out.board.remaining) {
                timer.check(); auto selected=choose(out.board,mixed);
                if (selected.ops.empty()) throw runtime_error("packet_no_descent_or_delivery");
                int oldRemaining=out.board.remaining,oldPhi=phi;
                out.append(selected.ops); phi=tours.total(out.board); ++macros;

                for (int p:g.floor) if (out.board.cells[p].h>7) throw runtime_error("packet_unstable_full_tower");
                if (oldPhi-phi!=selected.gain || oldRemaining-out.board.remaining!=selected.received)
                throw runtime_error("packet_prediction_mismatch");
                if (phi>oldPhi || (phi==oldPhi && out.board.remaining>=oldRemaining))
                throw runtime_error("packet_non_decreasing_state");

                if (out.ops.size()>100000) throw runtime_error("packet_output_limit");
                if (out.board.remaining && (macros%16)==0) {
                    auto completed=finishCopy(out);

                    solver.keep({move(completed)});
                }
                // Exact estimate cache; clearing changes computation time, not values.
                if (tours.cache.size()>32768) tours.cache.clear();
            }

            solver.keep({out.ops});

            return;
        }
    };

    // Operation-priced event construction. A rollout is an evaluation, not an ID
    // partition: only a progress-making prefix is committed to the global board.

    struct ProbeLimit {};
    struct AddTime {
        double& target; Clock::time_point start=Clock::now();
        ~AddTime() { target+=elapsed(start); }
    };
    struct RolloutBuilder : PacketBuilder {
        enum Tag { ROUTE, MEETING, SUPPORT };
        static constexpr int POOL_PER_KIND=4, PROBE_CELLS=128;
        struct Proposal {
            vector<Op> ops;
            array<int,4> roots{};
            int count=0,tag=ROUTE,rank=-INF,gain=0,received=0,q=0,target=-1;
            bool full=false;
        };
        struct Closed {
            vector<Op> ops;
            vector<int> ends;
            int count=0;
            uint64_t foreign=0;
        };
        struct Choice {
            vector<Op> ops;
            int q=0;
            int saving=0,received=0,gain=0;
            bool priced=false;
        };

        // A constant-size physical overlay for enumerating preparations. It uses
        // actual IDs and consumption, so aliases such as launch back to a donor
        // do not need special colour-word formulas.
        struct Small {
            const Grid& g; const Board& board;
            array<int,4> pos{}; array<Tower,4> cells{};
            int size=0,received=0;
            const Tower& at(int p) const {
                for (int i=0;i<size;++i) if (pos[i]==p) return cells[i];
                return board.cells[p];
            }
            Tower& edit(int p) {
                for (int i=0;i<size;++i) if (pos[i]==p) return cells[i];
                if (size==4) throw runtime_error("preparation_overlay_size");
                pos[size]=p; cells[size]=board.cells[p]; return cells[size++];
            }
            Word word(int p) const {
                Word w; auto& a=at(p); w.h=a.h;
                for (int i=0;i<a.h;++i) w.c[i]=g.color[a.ids[i]];
                return w;
            }
            bool emit(Op& op) {
                int k=at(op.p).h-op.q;
                if (op.q<1 || k<0 || op.len>k+1) return false;
                int y=g.ray[op.p][op.d][op.len];
                if (y<0 || at(y).h+op.q>8) return false;
                auto& a=edit(op.p); auto& b=edit(y); op.k=k;
                for (int j=a.h-1;j>=k;--j) b.ids[b.h++]=a.ids[j];
                a.h=k;
                for (int p:{op.p,y}) {
                    auto& t=edit(p);
                    while (t.h && g.color[t.ids[t.h-1]]==g.nest[p]) { --t.h; ++received; }
                }
                return true;
            }
            int gain(TourCost& cost) const {
                int value=0;
                for (int i=0;i<size;++i)
                value+=cost.value(pos[i],packetWord(g,board,pos[i]))-cost.value(pos[i],word(pos[i]));
                return value;
            }
        };
        long long probeLeft=0,trialLeft=0;
        bool limited=false;
        vector<Proposal> pool;
        Choice fallback;
        unordered_map<uint64_t,Closed> beforeCache;
        vector<int> routeSeen,priceSeen,priceValue;
        int routeEpoch=0,priceEpoch=0;
        explicit RolloutBuilder(Solver& s):PacketBuilder(s),routeSeen(2*g.V,0),priceSeen(g.V,0),priceValue(g.V,0) {}

        uint64_t scopeKey(const Proposal& p) const {
            uint64_t key=static_cast<uint64_t>(p.count),base=static_cast<uint64_t>(g.V)+1;
            for (int i=0;i<p.count;++i) key=key*base+static_cast<uint64_t>(p.roots[i]+1);
            return key;
        }
        void roots(Proposal& p,const Board& board,initializer_list<int> cells) const {
            p.roots.fill(INF); p.count=0;
            for (int x:cells) if (x>=0 && board.cells[x].h &&
            find(p.roots.begin(),p.roots.end(),x)==p.roots.end()) {
                if (p.count==4) throw runtime_error("proposal_scope_size");
                p.roots[p.count++]=x;
            }
            sort(p.roots.begin(),p.roots.end());
        }
        vector<unsigned char> ownership(const Proposal& p,const Board& board) const {
            vector<unsigned char> owned(g.M,0);
            for (int i=0;i<p.count;++i) {
                const auto& t=board.cells[p.roots[i]];
                for (int j=0;j<t.h;++j) owned[t.ids[j]]=1;
            }
            return owned;
        }
        int ownedSuffix(const Tower& t,const vector<unsigned char>& owned) const {
            int n=0; for (int j=t.h-1;j>=0 && owned[t.ids[j]];--j) ++n; return n;
        }
        uint64_t foreignHash(const Board& board,const vector<unsigned char>& owned) const {
            uint64_t h=1469598103934665603ULL;
            for (int id=0;id<g.M;++id) if (!owned[id]) {
                h^=static_cast<uint64_t>(id+1); h*=1099511628211ULL;
                h^=static_cast<uint64_t>(board.loc[id]+2); h*=1099511628211ULL;
            }
            return h;
        }
        template<class Visit> void route(const Board& board,const Query& a,Visit&& visit,int& limit,bool probe) {

            if (++routeEpoch==INT_MAX) { fill(routeSeen.begin(),routeSeen.end(),0); routeEpoch=1; }
            int first=a.p*a.orientations,head=0,tail=0; queue[tail++]=first; distance[first]=0; routeSeen[first]=routeEpoch;
            while (head<tail) {
                int index=queue[head++],p=index/a.orientations,side=index%a.orientations,steps=distance[index];
                if (steps>limit) break;
                if (probe && limited) {
                    if (probeLeft<=0 || trialLeft<=0) throw ProbeLimit{};
                    --probeLeft; --trialLeft;
                }

                if ((head&63)==0) timer.check();
                if (index!=first) {
                    visit(index,p,side,steps);
                    if (a.cargo.c[side ? 0 : a.q-1]==g.nest[p]) continue;
                }
                if (steps>=limit) continue;
                int bg=p==a.p ? a.prefix.h : board.cells[p].h;
                int range=index==first ? board.cells[a.p].h-a.q+1 : bg+1;
                int nextSide=a.orientations==2 ? 1-side : 0;
                for (int d=0;d<4;++d) for (int l=1;l<=range;++l) {
                    int y=g.ray[p][d][l]; if (y<0) break;
                    int h=y==a.p ? a.prefix.h : board.cells[y].h;
                    if (h+a.q>8) continue;
                    int next=y*a.orientations+nextSide;
                    if (routeSeen[next]==routeEpoch) continue;
                    routeSeen[next]=routeEpoch; distance[next]=steps+1; previous[next]=index;
                    direction[next]=static_cast<unsigned char>(d); length[next]=static_cast<unsigned char>(l);
                    queue[tail++]=next;
                }
            }
        }
        // One actual reception, including exposing a buried run AT its nest.
        // Rollouts move only the named IDs; all other towers can still be feet.
        Move delivery(const Board& board,const vector<unsigned char>& owned,const vector<int>& ids,bool progress) {
            Move best; int full=-1,live=0;
            vector<int> sources;
            for (int id:ids) if (board.loc[id]>=0 && find(sources.begin(),sources.end(),board.loc[id])==sources.end())
            sources.push_back(board.loc[id]);
            sort(sources.begin(),sources.end());
            for (int p:sources) {
                live+=ownedSuffix(board.cells[p],owned);
                if (board.cells[p].h==8) full=p;
            }
            if (++priceEpoch==INT_MAX) { fill(priceSeen.begin(),priceSeen.end(),0); priceEpoch=1; }
            auto value=[&](int p) {
                if (priceSeen[p]!=priceEpoch) { priceSeen[p]=priceEpoch; priceValue[p]=tours.value(p,packetWord(g,board,p)); }
                return priceValue[p];
            };
            auto betterDelivery=[](int got,int cost,int gain,int q,const Move& old) {
                if (old.ops.empty()) return true;
                long long a=static_cast<long long>(got)*old.cost(),b=static_cast<long long>(old.received)*cost;
                if (a!=b) return a>b;
                if (gain!=old.gain) return gain>old.gain;
                if (q!=old.q) return q>old.q;
                return cost<old.cost();
            };
            for (int p:sources) {
                if (full>=0 && p!=full) continue;
                for (int q=ownedSuffix(board.cells[p],owned);q>=1;--q) {
                    auto a=query(board,p,q); int prefixCost=tours.value(p,a.prefix);
                    int limit=best.ops.empty() ? INF : static_cast<int>(static_cast<long long>(live)*best.cost()/best.received);
                    route(board,a,[&](int index,int y,int side,int steps) {
                        if (g.nest[y]<0 && !a.sourceReceived) return;
                        int received=0; Word w=landing(board,a,y,side,received);
                        int gain=value(p)+(y==p ? 0 : value(y))-tours.value(y,w)-(y==p ? 0 : prefixCost);
                        if (received && w.h<=7 && (!progress || gain>=0) &&
                        betterDelivery(received,steps,gain,q,best)) {
                            best={path(a,index),gain,received,q,y,0};
                            // Equal-ratio endpoints remain eligible for tie-breaking.
                            limit=min(limit,static_cast<int>(static_cast<long long>(live)*best.cost()/best.received));
                        }
                        if (g.nest[y]<0 || w.h<2) return;
                        Tower bg=board.cells[y]; if (y==p) bg.h=a.prefix.h;
                        int foreignPrefix=bg.h-ownedSuffix(bg,owned);
                        for (int k=foreignPrefix+1;k<w.h;++k) if (w.c[k-1]==g.nest[y]) {
                            Word left=w; left.h=k; int exposed=left.consume(g.nest[y]),suffix=w.h-k;
                            for (int d=0;d<4;++d) for (int l=1;l<=k+1;++l) {
                                int z=g.ray[y][d][l]; if (z<0) break;
                                Word target=z==p ? a.prefix : packetWord(g,board,z);
                                if (target.h+suffix>8) continue;
                                for (int i=w.h-1;i>=k;--i) target.c[target.h++]=w.c[i];
                                int got=received+exposed+target.consume(g.nest[z]);
                                if (target.h>7) continue;
                                int before=value(p)+(y==p ? 0 : value(y))+(z==p ? 0 : value(z));
                                int after=tours.value(y,left)+tours.value(z,target)+(p==y || p==z ? 0 : prefixCost);
                                int delta=before-after;
                                if ((!progress || delta>=0) && betterDelivery(got,steps+1,delta,q,best)) {
                                    best={path(a,index),delta,got,q,z,0};
                                    best.ops.push_back({y,suffix,d,l,-1});
                                    limit=min(limit,static_cast<int>(static_cast<long long>(live)*best.cost()/best.received));
                                }
                            }
                        }
                    },limit,true);
                }
            }
            return best;
        }
        Closed closeOwned(const Board& board,const vector<unsigned char>& owned) {

            Closed result; Builder out(g,timer,board);
            vector<int> ids;
            for (int id=0;id<g.M;++id) if (owned[id]) { ids.push_back(id); result.count+=board.loc[id]>=0; }
            int left=result.count;
            while (left) {
                auto step=delivery(out.board,owned,ids,false);
                if (step.ops.empty()) throw runtime_error("local_delivery_missing");
                int old=out.board.remaining;
                for (const auto& op:step.ops) {
                    if (ownedSuffix(out.board.cells[op.p],owned)<op.q) throw runtime_error("local_foreign_cargo");
                    out.emit(op.p,op.q,op.d,op.len);
                }
                int got=old-out.board.remaining;
                if (got<=0 || got!=step.received) throw runtime_error("local_reception_prediction");
                left-=got; result.ends.push_back(sz(out.ops.size()));
            }
            if (left || board.remaining-out.board.remaining!=result.count) throw runtime_error("local_count_mismatch");
            for (int id=0;id<g.M;++id) {
                if (owned[id] && out.board.loc[id]>=0) throw runtime_error("local_owned_not_received");
                if (!owned[id] && out.board.loc[id]!=board.loc[id]) throw runtime_error("local_foreign_changed");
            }
            result.foreign=foreignHash(out.board,owned); result.ops=move(out.ops);
            return result;
        }
        void offer(Proposal p) {

            auto same=[&](const Proposal& b) {
                if (b.tag!=p.tag || scopeKey(b)!=scopeKey(p)) return false;
                return p.tag!=ROUTE || b.target==p.target;
            };
            auto old=find_if(pool.begin(),pool.end(),same);
            if (old!=pool.end()) { if (old->rank<p.rank) *old=move(p); return; }
            int count=0,worst=-1;
            for (int i=0;i<sz(pool.size());++i) if (pool[i].tag==p.tag) {
                ++count; if (worst<0 || pool[i].rank<pool[worst].rank) worst=i;
            }
            if (count<POOL_PER_KIND) pool.push_back(move(p));
            else if (pool[worst].rank<p.rank) pool[worst]=move(p);
        }
        bool wantsPool(const Proposal& p) const {
            int count=0,worst=INF;
            for (const auto& b:pool) if (b.tag==p.tag) {
                if (scopeKey(b)==scopeKey(p) && (p.tag!=ROUTE || b.target==p.target)) return p.rank>b.rank;
                ++count; worst=min(worst,b.rank);
            }
            return count<POOL_PER_KIND || p.rank>worst;
        }
        bool wantsFallback(const Proposal& p,int steps) const {
            if (p.full || p.gain<0 || (!p.gain && !p.received)) return false;
            if (fallback.ops.empty()) return true;
            long long a=static_cast<long long>(p.gain)*sz(fallback.ops.size());
            long long b=static_cast<long long>(fallback.gain)*steps;
            if (a!=b) return a>b;
            a=static_cast<long long>(p.received)*sz(fallback.ops.size());
            b=static_cast<long long>(fallback.received)*steps;
            if (a!=b) return a>b;
            if (p.q!=fallback.q) return p.q>fallback.q;
            return steps<sz(fallback.ops.size());
        }
        void fallbackOffer(const Proposal& p) {
            if (!wantsFallback(p,sz(p.ops.size()))) return;
            fallback.ops=p.ops; fallback.q=p.q; fallback.gain=p.gain; fallback.received=p.received;
        }
        void ordinary(const Board& board) {
            vector<int> values(g.V,0); for (int p:g.floor) values[p]=tours.value(p,packetWord(g,board,p));
            for (int p:g.floor) for (int q=board.cells[p].h;q>=1;--q) {
                timer.check(); auto a=query(board,p,q); int prefixCost=tours.value(p,a.prefix),limit=INF;
                route(board,a,[&](int index,int y,int side,int steps) {

                    // Empty intermediate landings are routing states, not separate
                    // proposals unless source consumption already makes progress.
                    if (!board.cells[y].h && g.nest[y]<0 && !a.sourceReceived) return;
                    int received=0; Word w=landing(board,a,y,side,received);
                    int gain=values[p]+(y==p ? 0 : values[y])-tours.value(y,w)-(y==p ? 0 : prefixCost);
                    Proposal item; item.gain=gain; item.received=received; item.q=q; item.target=y; item.full=w.h==8;
                    roots(item,board,{p,y}); item.rank=gain-steps;
                    if ((w.h==8 || !w.pure()) && gain<=0) {
                        auto profile=tours.profile(w);
                        item.rank+=tours.value(y,w)-min(profile.afterBottom,profile.afterTop)-
                        max(1,min(g.hd(y,profile.bottom),g.hd(y,profile.top)));
                    }
                    if (item.rank<0 && gain<=0 && !received) return;
                    if (!wantsPool(item) && !wantsFallback(item,steps)) return;
                    item.ops=path(a,index); fallbackOffer(item); offer(move(item));
                },limit,false);
            }
        }
        void preparations(const Board& board) {

            for (int p:g.floor) if (board.cells[p].h) {
                timer.check(); int h=board.cells[p].h,run=1;
                while (run<h && g.color[board.cells[p].ids[h-run-1]]==g.color[board.cells[p].ids[h-1]]) ++run;
                vector<int> cuts{h}; if (run!=h) cuts.push_back(run);
                for (int qf:cuts) for (int d=0;d<4;++d) {
                    int u=g.ray[p][d][1]; if (u<0) continue;
                    Small foot{g,board}; Op setup{p,qf,d,1,-1};
                    if (!foot.emit(setup) || !foot.at(u).h || foot.at(u).h>7) continue;
                    for (int axis=0;axis<4;++axis) for (int len=1;len<=8;++len) {
                        int b=g.ray[u][axis][len]; if (b<0) break;
                        if (b==p || !foot.at(b).h) continue;
                        int bh=foot.at(b).h,maxq=min(bh-len+1,8-foot.at(u).h);
                        if (maxq<1) continue;
                        int same=1;
                        while (same<bh && g.color[foot.at(b).ids[bh-same-1]]==g.color[foot.at(b).ids[bh-1]]) ++same;
                        vector<int> incoming{maxq,min(maxq,same),1};
                        sort(incoming.begin(),incoming.end()); incoming.erase(unique(incoming.begin(),incoming.end()),incoming.end());
                        for (int qb:incoming) {
                            Small meet=foot; Op arrive{b,qb,axis^1,len,-1};
                            int oldGot=meet.received;
                            if (!meet.emit(arrive)) continue;
                            Proposal item; item.tag=MEETING; item.ops={setup,arrive}; item.q=qf; item.target=u;
                            item.gain=meet.gain(tours); item.received=meet.received; item.full=meet.at(u).h==8;
                            roots(item,board,{p,u,b}); item.rank=item.gain-2;
                            if (item.full) {
                                auto word=meet.word(u); auto profile=tours.profile(word);
                                item.rank+=tours.value(u,word)-min(profile.afterBottom,profile.afterTop)-
                                max(1,min(g.hd(u,profile.bottom),g.hd(u,profile.top)));
                            }
                            if (item.rank>=0 || item.received) offer(item);
                            // Incoming cargo must still exist as a top suffix.
                            if (meet.received!=oldGot+query(board,b,qb).sourceReceived) continue;
                            int cut=meet.at(u).h-qb;
                            for (int launch=0;launch<4;++launch) for (int jumpLen=2;jumpLen<=cut+1;++jumpLen) {
                                int z=g.ray[u][launch][jumpLen]; if (z<0) break;
                                Small flight=meet; Op jump{u,qb,launch,jumpLen,-1};
                                if (!flight.emit(jump)) continue;
                                Proposal plan; plan.tag=SUPPORT; plan.ops={setup,arrive,jump}; plan.q=qf; plan.target=z;
                                plan.gain=flight.gain(tours); plan.received=flight.received; plan.full=flight.at(z).h==8;
                                roots(plan,board,{p,u,b,z}); plan.rank=plan.gain-3;
                                if (plan.full) {
                                    auto word=flight.word(z); auto profile=tours.profile(word);
                                    plan.rank+=tours.value(z,word)-min(profile.afterBottom,profile.afterTop)-
                                    max(1,min(g.hd(z,profile.bottom),g.hd(z,profile.top)));
                                }
                                if (plan.rank>=0 || plan.received) offer(move(plan));
                            }
                        }
                    }
                }
            }
        }
        Choice select(const Board& board,bool prepare) {
            pool.clear(); beforeCache.clear(); fallback={}; ordinary(board);
            if (prepare) preparations(board);
            // Interleave the three kinds so cheap ordinary merges cannot exclude
            // every support candidate before the rollout work budget is spent.
            stable_sort(pool.begin(),pool.end(),[](const Proposal& a,const Proposal& b) { return a.rank>b.rank; });
            vector<Proposal> ordered;
            for (int i=0;i<POOL_PER_KIND;++i) for (int tag=0;tag<3;++tag) {
                int seen=0;
                for (const auto& p:pool) if (p.tag==tag && seen++==i) { ordered.push_back(p); break; }
            }
            probeLeft=static_cast<long long>(PROBE_CELLS)*g.V; limited=true;
            Choice best;
            for (const auto& p:ordered) {
                if (probeLeft<=0) break;
                trialLeft=min(probeLeft,static_cast<long long>(PROBE_CELLS/3)*g.V);
                timer.check();
                try {
                    auto owned=ownership(p,board); uint64_t key=scopeKey(p);
                    auto it=beforeCache.find(key);
                    if (it==beforeCache.end()) it=beforeCache.emplace(key,closeOwned(board,owned)).first;

                    const auto& before=it->second;
                    Builder preview(g,timer,board); preview.append(p.ops);
                    auto after=closeOwned(preview.board,owned);
                    if (before.foreign!=after.foreign) throw runtime_error("local_control_terminal_mismatch");

                    int saving=sz(before.ops.size())-sz(p.ops.size())-sz(after.ops.size());
                    if (saving<=0 || (best.priced && saving<best.saving)) continue;
                    // Full or uphill preparations are committed through the first
                    // stable, global-Phi-progress boundary of the evaluated plan.
                    int oldPhi=tours.total(board),end=0;
                    auto stable=[&]() {
                        for (int x:g.floor) if (preview.board.cells[x].h>7) return false;
                        int value=tours.total(preview.board);
                        return value<oldPhi || (value==oldPhi && preview.board.remaining<board.remaining);
                    };
                    bool ready=stable();
                    for (int boundary:after.ends) {
                        if (ready) break;
                        while (end<boundary) { const auto& op=after.ops[end++]; preview.emit(op.p,op.q,op.d,op.len); }
                        ready=stable();
                    }
                    if (!ready) throw runtime_error("priced_prefix_no_progress");
                    if (best.priced && saving==best.saving && preview.ops.size()>=best.ops.size()) continue;
                    Choice choice; choice.ops=preview.ops; choice.priced=true;
                    choice.saving=saving;
                    choice.gain=oldPhi-tours.total(preview.board); choice.received=board.remaining-preview.board.remaining;
                    best=move(choice);
                } catch (const ProbeLimit&) { }
            }
            limited=false;
            if (best.priced) return best;

            if (!fallback.ops.empty()) return fallback;
            vector<unsigned char> all(g.M,1); vector<int> ids(g.M); iota(ids.begin(),ids.end(),0);
            auto step=delivery(board,all,ids,true);
            if (step.ops.empty()) throw runtime_error("rollout_no_progress");
            Choice plain; plain.ops=move(step.ops); plain.gain=step.gain; plain.received=step.received;
            return plain;
        }

        void run(bool prepare) {
            macros=0; pool.clear(); beforeCache.clear(); limited=false;
            Builder out(g,timer); int phi=tours.total(out.board);
            while (out.board.remaining) {
                auto choice=select(out.board,prepare); int oldPhi=phi,oldRemaining=out.board.remaining;
                out.append(choice.ops); phi=tours.total(out.board); ++macros;
                if (oldPhi-phi!=choice.gain || oldRemaining-out.board.remaining!=choice.received)
                throw runtime_error("rollout_progress_prediction");
                for (int p:g.floor) if (out.board.cells[p].h>7) throw runtime_error("rollout_unstable_tower");
                if (phi>oldPhi || (phi==oldPhi && out.board.remaining>=oldRemaining)) throw runtime_error("rollout_non_progress");
                if (out.ops.size()>100000) throw runtime_error("rollout_output_limit");

                if (out.board.remaining && macros%16==0) {
                    auto completed=finishCopy(out);

                    solver.keep({move(completed)});
                }
                if (tours.cache.size()>32768) tours.cache.clear();
            }

            solver.keep({out.ops});

            return;
        }
    };

    // Post-construction search. All accepted replacements reconnect to the exact
    // old boundary; colour-equivalent disposable IDs may share search states.
    uint64_t scramble(uint64_t x) {
        x^=x>>30; x*=0xbf58476d1ce4e5b9ULL; x^=x>>27;
        x*=0x94d049bb133111ebULL; return x^(x>>31);
    }
    bool equalBoard(const Board& a,const Board& b) {
        if (a.remaining!=b.remaining || a.loc!=b.loc) return false;
        for (int p=0;p<sz(a.cells.size());++p) {
            if (a.cells[p].h!=b.cells[p].h) return false;
            for (int j=0;j<a.cells[p].h;++j) if (a.cells[p].ids[j]!=b.cells[p].ids[j]) return false;
        }
        return true;
    }
    struct PostStats {
        long long tasks[4]{},saved[4]{};
        double ms[4]{};
        int version=0;
        double beginMs=0;
    };
    struct DisableSlice {
        Timer& timer; double old;
        explicit DisableSlice(Timer& t):timer(t),old(t.slice) { timer.slice=1e100; }
        ~DisableSlice() { timer.slice=old; }
    };
    struct PlanIndex {
        const Grid& g; Timer& timer;
        vector<Op> ops;
        vector<Board> states;
        vector<double> risk;
        vector<pair<int,int>> closed;
        vector<vector<int>> moved;
        vector<vector<int>> components,componentOps;
        PlanIndex(const Grid& grid,Timer& clock,const vector<Op>& trace):g(grid),timer(clock),ops(trace) {
            states.reserve(ops.size()+1); states.emplace_back(g); risk.push_back(0);
            moved.reserve(ops.size());
            vector<unsigned char> active(g.M,0); int open=0,chunk=0;
            vector<int> owner(g.M),cargoOwner(g.M);
            iota(owner.begin(),owner.end(),0); iota(cargoOwner.begin(),cargoOwner.end(),0);
            auto cargoRoot=[&](int id) {
                while (cargoOwner[id]!=id) { cargoOwner[id]=cargoOwner[cargoOwner[id]]; id=cargoOwner[id]; }
                return id;
            };
            auto findRoot=[&](int id) { while (owner[id]!=id) { owner[id]=owner[owner[id]]; id=owner[id]; } return id; };
            for (int i=0;i<sz(ops.size());++i) {
                if ((i&15)==0) timer.check();
                const auto& op=ops[i]; Board next=states.back(); const auto& src=next.cells[op.p];
                int y=g.ray[op.p][op.d][op.len],distance=0;
                int connected=findRoot(src.ids[0]);
                for (int p:{op.p,y}) for (int j=0;j<next.cells[p].h;++j) {
                    int id=findRoot(next.cells[p].ids[j]); if (id!=connected) owner[id]=connected;
                }
                vector<int> ids;
                for (int j=src.h-op.q;j<src.h;++j) {
                    int id=src.ids[j]; ids.push_back(id);
                    distance+=g.hd(op.p,g.color[id])-g.hd(y,g.color[id]);
                    if (!active[id]) { active[id]=1; ++open; }
                }
                int cargo=cargoRoot(ids[0]);
                for (int id:ids) cargoOwner[cargoRoot(id)]=cargo;
                // These feet alone are sufficient for this launch. Other background
                // IDs need not be bound to the complete delivery component.
                int cut=src.h-op.q;
                for (int j=cut-(op.len-1);j<cut;++j) cargoOwner[cargoRoot(src.ids[j])]=cargo;
                Op physical=op; next.apply(g,physical); ops[i]=physical;
                for (int p:{op.p,y}) for (int j=0;j<states.back().cells[p].h;++j) {
                    int id=states.back().cells[p].ids[j];
                    if (next.loc[id]<0 && states.back().loc[id]>=0 && active[id]) { active[id]=0; --open; }
                }
                int got=states.back().remaining-next.remaining;
                risk.push_back(risk.back()+1.0+2.0/static_cast<double>(op.q)+
                0.2*max(0,-distance)+0.25*(got==0));
                moved.push_back(move(ids)); states.push_back(move(next));
                if (!open) { if (i+1>chunk) closed.emplace_back(chunk,i+1); chunk=i+1; }
            }
            if (states.back().remaining) throw runtime_error("post_initial_not_complete");
            map<int,int> groups;
            for (int id=0;id<g.M;++id) {
                int r=findRoot(id); auto [it,inserted]=groups.emplace(r,sz(groups.size()));
                if (inserted) { components.emplace_back(); componentOps.emplace_back(); }
                components[it->second].push_back(id);
            }
            for (int i=0;i<sz(ops.size());++i) componentOps[groups.at(findRoot(moved[i][0]))].push_back(i);
            map<int,int> cargoGroups; vector<vector<int>> cargoIds,cargoOps;
            for (int id=0;id<g.M;++id) {
                auto [it,inserted]=cargoGroups.emplace(cargoRoot(id),sz(cargoGroups.size()));
                if (inserted) { cargoIds.emplace_back(); cargoOps.emplace_back(); }
                cargoIds[it->second].push_back(id);
            }
            for (int i=0;i<sz(ops.size());++i) cargoOps[cargoGroups.at(cargoRoot(moved[i][0]))].push_back(i);
            set<vector<int>> unique(components.begin(),components.end());
            for (int c=0;c<sz(cargoIds.size());++c) {
                if (cargoOps[c].empty() || cargoIds[c].size()>64 || !unique.insert(cargoIds[c]).second) continue;
                components.push_back(move(cargoIds[c])); componentOps.push_back(move(cargoOps[c]));
            }
        }
    };
    struct LocalModel {
        const Grid& g; Timer& timer; TourCost& tours;
        Board start,goal;
        vector<unsigned char> owned;
        vector<int> targetSlot;
        vector<int> ids;
        bool closed=true;
        LocalModel(const Grid& grid,Timer& clock,TourCost& prices,const PlanIndex& index,int first,int last)
        :g(grid),timer(clock),tours(prices),start(index.states[first]),goal(index.states[last]),
        owned(g.M,0),targetSlot(g.M,-1) {
            for (int i=first;i<last;++i) {
                if ((i&15)==0) timer.check();
                for (int id:index.moved[i]) owned[id]=1;
            }
            for (int id=0;id<g.M;++id) {
                if (start.loc[id]>=0 && goal.loc[id]<0) owned[id]=1;
                if (owned[id]) { ids.push_back(id); if (goal.loc[id]>=0) closed=false; }
            }
            for (int p:g.floor) for (int j=0;j<goal.cells[p].h;++j) targetSlot[goal.cells[p].ids[j]]=j;
        }
        LocalModel(const Grid& grid,Timer& clock,TourCost& prices,const Board& initial,const vector<int>& selected)
        :g(grid),timer(clock),tours(prices),start(initial),goal(initial),owned(g.M,0),targetSlot(g.M,-1),ids(selected) {
            for (int id:ids) {
                owned[id]=1;
                if (goal.loc[id]>=0) { goal.loc[id]=-1; --goal.remaining; }
            }
            for (int p:g.floor) {
                auto old=goal.cells[p]; goal.cells[p].h=0;
                for (int j=0;j<old.h;++j) if (!owned[old.ids[j]]) {
                    int id=old.ids[j]; targetSlot[id]=goal.cells[p].h; goal.cells[p].ids[goal.cells[p].h++]=id;
                }
            }
        }
        int token(int id) const { return owned[id] && goal.loc[id]<0?g.color[id]+1:g.K+id+1; }
        uint64_t cellKey(int p,const Tower& t) const {
            uint64_t h=scramble(static_cast<uint64_t>(p+1)*17+static_cast<uint64_t>(t.h));
            for (int j=0;j<t.h;++j) h^=scramble(static_cast<uint64_t>(token(t.ids[j]))*
            0x9e3779b97f4a7c15ULL+static_cast<uint64_t>((p*8+j)*2+1));
            return h;
        }
        uint64_t key(const Board& b) const {
            uint64_t h=0; for (int p:g.floor) h^=cellKey(p,b.cells[p]); return h;
        }
        bool equivalent(const Board& a,const Board& b) const {
            if (a.remaining!=b.remaining) return false;
            for (int p:g.floor) {
                if (a.cells[p].h!=b.cells[p].h) return false;
                for (int j=0;j<a.cells[p].h;++j) if (token(a.cells[p].ids[j])!=token(b.cells[p].ids[j])) return false;
            }
            return true;
        }
        int suffix(const Tower& t) const {
            int q=0; for (int j=t.h-1;j>=0 && owned[t.ids[j]];--j) ++q; return q;
        }
        int borrowHelpers(int anchor,uint64_t salt) {
            int best=-1; double score=1e100;
            for (int p:g.floor) {
                const auto& t=start.cells[p]; if (!t.h || t.h>3 || g.dist[p][anchor]>3) continue;
                bool fresh=true;
                for (int j=0;j<t.h;++j) {
                    int id=t.ids[j];
                    if (owned[id] || goal.loc[id]!=p || targetSlot[id]!=j) fresh=false;
                }
                if (!fresh) continue;
                double rank=g.dist[p][anchor]+0.2*t.h+
                static_cast<double>(scramble(salt^static_cast<uint64_t>(p))%1024)/2048;
                if (rank<score) { score=rank; best=p; }
            }
            if (best<0) return 0;
            for (int j=0;j<start.cells[best].h;++j) { int id=start.cells[best].ids[j]; owned[id]=1; ids.push_back(id); }
            closed=false; return start.cells[best].h;
        }
        struct Estimate { double cargo=0,positions=0; int towers=0,supports=0; };
        Estimate cellEstimate(int p,const Tower& t) const {
            Estimate e; Word disposable; int count=0,alive=0,lastSlot=-1;
            for (int j=0;j<t.h;++j) {
                int id=t.ids[j]; if (!owned[id]) continue; ++count;
                int target=goal.loc[id];
                if (target<0) {
                    disposable.c[disposable.h++]=g.color[id];
                    e.positions+=0.10*g.hd(p,g.color[id]);
                } else {
                    ++alive; e.positions+=g.dist[p][target];
                    if (p==target) {
                        e.cargo+=0.65*abs(j-targetSlot[id]);
                        if (lastSlot>targetSlot[id]) e.cargo+=1.25;
                        lastSlot=targetSlot[id];
                    }
                }
            }
            if (!count) return e;
            e.towers=1; e.supports=min(4,max(0,t.h-1));
            e.cargo+=0.85*tours.value(p,disposable);
            if (alive) e.cargo+=e.positions/static_cast<double>(min(4,count));
            else e.cargo+=e.positions;
            if (t.h==8) e.cargo+=0.25;
            return e;
        }
        Estimate estimate(const Board& b) const {
            Estimate e;
            for (int p:g.floor) { auto c=cellEstimate(p,b.cells[p]);
                e.cargo+=c.cargo; e.positions+=c.positions; e.towers+=c.towers; e.supports+=c.supports; }
            return e;
        }
    };

    // Greedy reception repairs are alternatives to a fixed target beam. They may
    // receive any owned colour first, but never remove an outside or surviving ID.
    struct RepairRouter : RolloutBuilder {
        explicit RepairRouter(Solver& s):RolloutBuilder(s) {}
        Move repairMove(const Board& board,const LocalModel& model,int profile,uint64_t salt,int budget) {
            Move best; double bestRank=1e100; int full=-1;
            vector<int> sources;
            for (int id:model.ids) if (board.loc[id]>=0 &&
            find(sources.begin(),sources.end(),board.loc[id])==sources.end()) sources.push_back(board.loc[id]);
            for (int p:sources) if (board.cells[p].h==8) full=p;
            auto rank=[&](int received,int cost,int gain,int q,int destination) {
                double efficiency=static_cast<double>(cost)/received;
                double noise=static_cast<double>(scramble(salt^static_cast<uint64_t>(destination*64+q))%1024)/1024;
                return efficiency+(profile==0?0.0:profile==1?0.35*noise:0.15*noise)-
                (profile==2?0.018:0.004)*gain-(profile==1?0.012:0.0)*q;
            };
            auto survives=[&](const Query& a,int p,const Word& w) {
                // All IDs below the owned suffix are stationary background. Their
                // old top was stable, so receipt can only consume the owned part.
                const auto& src=board.cells[a.p];
                int sourceCut=src.h-a.q;
                for (int j=a.prefix.h;j<sourceCut;++j) if (!model.owned[src.ids[j]]) return false;
                (void)p; (void)w; return true;
            };
            try { for (int p:sources) {
                    if (full>=0 && p!=full) continue;
                    for (int q=model.suffix(board.cells[p]);q>=1;--q) {
                        auto a=query(board,p,q); int limit=budget;
                        int oldValue=tours.value(p,packetWord(g,board,p));
                        route(board,a,[&](int index,int y,int side,int steps) {
                            if (g.nest[y]<0 && !a.sourceReceived) return;
                            int received=0; Word w=landing(board,a,y,side,received);
                            if (!survives(a,y,w)) return;
                            int gain=oldValue+tours.value(y,packetWord(g,board,y))-
                            tours.value(p,a.prefix)-tours.value(y,w);
                            if (received && w.h<=7) {
                                double score=rank(received,steps,gain,q,y);
                                if (score<bestRank) { bestRank=score; best={path(a,index),gain,received,q,y,0}; }
                            }
                            if (g.nest[y]<0 || w.h<2 || steps>=budget) return;
                            Tower bg=board.cells[y]; if (y==p) bg.h=a.prefix.h;
                            int protect=bg.h-model.suffix(bg);
                            for (int cut=protect+1;cut<w.h;++cut) if (w.c[cut-1]==g.nest[y]) {
                                Word left=w; left.h=cut; int exposed=left.consume(g.nest[y]);
                                for (int d=0;d<4;++d) for (int len=1;len<=cut+1;++len) {
                                    int z=g.ray[y][d][len]; if (z<0) break;
                                    int suffix=w.h-cut; Word target=z==p?a.prefix:packetWord(g,board,z);
                                    if (target.h+suffix>8) continue;
                                    for (int j=w.h-1;j>=cut;--j) target.c[target.h++]=w.c[j];
                                    int got=received+exposed+target.consume(g.nest[z]);
                                    if (target.h>7) continue;
                                    int delta=gain+tours.value(y,w)+(z==p?0:tours.value(z,packetWord(g,board,z)))-
                                    tours.value(y,left)-tours.value(z,target);
                                    double score=rank(got,steps+1,delta,q,z);
                                    if (score<bestRank) {
                                        bestRank=score; best={path(a,index),delta,got,q,z,0};
                                        best.ops.push_back({y,suffix,d,len,-1});
                                    }
                                }
                            }
                        },limit,false);
                    }
                } } catch (const SliceExpired&) {
                if (best.ops.empty()) throw;
            }
            return best;
        }
    };

    struct BeamJob {
        struct Node {
            Board board; int parent=-1,depth=0; Op op{};
            uint64_t hash=0; LocalModel::Estimate value;
            explicit Node(const Board& b):board(b) {}
        };
        struct Edge {
            int parent=0,y=-1,received=0; Op op{}; Tower source,landing;
            uint64_t hash=0; LocalModel::Estimate value;
        };
        struct Ranked { double score; Edge edge; };
        struct HeapOrder { bool operator()(const Ranked& a,const Ranked& b) const { return a.score<b.score; } };
        struct Repair {
            Builder out; vector<Op> prefix; int left=0,profile=0;
            uint64_t salt=0; bool done=false;
            Repair(const Grid& g,Timer& timer,const Board& b,vector<Op> path,int mode,uint64_t seed,
            const LocalModel& model):out(g,timer,b),prefix(move(path)),profile(mode),salt(seed) {
                for (int id:model.ids) left+=b.loc[id]>=0;
            }
        };
        const Grid& g; Timer& timer; RepairRouter& router;
        const PlanIndex& index; LocalModel model;
        int first,last,method,width,profile,component=-1,oldCost,upper,depth=0,parentCursor=0,opCursor=0;
        int oldSeed=0,repairCursor=0; uint64_t salt;
        bool done=false,seedable=true,hasResult=false;
        long long work=0; double usedMs=0;
        vector<Node> nodes; vector<int> frontier;
        array<vector<Ranked>,4> heaps;
        vector<Op> pending,result,oldOps;
        vector<int> repairNodes;
        unique_ptr<Repair> repair;
        unordered_map<uint64_t,int> visited;
        unordered_set<uint64_t> layerSeen;
        unordered_map<uint64_t,vector<int>> checkpoints;
        vector<Board> oldStates;
        struct CachedCell { uint64_t hash=0; LocalModel::Estimate value; };
        vector<CachedCell> cellCache;
        vector<int> cacheStamp;
        int cacheParent=-1,cacheEpoch=0;
        BeamJob(Solver& s,RepairRouter& routes,const PlanIndex& source,
        int begin,int end,int kind,int beamWidth,int mode,uint64_t seed,int group=-1)
        :g(s.g),timer(s.timer),router(routes),index(source),
        model(group<0?LocalModel(g,timer,router.tours,index,begin,end):
        LocalModel(g,timer,router.tours,index.states[begin],index.components[group])),
        first(begin),last(end),method(kind),width(beamWidth),profile(mode),component(group),
        oldCost(group<0?end-begin:sz(index.componentOps[group].size())),upper(oldCost+(s.timer.ms()>=1700?1:-1)),salt(seed) {
            cellCache.resize(g.V); cacheStamp.assign(g.V,0);
            timer.check();
            if (method==1 && profile>=4 && first<sz(index.ops.size())) model.borrowHelpers(index.ops[first].p,salt);
            if (component<0) {
                oldOps.assign(index.ops.begin()+first,index.ops.begin()+last);
                oldStates.reserve(oldOps.size()+1);
                for (int i=first;i<=last;++i) {
                    oldStates.push_back(index.states[i]); checkpoints[model.key(index.states[i])].push_back(i-first);
                }
            } else {
                seedable=false;
                for (int i:index.componentOps[component]) oldOps.push_back(index.ops[i]);
                oldStates.push_back(model.start);
                for (const auto& op:oldOps) {
                    timer.check();
                    Board next=oldStates.back(); const auto& src=next.cells[op.p];
                    if (src.h<op.q || model.suffix(src)<op.q) break;
                    Op physical=op;
                    try { next.apply(g,physical); } catch (const runtime_error&) { break; }
                    bool valid=true;
                    for (int id=0;id<g.M;++id) if (!model.owned[id] && next.loc[id]!=model.start.loc[id]) { valid=false; break; }
                    if (!valid) break;
                    oldStates.push_back(move(next));
                }
                seedable=oldStates.size()==oldOps.size()+1 && equalBoard(oldStates.back(),model.goal);
                if (seedable) for (int i=0;i<sz(oldStates.size());++i) checkpoints[model.key(oldStates[i])].push_back(i);
            }
            Node root(model.start); root.hash=model.key(root.board); root.value=model.estimate(root.board);
            nodes.push_back(move(root)); frontier.push_back(0); visited[nodes[0].hash]=0;
            for (auto& heap:heaps) heap.reserve(static_cast<size_t>(width+1));
            if (method==3 || (model.closed && method==2)) repairNodes.push_back(0);
            connect(0);
        }
        vector<Op> path(int node) const {
            vector<Op> ops;
            while (nodes[node].parent>=0) { ops.push_back(nodes[node].op); node=nodes[node].parent; }
            reverse(ops.begin(),ops.end()); return ops;
        }
        bool terminal(const Board& b) const { return b.remaining==model.goal.remaining && equalBoard(b,model.goal); }
        void offerResult(vector<Op> ops) {
            if (sz(ops.size())>upper || (hasResult && ops.size()>=result.size())) return;
            DisableSlice atomic(timer);
            Builder verify(g,timer,model.start); verify.append(ops);
            if (!terminal(verify.board)) { return; }
            result=move(verify.ops); hasResult=true;
        }
        void connect(int node) {
            const auto& n=nodes[node];
            if (terminal(n.board)) offerResult(path(node));
            if (!seedable) return;
            auto found=checkpoints.find(n.hash); if (found==checkpoints.end()) return;
            for (int at:found->second) if (at>n.depth && model.equivalent(n.board,oldStates[at])) {
                auto ops=path(node); ops.insert(ops.end(),oldOps.begin()+at,oldOps.end()); offerResult(move(ops));
                break;
            }
        }
        const CachedCell& cached(int parent,int p) {
            if (cacheParent!=parent) { cacheParent=parent; ++cacheEpoch; }
            if (cacheStamp[p]!=cacheEpoch) {
                cacheStamp[p]=cacheEpoch;
                const auto& t=nodes[parent].board.cells[p];
                cellCache[p].hash=model.cellKey(p,t); cellCache[p].value=model.cellEstimate(p,t);
            }
            return cellCache[p];
        }
        bool transition(int parent,const Op& op,Edge& e) {
            const auto& n=nodes[parent]; int y=g.ray[op.p][op.d][op.len];
            if (y<0 || n.board.cells[y].h+op.q>8) return false;
            e.parent=parent; e.y=y; e.op=op; e.op.k=n.board.cells[op.p].h-op.q;
            e.source=n.board.cells[op.p]; e.landing=n.board.cells[y];
            for (int j=e.source.h-1;j>=e.op.k;--j) e.landing.ids[e.landing.h++]=e.source.ids[j];
            e.source.h=e.op.k;
            for (int which=0;which<2;++which) {
                int p=which?y:op.p; auto& t=which?e.landing:e.source;
                while (t.h && g.nest[p]==g.color[t.ids[t.h-1]]) {
                    int id=t.ids[t.h-1];
                    if (!model.owned[id]) { return false; }
                    if (model.goal.loc[id]>=0) { return false; }
                    --t.h; ++e.received;
                }
            }
            const auto& oldSource=cached(parent,op.p); const auto& oldLanding=cached(parent,y);
            e.hash=n.hash^oldSource.hash^oldLanding.hash^model.cellKey(op.p,e.source)^model.cellKey(y,e.landing);
            auto found=visited.find(e.hash);
            if (found!=visited.end() && found->second<=n.depth+1) { return false; }
            if (!layerSeen.insert(e.hash).second) { return false; }
            const auto& a=oldSource.value; const auto& b=oldLanding.value;
            auto c=model.cellEstimate(op.p,e.source),d=model.cellEstimate(y,e.landing);
            e.value=n.value;
            e.value.cargo+=c.cargo+d.cargo-a.cargo-b.cargo;
            e.value.positions+=c.positions+d.positions-a.positions-b.positions;
            e.value.towers+=c.towers+d.towers-a.towers-b.towers;
            e.value.supports+=c.supports+d.supports-a.supports-b.supports;
            return true;
        }
        int materialize(const Edge& e) {
            Node next(nodes[e.parent].board); next.parent=e.parent; next.depth=nodes[e.parent].depth+1;
            next.op=e.op; next.hash=e.hash; next.value=e.value;
            next.board.cells[e.op.p]=e.source; next.board.cells[e.y]=e.landing;
            for (int p:{e.op.p,e.y}) for (int j=0;j<nodes[e.parent].board.cells[p].h;++j)
            next.board.loc[nodes[e.parent].board.cells[p].ids[j]]=-1;
            for (int p:{e.op.p,e.y}) for (int j=0;j<next.board.cells[p].h;++j) next.board.loc[next.board.cells[p].ids[j]]=p;
            next.board.remaining-=e.received;
            auto found=visited.find(next.hash);
            if (found==visited.end() || found->second>next.depth) visited[next.hash]=next.depth;
            nodes.push_back(move(next));  return sz(nodes.size())-1;
        }
        double score(const Edge& e,int lane) const {
            const auto& v=e.value; double noise=static_cast<double>(scramble(e.hash^salt)%4096)/4096.0;
            if (lane==0) return v.cargo+0.08*v.positions;
            if (lane==1) return 0.23*v.positions+0.30*v.towers;
            if (lane==2) return 0.85*v.cargo-0.24*v.supports;
            return (profile&1?0.85:1.1)*v.cargo+0.9*noise-0.12*e.op.q;
        }
        void retain(const Edge& e) {
            for (int lane=0;lane<4;++lane) {
                auto& heap=heaps[lane]; Ranked r{score(e,lane),e};
                if (sz(heap.size())<width) { heap.push_back(move(r)); push_heap(heap.begin(),heap.end(),HeapOrder{}); }
                else if (r.score<heap.front().score) {
                    pop_heap(heap.begin(),heap.end(),HeapOrder{}); heap.back()=move(r); push_heap(heap.begin(),heap.end(),HeapOrder{});
                }
            }
        }
        void moves(int node) {
            pending.clear(); opCursor=0; const auto& board=nodes[node].board;
            vector<int> sources;
            for (int id:model.ids) if (board.loc[id]>=0) sources.push_back(board.loc[id]);
            sort(sources.begin(),sources.end()); sources.erase(unique(sources.begin(),sources.end()),sources.end());
            for (int p:sources) for (int q=model.suffix(board.cells[p]);q>=1;--q) {
                int range=board.cells[p].h-q+1;
                for (int d=0;d<4;++d) for (int len=1;len<=range;++len) {
                    int y=g.ray[p][d][len]; if (y<0) break;
                    if (board.cells[y].h+q<=8) pending.push_back({p,q,d,len,-1});
                }
            }
        }
        void nextLayer() {
            DisableSlice atomic(timer);
            ++depth; vector<int> next; unordered_set<uint64_t> selected;
            for (auto& heap:heaps) sort(heap.begin(),heap.end(),[](const Ranked& a,const Ranked& b){return a.score<b.score;});
            for (int i=0;i<width && sz(next.size())<width;++i) for (int lane=0;lane<4 && sz(next.size())<width;++lane) {
                if (i>=sz(heaps[lane].size())) continue;
                const auto& e=heaps[lane][i].edge;
                if (selected.insert(e.hash).second) { int id=materialize(e); connect(id); next.push_back(id); }
            }
            if (depth<sz(oldStates.size()) && depth<upper) {
                Node seed(oldStates[depth]); seed.parent=oldSeed; seed.depth=depth; seed.op=oldOps[depth-1];
                seed.hash=model.key(seed.board); seed.value=model.estimate(seed.board);
                nodes.push_back(move(seed)); oldSeed=sz(nodes.size())-1;
                if (selected.insert(nodes[oldSeed].hash).second) next.push_back(oldSeed);
            }
            for (auto& heap:heaps) heap.clear();
            layerSeen.clear();
            frontier=move(next); parentCursor=0; pending.clear(); opCursor=0;
            if (model.closed && (depth==2 || depth%4==0)) {
                repairNodes.clear(); repairCursor=0;
                vector<pair<double,int>> ranks;
                for (int node:frontier) ranks.emplace_back(nodes[node].value.cargo,node);
                sort(ranks.begin(),ranks.end());
                for (int i=0;i<min(2,sz(ranks.size()));++i) repairNodes.push_back(ranks[i].second);
            }
            if (frontier.empty() || depth>=min(upper,72) || nodes.size()>=1536 || visited.size()>=262144) done=true;
        }
        void continueRepair() {
            if (!repair) {
                if (repairCursor>=sz(repairNodes.size())) return;
                int node=repairNodes[repairCursor++];
                repair=make_unique<Repair>(g,timer,nodes[node].board,path(node),profile%3,
                salt^static_cast<uint64_t>(node),model);
            }
            repair->left=0;
            for (int id:model.ids) repair->left+=repair->out.board.loc[id]>=0;
            while (!repair->done && repair->left) {
                timer.check();
                if (sz(repair->prefix.size()+repair->out.ops.size())>=upper) { repair->done=true; break; }
                auto step=router.repairMove(repair->out.board,model,repair->profile,
                repair->salt^static_cast<uint64_t>(repair->out.ops.size()),upper-sz(repair->prefix.size()+repair->out.ops.size()));
                if (step.ops.empty()) { repair->done=true; break; }
                int before=repair->out.board.remaining;
                for (const auto& op:step.ops) {
                    if (model.suffix(repair->out.board.cells[op.p])<op.q) { repair->done=true; break; }
                    repair->out.emit(op.p,op.q,op.d,op.len);
                }
                if (repair->done) break;
                int got=before-repair->out.board.remaining;
                if (got<=0 || got!=step.received) { repair->done=true; break; }
                repair->left-=got;
            }
            if (!repair->left) {
                auto ops=repair->prefix; ops.insert(ops.end(),repair->out.ops.begin(),repair->out.ops.end()); offerResult(move(ops));
                repair->done=true;
            }
            if (repair->done) repair.reset();
        }
        void advance(double quantum) {
            auto began=Clock::now(); timer.slice=min(timer.deadline,timer.ms()+quantum);
            try {
                for (;;) {
                    if (hasResult && sz(result.size())<oldCost) break;
                    timer.check();
                    if (repair || repairCursor<sz(repairNodes.size())) { continueRepair(); continue; }
                    if (method==3) { done=true; break; }
                    if (done) break;
                    if (parentCursor>=sz(frontier.size())) { nextLayer(); continue; }
                    int parent=frontier[parentCursor];
                    if (pending.empty()) { moves(parent); }
                    while (opCursor<sz(pending.size())) {
                        if ((work++&31)==0) timer.check();
                        Op op=pending[opCursor++];  Edge e;
                        if (transition(parent,op,e)) retain(e);
                    }
                    ++parentCursor; pending.clear(); opCursor=0;
                }
            } catch (const SliceExpired&) {}
            catch (const TimeExpired&) { timer.slice=1e100; usedMs+=elapsed(began); throw; }
            timer.slice=1e100; usedMs+=elapsed(began);
        }
    };

    struct PostSearch {
        Solver& solver; const Grid& g; Timer& timer;
        RepairRouter router; PostStats stats;
        unique_ptr<PlanIndex> index;
        array<unique_ptr<BeamJob>,4> jobs;
        uint64_t random=0x4b1d5f7a9e3779b9ULL;
        int round=0,routeCursor=0,routeVersion=-1;
        double lastBest=0;
        set<tuple<int,int,int,int,int>> requested;
        unordered_map<uint64_t,int> tried;
        vector<int> focusVisits;
        double runStart=0;
        explicit PostSearch(Solver& s):solver(s),g(s.g),timer(s.timer),router(s),focusVisits(g.V,0) {
            stats.beginMs=timer.ms(); lastBest=stats.beginMs;
        }
        uint64_t nextRandom() { random+=0x9e3779b97f4a7c15ULL; return scramble(random); }
        double unit() { return static_cast<double>(nextRandom()%1048576)/1048576.0; }
        void refresh(const vector<Op>& ops) {
            auto next=make_unique<PlanIndex>(g,timer,ops);
            for (auto& job:jobs) { job.reset(); }
            index=move(next); requested.clear(); ++stats.version; routeCursor=0; routeVersion=-1;

        }
        bool replace(int first,int last,int component,const vector<Op>& block,int method) {
            timer.check(); vector<Op> candidate;
            if (component<0) {
                candidate.insert(candidate.end(),index->ops.begin(),index->ops.begin()+first);
                candidate.insert(candidate.end(),block.begin(),block.end());
                candidate.insert(candidate.end(),index->ops.begin()+last,index->ops.end());
            } else {
                vector<unsigned char> removed(index->ops.size(),0);
                for (int i:index->componentOps[component]) removed[i]=1;
                candidate.insert(candidate.end(),index->ops.begin(),index->ops.begin()+first);
                candidate.insert(candidate.end(),block.begin(),block.end());
                for (int i=first;i<sz(index->ops.size());++i) if (!removed[i]) candidate.push_back(index->ops[i]);
            }
            int old=sz(index->ops.size()),now=sz(candidate.size()),delta=now-old;
            bool same=candidate.size()==index->ops.size();
            if (same) for (int i=0;i<now;++i) {
                const auto& a=candidate[i]; const auto& b=index->ops[i];
                if (tie(a.p,a.q,a.d,a.len)!=tie(b.p,b.q,b.d,b.len)) { same=false; break; }
            }
            if (same) return false;
            bool accept=delta<0;
            double progress=min(1.0,(timer.ms()-stats.beginMs)/max(1.0,1900.0-stats.beginMs));
            double temperature=1.5*(1.0-progress)+0.12;
            int slack=max(2,min(8,sz(solver.best.ops.size())/80));
            if (!accept && method && progress>0.75 && delta<=1 && now<=sz(solver.best.ops.size())+slack)
            accept=unit()<(delta==0?0.04:0.015*exp(-delta/temperature));
            if (!accept) return false;
            Builder checked(g,timer); checked.ops.reserve(candidate.size());
            try {
                for (int i=0;i<now;++i) {
                    if ((i&31)==0) timer.check();
                    const auto& op=candidate[i]; checked.emit(op.p,op.q,op.d,op.len);
                }
            } catch (const runtime_error&) {
                return false;
            }
            if (checked.board.remaining) { return false; }
            int previousBest=sz(solver.best.ops.size());
            solver.keep({checked.ops});
            int saving=previousBest-sz(solver.best.ops.size());
            stats.saved[method]+=saving;
            if (saving) {
                lastBest=timer.ms();

            }

            auto nextIndex=make_unique<PlanIndex>(g,timer,checked.ops);

            for (auto& job:jobs) { job.reset(); }
            index=move(nextIndex); requested.clear(); ++stats.version; routeCursor=0; routeVersion=-1;
            return true;
        }
        void shortcut(double quantum) {
            auto start=Clock::now(); timer.slice=min(timer.deadline,timer.ms()+quantum);
            try {
                if (routeVersion!=stats.version) { routeVersion=stats.version; routeCursor=0; }
                while (routeCursor<sz(index->ops.size())) {
                    timer.check(); int first=routeCursor++,last=first+1;
                    vector<int> cargo=index->moved[first]; sort(cargo.begin(),cargo.end());
                    for (int i=first+1;i<min(sz(index->ops.size()),first+32);++i) {
                        auto next=index->moved[i]; sort(next.begin(),next.end());
                        if (cargo!=next) break;
                        last=i+1; if (index->states[i].remaining!=index->states[i+1].remaining) break;
                    }
                    if (last-first<2) continue;
                    const auto& initial=index->states[first]; int p=index->ops[first].p,q=index->ops[first].q;
                    int y=g.ray[index->ops[last-1].p][index->ops[last-1].d][index->ops[last-1].len];
                    auto a=router.query(initial,p,q); a.orientations=q>1?2:1;
                    int goal=y*a.orientations+(a.orientations==2?(last-first)%2:0),end=-1,limit=last-first-1;

                    router.route(initial,a,[&](int at,int,int,int){ if (at==goal) end=at; },limit,false);
                    if (end<0) continue;
                    auto block=router.path(a,end); Builder test(g,timer,initial); test.append(block);
                    if (!equalBoard(test.board,index->states[last])) continue;
                    timer.slice=1e100;
                    if (replace(first,last,-1,test.ops,0)) break;
                    timer.slice=min(timer.deadline,timer.ms()+max(0.1,quantum-elapsed(start)));
                }
            } catch (const SliceExpired&) {}
            catch (const TimeExpired&) { timer.slice=1e100;  throw; }
            timer.slice=1e100;
        }
        int scopeSize(int first,int last) const {
            vector<unsigned char> selected(g.M,0); int count=0;
            for (int i=first;i<last;++i) for (int id:index->moved[i]) if (!selected[id]) { selected[id]=1; ++count; }
            return count;
        }
        unique_ptr<BeamJob> create(int method) {
            int actions=sz(index->ops.size()); if (actions<2) return nullptr;
            int profile=round%8;
            for (int attempt=0;attempt<16;++attempt) {
                timer.check(); int first=0,last=actions,component=-1,ids=0;
                if (method==1) {
                    const int lengths[4]={10,16,24,32}; int length=min(actions,lengths[(round+attempt)%4]);
                    double bestRank=-1;
                    for (int trial=0;trial<8;++trial) {
                        int begin=static_cast<int>(nextRandom()%static_cast<uint64_t>(actions-length+1));
                        int n=scopeSize(begin,begin+length);
                        double value=(index->risk[begin+length]-index->risk[begin])*(0.7+0.6*unit())/
                        ((1+0.012*n*n)*(1+0.08*focusVisits[index->ops[begin].p]));
                        if (value>bestRank) { bestRank=value; first=begin; last=begin+length; ids=n; }
                    }
                    if (ids>40) continue;
                } else {
                    bool chosen=false;
                    if (!index->components.empty() && nextRandom()%3!=0) {
                        double bestRank=-1;
                        for (int group=0;group<sz(index->components.size());++group) {
                            int n=sz(index->components[group].size()),cost=sz(index->componentOps[group].size());
                            if (n>(method==3?64:40) || cost<4) continue;
                            if (index->componentOps[group].empty()) continue;
                            int start=index->componentOps[group].front(),finish=index->componentOps[group].back()+1;
                            auto target=make_tuple(method,start,finish,group,profile);
                            if (requested.count(target)) continue;
                            double value=static_cast<double>(cost)*(0.6+unit())/
                            ((1+0.018*n*n)*(1+0.06*focusVisits[index->ops[start].p]));
                            if (value>bestRank) { bestRank=value; component=group; ids=n; chosen=true; }
                        }
                    }
                    if (!chosen && !index->closed.empty() && nextRandom()%2==0) {
                        int at=static_cast<int>(nextRandom()%index->closed.size());
                        int count=1+static_cast<int>(nextRandom()%3);
                        first=index->closed[at].first; last=index->closed[min(sz(index->closed.size())-1,at+count-1)].second;
                        ids=scopeSize(first,last); chosen=last-first>=4 && ids<=(method==3?64:40);
                    }
                    if (!chosen) {
                        int cap=g.M<=40?g.M:(method==3?32:24),anchor=0;
                        while (anchor<actions && index->states[anchor].remaining>cap) ++anchor;
                        if (anchor>=actions-2) continue;
                        int range=max(1,(actions-anchor)*2/3);
                        first=anchor+static_cast<int>(nextRandom()%static_cast<uint64_t>(range)); last=actions;
                        ids=index->states[first].remaining;
                    }
                }
                int width=ids<=12?32:ids<=24?24:16;
                if (component>=0) {
                    first=index->componentOps[component].front(); last=index->componentOps[component].back()+1;
                }
                auto request=make_tuple(method,first,last,component,profile);
                if (!requested.insert(request).second) { continue; }
                auto job=make_unique<BeamJob>(solver,router,*index,first,last,method,width,profile,nextRandom(),component);
                if (method!=1 && !job->model.closed) { continue; }
                uint64_t signature=job->nodes[0].hash^scramble(job->model.key(job->model.goal))^
                scramble(static_cast<uint64_t>(job->oldCost)*64+static_cast<uint64_t>(method*8+profile));
                if (tried[signature]>=2) continue;
                ++tried[signature]; ++focusVisits[index->ops[first].p];
                ++stats.tasks[method]; return job;
            }
            return nullptr;
        }
        void beamStep(int method) {
            AddTime methodTime{stats.ms[method]};
            if (!jobs[method]) jobs[method]=create(method);

            if (!jobs[method]) { ++stats.tasks[method];  return; }
            auto& job=*jobs[method]; job.advance(method==2?10.0:6.0);

            if (job.hasResult) {
                int first=job.first,last=job.last,component=job.component; auto block=job.result;
                if (replace(first,last,component,block,method)) return;
                job.hasResult=false; job.result.clear();
            }
            if (job.done || job.usedMs>(method==2?42.0:method==1?30.0:12.0)) { jobs[method].reset(); }
        }
        void run() {
            refresh(solver.best.ops); runStart=timer.ms();

            for (;;) {
                timer.check(); ++round;
                if (sz(index->ops.size())<2) break;
                if (round%24==0 && index->ops.size()>solver.best.ops.size() && timer.ms()-lastBest>40) {
                    refresh(solver.best.ops);
                }
                if (routeCursor<sz(index->ops.size()) && (round==1 || round%12==0)) { shortcut(5.0); continue; }
                int method=1; double best=-1;
                for (int k=1;k<4;++k) {
                    double total=stats.ms[1]+stats.ms[2]+stats.ms[3];
                    double score=static_cast<double>(stats.saved[k]+1)/(stats.ms[k]+60)+
                    0.008*sqrt(std::log(2+total/6)/(1+stats.ms[k]/6));
                    if (k==3 && stats.ms[k]>max(8.0,0.04*(timer.ms()-runStart))) score*=0.08;
                    if (score>best) { best=score; method=k; }
                }
                beamStep(method);
            }
        }

    };

} // namespace ahc

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    ahc::Timer timer;
    try {
        ahc::Grid grid; grid.read(); grid.prepare(timer);
        ahc::Solver solver(grid,timer);
        ahc::Builder insurance(grid,timer);
        for (int id=0;id<grid.M;++id) insurance.direct(grid.origin[id],1,grid.color[id]);
        if (insurance.board.remaining) throw runtime_error("insurance_incomplete");
        solver.keep({move(insurance.ops)});
        timer.enabled=true;
        bool baselineStopped=false;
        auto stage=[&](const char* name,auto&& work) {
            if (baselineStopped) return;
            timer.slice=1e100;
            if (string_view(name)=="frame_dp") timer.slice=min(timer.deadline,timer.ms()+100);
            if (string_view(name)=="packet_pure") timer.slice=min(timer.deadline,timer.ms()+180);
            if (string_view(name)=="packet_compound") timer.slice=min(timer.deadline,timer.ms()+220);
            try { timer.check(); work(); }
            catch (const ahc::TimeExpired&) { baselineStopped=true; }
            catch (const ahc::SliceExpired&) {}
            catch (const exception&) {}
            timer.slice=1e100;
        };
        if (grid.M<=32) stage("frame_dp",[&] { solver.run(); });
        ahc::RolloutBuilder rollout(solver); ahc::PacketBuilder packets(solver);
        stage("packet_rollout",[&] { rollout.run(false); });
        stage("packet_mixed",[&] { packets.run(true); });
        if (grid.M<=96) stage("packet_pure",[&] { packets.run(false); });
        if (timer.ms()<800) stage("packet_compound",[&] { rollout.run(true); });
        if (!baselineStopped) {
            try { timer.check(); }
            catch (const ahc::TimeExpired&) { baselineStopped=true; }
        }
        timer.deadline=1900; timer.slice=1e100;
        ahc::PostSearch post(solver);
        try { post.run(); }
        catch (const ahc::TimeExpired&) {}
        catch (const exception&) {}
        timer.enabled=false; timer.slice=1e100;
        cout.write(solver.outputText.data(),static_cast<streamsize>(solver.outputText.size()));
        cout.flush();
    } catch (...) { return 1; }
    return 0;
}
