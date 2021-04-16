/* Operations:
 * 1 u x : set val[u] = x
 * 2 u v : sum of val[i] in (u,v) path
*/

#define ll long long
#define pb push_back
const ll sz = 3e4 + 10, LN = 16, root = 1;
vector <ll> g[sz];
ll sub[sz], in[sz], out[sz], head[sz], depth[sz], tim;
ll pa[LN][sz], tr[4*sz];

void dfs_siz(ll u, ll p, ll d)
{
    depth[u] = d, sub[u] = 1, pa[0][u] = p;
    for(ll &v : g[u]) {
        if(v == p)
            continue;
        dfs_siz(v, u, d+1);
        sub[u] += sub[v];
        if(sub[v] > sub[ g[u][0] ])
            swap(v, g[u][0]);
    }
}

/* DFS for Heavy-Light Decomposition
 * head[u] = Head of the chain of node u
 * Operations can be performed in [in[head[u]],in[u]] range.
 * As DFS-time is used, [in[u],out[u]] range can be used
   for performing operations on the subtree of node u.
*/
void dfs_hld(ll u, ll p)
{
    if(u == root) head[u] = root;
    in[u] = ++tim;
    for(ll &v : g[u]) {
        if(v == p)
            continue;
        head[v] = (v==g[u][0]? head[u] : v);
        dfs_hld(v, u);
    }
    out[u] = tim;
}

ll LCA(ll u, ll v)
{
    if(depth[u] < depth[v]) swap(u,v);
    ll diff = depth[u] - depth[v];
    for(ll i = 0; i < LN; i++) if( (diff>>i)&1 ) u = pa[i][u];
    if(u == v) return u;
    for(ll i = LN-1; i >= 0; i--) {
        if(pa[i][u] != pa[i][v]) {
            u = pa[i][u];
            v = pa[i][v];
        }
    }
    return pa[0][u];
}

void build(ll lo, ll hi, ll node)
{
    if(lo == hi) {
        tr[node] = 0;
        return;
    }
    ll mid = lo+hi>>1, lft=node<<1, rgt=lft|1;
    build(lo, mid, lft);
    build(mid+1, hi, rgt);
    tr[node] = 0;
}

void update(ll lo, ll hi, ll idx, ll v, ll node)
{
    if(lo > idx || hi < idx)
        return;
    if(lo == hi) {
        tr[node] = v;
        return;
    }
    ll mid = lo+hi>>1, lft=node<<1, rgt=lft|1;
    update(lo, mid, idx, v, lft);
    update(mid+1, hi, idx, v, rgt);
    tr[node] = tr[lft] + tr[rgt];
}

ll query(ll lo, ll hi, ll l, ll r, ll node)
{
    if(lo > r || hi < l)
        return 0;
    if(lo >= l && hi <= r)
        return tr[node];
    ll mid = lo+hi>>1, lft=node<<1, rgt=lft|1;
    return query(lo, mid, l, r, lft)
        +  query(mid+1, hi, l, r, rgt);
}

void upd_val(ll u, ll val) {
    update(1, tim, in[u], val, 1);
}

ll query_chain(ll lca, ll u)
{
    ll h = -1, ret = 0;
    while(h != lca) {
        h = head[u];
        if(depth[h] < depth[lca]) h = lca;

        ret += query(1, tim, in[h], in[u], 1);
        u = pa[0][h];
    }
    return ret;
}

ll query_path(ll u, ll v)
{
    ll lca = LCA(u, v);
    ll ret = query_chain(lca, u)+query_chain(lca, v);

    // lca is counted twice. So, subtraction is needed.
    ret -= query_chain(lca, lca);
    return ret;
}
void clr(ll n) {
    tim = 0;
    for(ll i=1; i <=n; i++) {
        g[i].clear();
        for(ll j=0; j<LN; j++) pa[j][i] = -1;
    }
}
int main()
{
    ll t, n, q, u, v, op;
    cin >> t;
    while(t--) {
        scanf("%lld", &n); clr(n);

        for(ll i=1; i<n; i++) {
            scanf("%lld %lld", &u, &v);
            g[u].pb(v), g[v].pb(u);
        }

        dfs_siz(root, -1, 0);
        dfs_hld(root, -1);
        build(1, tim, 1);

        for(int i=1; i<LN; i++)
            for(int j=1; j<=n; j++)
                if(pa[i-1][j] != -1)
                    pa[i][j] = pa[i-1][pa[i-1][j]];

        scanf("%lld", &q);
        while(q--) {
            scanf("%lld %lld %lld", &op, &u, &v);
            if(op == 1) upd_val(u, v);
            else printf("%lld\n", query_path(u, v));
        }
    }
    return 0;
}
