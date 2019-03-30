#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "deanon.h"
#include "cs152coin.h"
#include "hmap.h"
il *il_singleton(const char* addr)
{
    il* l = (il*)malloc(sizeof(il));
    l->addr=addr;
    l->next=NULL;
    return l;
}

void il_free(il *ns)
{
    if(ns) {
        il_free(ns->next);
        free(ns);
    }
}

is *is_new()
{
    is *s=(is*)malloc(sizeof(is));
    if (s==NULL) {
        fprintf(stderr, "is_new: malloc failed\n");
        exit(1);
    }
    s->ns=NULL;
    return s;
}


void push(is *s, const char* addr)
{
    il *il=il_singleton(addr);
    il->next=s->ns;
    s->ns=il;
    return;
}

const char* pop(is *s)
{
    const char* addr=s->ns->addr;
    il *temp=s->ns;
    s->ns=s->ns->next;
    free(temp);
    return addr;
}

void is_free(is *s)
{
    if (s!=NULL) {
        il_free(s->ns);
        free(s);
    }
}


com_t* com_new()
{
    com_t* c=calloc(1,sizeof(com_t));
    c->visited=0;
    c->neigh=NULL;
    c->out=NULL;
    return c;
}

deanon_t* deanon_new()
{
    deanon_t* d=calloc(1,sizeof(deanon_t));
    if (d==NULL) {
        return NULL;
    }
    d->hmap=hmap_new();
    return d;
}

void deanon_free(deanon_t* d)
{
    hmap_free(d->hmap);
    free(d);
}

void add_address(const char* addr, deanon_t* d)
{
    hmap_t* hm=d->hmap;
    int i=hmap_put(addr, com_new(), hm);
    if (i==1) {
        return; 
	} else {
        return;
    }
}

void add_associations(dep_wd_t* trans_inputs[], unsigned int num_inputs, deanon_t* d)
{
    for (int i=0; i<num_inputs; i++) {
        const char* key=trans_inputs[i]->addr;
        add_address(key,d);
        com_t* temp=hmap_get(key,d->hmap);
        for (int j=0; j<num_inputs; j++) {
            if (i!=j) {
                temp->neigh=cluster_append(trans_inputs[j]->addr,temp->neigh);
            }
        }
    }
}

cluster_t* get_cluster(const char* addr, deanon_t* d)
{
    com_t* temp=hmap_get(addr,d->hmap);
    if (temp->out!=NULL) {
        return temp->out;
    }
    is* stack=is_new();
    push(stack,addr);
    while (stack->ns!=NULL) {
        const char* temp1=pop(stack);
        com_t* temp2=hmap_get(temp1,d->hmap);
        temp2->visited=1;
	    temp->out=cluster_append(temp1,temp->out);
        com_t* temp3=hmap_get(temp1,d->hmap);
        cluster_t* neigh=temp3->neigh;
        while (neigh!=NULL) {
            com_t* temp4=hmap_get(neigh->addr,d->hmap);
            if (temp4->visited==0) {
                temp4->visited=1;
		push(stack,neigh->addr);
            }
            neigh=neigh->next;            
        }
    }
    is_free(stack);
    cluster_t* temp5=temp->out;
    while (temp5!=NULL) {
        com_t* temp6=hmap_get(temp5->addr,d->hmap);
        temp6->out=temp->out;
        temp5=temp5->next;
    }
    return temp->out;
}


cluster_t* cluster_new(const char* addr)
{
    cluster_t* c=calloc(1,sizeof(cluster_t));
    c->addr=strdup(addr);
    c->next=NULL;
    return c;   
}

void cluster_free(cluster_t* c)
{
    cluster_t* temp=c;
    while (temp!=NULL) {
        cluster_t* temp1=temp;
        temp=temp->next;
        free(temp1->addr);
        free(temp1);
    }
}

cluster_t* cluster_append(const char* addr, cluster_t* c)
{
    cluster_t* cl=calloc(1,sizeof(cluster_t));
    cl->addr=strdup(addr);
    cl->next=c;
    return cl;
}    

// ===================================================================
// Add any other functions you need in order to implement deanon below
// ===================================================================


