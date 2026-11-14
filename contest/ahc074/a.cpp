// AHC074 exp_038 guard: expanded adaptive exact local BFS + multi-group bridges + dynamic restore-position search.
// New: choose among all legal owner-group positions when returning an active bridge,
// try completing the actual current request from each position, and compare the
// current move count plus expected future local cost. 192-sheet visibility is checked.
// Default configuration: exp037 100-seed best, all inputs consumed online.
#include <bits/stdc++.h>
using namespace std;
using u64=uint64_t;
using Clock=chrono::steady_clock;
struct Pat {int cv=0,bv=0;};
struct Graph{
 int b=0,n=0,S=0,goal=0; vector<Pat> pat;
 vector<array<uint8_t,8>> perm;
 vector<array<uint32_t,64>> adj;
 vector<uint8_t> vis,shown;
 vector<vector<uint8_t>> dist;
 vector<uint32_t> off,pred;
 array<int,128> repr{};vector<vector<int>> cost;
};
int EXTRA_A=9,EXTRA_B=4,EXTRA_C=17;string LAYOUT="";
static map<tuple<int,int,int>,shared_ptr<Graph>> cache;
static inline uint64_t packed(const array<uint8_t,8>&p,int n){uint64_t k=0;for(int i=0;i<n;i++)k=(k<<4)|p[i];return k;}
static vector<Pat> makePattern(int b,int n,int motif){
 vector<Pat> p; p.push_back({(1<<b)-1,0});
 for(int i=0;i<b;i++)p.push_back({1<<i,1<<i});
 int full=(1<<b)-1;
 for(int k=0;(int)p.size()<n;k++){
  int bits=0,cover=full;
  if(motif==0){for(int j=0;j<b;j++)if(((j+k)&1)==0)bits|=1<<j;}
  if(motif==1){for(int j=0;j<b;j++)if((j+k)%3==0)bits|=1<<j;}
  if(motif==2){for(int j=0;j<b;j++)if((j+(k*2))%3==0)bits|=1<<j; cover=full^(1<<((k+1)%b));}
  if(motif==3){for(int j=0;j<b;j++)if((j+k)%3!=0)bits|=1<<j;}
  if(motif==4){bits=((1<<b)-1)^(1<<(k%b));}
  if(motif==5){bits=1<<(k%b);cover=full^(1<<((k+1)%b));bits&=cover;}
  if(motif==6){bits=(k==0?EXTRA_A:k==1?EXTRA_B:EXTRA_C)&full;cover=full;}
  p.push_back({cover,bits&cover});
 }
 return p;
}
static shared_ptr<Graph> getGraph(int b,int n,int motif){
 auto key=make_tuple(b,n,motif);auto it=cache.find(key);if(it!=cache.end())return it->second;
 auto st=Clock::now();auto x=make_shared<Graph>();x->b=b;x->n=n;x->goal=1<<b;x->pat=makePattern(b,n,motif);
 array<uint8_t,8> p{};for(int i=0;i<n;i++)p[i]=i;
 do{x->perm.push_back(p);}while(next_permutation(p.begin(),p.begin()+n));
 x->S=x->perm.size();int S=x->S;
 unordered_map<u64,int> ids;ids.reserve(S*2);for(int i=0;i<S;i++)ids.emplace(packed(x->perm[i],n),i);
 x->adj.resize(S);x->vis.resize(S);x->shown.resize(S);
 vector<uint32_t> deg(S);
 for(int s=0;s<S;s++){
  auto &o=x->perm[s];int cover=0,shown=0,vis=0;
  for(int i=0;i<n;i++){auto &a=x->pat[o[i]];if(a.cv&~cover)vis|=1<<i;shown|=a.bv&~cover;cover|=a.cv;}
  x->shown[s]=shown;x->vis[s]=vis;
  for(int i=0;i<n;i++)if(vis>>i&1)for(int d=0;d<n;d++)if(d!=i){
   auto t=o;int k=t[i];if(i<d)for(int j=i;j<d;j++)t[j]=t[j+1];else for(int j=i;j>d;j--)t[j]=t[j-1];t[d]=k;
   int ns=ids.at(packed(t,n));x->adj[s][i*8+d]=ns;deg[ns]++;
  }
 }
 x->off.resize(S+1);for(int s=0;s<S;s++)x->off[s+1]=x->off[s]+deg[s];x->pred.resize(x->off[S]);vector<uint32_t> cur=x->off;
 for(int s=0;s<S;s++)for(int i=0;i<n;i++)if(x->vis[s]>>i&1)for(int d=0;d<n;d++)if(d!=i){int ns=x->adj[s][i*8+d];x->pred[cur[ns]++]=s;}
 x->dist.assign(x->goal,vector<uint8_t>(S,255));vector<uint32_t>q(S);
 for(int t=0;t<x->goal;t++){
  auto &ds=x->dist[t];uint32_t head=0,tail=0;
  for(int s=0;s<S;s++)if(x->shown[s]==t){ds[s]=0;q[tail++]=s;}
  while(head<tail){int s=q[head++];uint8_t d=ds[s]+1;
   for(int z=x->off[s];z<x->off[s+1];z++){int prev=x->pred[z];if(ds[prev]==255){ds[prev]=d;q[tail++]=prev;}}
  }
  if(tail!=(uint32_t)S){cerr<<"GRAPH UNREACHABLE b="<<b<<" n="<<n<<" motif="<<motif<<" t="<<t<<" count="<<tail<<"\n";exit(2);}
 }
 for(int t=0;t<x->goal;t++){
  int bs=-1,bc=INT_MAX;
  for(int s=0;s<S;s++)if(x->shown[s]==t){int c=0;for(int u=0;u<x->goal;u++)c+=x->dist[u][s];if(c<bc){bc=c;bs=s;}}
  x->repr[t]=bs;
 }
 x->cost.assign(x->goal,vector<int>(x->goal));
 for(int a=0;a<x->goal;a++)for(int b0=0;b0<x->goal;b0++)x->cost[a][b0]=x->dist[b0][x->repr[a]];
 cache.emplace(key,x);
 cerr<<"GRAPH b="<<b<<" n="<<n<<" motif="<<motif<<" states="<<S<<" edges="<<x->off[S]<<" build_ms="<<chrono::duration<double,milli>(Clock::now()-st).count()<<"\n";
 return x;
}
struct Group{int b=0,n=0,motif=0,offset=0; vector<int> bits;shared_ptr<Graph> gr;vector<int>order,physical;int st=0;vector<uint32_t>future;};
int N=64,K=192,P=256,R=1024,BN=5,EMIN=2,EMAX=2,BUDGET=192,MAX_STATES=40320,MOTIF=1,BITITER=50000,ALLOC=0,SEED=0;
bool offline=false,stats=false,multi=false;
int RESTORE_BRANCH=3,RESTORE_BRANCH_W=2,RESTORE_BRANCH_EVAL=0,RESTORE_FORCE_PID=-1,RESTORE_FORCE_POS=-1;
int RESTORE_DEPTH=1,RESTORE_W=1,RESTORE_SCALE=4,RESTORE_CHANGES=0;
int TIME_GUARD=1820,BR_KEEP=0,BR_KEEP_PENALTY=0,BR_KEEP_USED=0,BR_KEEP_EVAL=0,BR_RETARGET=2,BR_RETARGET_USED=0,BR_RETARGET_EVAL=0,KEEP_RESTORE=1;
int BR_RELAX=1,BR_EMBED=1,BR_EMBED_USED=0,FUT_WEIGHT=1,FUT_SLACK=0,FUT_USED=0,FUT_EXTRAMOVES=0,BR_EXPOSE=1,BR_EXPOSE_CAP=8,BR_DEPTH=1,INTRA=1,HILL=2,BR_PAIR=0,DESIGN_K=80,DESIGN_WEIGHT2=20,MASK_SCAN=1,RESTARTS=1; int JOINT=0,JOINT_BUDGET=103,JOINT_MIN_B=4,JOINT_MAX_B=6,JOINT_SEED=0;
long long jointAccepted=0,jointWidthAccepted=0,jointPaperAccepted=0;
vector<Group> groups;vector<u64> targets;vector<int> bitassign;
static u64 seedrng;
static u64 rng(){u64 z=(seedrng+=0x9e3779b97f4a7c15ull);z=(z^(z>>30))*0xbf58476d1ce4e5b9ull;z=(z^(z>>27))*0x94d049bb133111ebull;return z^(z>>31);}
static int targetFor(const Group &g,u64 p){int v=0;for(int i=0;i<g.b;i++)v|=((p>>g.bits[i])&1ull)<<i;return v;}
static int computeCost(const vector<int>&bits,int b,const Graph&g){
 vector<int>f(1<<b);for(auto p:targets){int v=0;for(int i=0;i<b;i++)v|=((p>>bits[i])&1ull)<<i;f[v]++;}
 int64_t cost=0;for(int a=0;a<(1<<b);a++)for(int c=0;c<(1<<b);c++)cost+=int64_t(f[a])*f[c]*g.cost[a][c];return int(cost/128);
}
static void allocate(){
 groups.clear();vector<int> sizes;if(LAYOUT.empty()){for(int i=0;i<64;){int b=min(BN,64-i);sizes.push_back(b);i+=b;}}else{stringstream ss(LAYOUT);string x;while(getline(ss,x,','))sizes.push_back(stoi(x));}int pos=0;for(int b:sizes){if(b<1||b>7||pos+b>64){cerr<<"BAD LAYOUT"<<endl;abort();}Group g;g.b=b;g.n=b+1;g.motif=MOTIF;for(int j=0;j<b;j++)g.bits.push_back(bitassign[pos+j]);groups.push_back(move(g));pos+=b;}if(pos!=64){cerr<<"BAD LAYOUT SUM="<<pos<<endl;abort();}
 int G=groups.size();int len=0;for(auto &g:groups)len+=g.n;int minBudget=len;
 for(auto &g:groups){
  int maxn=min(8,g.b+1+EMAX),minn=g.b+1+EMIN;
  while(g.n<minn && g.n<maxn && (int)tgamma(g.n+2)<=MAX_STATES)g.n++;
 }
 len=0;for(auto &g:groups)len+=g.n;
 if(len>BUDGET){cerr<<"BUDGET insufficient minimum="<<len<<" cap="<<BUDGET<<"\n";exit(4);}
 if(ALLOC==0){for(auto &g:groups){int mx=min(8,g.b+1+EMAX);while(g.n<mx&&(int)tgamma(g.n+2)<=MAX_STATES && len<BUDGET){g.n++;len++;}}}
 else{
  for(;;){double bg=0;int best=-1;
   for(int i=0;i<G;i++){
    auto &g=groups[i];if(g.n>=min(8,g.b+1+EMAX) || len>=BUDGET || (int)tgamma(g.n+2)>MAX_STATES)continue;
    int c1=computeCost(g.bits,g.b,*getGraph(g.b,g.n,MOTIF));
    int c2=computeCost(g.bits,g.b,*getGraph(g.b,g.n+1,MOTIF));
    double gain=c1-c2;
    if(gain>bg){bg=gain;best=i;}
   }
   if(best<0)break;groups[best].n++;len++;
  }
 }
 int offset=0;for(auto &g:groups){g.offset=offset;offset+=g.n;g.gr=getGraph(g.b,g.n,MOTIF);}
 cerr<<"ALLOC base="<<minBudget<<" used="<<len<<" groups="<<G<<" maxstates="<<MAX_STATES<<" n=";for(auto &g:groups)cerr<<g.n<<",";cerr<<"\n";
}
static void optimizeBits(){
 bitassign.resize(64);iota(bitassign.begin(),bitassign.end(),0);if(BITITER==0)return;
 for(auto &g:groups){g.gr=getGraph(g.b,g.n,MOTIF);}
 auto update=[&](int gidx){int k=0;for(auto &g:groups){if(k<=gidx && gidx<k+g.b){return computeCost(vector<int>(bitassign.begin()+k,bitassign.begin()+k+g.b),g.b,*g.gr);}k+=g.b;}return 0;};
 vector<int> costs(groups.size());int cv=0;for(int i=0;i<(int)groups.size();i++){int start=0;for(int j=0;j<i;j++)start+=groups[j].b;costs[i]=computeCost(vector<int>(bitassign.begin()+start,bitassign.begin()+start+groups[i].b),groups[i].b,*groups[i].gr);cv+=costs[i];}
 int best=cv;vector<int> bestBits=bitassign;
 for(int iter=0;iter<BITITER;iter++){
  int i=rng()%64,j=rng()%63;if(j>=i)j++;int a=-1,b=-1,k=0;for(int gg=0;gg<(int)groups.size();gg++){if(i>=k&&i<k+groups[gg].b)a=gg;if(j>=k&&j<k+groups[gg].b)b=gg;k+=groups[gg].b;}
  if(a==b && !INTRA)continue;swap(bitassign[i],bitassign[j]);
  int ca=update(i),cb=(a==b?0:update(j)),delta=ca+cb-costs[a]-(a==b?0:costs[b]);double temp=500.*pow(0.001,double(iter)/BITITER);
  if(delta<=0 || ((rng()>>11)*(1.0/9007199254740992.0))<exp(-double(delta)/temp)){
   costs[a]=ca;if(a!=b)costs[b]=cb;cv+=delta;if(cv<best){best=cv;bestBits=bitassign;}
  }else swap(bitassign[i],bitassign[j]);
 }
 bitassign=bestBits;
 if(HILL>0){
   vector<int>gid(64);int k=0;for(int g=0;g<(int)groups.size();g++)for(int z=0;z<groups[g].b;z++)gid[k++]=g;
   for(int g=0;g<(int)groups.size();g++){int pos=0;for(int h=0;h<g;h++)pos+=groups[h].b;costs[g]=update(pos);}
   vector<pair<int,int>>pairs;for(int i=0;i<64;i++)for(int j=i+1;j<64;j++)pairs.push_back({i,j});
   for(int pass=0;pass<HILL;pass++){
    for(int i=(int)pairs.size()-1;i>0;i--)swap(pairs[i],pairs[rng()%(i+1)]);
    int changed=0;
    for(auto [i,j]:pairs){int a=gid[i],b=gid[j];swap(bitassign[i],bitassign[j]);
     int ca=update(i),cb=(a==b?0:update(j)),delta=ca+cb-costs[a]-(a==b?0:costs[b]);
     if(delta<0){costs[a]=ca;if(a!=b)costs[b]=cb;changed++;}else swap(bitassign[i],bitassign[j]);
    } if(!changed)break;
   }
 }
 int k=0;for(auto &g:groups){g.bits.assign(bitassign.begin()+k,bitassign.begin()+k+g.b);k+=g.b;}
}

static int jointObjectiveGroup(int i){
 auto &g=groups[i];return computeCost(g.bits,g.b,*g.gr);
}
static void rebuildBits(){int k=0;for(auto &g:groups){g.bits.assign(bitassign.begin()+k,bitassign.begin()+k+g.b);k+=g.b;}}
static int currentPaperCount(){int z=0;for(auto &g:groups)z+=g.n;return z;}
static void optimizeJoint(){
 if(JOINT<=0)return;
 int G=groups.size();rebuildBits();
 vector<int> cv(G);long long total=0;for(int i=0;i<G;i++){cv[i]=jointObjectiveGroup(i);total+=cv[i];}
 long long best=total;vector<int> bestBits=bitassign;vector<pair<int,int>>bestShapes;for(auto &g:groups)bestShapes.push_back({g.b,g.n});
 int nprinted=currentPaperCount();
 for(int it=0;it<JOINT;it++){
  int a=rng()%G;int op=rng()%100,b=a;int i=-1,j=-1;int oldna=groups[a].n,oldnb=0,oldba=groups[a].b,oldbb=0;
  // 0 = local bit swap; 1 = change bit-group width; 2 = change number of sheets.
  if(op<60){
   b=rng()%(G-1);if(b>=a)b++;int startA=0,startB=0;
   for(int x=0;x<a;x++)startA+=groups[x].b;
   for(int x=0;x<b;x++)startB+=groups[x].b;
   i=startA+rng()%groups[a].b;j=startB+rng()%groups[b].b;
   swap(bitassign[i],bitassign[j]);
  }else if(op<83){
   if(a==G-1)continue; // No wraparound: changing the last/first lengths would shift every intervening group.
   // Transfer exactly one boundary bit between two adjacent groups, preserving
   // 64 total bits. It is a *joint width + paper* choice when the donor's
   // minimum number of sheets changes.
   b=(a+1)%G;oldnb=groups[b].n;oldbb=groups[b].b;
   int direction=(rng()&1)?1:-1;
   int newA=groups[a].b+direction,newB=groups[b].b-direction;
   if(newA<JOINT_MIN_B||newA>JOINT_MAX_B||newB<JOINT_MIN_B||newB>JOINT_MAX_B)continue;
   if(groups[a].n<newA+1||groups[b].n<newB+1)continue;
   groups[a].b=newA;groups[b].b=newB;
   groups[a].gr=getGraph(newA,groups[a].n,MOTIF);groups[b].gr=getGraph(newB,groups[b].n,MOTIF);
  }else{
   int direction=(rng()&1)?1:-1;
   int nn=groups[a].n+direction;
   if(nn<groups[a].b+1||nn>8||nprinted+direction>JOINT_BUDGET)continue;
   groups[a].n=nn;groups[a].gr=getGraph(groups[a].b,nn,MOTIF);
   nprinted+=direction;
  }
  if(op>=60 && op<83 && b<a){
   // Keep a contiguous bit assignment. For wrap-around, move via a normal
   // cyclic boundary: swapping group lengths alone is not an issue.
  }
  rebuildBits();
  int ca=jointObjectiveGroup(a), cb=a==b?0:jointObjectiveGroup(b);
  long long delta=(long long)ca+cb-cv[a]-(a==b?0:cv[b]);
  // Simulated annealing on a normalized proxy (computeCost divides by 128).
  double progress=double(it)/JOINT,temp=400.*pow(0.003,progress);
  if(delta<=0 || (rng()>>11)*0x1.0p-53 < exp(-double(delta)/temp)){
   cv[a]=ca;if(a!=b)cv[b]=cb;total+=delta;jointAccepted++;
   if(op>=60&&op<83)jointWidthAccepted++;if(op>=83)jointPaperAccepted++;
   if(total<best){best=total;bestBits=bitassign;bestShapes.clear();for(auto &g:groups)bestShapes.push_back({g.b,g.n});}
  }else{
   if(op<60)swap(bitassign[i],bitassign[j]);
   else if(op<83){groups[a].b=oldba;groups[b].b=oldbb;groups[a].gr=getGraph(oldba,groups[a].n,MOTIF);groups[b].gr=getGraph(oldbb,groups[b].n,MOTIF);}
   else {nprinted-=groups[a].n-oldna;groups[a].n=oldna;groups[a].gr=getGraph(groups[a].b,oldna,MOTIF);}
  }
 }
 bitassign=bestBits;
 for(int i=0;i<G;i++){groups[i].b=bestShapes[i].first;groups[i].n=bestShapes[i].second;groups[i].gr=getGraph(groups[i].b,groups[i].n,MOTIF);}
 rebuildBits();int offset=0;for(auto &g:groups){g.offset=offset;offset+=g.n;}
 if(stats){cerr<<"JOINT objective="<<best<<" sheets="<<currentPaperCount()<<" accepted="<<jointAccepted<<" widthAccept="<<jointWidthAccepted<<" paperAccept="<<jointPaperAccepted<<" shape=";for(auto &g:groups)cerr<<g.b<<"x"<<g.n<<",";cerr<<"\n";}
}

static void initCase(){
 seedrng=0x12605b4701ull;for(u64 t:targets)seedrng^=t+0x9e3779b97f4a7c15ull+(seedrng<<6)+(seedrng>>2);seedrng+=SEED;
 bitassign.resize(64);iota(bitassign.begin(),bitassign.end(),0);
 allocate();
 if(RESTARTS<=1){optimizeBits();}
 else {
   long long bestScore=LLONG_MAX;vector<int>bestAssignment;
   for(int k=0;k<RESTARTS;k++){
    optimizeBits();long long sc=0;
    for(auto &g:groups)sc+=computeCost(g.bits,g.b,*g.gr);
    if(sc<bestScore){bestScore=sc;bestAssignment=bitassign;}
   }
   bitassign=bestAssignment;int q=0;for(auto &g:groups){g.bits.assign(bitassign.begin()+q,bitassign.begin()+q+g.b);q+=g.b;}
 }
 if(ALLOC==1){allocate();optimizeBits();}
 optimizeJoint();
 for(auto &g:groups){auto& gr=*g.gr;g.future.assign(gr.S,0);int best=INT_MAX,st=0;
  vector<int>freq(gr.goal);
  for(u64 t:targets)freq[targetFor(g,t)]++;
  for(int s=0;s<gr.S;s++){
   int v=0;for(int t=0;t<gr.goal;t++)v+=freq[t]*gr.dist[t][s];g.future[s]=v;
   if(v<best){best=v;st=s;}
  }
  g.st=st;g.order.assign(gr.perm[st].begin(),gr.perm[st].begin()+g.n);g.physical.resize(g.n);for(int j=0;j<g.n;j++)g.physical[g.order[j]]=g.offset+j;
 }
}

// Cross-group simulation. A source from a physically deeper group can cover
// earlier recipient bits only when lifted to the global top. No paper is
// moved illegally: visibility is rechecked on all 192 physical sheets.
int BRIDGE=1, BR_BITS=3, BR_MAX_ACTIVE=3, BR_THRESHOLD=1, BR_DESIGN=1, BR_SOURCE=1, BR_COUNT=5, BR_TARGETS=2, BR_BITS2=5, BR_DESIGN_WEIGHT=0;
struct Restricted {
 int n=0,S=0,goalCount=0,fullMask=0,mask=0,src=-1;
 vector<uint16_t> vis;
 vector<vector<uint8_t>> dist;
};
map<tuple<Graph*,int,int>,shared_ptr<Restricted>> restrictCache;
static int compressMasked(int target,int mask,int b){int r=0,j=0;for(int i=0;i<b;i++)if(mask>>i&1){r|=((target>>i)&1)<<j;j++;}return r;}
static shared_ptr<Restricted> restrictGraph(Graph &gr,int coveredMask,int ownerSource=-1){
 auto key=make_tuple(&gr,coveredMask,ownerSource);auto it=restrictCache.find(key);if(it!=restrictCache.end())return it->second;
 auto rs=make_shared<Restricted>();rs->n=gr.n;rs->S=gr.S;rs->mask=((1<<gr.b)-1)^coveredMask;rs->src=ownerSource;
 rs->goalCount=1<<popcount((unsigned)rs->mask);rs->vis.resize(gr.S);
 vector<uint32_t>deg(gr.S);int n=gr.n;
 auto eligible=[&](int s){return ownerSource<0 || gr.perm[s][0]==ownerSource;};
 for(int s=0;s<gr.S;s++)if(eligible(s)){
  int seen=0,v=0;
  for(int i=0;i<n;i++){
   auto p=gr.pat[gr.perm[s][i]];int cv=p.cv&rs->mask;
   if((ownerSource<0||i!=0)&&(cv&~seen))v|=1<<i;
   seen|=cv;
  }
  rs->vis[s]=v;
  for(int i=0;i<n;i++)if(v>>i&1)for(int d=(ownerSource<0?0:1);d<n;d++)if(i!=d){int ns=gr.adj[s][i*8+d];deg[ns]++;}
 }
 vector<uint32_t>off(gr.S+1);for(int s=0;s<gr.S;s++)off[s+1]=off[s]+deg[s];
 vector<uint32_t> pred(off.back()),cur=off;
 for(int s=0;s<gr.S;s++)if(eligible(s))for(int i=0;i<n;i++)if(rs->vis[s]>>i&1)for(int d=(ownerSource<0?0:1);d<n;d++)if(i!=d){int ns=gr.adj[s][i*8+d];pred[cur[ns]++]=s;}
 rs->dist.assign(rs->goalCount,vector<uint8_t>(gr.S,255));vector<uint32_t>q(gr.S);
 for(int tg=0;tg<rs->goalCount;tg++){
  auto &dis=rs->dist[tg];int head=0,tail=0;
  for(int s=0;s<gr.S;s++)if(eligible(s) && compressMasked(gr.shown[s],rs->mask,gr.b)==tg){dis[s]=0;q[tail++]=s;}
  while(head<tail){int state=q[head++],nd=dis[state]+1;for(uint32_t j=off[state];j<off[state+1];j++){int prev=pred[j];if(dis[prev]==255){dis[prev]=nd;q[tail++]=prev;}}}
  // Some restricted components can be unreachable; not a correctness error.
 }
 restrictCache.emplace(key,rs);
 if(stats)cerr<<"RESTRICT b="<<gr.b<<" n="<<gr.n<<" cover="<<coveredMask<<" src="<<ownerSource<<" states="<<gr.S<<" edges="<<off.back()<<"\n";
 return rs;
}

struct Preparation{
  int src=0,mask=0; vector<vector<uint8_t>> dist;
};
map<pair<Graph*,int>,shared_ptr<Preparation>> prepCache;
static shared_ptr<Preparation> getPreparation(Graph &gr,int src){
  auto key=make_pair(&gr,src);auto it=prepCache.find(key);if(it!=prepCache.end())return it->second;
  auto st=Clock::now();auto rs=restrictGraph(gr,gr.pat[src].cv,src);
  auto p=make_shared<Preparation>();p->src=src;p->mask=rs->mask;
  p->dist.resize(rs->goalCount);int S=gr.S,n=gr.n;
  for(int t=0;t<rs->goalCount;t++){
    auto &ds=p->dist[t];ds.assign(S,255);
    array<vector<int>,255> q;
    for(int s=0;s<S;s++){
      int pos=-1;for(int j=0;j<n;j++)if(gr.perm[s][j]==src){pos=j;break;}
      if(!(gr.vis[s]>>pos&1))continue;
      int newst=pos?gr.adj[s][pos*8]:s;
      int dr=rs->dist[t][newst];if(dr==255 || dr>=254)continue;
      int v=1+dr;ds[s]=v;q[v].push_back(s);
    }
    for(int d=1;d<254;d++){
      while(!q[d].empty()){
        int at=q[d].back();q[d].pop_back();if(ds[at]!=d)continue;
        for(uint32_t j=gr.off[at];j<gr.off[at+1];j++){
          int prev=gr.pred[j];if(ds[prev]>d+1){ds[prev]=d+1;q[d+1].push_back(prev);}
        }
      }
    }
  }
  prepCache.emplace(key,p);
  if(stats)cerr<<"PREPARATION b="<<gr.b<<" src="<<src<<" build_ms="<<chrono::duration<double,milli>(Clock::now()-st).count()<<"\n";
  return p;
}
struct BridgeDef {int g=-1,h=-1,source=1,mask=0,pat=0,h2=-1,mask2=0,pat2=0;};
vector<BridgeDef> bridgeDefs;
struct ActiveBridge{int g,h,source,phys,bid,mode,cut;};vector<ActiveBridge> activeBridges;
vector<int> world;array<u64,192> inkCover{},inkBlack{};
int bridgeUse=0,bridgeRestore=0,bridgeGain=0,bridgeCandidates=0;
static bool worldVisible(int phys){u64 seen=0;for(int x:world){if(x==phys)return (inkCover[x]&~seen)!=0;seen|=inkCover[x];}abort();}
static void emitPhysical(vector<pair<int,int>>&out,int pid,int dest){
 int old=find(world.begin(),world.end(),pid)-world.begin();if(old>=192||dest<0||dest>=192||!worldVisible(pid)){cerr<<"ILLEGAL PHYSICAL paper="<<pid<<" dest="<<dest<<"\n";abort();}
 world.erase(world.begin()+old);world.insert(world.begin()+dest,pid);out.push_back({pid,dest});
}
static int realIndex(int id){auto it=find(world.begin(),world.end(),id);if(it==world.end())abort();return int(it-world.begin());}
static void localStep(Group&g,int i,int d,vector<pair<int,int>>&out){
 int n=g.n,oldSt=g.st,pid=g.order[i];
 if(i<d)for(int j=i;j<d;j++)g.order[j]=g.order[j+1];else for(int j=i;j>d;j--)g.order[j]=g.order[j-1];g.order[d]=pid;

 int next=g.gr->adj[oldSt][i*8+d];g.st=next;
 int phys=g.physical[pid];int old=realIndex(phys);world.erase(world.begin()+old);
 int dest;
 if(d+1<n)dest=realIndex(g.physical[g.order[d+1]]);
 else dest=realIndex(g.physical[g.order[d-1]])+1;
 world.insert(world.begin()+dest,phys);
 out.push_back({phys,dest});
}


static void solveLocalized(Group &g,int goal,vector<pair<int,int>>&out,shared_ptr<Restricted>rs=nullptr){
 auto &gr=*g.gr;int target=rs?compressMasked(goal,rs->mask,gr.b):goal;
 auto &ds=(rs?rs->dist[target]:gr.dist[goal]);
 // New search neighborhood: optionally spend an additional visible-sheet move
 // to reach a different target-achieving stack order with improved future expectation.
 if(FUT_WEIGHT && ds[g.st]!=255){
   int start=g.st;int baseDist=ds[start];
   auto endFollowingShortest=[&](int s){
     while(ds[s]!=255 && ds[s]>0){
       int bfuture=INT_MAX,bnext=-1;
       int v=rs?rs->vis[s]:gr.vis[s];
       for(int i=0;i<g.n;i++)if(v>>i&1)for(int j=rs&&rs->src>=0?1:0;j<g.n;j++)if(i!=j){
         int to=gr.adj[s][i*8+j];if(ds[to]==ds[s]-1 && g.future[to]<bfuture){bfuture=g.future[to];bnext=to;}
       }
       if(bnext<0)abort();s=bnext;
     }
     return s;
   };
   int baselineEnd=endFollowingShortest(start);
   int64_t bestCost=int64_t(baseDist)*256+int64_t(FUT_WEIGHT)*g.future[baselineEnd];
   int bi=-1,bj=-1,bextra=0;
   auto v=rs?rs->vis[start]:gr.vis[start];
   for(int i=0;i<g.n;i++)if(v>>i&1)for(int j=rs&&rs->src>=0?1:0;j<g.n;j++)if(i!=j){
     int to=gr.adj[start][i*8+j];if(ds[to]==255)continue;
     int additional=1+int(ds[to])-baseDist;
     if(additional<0||additional>FUT_SLACK)continue;
     int finish=endFollowingShortest(to);
     int64_t cost=int64_t(1+ds[to])*256+int64_t(FUT_WEIGHT)*g.future[finish];
     if(cost<bestCost){bestCost=cost;bi=i;bj=j;bextra=additional;}
   }
   if(bi>=0){int pid=g.physical[g.order[bi]];if(!worldVisible(pid)){cerr<<"ILLEGAL FUTURE"<<endl;abort();}
     localStep(g,bi,bj,out);FUT_USED++;FUT_EXTRAMOVES+=bextra;
   }
 }
 while(ds[g.st]>0 && ds[g.st]!=255){
  int d=ds[g.st],bs=INT_MAX,ci=-1,cd=-1;
  auto allowed=rs?rs->vis[g.st]:gr.vis[g.st];
  for(int i=0;i<g.n;i++)if(allowed>>i&1)for(int j=rs&&rs->src>=0?1:0;j<g.n;j++)if(i!=j){int to=gr.adj[g.st][i*8+j];if(ds[to]!=d-1)continue;
   int sc=g.future[to];if(sc<bs){bs=sc;ci=i;cd=j;}}
  if(ci<0)abort();int phys=g.physical[g.order[ci]];if(!worldVisible(phys)){cerr<<"ILLEGAL LOCAL g="<<g.b<<" phys="<<phys<<"\n";abort();}
  localStep(g,ci,cd,out);
 }
 if(ds[g.st]==255){cerr<<"UNREACHABLE LOCAL\n";abort();}
}
static void restoreBridges(vector<pair<int,int>>&out,u64 goal){
 for(auto x:activeBridges){
   auto &g=groups[x.g];if(g.order[0]!=x.source)abort();
   int pid=x.phys;if(!worldVisible(pid)){cerr<<"ILLEGAL BRIDGE RESTORE "<<pid<<"\n";abort();}
   int target=targetFor(g,goal),bestPos=0,bst=g.st;
   if(RESTORE_DEPTH){
     int64_t bestValue=LLONG_MAX;
     for(int pos=0;pos<g.n;pos++){
       int ns=(pos==0)?g.st:g.gr->adj[g.st][pos];
       int d=g.gr->dist[target][ns];
       int64_t val=int64_t(256)*d+int64_t(RESTORE_W)*g.future[ns]/RESTORE_SCALE;
       if(val<bestValue){bestValue=val;bestPos=pos;bst=ns;}
     }
   }
   if(pid==RESTORE_FORCE_PID && RESTORE_FORCE_POS>=0){bestPos=RESTORE_FORCE_POS;bst=bestPos?g.gr->adj[g.st][bestPos]:g.st;}
   int old=realIndex(pid);world.erase(world.begin()+old);
   int dst;
   if(bestPos+1<g.n)dst=realIndex(g.physical[g.order[bestPos+1]]);
   else dst=realIndex(g.physical[g.order.back()])+1;
   world.insert(world.begin()+dst,pid);out.push_back({pid,dst});bridgeRestore++;
   if(bestPos){for(int j=0;j<bestPos;j++)g.order[j]=g.order[j+1];g.order[bestPos]=x.source;g.st=bst;RESTORE_CHANGES++;}
 }
 activeBridges.clear();
}

static void designBridges(){
 bridgeDefs.clear();activeBridges.clear();inkCover.fill(0);inkBlack.fill(0);
 for(auto &g:groups)for(int i=0;i<g.n;i++){
  int phys=g.physical[i];auto p=g.gr->pat[i];for(int j=0;j<g.b;j++)if(p.cv>>j&1){u64 bit=1ull<<g.bits[j];inkCover[phys]|=bit;if(p.bv>>j&1)inkBlack[phys]|=bit;}
 }
 for(int g=1;g<(int)groups.size();g++){
  if(groups[g].b<4||groups[g].n<groups[g].b+1)continue;
  for(int src=BR_SOURCE;src<BR_SOURCE+BR_COUNT && src<=groups[g].b;src++){auto &z=groups[g];int sourceBit=z.gr->pat[src].cv;
  if(popcount((unsigned)sourceBit)!=1)continue;
  struct First {int h,pat,mask,score;};vector<First> first;
  for(int h=0;h<g;h++)if(groups[h].b>=4&&groups[h].n>=groups[h].b+1){
   auto &rec=groups[h];int cnt=min(BR_BITS,rec.b);
   vector<int>masks;
   for(int mk=1;mk<(1<<rec.b);mk++)if(popcount((unsigned)mk)==cnt){
     if(MASK_SCAN==0 && mk!=((1<<cnt)-1))continue;
     if(MASK_SCAN==1){bool consecutive=false;for(int pos=0;pos<rec.b;pos++){int cm=0;for(int z=0;z<cnt;z++)cm|=1<<((pos+z)%rec.b);if(cm==mk)consecutive=true;}if(!consecutive)continue;}
     masks.push_back(mk);
   }
   for(int mask:masks){auto rgraph=restrictGraph(*rec.gr,mask);
   for(int pat=0;pat<(1<<cnt);pat++){
    int score=0;
    for(u64 p:targets){int tg=targetFor(z,p),th=targetFor(rec,p);if((tg&sourceBit)!=(z.gr->pat[src].bv&sourceBit)||compressMasked(th,mask,rec.b)!=pat)continue;
     int dh=rec.gr->dist[th][rec.st],rh=rgraph->dist[compressMasked(th,rgraph->mask,rec.b)][rec.st];
     if(rh!=255)score+=max(0,dh-rh)*100;
    }
    if(score>0)first.push_back({h,pat,mask,score});
   }
   }
  }
  if(first.empty())continue;
  stable_sort(first.begin(),first.end(),[](const auto&a,const auto&b){return a.score>b.score;});
  int hbest=-1,tbest=0,scoreBest=INT_MIN,mbest=0;
  int h2best=-1,mbest2=0,tbest2=0,score2best=0;
  for(int fi=0;fi<min((int)first.size(),DESIGN_K);fi++){
    auto f=first[fi];int hh=-1,mm=0,pp0=0,ss=0;
    if(BR_TARGETS>=2){
     for(int h2=0;h2<g;h2++)if(h2!=f.h && groups[h2].b>=4){
      auto &r2=groups[h2];int mask2=(1<<min(BR_BITS2,r2.b))-1;
      auto rr2=restrictGraph(*r2.gr,mask2);
      vector<int> scores(1<<popcount((unsigned)mask2));
      for(u64 prod:targets){int tg=targetFor(z,prod),th=targetFor(groups[f.h],prod),t2=targetFor(r2,prod);
       if((tg&sourceBit)!=(z.gr->pat[src].bv&sourceBit)||compressMasked(th,f.mask,groups[f.h].b)!=f.pat)continue;
       int goal2=compressMasked(t2,mask2,r2.b);
       int dd=r2.gr->dist[t2][r2.st],rd=rr2->dist[compressMasked(t2,rr2->mask,r2.b)][r2.st];
       if(rd!=255)scores[goal2]+=max(0,dd-rd)*100;
      }
      for(int p=0;p<(int)scores.size();p++)if(scores[p]>ss){ss=scores[p];hh=h2;mm=mask2;pp0=p;}
     }
    }
    if(BR_TARGETS==2&&hh<0)continue;
    int value=f.score+DESIGN_WEIGHT2*ss;
    if(value>scoreBest){scoreBest=value;hbest=f.h;tbest=f.pat;mbest=f.mask;h2best=hh;mbest2=mm;tbest2=pp0;score2best=ss;}
  }
  if(hbest<0)continue;
  if(h2best<0 && BR_TARGETS==2)continue;
  bridgeDefs.push_back({g,hbest,src,mbest,tbest,h2best,mbest2,tbest2});
  int pid=z.physical[src],pos=0;for(int j=0;j<groups[hbest].b;j++)if(mbest>>j&1){u64 bit=1ull<<groups[hbest].bits[j];inkCover[pid]|=bit;if(tbest>>popcount((unsigned)(mbest&((1<<j)-1)))&1)inkBlack[pid]|=bit;}
  if(h2best>=0){for(int j=0;j<groups[h2best].b;j++)if(mbest2>>j&1){u64 bit=1ull<<groups[h2best].bits[j];inkCover[pid]|=bit;if(tbest2>>popcount((unsigned)(mbest2&((1<<j)-1)))&1)inkBlack[pid]|=bit;}}
  }
 }
 world.resize(192);iota(world.begin(),world.end(),0);
}
struct EmbedPlan{int prep=999,cut=-1,pi=-1,pd=-1;};
static int expandedPat(int mask,int val,int b){
 int bits=0,j=0;for(int i=0;i<b;i++)if(mask>>i&1){bits|=((val>>j)&1)<<i;j++;}return bits;
}
static bool correctWithEmbedded(const Graph &gr,int state,int cut,int mask,int black,int target){
 int covered=0,shown=0;
 for(int j=0;j<=gr.n;j++){
   if(j==cut){shown|=black&~covered;covered|=mask;}
   if(j<gr.n){auto p=gr.pat[gr.perm[state][j]];shown|=p.bv&~covered;covered|=p.cv;}
 }
 return shown==target;
}
static EmbedPlan findEmbed(const Group &r,int goal,int mask,int val){
 auto &gr=*r.gr;int blk=expandedPat(mask,val,r.b);
 EmbedPlan ans;
 auto check=[&](int st,int prep,int i,int d){
   for(int cut=0;cut<=r.n;cut++){
      if(correctWithEmbedded(gr,st,cut,mask,blk,goal)){
        if(prep<ans.prep || (prep==ans.prep&&r.future[st]<r.future[(ans.pi<0)?r.st:gr.adj[r.st][ans.pi*8+ans.pd]]))
         ans={prep,cut,i,d};
        return;
      }
   }
 };
 check(r.st,0,-1,-1);
 if(BR_EMBED>=1 && ans.prep>0){
   for(int i=0;i<r.n;i++)if(gr.vis[r.st]>>i&1)for(int d=0;d<r.n;d++)if(d!=i){
      int ns=gr.adj[r.st][i*8+d];check(ns,1,i,d);
   }
 }
 return ans;
}
static void activateBridge(const BridgeDef &c,vector<pair<int,int>>&out,int lowest=-1,int goal=-1,int embedCut=-1,int activeMode=3){
 if(BR_EXPOSE && goal>=0){
   auto &z=groups[c.g];auto &gr=*z.gr;
   auto prep=getPreparation(gr,c.source);auto rs=restrictGraph(gr,gr.pat[c.source].cv,c.source);
   int t=compressMasked(goal,prep->mask,gr.b);
   auto &ds=prep->dist[t];int steps=0;
   for(;;){
     int idx=-1;for(int j=0;j<z.n;j++)if(z.order[j]==c.source){idx=j;break;}
     int s=z.st,newst=idx?gr.adj[s][idx*8]:s;
     int dr=rs->dist[t][newst];
     if((gr.vis[s]>>idx&1)&&dr!=255 && ds[s]==dr+1)break;
     if(ds[s]==255||ds[s]==0||steps++>BR_EXPOSE_CAP+5){cerr<<"PREP_FAIL\n";abort();}
     bool found=false;
     for(int i=0;i<z.n&&!found;i++)if(gr.vis[s]>>i&1)for(int d=0;d<z.n;d++)if(i!=d){
       int to=gr.adj[s][i*8+d];if(ds[to]==ds[s]-1){
         int pid=z.physical[z.order[i]];if(!worldVisible(pid)){cerr<<"ILLEGAL_PREP\n";abort();}
         localStep(z,i,d,out);found=true;break;
       }
     }
     if(!found){cerr<<"NO_PREP_EDGE\n";abort();}
   }
 }

 auto &z=groups[c.g];int index=-1;for(int j=0;j<z.n;j++)if(z.order[j]==c.source)index=j;
 if(index<0||!(z.gr->vis[z.st]>>index&1))abort();
 int pid=z.physical[c.source],newst=(index?z.gr->adj[z.st][index*8]:z.st);
 int dest=lowest<0?0:realIndex(groups[lowest].physical[groups[lowest].order[0]]);
 if(embedCut>=0){auto &r=groups[lowest];if(embedCut<r.n)dest=realIndex(r.physical[r.order[embedCut]]);
 else dest=realIndex(r.physical[r.order.back()])+1;}

 emitPhysical(out,pid,dest);
 for(int i=index;i>0;i--)z.order[i]=z.order[i-1];z.order[0]=c.source;z.st=newst;
 activeBridges.push_back({c.g,c.h,c.source,pid,int(&c-bridgeDefs.data()),activeMode,embedCut});bridgeUse++;
}
static int runBridgeOrder(u64 goal,vector<pair<int,int>>&out,bool preserve=false){
 if(!preserve)restoreBridges(out,goal);
 vector<bool> reserved(groups.size(),false);vector<int>mode(groups.size(),0);
 if(preserve){
   for(const auto &a:activeBridges){
     const auto &c=bridgeDefs[a.bid];
     if(mode[a.g])abort();mode[a.g]=a.bid+1;reserved[a.g]=true;
     if(a.mode&(1|4)){if(mode[c.h])abort();mode[c.h]=(a.mode&4)?INT_MIN:-(a.bid+1);reserved[c.h]=true;}
     if(a.mode&(2|8)){if(mode[c.h2])abort();mode[c.h2]=(a.mode&8)?INT_MIN:-(a.bid+1);reserved[c.h2]=true;}
   }
 }
 for(int iter=0;iter<BR_MAX_ACTIVE;iter++){
  int best=-1,bestScore=0,bestDepth=-1,bestRecMode=0;EmbedPlan bestEmbed;
  for(int bi=0;bi<(int)bridgeDefs.size();bi++){
   auto &c=bridgeDefs[bi];
   if(c.g<0||reserved[c.g]||((!BR_RELAX||preserve)&&(reserved[c.h]||(c.h2>=0&&reserved[c.h2]))))continue;
   auto &z=groups[c.g];int tg=targetFor(z,goal);
   if(((tg^z.gr->pat[c.source].bv)&z.gr->pat[c.source].cv))continue;
   int pos=-1;for(int j=0;j<z.n;j++)if(z.order[j]==c.source)pos=j;
   if(pos<0)abort();
   bool vis=(z.gr->vis[z.st]>>pos&1);
   if(!vis&&!BR_EXPOSE)continue;
   auto own=restrictGraph(*z.gr,z.gr->pat[c.source].cv,c.source);
   int partial=compressMasked(tg,own->mask,z.b),ocost=255;
   if(BR_EXPOSE){auto prep=getPreparation(*z.gr,c.source);ocost=prep->dist[partial][z.st];}
   else {int newst=pos?z.gr->adj[z.st][pos*8]:z.st;int rr=own->dist[partial][newst];if(rr!=255)ocost=rr+1;}
   if(ocost==255)continue;
   int dg=z.gr->dist[tg][z.st],ownerGain=dg-ocost;
   // Cache the marginal value of activating each recipient independently.
   int gain1=INT_MIN/4,gain2=INT_MIN/4;
   auto &r=groups[c.h];int th=targetFor(r,goal);
   if(compressMasked(th,c.mask,r.b)==c.pat){auto rr=restrictGraph(*r.gr,c.mask);
    int dr=rr->dist[compressMasked(th,rr->mask,r.b)][r.st];
    if(dr!=255)gain1=r.gr->dist[th][r.st]-dr;
   }
   if(c.h2>=0){auto &r2=groups[c.h2];int th2=targetFor(r2,goal);
    if(compressMasked(th2,c.mask2,r2.b)==c.pat2){auto rr2=restrictGraph(*r2.gr,c.mask2);
     int dr=rr2->dist[compressMasked(th2,rr2->mask,r2.b)][r2.st];
     if(dr!=255)gain2=r2.gr->dist[th2][r2.st]-dr;
    }
   }
   int gg=INT_MIN/4,dd=-1,mm=0;EmbedPlan candidateEmbed;
   if(gain1>INT_MIN/8 && (c.h2<0 || gain2>INT_MIN/8) && !reserved[c.h] && (c.h2<0 || !reserved[c.h2])){
     gg=ownerGain+gain1+(c.h2<0?0:gain2);mm=3;dd=-1; // existing full-global activation
   }
   if(BR_DEPTH && c.h2>=0){
     // Activate only the deeper of h/h2. The upper recipient remains hidden by its own all-white sheet.
     bool lowIsFirst=c.h>c.h2;
     int marginal=lowIsFirst?gain1:gain2;
     if(!reserved[lowIsFirst?c.h:c.h2] && marginal>INT_MIN/8 && ownerGain+marginal>gg){gg=ownerGain+marginal;mm=lowIsFirst?1:2;dd=lowIsFirst?c.h:c.h2;}
   }
   if(BR_EMBED && c.h2>=0){
     bool lowIsFirst=c.h>c.h2;
     auto &rlow=groups[lowIsFirst?c.h:c.h2];
     int lgoal=lowIsFirst?th:targetFor(rlow,goal);
     int lmask=lowIsFirst?c.mask:c.mask2;
     int lpat=lowIsFirst?c.pat:c.pat2;
     EmbedPlan ep=findEmbed(rlow,lgoal,lmask,lpat);
     int potential=ownerGain+rlow.gr->dist[lgoal][rlow.st]-ep.prep;
     if(!reserved[lowIsFirst?c.h:c.h2] && ep.cut>=0&&potential>gg){gg=potential;mm=lowIsFirst?4:8;dd=lowIsFirst?c.h:c.h2;candidateEmbed=ep;}
   }
   if(gg>=BR_THRESHOLD && (!BR_EXPOSE || ocost <= dg+BR_EXPOSE_CAP)){bridgeCandidates++;if(gg>bestScore){bestScore=gg;best=bi;bestDepth=dd;bestRecMode=mm;bestEmbed=candidateEmbed;}}
  }
  if(best<0)break;
  const auto &c=bridgeDefs[best];
  if(bestRecMode&(4|8)){
    int rec=(bestRecMode&4)?c.h:c.h2;
    if(bestEmbed.prep==1){auto &r=groups[rec];int pid=r.physical[r.order[bestEmbed.pi]];
      if(!worldVisible(pid)){cerr<<"ILLEGAL_EMBED_PREP\n";abort();}
      localStep(r,bestEmbed.pi,bestEmbed.pd,out);
    }
    BR_EMBED_USED++;
  }
  activateBridge(c,out,bestDepth,targetFor(groups[c.g],goal),bestEmbed.cut,bestRecMode);
  reserved[c.g]=true;
  if(!BR_RELAX || bestRecMode==3){reserved[c.h]=true;if(c.h2>=0)reserved[c.h2]=true;}
  else if(bestRecMode&(1|4))reserved[c.h]=true;
  else if(bestRecMode&(2|8))reserved[c.h2]=true;
  mode[c.g]=best+1;
  if(bestRecMode&1)mode[c.h]=-(best+1);
  if(c.h2>=0&&(bestRecMode&2))mode[c.h2]=-(best+1);
  if(bestRecMode&4)mode[c.h]=INT_MIN;
  if(c.h2>=0&&(bestRecMode&8))mode[c.h2]=INT_MIN;
  bridgeGain+=bestScore;
 }
 for(int i=0;i<(int)groups.size();i++){
  auto &g=groups[i];int target=targetFor(g,goal);
  if(mode[i]==INT_MIN)continue;
  if(mode[i]>0){auto &c=bridgeDefs[mode[i]-1];solveLocalized(g,target,out,restrictGraph(*g.gr,g.gr->pat[c.source].cv,c.source));}
  else if(mode[i]<0){auto &c=bridgeDefs[-mode[i]-1];solveLocalized(g,target,out,restrictGraph(*g.gr,c.h==i?c.mask:c.mask2));}
  else solveLocalized(g,target,out);
 }
 u64 seen=0,shown=0;for(int phys:world){shown|=inkBlack[phys]&~seen;seen|=inkCover[phys];}
 if(shown!=goal){cerr<<"BAD_DISPLAY expected="<<goal<<" observed="<<shown<<"\n";abort();}
 return out.size();
}

// Alternative operation policy: keep a bridge from the previous order on the
// stack, instead of restoring it unconditionally. The next actual request is
// known at this point, but no later request is inspected. Trial plans are
// rolled back to the exact physical stack before selecting the cheaper route.
struct LocalSnap{vector<int>world0;vector<ActiveBridge>active0;vector<int>states;vector<vector<int>>orders;int uses,restores,gain,candidates,embedUsed,futUsed,futExtra,restoreChanges;};
static LocalSnap captureSnap(){LocalSnap x;x.world0=world;x.active0=activeBridges;for(auto &g:groups){x.states.push_back(g.st);x.orders.push_back(g.order);}x.uses=bridgeUse;x.restores=bridgeRestore;x.gain=bridgeGain;x.candidates=bridgeCandidates;x.embedUsed=BR_EMBED_USED;x.futUsed=FUT_USED;x.futExtra=FUT_EXTRAMOVES;x.restoreChanges=RESTORE_CHANGES;return x;}
static void restoreSnap(const LocalSnap &x){world=x.world0;activeBridges=x.active0;for(int i=0;i<(int)groups.size();i++){groups[i].st=x.states[i];groups[i].order=x.orders[i];}bridgeUse=x.uses;bridgeRestore=x.restores;bridgeGain=x.gain;bridgeCandidates=x.candidates;BR_EMBED_USED=x.embedUsed;FUT_USED=x.futUsed;FUT_EXTRAMOVES=x.futExtra;RESTORE_CHANGES=x.restoreChanges;}
static bool canKeepBridge(const ActiveBridge &a,u64 goal){
 const auto &c=bridgeDefs[a.bid];auto &z=groups[a.g];int t=targetFor(z,goal);
 if(((t^z.gr->pat[c.source].bv)&z.gr->pat[c.source].cv)!=0)return false;
 auto own=restrictGraph(*z.gr,z.gr->pat[c.source].cv,c.source);
 if(own->dist[compressMasked(t,own->mask,z.b)][z.st]==255)return false;
 for(int q=0;q<2;q++){
   int bit=q?2:1;if(!(a.mode&(bit|(q?8:4))))continue;
   int h=q?c.h2:c.h;if(h<0)return false;auto &r=groups[h];
   int mask=q?c.mask2:c.mask,pat=q?c.pat2:c.pat,rg=targetFor(r,goal);
   if(a.mode&(q?8:4)){
     // A frozen recipient with a foreign paper inside: check complete display.
     int black=0,j=0;for(int p=0;p<r.b;p++)if(mask>>p&1){black|=((pat>>j)&1)<<p;j++;}
     int covered=0,shown=0;
     for(int p=0;p<=r.n;p++){
       if(p==a.cut){shown|=black&~covered;covered|=mask;}
       if(p<r.n){auto ink=r.gr->pat[r.order[p]];shown|=ink.bv&~covered;covered|=ink.cv;}
     }
     if(shown!=rg)return false;
   }else{
     if(compressMasked(rg,mask,r.b)!=pat)return false;
     auto rr=restrictGraph(*r.gr,mask);if(rr->dist[compressMasked(rg,rr->mask,r.b)][r.st]==255)return false;
   }
 }
 return true;
}
static void restoreNonkept(unsigned bits,vector<pair<int,int>>&out,u64 goal){
 vector<ActiveBridge> remain;
 for(int i=0;i<(int)activeBridges.size();i++){
   auto a=activeBridges[i];if((bits>>i)&1){remain.push_back(a);continue;}
   auto &g=groups[a.g];if(g.order[0]!=a.source||!worldVisible(a.phys))abort();
   int bestPos=0,bst=g.st;
   if(KEEP_RESTORE){
     int target=targetFor(g,goal);int64_t bv=LLONG_MAX;
     for(int pos=0;pos<g.n;pos++){
       int ns=pos?g.gr->adj[g.st][pos]:g.st;
       int d=g.gr->dist[target][ns];
       int64_t val=256LL*d+int64_t(RESTORE_W)*g.future[ns]/RESTORE_SCALE;
       if(val<bv){bv=val;bestPos=pos;bst=ns;}
     }
   }
   int old=realIndex(a.phys);world.erase(world.begin()+old);
   int dst=(bestPos+1<g.n)?realIndex(g.physical[g.order[bestPos+1]]):realIndex(g.physical[g.order.back()])+1;
   world.insert(world.begin()+dst,a.phys);out.push_back({a.phys,dst});bridgeRestore++;
   if(bestPos){for(int j=0;j<bestPos;j++)g.order[j]=g.order[j+1];g.order[bestPos]=a.source;g.st=bst;}
 }
 activeBridges=move(remain);
}
// Retarget one previously active bridge instead of returning it to its owner.
// Its source remains at index zero in the owner's logical permutation.
static bool relocateActive(ActiveBridge &a,int newMode,int newCut,vector<pair<int,int>>&out){
 const auto &c=bridgeDefs[a.bid];
 if(!worldVisible(a.phys))return false;
 int old=realIndex(a.phys);
 world.erase(world.begin()+old);
 int dst=0;
 if(newMode!=3){
   int rec=(newMode&(1|4))?c.h:c.h2;
   if(rec<0)abort();auto &r=groups[rec];
   if(newMode&(4|8)){
     if(newCut<r.n)dst=realIndex(r.physical[r.order[newCut]]);
     else dst=realIndex(r.physical[r.order.back()])+1;
   }else dst=realIndex(r.physical[r.order[0]]);
 }
 world.insert(world.begin()+dst,a.phys);
 // An unchanged position is not an operation (the logical mode can still change).
 if(dst!=old)out.push_back({a.phys,dst});
 a.mode=newMode;a.cut=newCut;
 return true;
}
static bool runKeepOrder(u64 goal,unsigned keepBits,vector<pair<int,int>>&out){
 for(int i=0;i<(int)activeBridges.size();i++)if((keepBits>>i&1)&&!canKeepBridge(activeBridges[i],goal))return false;
 restoreNonkept(keepBits,out,goal);
 runBridgeOrder(goal,out,true);
 return true;
}

static void runBestOrder(u64 goal,vector<pair<int,int>>&out){
 if((!BR_KEEP&&!RESTORE_BRANCH)||activeBridges.empty()){runBridgeOrder(goal,out);return;}
 LocalSnap before=captureSnap();
 vector<pair<int,int>> regular;runBridgeOrder(goal,regular);
 auto endpoint=[&](const LocalSnap &ss)->int64_t{
   int64_t v=0;for(int i=0;i<(int)groups.size();i++)v+=groups[i].future[ss.states[i]];
   return v;
 };
 int bestCost=(int)regular.size();int bestMask=0;int64_t bestScore=int64_t(bestCost)*256+int64_t(RESTORE_BRANCH_W)*endpoint(captureSnap())/4;
 LocalSnap selected=captureSnap();vector<pair<int,int>> selectedPlan=regular;
 restoreSnap(before);
 if(RESTORE_BRANCH){
   for(const auto &a:before.active0){
     auto &g=groups[a.g];int target=targetFor(g,goal);
     vector<pair<int64_t,int>> order;
     for(int pos=0;pos<g.n;pos++){
       int ns=pos?g.gr->adj[g.st][pos]:g.st;
       order.push_back({int64_t(g.gr->dist[target][ns])*256+int64_t(RESTORE_W)*g.future[ns]/RESTORE_SCALE,pos});
     }
     sort(order.begin(),order.end());
     int count=0;
     for(auto [v,pos]:order){
       if(count++>=RESTORE_BRANCH+1)break;
       RESTORE_FORCE_PID=a.phys;RESTORE_FORCE_POS=pos;
       vector<pair<int,int>> trial;
       runBridgeOrder(goal,trial);RESTORE_BRANCH_EVAL++;
       LocalSnap newSnap=captureSnap();
       int64_t sc=int64_t(trial.size())*256+int64_t(RESTORE_BRANCH_W)*endpoint(newSnap)/4;
       if(sc<bestScore){bestScore=sc;bestCost=trial.size();bestMask=-1;selected=newSnap;selectedPlan=move(trial);}
       RESTORE_FORCE_PID=-1;RESTORE_FORCE_POS=-1;restoreSnap(before);
     }
   }
 }
 if(BR_KEEP){
   int n=(int)before.active0.size();
   for(int bits=1;bits<(1<<n);bits++){
     if(BR_KEEP==1&&bits!=(1<<n)-1)continue;
     bool possible=true;for(int i=0;i<n;i++)if((bits>>i&1)&&!canKeepBridge(activeBridges[i],goal)){possible=false;break;}
     if(!possible)continue;
     BR_KEEP_EVAL++;
     vector<pair<int,int>> trial;
     if(runKeepOrder(goal,bits,trial)){
       LocalSnap newSnap=captureSnap();
       int64_t sc=int64_t(trial.size()+BR_KEEP_PENALTY)*256+int64_t(RESTORE_BRANCH_W)*endpoint(newSnap)/4;
       if(sc<bestScore){bestScore=sc;bestCost=trial.size();bestMask=bits;selected=newSnap;selectedPlan=move(trial);}
     }
     restoreSnap(before);
   }
 }
 if(BR_RETARGET && !before.active0.empty()){
   // Each candidate keeps exactly one old active bridge, returns the others,
   // then moves it to a different legal interception height (including embed).
   for(int ai=0;ai<(int)before.active0.size();ai++){
     const auto &oldBridge=before.active0[ai];const auto &c=bridgeDefs[oldBridge.bid];
     auto &r=groups[max(c.h,c.h2)];int t=targetFor(r,goal);
     vector<pair<int,int>> choices;
     choices.push_back({3,-1});
     int lowMode=c.h>c.h2?1:2;
     choices.push_back({lowMode,-1});
     int embedMode=c.h>c.h2?4:8;
     if(BR_RETARGET>=2){
       int mask=(c.h>c.h2)?c.mask:c.mask2;
       int pat=(c.h>c.h2)?c.pat:c.pat2;
       int black=expandedPat(mask,pat,r.b);
       for(int cut=0;cut<=r.n;cut++)if(correctWithEmbedded(*r.gr,r.st,cut,mask,black,t)){
         choices.push_back({embedMode,cut});break;
       }
     }
     for(auto [newMode,newCut]:choices){
       restoreSnap(before);vector<pair<int,int>> trial;
       restoreNonkept(1u<<ai,trial,goal);
       if(activeBridges.size()!=1)abort();
       auto &a=activeBridges[0];
       if(!relocateActive(a,newMode,newCut,trial))continue;
       if(!canKeepBridge(a,goal))continue;
       BR_RETARGET_EVAL++;
       runBridgeOrder(goal,trial,true);
       LocalSnap finalSnap=captureSnap();
       int64_t score=int64_t(trial.size())*256+int64_t(RESTORE_BRANCH_W)*endpoint(finalSnap)/4;
       if(score<bestScore){bestScore=score;bestCost=trial.size();bestMask=-2;selected=move(finalSnap);selectedPlan=move(trial);}
     }
   }
 }
 restoreSnap(selected);if(bestMask>0)BR_KEEP_USED++;if(bestMask==-2)BR_RETARGET_USED++;out=move(selectedPlan);
}

int main(int argc,char **argv){
 ios::sync_with_stdio(false);cin.tie(nullptr);
 for(int i=1;i<argc;i++){string s=argv[i];auto get=[&](string name,int &v){if(s.starts_with(name+"=")){v=stoi(s.substr(name.size()+1));return true;}return false;};
  if(s.starts_with("--layout=")){LAYOUT=s.substr(9);continue;}if(get("--restore-branch",RESTORE_BRANCH)||get("--restore-branch-w",RESTORE_BRANCH_W)||get("--restore-depth",RESTORE_DEPTH)||get("--restore-w",RESTORE_W)||get("--restore-scale",RESTORE_SCALE)||get("--bridge-keep",BR_KEEP)||get("--time-guard",TIME_GUARD)||get("--bridge-retarget",BR_RETARGET)||get("--keep-restore",KEEP_RESTORE)||get("--bridge-keep-penalty",BR_KEEP_PENALTY)||get("--bridge-relax",BR_RELAX)||get("--bridge-embed",BR_EMBED)||get("--future-weight",FUT_WEIGHT)||get("--future-slack",FUT_SLACK)||get("--bridge-expose",BR_EXPOSE)||get("--bridge-expose-cap",BR_EXPOSE_CAP)||get("--bridge-depth",BR_DEPTH)||get("--bridge",BRIDGE)||get("--bridge-bits",BR_BITS)||get("--bridge-max",BR_MAX_ACTIVE)||get("--bridge-threshold",BR_THRESHOLD)||get("--bridge-design",BR_DESIGN)||get("--bridge-source",BR_SOURCE)||get("--bridge-count",BR_COUNT)||get("--bridge-targets",BR_TARGETS)||get("--bridge-bits2",BR_BITS2)||get("--intra",INTRA)||get("--hill",HILL)||get("--bridge-pair",BR_PAIR)||get("--design-k",DESIGN_K)||get("--mask-scan",MASK_SCAN)||get("--restarts",RESTARTS)||get("--design-weight2",DESIGN_WEIGHT2)||get("--joint",JOINT)||get("--joint-budget",JOINT_BUDGET)||get("--joint-min-b",JOINT_MIN_B)||get("--joint-max-b",JOINT_MAX_B)||get("--extra-a",EXTRA_A)||get("--extra-b",EXTRA_B)||get("--extra-c",EXTRA_C)||get("--bits",BN)||get("--min-extra",EMIN)||get("--max-extra",EMAX)||get("--max-states",MAX_STATES)||get("--budget",BUDGET)||get("--motif",MOTIF)||get("--bit-iter",BITITER)||get("--alloc",ALLOC)||get("--seed",SEED))continue;
  if(s=="--offline")offline=true;if(s=="--stats")stats=true;if(s=="--multi")multi=true;
 }
 auto t0=Clock::now();int idx=0;
 const int FULL_RESTORE=RESTORE_BRANCH,FULL_RETARGET=BR_RETARGET,FULL_KEEP=BR_KEEP,FULL_THRESHOLD=BR_THRESHOLD;
 while(cin>>N>>K>>P>>R){
  auto caseStart=Clock::now();RESTORE_BRANCH=FULL_RESTORE;BR_RETARGET=FULL_RETARGET;BR_KEEP=FULL_KEEP;BR_THRESHOLD=FULL_THRESHOLD;
  targets.assign(P,0);for(int i=0;i<P;i++){string s;cin>>s;for(int j=0;j<64;j++)if(s[j]=='1')targets[i]|=1ull<<j;}
  initCase();int printed=0;for(auto &g:groups)printed+=g.n;
  vector<string>patterns(192,string(64,'.'));
  for(auto &g:groups)for(int i=0;i<g.n;i++){auto &pat=g.gr->pat[g.order[i]];for(int j=0;j<g.b;j++)if(pat.cv>>j&1)patterns[g.offset+i][g.bits[j]]=(pat.bv>>j&1)?'1':'0';}
  if(BRIDGE){designBridges();for(int i=0;i<192;i++){patterns[i]=string(64,'.');for(int j=0;j<64;j++)if(inkCover[i]>>j&1)patterns[i][j]=(inkBlack[i]>>j&1)?'1':'0';}}
  if(!BRIDGE){world.resize(192);iota(world.begin(),world.end(),0);inkCover.fill(0);inkBlack.fill(0);for(int i=0;i<192;i++)for(int j=0;j<64;j++){if(patterns[i][j]!='.'){inkCover[i]|=1ull<<j;if(patterns[i][j]=='1')inkBlack[i]|=1ull<<j;}}}
  if(!offline){for(auto &x:patterns)cout<<x<<'\n';cout<<flush;}
  int moves=0,maxTurn=0;array<int,192> moveCount{};auto t1=Clock::now();
  int guardStage=0,guardJ=0;double guardMs=0,warmupMs=0;
  for(int j=0;j<R;j++){int product;if(!(cin>>product))break;vector<pair<int,int>> plan;
   // Conservative anytime fallback. It never needs a future request ID.
   if(TIME_GUARD>0 && j>=128 && j%16==0){
     auto now=Clock::now();
     double elapsed=chrono::duration<double,milli>(now-caseStart).count();
     double online=chrono::duration<double,milli>(now-t1).count();
     if(j==128)warmupMs=online;
     double recentRate=(online-warmupMs)/max(1,j-128);
     if(guardStage==0 && j>=256 && elapsed+(R-j)*recentRate>TIME_GUARD){
       guardStage=1;guardJ=j;guardMs=elapsed;
       RESTORE_BRANCH=0;BR_RETARGET=0;BR_KEEP=0;
       if(stats)cerr<<"GUARD stage=1 turn="<<j<<" elapsed="<<elapsed<<"\n";
     }else if(guardStage>=1 && j>=guardJ+48){
       double newRate=(elapsed-guardMs)/(j-guardJ);
       if(guardStage==1 && elapsed+(R-j)*newRate>TIME_GUARD+30){
         guardStage=2;guardJ=j;guardMs=elapsed;BR_THRESHOLD=2;
         if(stats)cerr<<"GUARD stage=2 turn="<<j<<" elapsed="<<elapsed<<"\n";
       }else if(guardStage==2 && elapsed+(R-j)*newRate>TIME_GUARD+80){
         guardStage=3;BR_THRESHOLD=1000;
         if(stats)cerr<<"GUARD stage=3 turn="<<j<<" elapsed="<<elapsed<<"\n";
       }
     }
   }
   if(BRIDGE)runBestOrder(targets[product],plan);else for(auto &g:groups)solveLocalized(g,targetFor(g,targets[product]),plan);
   moves+=plan.size();maxTurn=max(maxTurn,(int)plan.size());for(auto [p,d]:plan)moveCount[p]++;
   if(!offline){cout<<plan.size()<<'\n';for(auto [p,d]:plan)cout<<p<<' '<<d<<'\n';cout<<flush;}
  }
  if(stats)cerr<<"RESULT seed="<<idx<<" score="<<moves<<" maxTurn="<<maxTurn<<" sheets="<<printed<<" bridges="<<bridgeUse<<" restored="<<bridgeRestore<<" restoreChanges="<<RESTORE_CHANGES<<" restoreBranchEval="<<RESTORE_BRANCH_EVAL<<" bridgePotentialGain="<<bridgeGain<<" bridgeCandidates="<<bridgeCandidates<<" retargetUsed="<<BR_RETARGET_USED<<" retargetEval="<<BR_RETARGET_EVAL<<" keepUsed="<<BR_KEEP_USED<<" keepEval="<<BR_KEEP_EVAL<<" embedUsed="<<BR_EMBED_USED<<" futureUsed="<<FUT_USED<<" futureExtra="<<FUT_EXTRAMOVES<<" distinctMoved="<<count_if(moveCount.begin(),moveCount.end(),[](int v){return v>0;})<<" joint="<<JOINT<<" online_ms="<<chrono::duration<double,milli>(Clock::now()-t1).count()<<" init_ms="<<chrono::duration<double,milli>(t1-t0).count()<<"\n";
  idx++;if(!multi)break;
 }
}
