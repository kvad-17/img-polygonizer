#include "poly_compress.h"

poly_compress::poly_compress()
{

}

poly* poly_compress::decompress()
{
    poly *T, *P = new poly(0,0,256,0,0,256,256,256);
    QQueue<poly*> C, N;
    int n = 0;

    C.enqueue(P);

    if(tree_struct[0] || tree_struct[1]) P->split(0);

    for(int i = 0; i < tree_struct.size(); i+=2)
    {
        T = C.dequeue();

        if(tree_struct[i])
        {
            T->Q->split(0);
            N.enqueue(T->Q);
        }else T->Q->gray = tree_data[n++];
        if(tree_struct[i+1])
        {
            T->P->split(0);
            N.enqueue(T->P);
        }else T->P->gray = tree_data[n++];


        if(C.empty()) C.swap(N);
    }
    return P;
}

void poly_compress::print()
{
    QString pstr;
    int t = 1, c = 0, p = 0;

    qDebug() << tree_struct.size();
    qDebug() << tree_data.size();

    for(int i = 0; i < tree_struct.size(); i+=2)
    {
        p+=2;
        if(tree_struct[i]) c++;
        if(tree_struct[i+1]) c++;
        pstr+= tree_struct[i]?"1":"0";
        pstr+= tree_struct[i+1]?"1":"0";
        if(p==t*2)
        {
            t = c;
            c = p = 0;
            pstr+= " ";
        }
    }
    qDebug() << pstr;
}

poly_compress::poly_compress (const poly& P)
{
    QQueue<poly const*> Q, T;
    T.enqueue(&P);

    poly const* tpoly;
    bool qs, ps;

    while(!T.empty() || !Q.empty())
    {
        if(Q.empty()) Q.swap(T);

        tpoly = Q.dequeue();

        tree_struct.append( qs = tpoly->Q->is_splitted );
        tree_struct.append( ps = tpoly->P->is_splitted );
        if(qs) T.enqueue(tpoly->Q); else tree_data.append(tpoly->Q->gray);
        if(ps) T.enqueue(tpoly->P); else tree_data.append(tpoly->P->gray);

    }
    if(tree_struct.empty())
    {
        tree_struct.append(0);
        tree_struct.append(0);
    }
}
